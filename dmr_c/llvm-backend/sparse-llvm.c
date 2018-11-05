/**
* Sparse LLVM backend
*
* Original copyrights: Pekka Enberg and Jeff Garzik (https://lwn.net/Articles/456709/)
* Additional copyrights: Dibyendu Majumdar and Luc Van Oostenryck
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <llvm-c/Core.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Target.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <port.h>
#include <dmr_c.h>
#include <lib.h>
#include <symbol.h>
#include <expression.h>
#include <linearize.h>
#include <flow.h>

struct function {
	LLVMBuilderRef			builder;
	LLVMTypeRef			type;
	LLVMValueRef			fn;
	LLVMModuleRef			module;
	LLVMTypeRef			return_type;
};

static LLVMTypeRef type_to_llvmtype(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node);
static LLVMValueRef constant_value(struct dmr_C *C, LLVMModuleRef module, unsigned long long val, LLVMTypeRef dtype);

static const char *get_llvmtypekind_name(LLVMTypeKind kind) {
	switch (kind) {
	case LLVMVoidTypeKind: return "VoidType";        /**< type with no size */
	case LLVMHalfTypeKind: return "HalfFloatType";        /**< 16 bit floating point type */
	case LLVMFloatTypeKind: return "FloatType";       /**< 32 bit floating point type */
	case LLVMDoubleTypeKind: return "DoubleType";      /**< 64 bit floating point type */
	case LLVMX86_FP80TypeKind: return "LongDoubleType";    /**< 80 bit floating point type (X87) */
	case LLVMFP128TypeKind: return "128BitDoubleType";       /**< 128 bit floating point type (112-bit mantissa)*/
	case LLVMPPC_FP128TypeKind: return "128BitPPCDoubleType";   /**< 128 bit floating point type (two 64-bits) */
	case LLVMLabelTypeKind: return "LabelType";       /**< Labels */
	case LLVMIntegerTypeKind: return "IntegerType";     /**< Arbitrary bit width integers */
	case LLVMFunctionTypeKind: return "FunctionType";    /**< Functions */
	case LLVMStructTypeKind: return "StructType";      /**< Structures */
	case LLVMArrayTypeKind: return "ArrayType";       /**< Arrays */
	case LLVMPointerTypeKind: return "PointerType";     /**< Pointers */
	case LLVMVectorTypeKind: return "VectorType";      /**< SIMD 'packed' format, or other vector type */
	case LLVMMetadataTypeKind: return "MetadataType";    /**< Metadata */
	case LLVMX86_MMXTypeKind: return "MMXType";     /**< X86 MMX */
	case LLVMTokenTypeKind: return "TokenType";        /**< Tokens */
	default: assert(0);
	}
	return "";
}

static LLVMTypeRef get_symnode_type(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym)
{
	assert(sym->type == SYM_NODE);
	return 	type_to_llvmtype(C, module, sym->ctype.base_type, sym);
}

static LLVMTypeRef get_symnode_or_basetype(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym)
{
	if (sym->type == SYM_NODE) {
		assert(sym->ctype.base_type->type != SYM_NODE);
		return type_to_llvmtype(C, module, sym->ctype.base_type, sym);
	}
	return type_to_llvmtype(C, module, sym, NULL);
}


static LLVMTypeRef func_return_type(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node)
{
	return type_to_llvmtype(C, module, sym->ctype.base_type, sym_node);
}

static LLVMTypeRef sym_func_type(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node)
{
	LLVMTypeRef *arg_type;
	LLVMTypeRef func_type;
	LLVMTypeRef ret_type;
	struct symbol *arg;
	int n_arg = 0;

	/* to avoid strangeness with varargs [for now], we build
	* the function and type anew, for each call.  This
	* is probably wrong.  We should look up the
	* symbol declaration info.
	*/

	ret_type = func_return_type(C, module, sym, sym_node);
	if (!ret_type)
		return NULL;

	/* count args, build argument type information */
	FOR_EACH_PTR(sym->arguments, arg) {
		n_arg++;
	} END_FOR_EACH_PTR(arg);

	arg_type = alloca(n_arg * sizeof(LLVMTypeRef));

	int idx = 0;
	FOR_EACH_PTR(sym->arguments, arg) {
		struct symbol *arg_sym = arg;

		arg_type[idx] = get_symnode_type(C, module, arg_sym);
		if (!arg_type[idx])
			return NULL;
		idx++;
	} END_FOR_EACH_PTR(arg);
	func_type = LLVMFunctionType(ret_type, arg_type, n_arg,
		sym->variadic);

	return func_type;
}

static LLVMTypeRef sym_array_type(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node)
{
	LLVMTypeRef elem_type;
	struct symbol *base_type;

	base_type = sym->ctype.base_type;
	/* empty struct is undefined [6.7.2.1(8)] */
	unsigned int array_bit_size = sym->bit_size;
	if (array_bit_size == 0 || (int)array_bit_size == -1) {
		if (sym_node != NULL)
			array_bit_size = sym_node->bit_size;
	}
	if (base_type->bit_size == 0 || base_type->bit_size == -1 || array_bit_size == 0 || (int)array_bit_size == -1) {
		fprintf(stderr, "array size can not be determined\n");
		return NULL;
	}
	elem_type = type_to_llvmtype(C, module, base_type, sym_node);
	if (!elem_type)
		return NULL;

	return LLVMArrayType(elem_type, array_bit_size / base_type->bit_size);
}

#define MAX_STRUCT_MEMBERS 256

static LLVMTypeRef sym_struct_type(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node)
{
	LLVMTypeRef elem_types[MAX_STRUCT_MEMBERS];
	struct symbol *member;
	char buffer[256];
	LLVMTypeRef ret;
	unsigned nr = 0;
	(void)sym_node;
	snprintf(buffer, sizeof(buffer), "struct.%s", sym->ident ? sym->ident->name : "anno");
	ret = LLVMStructCreateNamed(LLVMGetModuleContext(module), buffer);
	/* set ->aux to avoid recursion */
	sym->aux = ret;

	FOR_EACH_PTR(sym->symbol_list, member) {
		LLVMTypeRef member_type;

		if (nr >= MAX_STRUCT_MEMBERS) {
			// TODO error message
			return NULL;
		}

		member_type = get_symnode_type(C, module, member);
		if (!member_type)
			return NULL;

		elem_types[nr++] = member_type;
	} END_FOR_EACH_PTR(member);

	LLVMStructSetBody(ret, elem_types, nr, 0 /* packed? */);
	return ret;
}

static LLVMTypeRef sym_union_type(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node)
{
	LLVMTypeRef elem_types[1];
	char buffer[256];
	LLVMTypeRef type;
	(void)C; (void)sym_node;
	snprintf(buffer, sizeof(buffer), "union.%s", sym->ident ? sym->ident->name : "anno");
	type = LLVMStructCreateNamed(LLVMGetModuleContext(module), buffer);
	/* set ->aux to avoid recursion */
	sym->aux = type;

	unsigned union_size;

	if (sym->bit_size > 0 && sym->bit_size != -1) {
		/*
		* There's no union support in the LLVM API so we treat unions as
		* opaque structs. The downside is that we lose type information on the
		* members but as LLVM doesn't care, neither do we.
		*/
		union_size = sym->bit_size / 8;
		if (union_size > 0) {
			elem_types[0] = LLVMArrayType(LLVMInt8TypeInContext(LLVMGetModuleContext(module)), union_size);

			LLVMStructSetBody(type, elem_types, 1, 0 /* packed? */);
		}
	}
	return type;
}

static LLVMTypeRef sym_ptr_type(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node)
{
	LLVMTypeRef type;

	/* 'void *' is treated like 'char *' */
	if (dmrC_is_void_type(C->S, sym->ctype.base_type))
		type = LLVMInt8TypeInContext(LLVMGetModuleContext(module));
	else {
		type = type_to_llvmtype(C, module, sym->ctype.base_type, sym_node);
		if (!type)
			return NULL;
	}
	return LLVMPointerType(type, 0);
}

static LLVMTypeRef int_type_by_size(LLVMModuleRef module, int size)
{
	LLVMTypeRef ret = NULL;
	switch (size) {
	case 1:
		ret = LLVMInt1TypeInContext(LLVMGetModuleContext(module));
		break;
	case 8:
		ret = LLVMInt8TypeInContext(LLVMGetModuleContext(module));
		break;
	case 16:
		ret = LLVMInt16TypeInContext(LLVMGetModuleContext(module));
		break;
	case 32:
		ret = LLVMInt32TypeInContext(LLVMGetModuleContext(module));
		break;
	case 64:
		ret = LLVMInt64TypeInContext(LLVMGetModuleContext(module));
		break;
	default:
		fprintf(stderr, "invalid bit size %d\n", size);
	}
	return ret;
}

static LLVMTypeRef sym_basetype_type(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node)
{
	LLVMTypeRef ret = NULL;
	(void)sym_node;
	if (dmrC_is_float_type(C->S, sym)) {
		switch (sym->bit_size) {
		case 32:
			ret = LLVMFloatTypeInContext(LLVMGetModuleContext(module));
			break;
		case 64:
			ret = LLVMDoubleTypeInContext(LLVMGetModuleContext(module));
			break;
		case 80:
			ret = LLVMX86FP80TypeInContext(LLVMGetModuleContext(module));
			break;
		default:
			fprintf(stderr, "invalid bit size %d for type %d\n", sym->bit_size, sym->type);
			return NULL;
		}
	}
	else {
		if (sym->bit_size == -1)
			ret = LLVMVoidTypeInContext(LLVMGetModuleContext(module));
		else
			ret = int_type_by_size(module, sym->bit_size);
	}

	return ret;
}

static int is_aggregate_type(struct symbol *sym)
{
	if (sym->type == SYM_NODE)
		return is_aggregate_type(sym->ctype.base_type);
	switch (sym->type) {
	case SYM_UNION:
	case SYM_STRUCT:
	case SYM_ARRAY:
		return true;
	default:
		return false;
	}
}

static LLVMTypeRef type_to_llvmtype(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym, struct symbol *sym_node)
{
	LLVMTypeRef ret = NULL;

	assert(sym->type != SYM_NODE);
	assert(sym_node == NULL || sym_node->type == SYM_NODE);
	if (sym->aux)
		return sym->aux;

	switch (sym->type) {
	case SYM_BITFIELD:
		ret = LLVMIntTypeInContext(LLVMGetModuleContext(module), sym->bit_size);
		break;
	case SYM_RESTRICT:
	case SYM_ENUM:
		ret = type_to_llvmtype(C, module, sym->ctype.base_type, sym_node);
		break;
	case SYM_BASETYPE:
		ret = sym_basetype_type(C, module, sym, sym_node);
		break;
	case SYM_PTR:
		ret = sym_ptr_type(C, module, sym, sym_node);
		break;
	case SYM_UNION:
		ret = sym_union_type(C, module, sym, sym_node);
		break;
	case SYM_STRUCT:
		ret = sym_struct_type(C, module, sym, sym_node);
		break;
	case SYM_ARRAY:
		ret = sym_array_type(C, module, sym, sym_node);
		break;
	case SYM_FN:
		ret = sym_func_type(C, module, sym, sym_node);
		break;
	default:
		return NULL;
	}

	/* cache the result */
	sym->aux = ret;
	return ret;
}

static LLVMTypeRef insn_symbol_type(struct dmr_C *C, LLVMModuleRef module, struct instruction *insn)
{
	if (insn->type) {
		//printf("insn %s sym type %s\n", dmrC_show_instruction(C, insn), dmrC_get_type_name(insn->type->type));
		return get_symnode_or_basetype(C, module, insn->type);
	}
	LLVMTypeRef ret = int_type_by_size(module, insn->size);
	if (!ret) {
		dmrC_sparse_error(C, insn->pos, "invalid bit size %d", insn->size);
	}
	return ret;
}

static LLVMLinkage data_linkage(struct dmr_C *C, struct symbol *sym)
{
	(void)C;
	if (sym->ctype.modifiers & MOD_STATIC)
		return LLVMPrivateLinkage;
	return LLVMExternalLinkage;
}

static LLVMLinkage function_linkage(struct dmr_C *C, struct symbol *sym)
{
	(void)C;
	if (sym->ctype.modifiers & MOD_STATIC)
		return LLVMInternalLinkage;
	return LLVMExternalLinkage;
}

static LLVMValueRef build_cast(struct dmr_C *C, struct function *fn, LLVMValueRef val, LLVMTypeRef dtype, const char *target_name, int unsigned_cast)
{
	LLVMTypeRef valtype = LLVMTypeOf(val);
	LLVMTypeKind valkind = LLVMGetTypeKind(valtype);
	LLVMTypeKind dkind = LLVMGetTypeKind(dtype);
	LLVMOpcode op;
	(void)C;
	switch (dkind) {
	case LLVMIntegerTypeKind: {
		switch (valkind) {
		case LLVMPointerTypeKind:
			op = LLVMPtrToInt;
			break;
		case LLVMIntegerTypeKind: {
			unsigned val_width = LLVMGetIntTypeWidth(valtype);
			unsigned target_width = LLVMGetIntTypeWidth(dtype);
			if (target_width < val_width)
				op = LLVMTrunc;
			else if (target_width == val_width)
				op = LLVMBitCast;
			else
				op = unsigned_cast ? LLVMZExt : LLVMSExt;
			break;
		}
		case LLVMFloatTypeKind:
		case LLVMDoubleTypeKind:
			op = LLVMFPToSI;
			break;
		default:
			fprintf(stderr, "unsupported value type %s in cast to integer value\n", get_llvmtypekind_name(valkind));
			return NULL;
		}
		break;
	}
	case LLVMPointerTypeKind: {
		switch (valkind) {
		case LLVMPointerTypeKind:
			op = LLVMBitCast;
			break;
		case LLVMIntegerTypeKind:
			op = LLVMIntToPtr;
			break;
		default:
			fprintf(stderr, "unsupported value type %s in cast to ptr\n", get_llvmtypekind_name(valkind));
			return NULL;
		}
		break;
	}
	case LLVMFloatTypeKind:
	case LLVMDoubleTypeKind: {
		switch (valkind) {
		case LLVMIntegerTypeKind:
			op = unsigned_cast ? LLVMUIToFP : LLVMSIToFP;
			break;
		case LLVMFloatTypeKind: {
			if (dkind == LLVMFloatTypeKind)
				op = LLVMBitCast;
			else
				op = LLVMFPExt;
			break;
		}
		case LLVMDoubleTypeKind: {
			if (dkind == LLVMFloatTypeKind)
				op = LLVMFPTrunc;
			else
				op = LLVMBitCast;
			break;
		}
		default:
			fprintf(stderr, "unsupported value type %s in cast to floating point value\n", get_llvmtypekind_name(valkind));
			return NULL;
		}
		break;
	}
	default:
		if (dkind == valkind)
			op = LLVMBitCast;
		else {
			fprintf(stderr, "unsupported target type %s in cast from value kind %s\n", get_llvmtypekind_name(dkind), get_llvmtypekind_name(valkind));
			return NULL;
		}
	}
	return LLVMBuildCast(fn->builder, op, val, dtype, target_name);
}


#define MAX_PSEUDO_NAME 64

static const char * pseudo_name(struct dmr_C *C, pseudo_t pseudo, char *buf, size_t len)
{
	(void)C;
	buf[0] = '\0';
	switch (pseudo->type) {
	case PSEUDO_REG:
		snprintf(buf, len, "R%d", pseudo->nr);
		break;
	case PSEUDO_PHI:
		snprintf(buf, len, "PHI%d", pseudo->nr);
		break;
	case PSEUDO_SYM:
	case PSEUDO_VAL:
	case PSEUDO_ARG:
	case PSEUDO_VOID:
		break;
	default:
		assert(0);
	}

	return buf;
}

static LLVMValueRef build_local(struct dmr_C *C, struct function *fn, struct symbol *sym)
{
	const char *name = dmrC_show_ident(C, sym->ident);
	LLVMTypeRef type = get_symnode_type(C, fn->module, sym);
	LLVMValueRef result;
	char localname[256] = { 0 };
	snprintf(localname, sizeof localname, "%s_%p.", name, sym);
	if (dmrC_is_static(sym) || dmrC_is_extern(sym) || dmrC_is_toplevel(sym)) {
		result = LLVMGetNamedGlobal(fn->module, localname);
		if (!result) {
			result = LLVMAddGlobal(fn->module, type, localname);
			if (!dmrC_is_extern(sym))
				LLVMSetLinkage(result, LLVMInternalLinkage);
			else
				LLVMSetLinkage(result, LLVMExternalLinkage);
		}
	}
	else {
		/* insert alloca into entry block */
		/* LLVM requires allocas to be at the start */
		LLVMBasicBlockRef entrybbr = LLVMGetEntryBasicBlock(fn->fn);
		/* Use temporary Builder as we don't want to mess the function builder */
		LLVMBuilderRef tmp_builder = LLVMCreateBuilderInContext(LLVMGetModuleContext(fn->module));
		LLVMValueRef firstins = LLVMGetFirstInstruction(entrybbr);
		if (firstins)
			LLVMPositionBuilderBefore(tmp_builder, firstins);
		else
			LLVMPositionBuilderAtEnd(tmp_builder, entrybbr);
		/* Since multiple locals may have same name but in different scopes we
		append the symbol's address to make each variable unique */
		result = LLVMBuildAlloca(tmp_builder, type, localname);
		if (sym->initialized && is_aggregate_type(sym)) {
			LLVMValueRef memsetfunc = LLVMGetNamedFunction(fn->module, "llvm.memset.p0i8.i32");
			assert(memsetfunc);
			LLVMValueRef resulti8 = LLVMBuildBitCast(fn->builder, result, LLVMPointerType(LLVMInt8TypeInContext(LLVMGetModuleContext(fn->module)), 0), LLVMGetValueName(result));
			LLVMValueRef args[5];
			args[0] = resulti8;
			args[1] = LLVMConstInt(LLVMInt8TypeInContext(LLVMGetModuleContext(fn->module)), 0, 0);
			args[2] = LLVMConstInt(LLVMInt32TypeInContext(LLVMGetModuleContext(fn->module)), sym->bit_size / C->target->bits_in_char, 0);
			args[3] = LLVMConstInt(LLVMInt32TypeInContext(LLVMGetModuleContext(fn->module)), sym->ctype.alignment, 0);
			args[4] = LLVMConstInt(LLVMInt1TypeInContext(LLVMGetModuleContext(fn->module)), 0, 0);
			LLVMBuildCall(fn->builder, memsetfunc, args, 5, "");
		}
		LLVMDisposeBuilder(tmp_builder);
	}
	sym->priv = result;
	return result;
}

static LLVMValueRef get_sym_value(struct dmr_C *C, struct function *fn, pseudo_t pseudo)
{
	LLVMValueRef result = NULL;
	struct symbol *sym = pseudo->sym;
	struct expression *expr;

	result = (LLVMValueRef)sym->priv;
	if (result)
		return result;

	assert(sym->bb_target == NULL);

	expr = sym->initializer;
	if (expr &&
		(!sym->ident ||
		(sym->ident && (expr->type == EXPR_VALUE || expr->type == EXPR_FVALUE)))) {
		switch (expr->type) {
		case EXPR_STRING: {
			const char *s = expr->string->data;
			LLVMValueRef indices[] = { LLVMConstInt(LLVMInt64TypeInContext(LLVMGetModuleContext(fn->module)), 0, 0),
				LLVMConstInt(LLVMInt64TypeInContext(LLVMGetModuleContext(fn->module)), 0, 0) };
			LLVMValueRef data;

			data = LLVMAddGlobal(fn->module, LLVMArrayType(LLVMInt8TypeInContext(LLVMGetModuleContext(fn->module)), strlen(s) + 1), ".str");
			LLVMSetLinkage(data, LLVMPrivateLinkage);
			LLVMSetGlobalConstant(data, 1);
			char *scopy = dmrC_allocator_allocate(&C->byte_allocator, strlen(s) + 1);
			strcpy(scopy, s);
			LLVMSetInitializer(data, LLVMConstStringInContext(LLVMGetModuleContext(fn->module), scopy, strlen(scopy) + 1, true));

			result = LLVMConstGEP(data, indices, ARRAY_SIZE(indices));
			sym->priv = result;
			break;
		}
		case EXPR_SYMBOL: {
			dmrC_sparse_error(C, expr->pos, "unresolved symbol reference in initializer\n");
			dmrC_show_expression(C, expr);
			return NULL;
			break;
		}
		case EXPR_VALUE: {
			LLVMTypeRef symtype = get_symnode_type(C, fn->module, sym);
			if (symtype == NULL) {
				dmrC_sparse_error(C, expr->pos, "invalid symbol type\n");
				dmrC_show_expression(C, expr);
				return NULL;
			}
			result = build_local(C, fn, sym);
			if (!result)
				return result;
			LLVMValueRef value = constant_value(C, fn->module, expr->value, symtype);
			if (dmrC_is_static(sym))
				LLVMSetInitializer(result, value);
			else
				LLVMBuildStore(fn->builder, value, result);
			sym->priv = result;
			break;
		}
		case EXPR_FVALUE: {
			LLVMTypeRef symtype = get_symnode_type(C, fn->module, sym);
			if (symtype == NULL) {
				dmrC_sparse_error(C, expr->pos, "invalid symbol type\n");
				dmrC_show_expression(C, expr);
				return NULL;
			}
			result = build_local(C, fn, sym);
			if (!result)
				return result;
			if (dmrC_is_static(sym))
				LLVMSetInitializer(result, LLVMConstReal(symtype, expr->fvalue));
			else
				LLVMBuildStore(fn->builder, LLVMConstReal(symtype, expr->fvalue), result);
			sym->priv = result;
			break;
		}
		default:
			dmrC_sparse_error(C, expr->pos, "unsupported expr type in initializer: %d\n", expr->type);
			dmrC_show_expression(C, expr);
			return NULL;
		}
	}
	else {
		const char *name = dmrC_show_ident(C, sym->ident);
		LLVMTypeRef type = get_symnode_type(C, fn->module, sym);
		if (LLVMGetTypeKind(type) == LLVMFunctionTypeKind) {
			result = LLVMGetNamedFunction(fn->module, name);
			if (!result)
				result = LLVMAddFunction(fn->module, name, type);
			sym->priv = result;
		}
		else if (dmrC_is_extern(sym) || dmrC_is_toplevel(sym)) {
			result = LLVMGetNamedGlobal(fn->module, name);
			if (!result) {
				result = LLVMAddGlobal(fn->module, type, name);
				if (dmrC_is_extern(sym))
					LLVMSetLinkage(result, LLVMExternalLinkage);
			}
			sym->priv = result;
		}
		else {
			if (dmrC_is_static(sym) && sym->initializer) {
				dmrC_sparse_error(C, sym->initializer->pos, "unsupported initializer for local static variable\n");
				dmrC_show_expression(C, sym->initializer);
				return NULL;
			}
			result = build_local(C, fn, sym);
			if (!result)
				return result;
			if (dmrC_is_static(sym)) {
				LLVMSetInitializer(result, LLVMConstNull(type));
			}
			sym->priv = result;
		}
	}
	return result;
}

static LLVMValueRef constant_value(struct dmr_C *C, LLVMModuleRef module, unsigned long long val, LLVMTypeRef dtype)
{
	LLVMTypeRef itype;
	LLVMValueRef result;

	LLVMTypeKind kind = LLVMGetTypeKind(dtype);
	switch (kind) {
	case LLVMPointerTypeKind:
		itype = LLVMIntTypeInContext(LLVMGetModuleContext(module), C->target->bits_in_pointer);
		result = LLVMConstInt(itype, val, 0);
		result = LLVMConstIntToPtr(result, dtype);
		break;
	case LLVMIntegerTypeKind:
		result = LLVMConstInt(dtype, val, 0);
		break;
	case LLVMFloatTypeKind:
	case LLVMDoubleTypeKind:
		result = LLVMConstReal(dtype, (double)(long long)val);
		break;
	default:
		fprintf(stderr, "unsupported pseudo value kind %s\n", get_llvmtypekind_name(kind));
		return NULL;
	}
	return result;
}

static LLVMValueRef val_to_value(struct dmr_C *C, struct function *fn, unsigned long long val, struct symbol *ctype)
{
	LLVMTypeRef dtype;

	if (!ctype)
		return NULL;
	dtype = get_symnode_or_basetype(C, fn->module, ctype);
	if (!dtype)
		return NULL;
	return constant_value(C, fn->module, val, dtype);
}

static LLVMValueRef pseudo_to_value(struct dmr_C *C, struct function *fn, struct symbol *ctype, pseudo_t pseudo)
{
	LLVMValueRef result = NULL;

	switch (pseudo->type) {
	case PSEUDO_REG:
		result = pseudo->priv;
		break;
	case PSEUDO_SYM:
		result = get_sym_value(C, fn, pseudo);
		break;
	case PSEUDO_VAL:
		result = val_to_value(C, fn, pseudo->value, ctype);
		break;
	case PSEUDO_ARG:
		result = LLVMGetParam(fn->fn, pseudo->nr - 1);
		break;
	case PSEUDO_PHI:
		result = pseudo->priv;
		break;
	case PSEUDO_VOID:
		result = NULL;
		break;
	default:
		break;
	}
	if (!result) {
		fprintf(stderr, "error: no result for pseudo\n");
		return NULL;
	}
	return result;
}

static LLVMValueRef ptr_toint(struct dmr_C *C, struct function *fn, LLVMValueRef val)
{
	if (LLVMGetTypeKind(LLVMTypeOf(val)) == LLVMPointerTypeKind) {
		LLVMTypeRef dtype = LLVMIntTypeInContext(LLVMGetModuleContext(fn->module), C->target->bits_in_pointer);
		val = LLVMBuildPtrToInt(fn->builder, val, dtype, LLVMGetValueName(val));
	}
	return val;
}

#if 0

static LLVMValueRef calc_gep(struct dmr_C *C, struct function *fn, LLVMBuilderRef builder, LLVMValueRef base, LLVMValueRef off)
{
	LLVMTypeRef type = LLVMTypeOf(base);
	unsigned int as = LLVMGetPointerAddressSpace(type);
	LLVMTypeRef bytep = LLVMPointerType(LLVMInt8TypeInContext(LLVMGetModuleContext(fn->module)), as);
	LLVMValueRef addr;

	/* convert base to char* type */
	base = LLVMBuildPointerCast(builder, base, bytep, "");
	/* addr = base + off */
	addr = LLVMBuildInBoundsGEP(builder, base, &off, 1, "");
	/* convert back to the actual pointer type */
	addr = LLVMBuildPointerCast(builder, addr, type, "");
	return addr;
}

#endif

static LLVMRealPredicate translate_fop(int opcode)
{
	static const LLVMRealPredicate trans_tbl[] = {
		[OP_SET_EQ] = LLVMRealOEQ,
		[OP_SET_NE] = LLVMRealUNE,
		[OP_SET_LE] = LLVMRealOLE,
		[OP_SET_GE] = LLVMRealOGE,
		[OP_SET_LT] = LLVMRealOLT,
		[OP_SET_GT] = LLVMRealOGT,
		/* Are these used with FP? */
		[OP_SET_B] = LLVMRealOLT,
		[OP_SET_A] = LLVMRealOGT,
		[OP_SET_BE] = LLVMRealOLE,
		[OP_SET_AE] = LLVMRealOGE,
	};

	return trans_tbl[opcode];
}

static LLVMIntPredicate translate_op(int opcode)
{
	static const LLVMIntPredicate trans_tbl[] = {
		[OP_SET_EQ] = LLVMIntEQ,
		[OP_SET_NE] = LLVMIntNE,
		[OP_SET_LE] = LLVMIntSLE,
		[OP_SET_GE] = LLVMIntSGE,
		[OP_SET_LT] = LLVMIntSLT,
		[OP_SET_GT] = LLVMIntSGT,
		[OP_SET_B] = LLVMIntULT,
		[OP_SET_A] = LLVMIntUGT,
		[OP_SET_BE] = LLVMIntULE,
		[OP_SET_AE] = LLVMIntUGE,
	};

	return trans_tbl[opcode];
}

/**
* Convert the pseudo to a value, and cast it to the expected type of the
* instruction. If ptrtoint is true then convert pointer values to integers.
*/
static LLVMValueRef get_operand(struct dmr_C *C, struct function *fn, struct symbol *ctype, pseudo_t pseudo, bool ptrtoint, bool unsigned_cast)
{
	LLVMValueRef target;

	LLVMTypeRef instruction_type = get_symnode_or_basetype(C, fn->module, ctype);
	if (instruction_type == NULL)
		return NULL;
	target = pseudo_to_value(C, fn, ctype, pseudo);
	if (!target)
		return NULL;
	if (ptrtoint && dmrC_is_ptr_type(ctype))
		target = ptr_toint(C, fn, target);
	else
		target = build_cast(C, fn, target, instruction_type, LLVMGetValueName(target), unsigned_cast);
	return target;
}

static LLVMValueRef output_op_binary(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef lhs, rhs, target;
	char target_name[64];

	LLVMTypeRef instruction_type = get_symnode_or_basetype(C, fn->module, insn->type);

	lhs = get_operand(C, fn, insn->type, insn->src1, 1, 0);
	if (!lhs)
		return NULL;

	rhs = get_operand(C, fn, insn->type, insn->src2, 1, 0);
	if (!rhs)
		return NULL;

	pseudo_name(C, insn->target, target_name, sizeof target_name);

	switch (insn->opcode) {
		/* Binary */
	case OP_ADD:
		if (dmrC_is_float_type(C->S, insn->type))
			target = LLVMBuildFAdd(fn->builder, lhs, rhs, target_name);
		else
			target = LLVMBuildAdd(fn->builder, lhs, rhs, target_name);
		break;
	case OP_SUB:
		if (dmrC_is_float_type(C->S, insn->type))
			target = LLVMBuildFSub(fn->builder, lhs, rhs, target_name);
		else
			target = LLVMBuildSub(fn->builder, lhs, rhs, target_name);
		break;
	case OP_MULU:
		if (dmrC_is_float_type(C->S, insn->type))
			target = LLVMBuildFMul(fn->builder, lhs, rhs, target_name);
		else
			target = LLVMBuildMul(fn->builder, lhs, rhs, target_name);
		break;
	case OP_MULS:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildMul(fn->builder, lhs, rhs, target_name);
		break;
	case OP_DIVU:
		if (dmrC_is_float_type(C->S, insn->type))
			target = LLVMBuildFDiv(fn->builder, lhs, rhs, target_name);
		else
			target = LLVMBuildUDiv(fn->builder, lhs, rhs, target_name);
		break;
	case OP_DIVS:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildSDiv(fn->builder, lhs, rhs, target_name);
		break;
	case OP_MODU:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildURem(fn->builder, lhs, rhs, target_name);
		break;
	case OP_MODS:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildSRem(fn->builder, lhs, rhs, target_name);
		break;
	case OP_SHL:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildShl(fn->builder, lhs, rhs, target_name);
		break;
	case OP_LSR:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildLShr(fn->builder, lhs, rhs, target_name);
		break;
	case OP_ASR:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildAShr(fn->builder, lhs, rhs, target_name);
		break;

		/* Logical */
	case OP_AND:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildAnd(fn->builder, lhs, rhs, target_name);
		break;
	case OP_OR:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildOr(fn->builder, lhs, rhs, target_name);
		break;
	case OP_XOR:
		assert(!dmrC_is_float_type(C->S, insn->type));
		target = LLVMBuildXor(fn->builder, lhs, rhs, target_name);
		break;
	case OP_AND_BOOL: {
		LLVMValueRef lhs_nz, rhs_nz;
		LLVMTypeRef dst_type;

		lhs_nz = LLVMBuildIsNotNull(fn->builder, lhs, "");
		rhs_nz = LLVMBuildIsNotNull(fn->builder, rhs, "");
		target = LLVMBuildAnd(fn->builder, lhs_nz, rhs_nz, target_name);

		dst_type = insn_symbol_type(C, fn->module, insn);
		if (!dst_type)
			return NULL;
		target = LLVMBuildZExt(fn->builder, target, dst_type, target_name);
		break;
	}
	case OP_OR_BOOL: {
		LLVMValueRef lhs_nz, rhs_nz;
		LLVMTypeRef dst_type;

		lhs_nz = LLVMBuildIsNotNull(fn->builder, lhs, "");
		rhs_nz = LLVMBuildIsNotNull(fn->builder, rhs, "");
		target = LLVMBuildOr(fn->builder, lhs_nz, rhs_nz, target_name);

		dst_type = insn_symbol_type(C, fn->module, insn);
		if (!dst_type)
			return NULL;
		target = LLVMBuildZExt(fn->builder, target, dst_type, target_name);
		break;
	}

	default:
		assert(0);
		return NULL;
	}

	target = build_cast(C, fn, target, instruction_type, target_name, 0);
	insn->target->priv = target;

	return target;
}

static inline struct symbol *pseudo_type(struct dmr_C *C, pseudo_t pseudo)
{
	switch (pseudo->type) {
	case PSEUDO_SYM:
	case PSEUDO_ARG:
		return pseudo->sym;
	case PSEUDO_REG:
	case PSEUDO_PHI:
		return pseudo->def->type;
	case PSEUDO_VAL:
		return C->target->size_t_ctype;
	case PSEUDO_VOID:
	default:
		return &C->S->void_ctype;
	}
}

static LLVMValueRef output_op_compare(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef lhs, rhs, target;
	char target_name[64];

	if (insn->src1->type == PSEUDO_VAL)
		lhs = val_to_value(C, fn, insn->src1->value, pseudo_type(C, insn->src2));
	else
		lhs = pseudo_to_value(C, fn, insn->type, insn->src1);
	if (!lhs)
		return NULL;
	if (insn->src2->type == PSEUDO_VAL)
		rhs = val_to_value(C, fn, insn->src2->value, pseudo_type(C, insn->src1));
	else
		rhs = pseudo_to_value(C, fn, insn->type, insn->src2);
	if (!rhs)
		return NULL;

	pseudo_name(C, insn->target, target_name, sizeof target_name);

	LLVMTypeRef dst_type = insn_symbol_type(C, fn->module, insn);
	if (!dst_type)
		return NULL;

	switch (LLVMGetTypeKind(LLVMTypeOf(lhs))) {
	case LLVMPointerTypeKind: {
		lhs = LLVMBuildPtrToInt(fn->builder, lhs, LLVMIntTypeInContext(LLVMGetModuleContext(fn->module), C->target->bits_in_pointer), "");
		if (LLVMGetTypeKind(LLVMTypeOf(rhs)) == LLVMPointerTypeKind) {
			rhs = LLVMBuildPtrToInt(fn->builder, rhs, LLVMIntTypeInContext(LLVMGetModuleContext(fn->module), C->target->bits_in_pointer), "");
		}
		LLVMIntPredicate op = translate_op(insn->opcode);
		target = LLVMBuildICmp(fn->builder, op, lhs, rhs, target_name);
		break;
	}

	case LLVMIntegerTypeKind: {
		LLVMIntPredicate op = translate_op(insn->opcode);
		target = LLVMBuildICmp(fn->builder, op, lhs, rhs, target_name);
		break;
	}

	case LLVMHalfTypeKind:
	case LLVMFloatTypeKind:
	case LLVMDoubleTypeKind:
	case LLVMX86_FP80TypeKind:
	case LLVMFP128TypeKind:
	case LLVMPPC_FP128TypeKind: {
		LLVMRealPredicate op = translate_fop(insn->opcode);

		target = LLVMBuildFCmp(fn->builder, op, lhs, rhs, target_name);
		break;
	}
	default:
		assert(0);
		return NULL;
	}

	target = LLVMBuildZExt(fn->builder, target, dst_type, target_name);

	insn->target->priv = target;

	return target;
}

static LLVMValueRef output_op_ret(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	pseudo_t pseudo = insn->src;

	if (pseudo && pseudo != VOID_PSEUDO(C) && LLVMGetTypeKind(fn->return_type) != LLVMVoidTypeKind) {
		LLVMValueRef result = get_operand(C, fn, insn->type, pseudo, 0, 0);
		if (!result)
			return NULL;
		return LLVMBuildRet(fn->builder, result);
	}
	else
		return LLVMBuildRetVoid(fn->builder);
}

static LLVMValueRef calc_memop_addr(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMTypeRef int_type, addr_type;
	LLVMValueRef src, off, addr;
	unsigned int as;

	/* int type large enough to hold a pointer */
	int_type = LLVMIntTypeInContext(LLVMGetModuleContext(fn->module), C->target->bits_in_pointer);
	off = LLVMConstInt(int_type, (int)insn->offset, 0);

	/* convert src to the effective pointer type */
	src = pseudo_to_value(C, fn, insn->type, insn->src);
	if (!src)
		return NULL;
	as = LLVMGetPointerAddressSpace(LLVMTypeOf(src));
	LLVMTypeRef symtype = insn_symbol_type(C, fn->module, insn);
	if (!symtype)
		return NULL;
	addr_type = LLVMPointerType(symtype, as);
#if 1
	src = ptr_toint(C, fn, src);
	addr = LLVMBuildAdd(fn->builder, src, off, "");
	addr = LLVMBuildIntToPtr(fn->builder, addr, addr_type, "");
#else
	src = LLVMBuildPointerCast(fn->builder, src, addr_type, "");

	/* addr = src + off */
	addr = calc_gep(C, fn->builder, src, off);
#endif
	return addr;
}


static LLVMValueRef output_op_load(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef addr, target;
	char name[MAX_PSEUDO_NAME];

	addr = calc_memop_addr(C, fn, insn);
	if (!addr)
		return NULL;

	/* perform load */
	pseudo_name(C, insn->target, name, sizeof name);
	target = LLVMBuildLoad(fn->builder, addr, name);

	insn->target->priv = target;
	return target;
}

static LLVMValueRef output_op_store(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef addr, target_in;
	LLVMTypeRef desttype;

	if (is_aggregate_type(insn->type)) {
		dmrC_sparse_error(C, insn->pos, "store to aggregate type is not yet supported, failure at insn %s\n", dmrC_show_instruction(C, insn));
		return NULL;
	}

	addr = calc_memop_addr(C, fn, insn);
	if (!addr)
		return NULL;

	target_in = pseudo_to_value(C, fn, insn->type, insn->target);
	if (!target_in)
		return NULL;

	desttype = insn_symbol_type(C, fn->module, insn);
	if (!desttype)
		return NULL;

	/* Cast to the right type - to resolve issue with union types */
	target_in = build_cast(C, fn, target_in, desttype, LLVMGetValueName(target_in), 0);
	if (!target_in)
		return NULL;

	/* perform store */
	return LLVMBuildStore(fn->builder, target_in, addr);
}

static LLVMValueRef bool_value(struct dmr_C *C, struct function *fn, LLVMValueRef value)
{
	(void)C;
	LLVMTypeRef type = LLVMTypeOf(value);
	if (type != LLVMInt1TypeInContext(LLVMGetModuleContext(fn->module))) {
		LLVMTypeKind kind = LLVMGetTypeKind(type);
		switch (kind) {
		case LLVMPointerTypeKind:
		case LLVMIntegerTypeKind:
			value = LLVMBuildIsNotNull(fn->builder, value, "cond");
			break;
		case LLVMFloatTypeKind:
		case LLVMDoubleTypeKind:
			value = LLVMBuildFCmp(fn->builder, LLVMRealUNE, value, LLVMConstReal(type, 0.0), "cond");
			break;
		default:
			return NULL;
		}
	}
	return value;
}

static LLVMValueRef output_op_cbr(struct dmr_C *C, struct function *fn, struct instruction *br)
{
	// FIXME - NEW_SSA changes appear to result in cond being PSEUDO_VAL in some cases
	// This is a workaround for VALUE PSEUDO appearing in cond
	struct symbol *ctype = br->type;
	if (!ctype && br->cond->type == PSEUDO_VAL)
		ctype = &C->S->llong_ctype;
	LLVMValueRef cond = pseudo_to_value(C, fn, ctype, br->cond);
	if (cond)
		cond = bool_value(C, fn, cond);
	if (!cond) {
		dmrC_sparse_error(C, br->pos, "failure at insn %s\n", dmrC_show_instruction(C, br));
		return NULL;
	}
	return LLVMBuildCondBr(fn->builder, cond,
		br->bb_true->priv,
		br->bb_false->priv);
}

static LLVMValueRef output_op_br(struct dmr_C *C, struct function *fn, struct instruction *br)
{
	(void)C;
	return LLVMBuildBr(fn->builder, br->bb_true->priv);
}


static LLVMValueRef output_op_sel(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef target, src1, src2, src3;
	LLVMTypeRef desttype = insn_symbol_type(C, fn->module, insn);
	if (!desttype)
		return NULL;

	src1 = pseudo_to_value(C, fn, insn->type, insn->src1);
	if (!src1)
		return NULL;
	src1 = bool_value(C, fn, src1);
	if (!src1)
		return NULL;

	src2 = get_operand(C, fn, insn->type, insn->src2, 0, 0);
	if (!src2)
		return NULL;
	src3 = get_operand(C, fn, insn->type, insn->src3, 0, 0);
	if (!src3)
		return NULL;

	target = LLVMBuildSelect(fn->builder, src1, src2, src3, "select");

	insn->target->priv = target;
	return target;
}

static LLVMValueRef output_op_switch(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef sw_val, target;
	struct basic_block *def = NULL;
	struct multijmp *jmp;
	int n_jmp = 0;

	FOR_EACH_PTR(insn->multijmp_list, jmp) {
		if (jmp->begin <= jmp->end) {	/* case M..N */
			n_jmp += (jmp->end - jmp->begin) + 1;
		}
		else					/* default case */
			def = jmp->target;
	} END_FOR_EACH_PTR(jmp);

	sw_val = pseudo_to_value(C, fn, insn->type, insn->target);
	if (!sw_val)
		return NULL;
	target = LLVMBuildSwitch(fn->builder, sw_val,
		def ? def->priv : NULL, n_jmp);

	FOR_EACH_PTR(insn->multijmp_list, jmp) {
		long long val;
		for (val = jmp->begin; val <= jmp->end; val++) {
			LLVMValueRef value = LLVMConstInt(LLVMTypeOf(sw_val), val, 0);
			LLVMAddCase(target, value, jmp->target->priv);
		}
	} END_FOR_EACH_PTR(jmp);

	return target;
}

static struct symbol *get_function_basetype(struct symbol *type)
{
	if (type->type == SYM_PTR)
		type = type->ctype.base_type;
	assert(type->type == SYM_FN);
	return type;
}

static LLVMValueRef output_op_call(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef target, func;
	int n_arg = 0, i;
	struct pseudo *arg;
	LLVMValueRef *args;
	char name[64];

	n_arg = ptrlist_size((struct ptr_list *)insn->arguments);
	args = alloca(n_arg * sizeof(LLVMValueRef));
	struct symbol *ftype = get_function_basetype(insn->fntype);

	i = 0;
	FOR_EACH_PTR(insn->arguments, arg) {
		LLVMValueRef value;
		struct symbol *atype;

		atype = dmrC_get_nth_symbol(ftype->arguments, i);
		value = NULL;
		if (arg->type == PSEUDO_VAL) {
			/* Value pseudos do not have type information. */
			/* Use the function prototype to get the type. */
			if (atype)
				value = val_to_value(C, fn, arg->value, atype);
			else {
				LLVMTypeRef type = int_type_by_size(fn->module, arg->size);
				if (!type) {
					dmrC_sparse_error(C, insn->pos, "pseudo value argument[%d] = %lld has invalid size %d\n", i+1, arg->value, arg->size);
				}
				else {
					value = constant_value(C, fn->module, arg->value, type); 
				}
			}
		}
		else {
			value = pseudo_to_value(C, fn, atype, arg);
		}
		if (!value)
			return NULL;
		if (atype) {
			LLVMTypeRef argtype = get_symnode_type(C, fn->module, atype);
			if (!argtype)
				return NULL;
			value = build_cast(C, fn, value, argtype, LLVMGetValueName(value), 0);
			if (!value)
				return NULL;
		}
		args[i++] = value;
	} END_FOR_EACH_PTR(arg);

	func = pseudo_to_value(C, fn, insn->type, insn->func);
	if (!func)
		return NULL;
	pseudo_name(C, insn->target, name, sizeof name);
	LLVMTypeRef function_type = type_to_llvmtype(C, fn->module, ftype, NULL);
	if (!function_type)
		return NULL;
	LLVMTypeRef fptr_type = LLVMPointerType(function_type, 0);
	LLVMTypeRef bytep = LLVMPointerType(LLVMInt8TypeInContext(LLVMGetModuleContext(fn->module)), 0);

	target = LLVMBuildBitCast(fn->builder, func, bytep, name);
	target = LLVMBuildBitCast(fn->builder, target, fptr_type, name);
	target = LLVMBuildCall(fn->builder, target, args, n_arg, name);

	insn->target->priv = target;

	return target;
}

static LLVMValueRef output_op_phisrc(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef v;
	struct instruction *phi;

	assert(insn->target->priv == NULL);

	/* target = src */
	v = get_operand(C, fn, insn->type, insn->phi_src, 0, 0);
	if (!v)
		return NULL;

	FOR_EACH_PTR(insn->phi_users, phi) {
		LLVMValueRef load, ptr;

		assert(phi->opcode == OP_PHI);
		/* phi must be load from alloca */
		load = phi->target->priv;
		assert(load);
		if (!load)
			return NULL;
		assert(LLVMGetInstructionOpcode(load) == LLVMLoad);
		ptr = LLVMGetOperand(load, 0);
		/* store v to alloca */
		LLVMTypeRef phi_type = insn_symbol_type(C, fn->module, phi);
		if (!phi_type)
			return NULL;
		v = build_cast(C, fn, v, phi_type, "", 0);
		if (!v)
			return NULL;
		v = LLVMBuildStore(fn->builder, v, ptr);
	} END_FOR_EACH_PTR(phi);
	return v;
}

static LLVMValueRef output_op_phi(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef load = insn->target->priv;
	(void)C;
	assert(load);
	if (!load)
		return NULL;

	/* forward load */
	assert(LLVMGetInstructionOpcode(load) == LLVMLoad);
	/* forward load has no parent block */
	assert(!LLVMGetInstructionParent(load));
	/* finalize load in current block  */
	LLVMInsertIntoBuilder(fn->builder, load);
	return load;
}

static LLVMValueRef output_op_ptrcast(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef src, target;
	LLVMTypeRef dtype;
	struct symbol *otype = insn->orig_type;
	char target_name[64];

	assert(dmrC_is_ptr_type(insn->type));

	src = insn->src->priv;
	if (!src)
		src = get_operand(C, fn, otype, insn->src, 1, 0);
	if (!src)
		return NULL;

	pseudo_name(C, insn->target, target_name, sizeof target_name);

	dtype = insn_symbol_type(C, fn->module, insn);
	if (!dtype)
		return NULL;

	target = build_cast(C, fn, src, dtype, target_name, 0);
	insn->target->priv = target;

	return target;
}

static LLVMValueRef output_op_cast(struct dmr_C *C, struct function *fn, struct instruction *insn, LLVMOpcode op)
{
	LLVMValueRef src, target;
	LLVMTypeRef dtype;
	struct symbol *otype = insn->orig_type;
	char target_name[64];

	if (dmrC_is_ptr_type(insn->type)) {
		return output_op_ptrcast(C, fn, insn);
	}

	src = insn->src->priv;
	if (!src)
		src = pseudo_to_value(C, fn, insn->type, insn->src);
	if (dmrC_is_int_type(C->S, otype)) {
		LLVMTypeRef stype = get_symnode_or_basetype(C, fn->module, otype);
		src = build_cast(C, fn, src, stype, LLVMGetValueName(src), op == LLVMZExt);
	}
	if (!src)
		return NULL;

	pseudo_name(C, insn->target, target_name, sizeof target_name);

	assert(!dmrC_is_float_type(C->S, insn->type));

	dtype = insn_symbol_type(C, fn->module, insn);
	if (!dtype)
		return NULL;
	target = build_cast(C, fn, src, dtype, target_name, op == LLVMZExt);
	insn->target->priv = target;

	return target;
}

static LLVMValueRef output_op_fpcast(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef src, target;
	char target_name[64];
	LLVMTypeRef dtype;
	struct symbol *otype = insn->orig_type;

	src = insn->src->priv;
	if (!src)
		src = pseudo_to_value(C, fn, insn->type, insn->src);
	if (!src)
		return NULL;

	pseudo_name(C, insn->target, target_name, sizeof target_name);

	dtype = insn_symbol_type(C, fn->module, insn);
	if (!dtype)
		return NULL;

	target = build_cast(C, fn, src, dtype, target_name, !dmrC_is_signed_type(otype));
	insn->target->priv = target;

	return target;
}


static LLVMValueRef output_op_copy(struct dmr_C *C, struct function *fn, struct instruction *insn,
	pseudo_t pseudo)
{
	LLVMValueRef src, target;
	LLVMTypeRef const_type;
	char target_name[64];

	pseudo_name(C, insn->target, target_name, sizeof target_name);
	src = pseudo_to_value(C, fn, insn->type, pseudo);
	if (!src)
		return NULL;

	const_type = insn_symbol_type(C, fn->module, insn);
	if (!const_type)
		return NULL;

	/*
	* This is nothing more than 'target = src'
	*
	* TODO: find a better way to provide an identity function,
	* than using "X + 0" simply to produce a new LLVM pseudo
	*/

	if (dmrC_is_float_type(C->S, insn->type))
		target = LLVMBuildFAdd(fn->builder, src,
			LLVMConstReal(const_type, 0.0), target_name);
	else
		target = LLVMBuildAdd(fn->builder, src,
			LLVMConstInt(const_type, 0, 0), target_name);

	insn->target->priv = target;
	return target;
}

static LLVMValueRef output_op_setval(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	struct expression *expr = insn->val;
	char target_name[64];
	LLVMTypeRef const_type;
	LLVMValueRef target = NULL;

	if (!expr)
		return NULL;

	pseudo_name(C, insn->target, target_name, sizeof target_name);
	const_type = insn_symbol_type(C, fn->module, insn);
	if (!const_type)
		return NULL;

	switch (expr->type) {
	case EXPR_FVALUE:
		target = LLVMConstReal(const_type, expr->fvalue);
		break;
	case EXPR_LABEL:
		target = LLVMBlockAddress(fn->fn, expr->symbol->bb_target->priv);
		break;
	default:
		dmrC_sparse_error(C, insn->pos, "unsupported expression type %d in setval\n", expr->type);
		dmrC_show_expression(C, expr);
		return NULL;
	}

	insn->target->priv = target;

	return target;
}

static LLVMValueRef output_op_symaddr(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef res, src;
	LLVMTypeRef dtype;
	char name[64];

	src = pseudo_to_value(C, fn, insn->type, insn->symbol);
	if (!src)
		return NULL;

	dtype = get_symnode_or_basetype(C, fn->module, insn->type);
	if (!dtype)
		return NULL;

	pseudo_name(C, insn->target, name, sizeof name);
	res = LLVMBuildBitCast(fn->builder, src, dtype, name);
	insn->target->priv = res;

	return res;
}

static LLVMValueRef output_op_not(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef src, target;
	char target_name[64];

	src = pseudo_to_value(C, fn, insn->type, insn->src);
	if (!src)
		return NULL;

	pseudo_name(C, insn->target, target_name, sizeof target_name);

	target = LLVMBuildNot(fn->builder, src, target_name);

	insn->target->priv = target;

	return target;
}

static LLVMValueRef output_op_neg(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef src, target;
	char target_name[64];

	src = pseudo_to_value(C, fn, insn->type, insn->src);
	if (!src)
		return NULL;

	pseudo_name(C, insn->target, target_name, sizeof target_name);

	if (dmrC_is_float_type(C->S, insn->type))
		target = LLVMBuildFNeg(fn->builder, src, target_name);
	else
		target = LLVMBuildNeg(fn->builder, src, target_name);

	insn->target->priv = target;

	return target;
}

/* return 1 on success, 0 on failure */
static int output_insn(struct dmr_C *C, struct function *fn, struct instruction *insn)
{
	LLVMValueRef v = NULL;
	switch (insn->opcode) {
	case OP_RET:
		v = output_op_ret(C, fn, insn);
		break;
	case OP_CBR:
		v = output_op_cbr(C, fn, insn);
		break;
	case OP_BR:
		v = output_op_br(C, fn, insn);
		break;
	case OP_SYMADDR:
		v = output_op_symaddr(C, fn, insn);
		break;
	case OP_SETVAL:
		v = output_op_setval(C, fn, insn);
		break;
	case OP_SWITCH:
		v = output_op_switch(C, fn, insn);
		break;
	case OP_COMPUTEDGOTO:
		dmrC_sparse_error(C, insn->pos, "computed goto not yet supported\n");
		return 0;
	case OP_PHISOURCE:
		v = output_op_phisrc(C, fn, insn);
		break;
	case OP_PHI:
		v = output_op_phi(C, fn, insn);
		break;
	case OP_LOAD:
		v = output_op_load(C, fn, insn);
		break;
	case OP_LNOP:
		dmrC_sparse_error(C, insn->pos, "lnop not yet supported\n");
		return 0;
	case OP_STORE:
		v = output_op_store(C, fn, insn);
		break;
	case OP_SNOP:
		dmrC_sparse_error(C, insn->pos, "snop not yet supported\n");
		return 0;
	case OP_CALL:
		v = output_op_call(C, fn, insn);
		break;
	case OP_CAST:
		v = output_op_cast(C, fn, insn, LLVMZExt);
		break;
	case OP_SCAST:
		v = output_op_cast(C, fn, insn, LLVMSExt);
		break;
	case OP_FPCAST:
		v = output_op_fpcast(C, fn, insn);
		break;
	case OP_PTRCAST:
		v = output_op_ptrcast(C, fn, insn);
		break;
	case OP_ADD:
	case OP_SUB:
	case OP_MULU:
	case OP_MULS:
	case OP_DIVU:
	case OP_DIVS:
	case OP_MODU:
	case OP_MODS:
	case OP_SHL:
	case OP_LSR:
	case OP_ASR:
	case OP_AND:
	case OP_OR:
	case OP_XOR:
	case OP_AND_BOOL:
	case OP_OR_BOOL:
		v = output_op_binary(C, fn, insn);
		break;
	case OP_SET_EQ:
	case OP_SET_NE:
	case OP_SET_LE:
	case OP_SET_GE:
	case OP_SET_LT:
	case OP_SET_GT:
	case OP_SET_B:
	case OP_SET_A:
	case OP_SET_BE:
	case OP_SET_AE:
		v = output_op_compare(C, fn, insn);
		break;
	case OP_SEL:
		v = output_op_sel(C, fn, insn);
		break;
	case OP_SLICE:
		dmrC_sparse_error(C, insn->pos, "slice not yet supported\n");
		return 0;
	case OP_NOT:
		v = output_op_not(C, fn, insn);
		break;
	case OP_NEG:
		v = output_op_neg(C, fn, insn);
		break;
	case OP_CONTEXT:
		dmrC_sparse_error(C, insn->pos, "context not yet supported\n");
		return 0;
	case OP_RANGE:
		dmrC_sparse_error(C, insn->pos, "range not yet supported\n");
		return 0;
	case OP_NOP:
		dmrC_sparse_error(C, insn->pos, "nop not yet supported\n");
		return 0;
	case OP_DEATHNOTE:
		return 1;
	case OP_ASM:
		dmrC_sparse_error(C, insn->pos, "asm not yet supported\n");
		return 0;
	case OP_COPY:
		v = output_op_copy(C, fn, insn, insn->src);
		break;
	default:
		return 1;
	}

	if (v == NULL)
		dmrC_sparse_error(C, insn->pos, "failed to output instruction %s\n", dmrC_show_instruction(C, insn));
	return v != NULL;
}

/* return 1 on success, 0 on failure */
static int output_bb(struct dmr_C *C, struct function *fn, struct basic_block *bb)
{
	struct instruction *insn;

	FOR_EACH_PTR(bb->insns, insn) {
		if (!insn->bb)
			continue;

		if (!output_insn(C, fn, insn)) {
			dmrC_sparse_error(C, insn->pos, "failed to output %s\n", dmrC_show_instruction(C, insn));
			return 0;
		}
	}
	END_FOR_EACH_PTR(insn);
	return 1;
}

#define MAX_ARGS	64

static LLVMValueRef output_fn(struct dmr_C *C, LLVMModuleRef module, struct entrypoint *ep)
{
	struct symbol *sym = ep->name;
	struct symbol *base_type = sym->ctype.base_type;
	struct symbol *ret_type = sym->ctype.base_type->ctype.base_type;
	LLVMTypeRef arg_types[MAX_ARGS];
	LLVMTypeRef return_type;
	struct function function = { .module = module };
	struct basic_block *bb;
	struct symbol *arg;
	const char *name;
	int nr_args = 0;

	FOR_EACH_PTR(base_type->arguments, arg) {
		if (nr_args >= MAX_ARGS)
			return NULL;
		arg_types[nr_args] = get_symnode_type(C, module, arg);
		if (!arg_types[nr_args])
			return NULL;
		nr_args++;
	} END_FOR_EACH_PTR(arg);

	name = dmrC_show_ident(C, sym->ident);

	return_type = type_to_llvmtype(C, module, ret_type, NULL);
	if (!return_type)
		return NULL;

	function.return_type = return_type;
	function.fn = LLVMGetNamedFunction(module, name);
	if (!function.fn) {
		function.type = LLVMFunctionType(return_type, arg_types, nr_args, base_type->variadic);
		function.fn = LLVMAddFunction(module, name, function.type);
		LLVMSetLinkage(function.fn, function_linkage(C, sym));
		sym->priv = function.fn;
	}
	else {
		function.type = LLVMTypeOf(function.fn);
	}

	LLVMSetFunctionCallConv(function.fn, LLVMCCallConv);

	function.builder = LLVMCreateBuilderInContext(LLVMGetModuleContext(module));

	/* give a name to each argument */
	for (int i = 0; i < nr_args; i++) {
		char name[MAX_PSEUDO_NAME];
		LLVMValueRef arg;

		arg = LLVMGetParam(function.fn, i);
		snprintf(name, sizeof name, "ARG%d", i + 1);
		LLVMSetValueName(arg, name);
	}

	/* create the BBs */
	FOR_EACH_PTR(ep->bbs, bb) {
		static int nr_bb;
		LLVMBasicBlockRef bbr;
		char bbname[32];
		struct instruction *insn;

		sprintf(bbname, "L%d", nr_bb++);
		bbr = LLVMAppendBasicBlockInContext(LLVMGetModuleContext(module), function.fn, bbname);

		bb->priv = bbr;

		/* allocate alloca for each phi */
		FOR_EACH_PTR(bb->insns, insn) {
			LLVMBasicBlockRef entrybbr;
			LLVMTypeRef phi_type;
			LLVMValueRef ptr;

			if (!insn->bb || insn->opcode != OP_PHI)
				continue;
			/* insert alloca into entry block */
			entrybbr = LLVMGetEntryBasicBlock(function.fn);
			LLVMPositionBuilderAtEnd(function.builder, entrybbr);
			phi_type = insn_symbol_type(C, module, insn);
			if (!phi_type) {
				LLVMDisposeBuilder(function.builder);
				return NULL;
			}
			ptr = LLVMBuildAlloca(function.builder, phi_type, "");
			/* emit forward load for phi */
			LLVMClearInsertionPosition(function.builder);
			insn->target->priv = LLVMBuildLoad(function.builder, ptr, "phi");
		} END_FOR_EACH_PTR(insn);
	}
	END_FOR_EACH_PTR(bb);

	FOR_EACH_PTR(ep->bbs, bb) {
		LLVMPositionBuilderAtEnd(function.builder, bb->priv);

		if (!output_bb(C, &function, bb)) {
			LLVMDisposeBuilder(function.builder);
			return NULL;
		}
	}
	END_FOR_EACH_PTR(bb);

	LLVMDisposeBuilder(function.builder);

	return function.fn;
}

/* returns NULL on failure */
static LLVMValueRef output_data(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym)
{
	struct expression *initializer = sym->initializer;
	LLVMValueRef initial_value = NULL;
	LLVMValueRef data = NULL;
	const char *name;

	if (initializer) {
		switch (initializer->type) {
		case EXPR_VALUE:
			initial_value = constant_value(C, module, initializer->value, get_symnode_type(C, module, sym));
			break;
		case EXPR_FVALUE:
			initial_value = LLVMConstReal(get_symnode_type(C, module, sym), initializer->fvalue);
			break;
		case EXPR_SYMBOL: {
			struct symbol *sym = initializer->symbol;
			if (sym->ident)
				initial_value = LLVMGetNamedGlobal(module, dmrC_show_ident(C, sym->ident));
			if (!initial_value)
				initial_value = output_data(C, module, sym);
			break;
		}
		case EXPR_STRING: {
			const char *s = initializer->string->data;
			char *scopy = dmrC_allocator_allocate(&C->byte_allocator, strlen(s) + 1);
			strcpy(scopy, s);
			initial_value = LLVMConstStringInContext(LLVMGetModuleContext(module), scopy, strlen(scopy) + 1, true);
			break;
		}
		default:
			dmrC_sparse_error(C, initializer->pos, "unsupported expr type in global data initializer: %d\n", initializer->type);
			dmrC_show_expression(C, initializer);
		}
		if (!initial_value)
			return NULL;
	}
	else {
		LLVMTypeRef type = get_symnode_type(C, module, sym);
		if (type == NULL)
			return NULL;
		initial_value = LLVMConstNull(type);
	}

	name = dmrC_show_ident(C, sym->ident);

	if (!sym->priv) {
		if (sym->ident)
			data = LLVMGetNamedGlobal(module, name);
		if (!data)
			data = LLVMAddGlobal(module, LLVMTypeOf(initial_value), name);

		LLVMSetLinkage(data, data_linkage(C, sym));
		if (sym->ctype.modifiers & MOD_CONST)
			LLVMSetGlobalConstant(data, 1);
		if (sym->ctype.modifiers & MOD_TLS)
			LLVMSetThreadLocal(data, 1);
		if (sym->ctype.alignment)
			LLVMSetAlignment(data, sym->ctype.alignment);

		if (!(sym->ctype.modifiers & MOD_EXTERN))
			LLVMSetInitializer(data, initial_value);

		sym->priv = data;
	}
	else {
		data = sym->priv;
		if (!(sym->ctype.modifiers & MOD_EXTERN))
			LLVMSetInitializer(data, initial_value);
	}
	return data;
}

static LLVMValueRef output_prototype(struct dmr_C *C, LLVMModuleRef module, struct symbol *sym)
{
	const char *name = dmrC_show_ident(C, sym->ident);
	struct symbol *base_type = sym;
	if (sym->type == SYM_NODE)
		base_type = sym->ctype.base_type;
	LLVMTypeRef ftype = sym_func_type(C, module, base_type, sym->type == SYM_NODE ? sym : NULL);
	if (!ftype)
		return NULL;
	LLVMValueRef result = LLVMGetNamedFunction(module, name);
	if (!result) {
		result = LLVMAddFunction(module, name, ftype);
		LLVMSetLinkage(result, function_linkage(C, sym));
		//LLVMSetFunctionCallConv(result, LLVMCCallConv);
	}
	sym->priv = result;
	return result;
}

/* returns 1 on success, 0 on failure */
static int compile(struct dmr_C *C, LLVMModuleRef module, struct symbol_list *list)
{
	struct symbol *sym;

	FOR_EACH_PTR(list, sym) {
		struct entrypoint *ep;
		dmrC_expand_symbol(C, sym);

		if (dmrC_is_prototype(sym)) {
			if (!output_prototype(C, module, sym))
				return 0;
			continue;
		}

		ep = dmrC_linearize_symbol(C, sym);
		LLVMValueRef result = NULL;
		if (ep)
			result = output_fn(C, module, ep);
		else
			result = output_data(C, module, sym);
		if (!result)
			return 0;
	}
	END_FOR_EACH_PTR(sym);

	return 1;
}

#if 0

#ifndef LLVM_DEFAULT_TARGET_TRIPLE
#define LLVM_DEFAULT_TARGET_TRIPLE LLVM_HOSTTRIPLE
#endif

#define X86_LINUX_LAYOUT \
	"e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-" \
	"i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-" \
	"a0:0:64-f80:32:32-n8:16:32-S128"

#define X86_64_LINUX_LAYOUT \
	"e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-" \
	"i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-" \
	"a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"

static void set_target(struct dmr_C *C, LLVMModuleRef module)
{
	char target[] = LLVM_DEFAULT_TARGET_TRIPLE;
	const char *arch, *vendor, *os, *env, *layout = NULL;
	char triple[256];

	arch = strtok(target, "-");
	vendor = strtok(NULL, "-");
	os = strtok(NULL, "-");
	env = strtok(NULL, "-");

	if (!os)
		return;
	if (!env)
		env = "unknown";

	if (!strcmp(arch, "x86_64") && !strcmp(os, "linux")) {
		if (C->arch_m64) {
			layout = X86_64_LINUX_LAYOUT;
		}
		else {
			arch = "i386";
			layout = X86_LINUX_LAYOUT;
		}
	}

	/* unsupported target */
	if (!layout)
		return;

	snprintf(triple, sizeof(triple), "%s-%s-%s-%s", arch, vendor, os, env);
	// FIXME - what do we do here?
	//LLVMSetTarget(module, triple);
	//LLVMSetDataLayout(module, layout);
}

#endif

static void add_intrinsics(LLVMModuleRef module)
{
	LLVMTypeRef param_types[] = { LLVMPointerType(LLVMInt8TypeInContext(LLVMGetModuleContext(module)), 0),
		LLVMInt8TypeInContext(LLVMGetModuleContext(module)),
		LLVMInt32TypeInContext(LLVMGetModuleContext(module)),
		LLVMInt32TypeInContext(LLVMGetModuleContext(module)),
		LLVMInt1TypeInContext(LLVMGetModuleContext(module)) };
	LLVMTypeRef fn_type = LLVMFunctionType(LLVMVoidTypeInContext(LLVMGetModuleContext(module)), param_types, 5, false);
	LLVMAddFunction(module, "llvm.memset.p0i8.i32", fn_type);
}

bool dmrC_llvmcompile(int argc, char **argv, LLVMModuleRef module,
	const char *inputbuffer)
{
	struct string_list *filelist = NULL;
	struct symbol_list *symlist;
	char *file;

	struct dmr_C *C = new_dmr_C();
	C->codegen = 1; /* disables macros related to vararg processing */

	symlist = dmrC_sparse_initialize(C, argc, argv, &filelist);
	// set_target(C, module);
	add_intrinsics(module);

	int rc = 0; /* 0 means OK, non-zero means error */
	if (!setjmp(C->jmpbuf)) {
		if (compile(C, module, symlist)) {
			/* need ->phi_users */
			/* This flag enables call to dmrC_track_pseudo_death() in
			linearize.c which sets
			phi_users list on PHISOURCE instructions  */
			C->dbg_dead = 1;
			FOR_EACH_PTR(filelist, file)
			{
				symlist = dmrC_sparse(C, file);
				if (C->die_if_error || !symlist) {
					rc = 1;
					break;
				}
				if (!compile(C, module, symlist)) {
					rc = 1;
					break;
				}
			}
			END_FOR_EACH_PTR(file);

			if (inputbuffer && rc == 0) {
				char *buffer = strdup(inputbuffer);
				if (!buffer)
					rc = 1;
				else {
					symlist = dmrC_sparse_buffer(C, "buffer", buffer, 0);
					free(buffer);
					if (C->die_if_error || !symlist) {
						rc = 1;
					}
					else if (!compile(C, module, symlist)) {
						rc = 1;
					}
				}
			}
		}
		else
			rc = 1;
	}
	else {
		rc = 1;
	}
	char *error_message = NULL;
	int dump_module = 0;
	if (rc == 1) {
		fprintf(stderr, "Failed to compile given inputs\n");
	}
	if (rc == 0 &&
		LLVMVerifyModule(module, LLVMPrintMessageAction, &error_message)) {
		dump_module = 1;
		rc = 1;
	}
	if (error_message) {
		if (strlen(error_message) > 0)
			fprintf(stderr, "%s\n\n", error_message);
		LLVMDisposeMessage(error_message);
	}
	if (rc == 0) {
		const char *filename = "out.bc";
		if (C->output_file_name[0])
			filename = C->output_file_name;
		LLVMWriteBitcodeToFile(module, filename);
	}
	else {
		if (dump_module)
			/* we only dump the LLVM module if verification fails */
			LLVMDumpModule(module);
	}
	destroy_dmr_C(C);

	return rc == 0;
}
