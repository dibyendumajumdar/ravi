#define LUA_CORE

#include "lprefix.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ravi_ast.h"

#include "lauxlib.h"

#define MAXVARS		125 


// README
// THIS IS NOT WORKING - IT IS WORK IN PROGRESS
// Set test_ast.c for a way to exercise this.
// Also we enable an API raviload() to get to the entry point here

// We need a simple bump allocator
// Following is based on code from Linus Torvalds sparse project

/*
* Our "blob" allocator works on chunks that are multiples
* of this size (the underlying allocator may be a mmap that
* cannot handle smaller chunks, for example, so trying to
* allocate blobs that aren't aligned is not going to work).
*/
#define CHUNK 32768

static size_t alignment = sizeof(double);

struct allocation_blob {
  struct allocation_blob *next;
  size_t left, offset;
  unsigned char data[];
};

struct allocator {
	const char *name_;
	struct allocation_blob *blobs_;
	size_t size_;
	unsigned int alignment_;
	unsigned int chunking_;
	void *freelist_;
	size_t allocations, total_bytes, useful_bytes;
};

/*
 * Userdata object to hold the abstract syntax tree
 */
struct ast_container {
	struct allocator *ast_node_allocator;
	struct allocator *string_allocator;
	struct ast_node *root;
};

enum ast_node_type {

};

struct ast_node {
	enum ast_node_type type;
};

static void *blob_alloc(size_t size) {
  void *ptr;
  ptr = malloc(size);
  if (ptr != NULL) memset(ptr, 0, size);
  return ptr;
}

static void blob_free(void *addr, size_t size) {
  (void)size;
  free(addr);
}

static void allocator_init(struct allocator *A, const char *name, size_t size,
	unsigned int alignment, unsigned int chunking) {
	A->name_ = name;
	A->blobs_ = NULL;
	A->size_ = size;
	A->alignment_ = alignment;
	A->chunking_ = chunking;
	A->freelist_ = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
}

static void drop_all_allocations(struct allocator *A) {
	struct allocation_blob *blob = A->blobs_;
	A->blobs_ = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
	A->freelist_ = NULL;
	while (blob) {
		struct allocation_blob *next = blob->next;
		dmrC_blob_free(blob, A->chunking_);
		blob = next;
	}
}

static void *allocator_allocate(struct allocator *A, size_t extra) {
	size_t size = extra + A->size_;
	size_t alignment = A->alignment_;
	struct allocation_blob *blob = A->blobs_;
	void *retval;

	assert(size <= A->chunking_);
	/*
	* NOTE! The freelist only works with things that are
	*  (a) sufficiently aligned
	*  (b) use a constant size
	* Don't try to free allocators that don't follow
	* these rules.
	*/
	if (A->freelist_) {
		void **p = (void **)A->freelist_;
		retval = p;
		A->freelist_ = *p;
		memset(retval, 0, size);
		return retval;
	}

	A->allocations++;
	A->useful_bytes += size;
	size = (size + alignment - 1) & ~(alignment - 1);
	if (!blob || blob->left < size) {
		size_t offset, chunking = A->chunking_;
		struct allocation_blob *newblob =
			(struct allocation_blob *)blob_alloc(chunking);
		if (!newblob) {
			fprintf(stderr, "out of memory\n");
			abort();
		}
		A->total_bytes += chunking;
		newblob->next = blob;
		blob = newblob;
		A->blobs_ = newblob;
		offset = offsetof(struct allocation_blob, data);
		offset = (offset + alignment - 1) & ~(alignment - 1);
		blob->left = chunking - offset;
		blob->offset = offset - offsetof(struct allocation_blob, data);
	}
	retval = blob->data + blob->offset;
	blob->offset += size;
	blob->left -= size;
	return retval;
}

static void allocator_free(struct allocator *A, void *entry) {
	void **p = (void **)entry;
	*p = A->freelist_;
	A->freelist_ = p;
}

static void show_allocations(struct allocator *x) {
  fprintf(stderr,
          "allocator: %d allocations, %d bytes (%d total bytes, "
          "%6.2f%% usage, %6.2f average size)\n",
          (int)x->allocations, (int)x->useful_bytes, (int)x->total_bytes,
          100 * (double)x->useful_bytes / x->total_bytes,
          (double)x->useful_bytes / x->allocations);
}

static void allocator_destroy(struct allocator *A) {
	allocator_drop_all_allocations(A);
	A->blobs_ = NULL;
	A->allocations = 0;
	A->total_bytes = 0;
	A->useful_bytes = 0;
	A->freelist_ = NULL;
}

static const char *AST_type = "Ravi.AST";

#define test_Ravi_AST(L, idx) \
  ((struct ast_container *)raviL_testudata(L, idx, AST_type))
#define check_Ravi_AST(L, idx) \
  ((struct ast_container *)raviL_checkudata(L, idx, AST_type))

static struct ast_container * new_ast_container(lua_State *L) {
	struct ast_container *container =
		(struct ast_container *)lua_newuserdata(L, sizeof(struct ast_container));
	raviL_getmetatable(L, AST_type);
	lua_setmetatable(L, -2);
	return container;
}

/* __gc function for IRBuilderHolder */
static int collect_ast_container(lua_State *L) {
	struct ast_container *container = check_Ravi_AST(L, 1);
	return 0;
}

static void parse_expression(LexState *ls);
static void parse_statement_list(LexState *ls);
static void parse_statement(LexState *ls);

static l_noret error_expected(LexState *ls, int token) {
    luaX_syntaxerror(ls,
        luaO_pushfstring(ls->L, "%s expected", luaX_token2str(ls, token)));
}

static int testnext(LexState *ls, int c) {
    if (ls->t.token == c) {
        luaX_next(ls);
        return 1;
    }
    else return 0;
}

static void check(LexState *ls, int c) {
    if (ls->t.token != c)
        error_expected(ls, c);
}

static void checknext(LexState *ls, int c) {
    check(ls, c);
    luaX_next(ls);
}


/*============================================================*/
/* GRAMMAR RULES */
/*============================================================*/


/*
** check whether current token is in the follow set of a block.
** 'until' closes syntactical blocks, but do not close scope,
** so it is handled in separate.
*/
static int block_follow(LexState *ls, int withuntil) {
    switch (ls->t.token) {
    case TK_ELSE: case TK_ELSEIF:
    case TK_END: case TK_EOS:
        return 1;
    case TK_UNTIL: return withuntil;
    default: return 0;
    }
}

static void enterlevel(LexState *ls) {
    lua_State *L = ls->L;
    ++L->nCcalls;
    // checklimit(ls->fs, L->nCcalls, LUAI_MAXCCALLS, "C levels");
}

static inline void leavelevel(LexState *ls) {
    ls->L->nCcalls--;
}


static void check_match(LexState *ls, int what, int who, int where) {
    if (!testnext(ls, what)) {
        if (where == ls->linenumber)
            error_expected(ls, what);
        else {
            luaX_syntaxerror(ls, luaO_pushfstring(ls->L,
                "%s expected (to close %s at line %d)",
                luaX_token2str(ls, what), luaX_token2str(ls, who), where));
        }
    }
}

/* Check that current token is a name, and advance */
static TString *check_name_and_next(LexState *ls) {
    TString *ts;
    check(ls, TK_NAME);
    ts = ls->t.seminfo.ts;
    luaX_next(ls);
    return ts;
}


/* create a new local variable in function scope, and set the
* variable type (RAVI - added type tt) */
static void new_localvar(LexState *ls, TString *name) {
}

/* create a new local variable
*/
static void new_localvarliteral_(LexState *ls, const char *name, size_t sz) {
    new_localvar(ls, luaX_newstring(ls, name, sz));
}

/* create a new local variable
*/
#define new_localvarliteral(ls,name) \
	new_localvarliteral_(ls, "" name, (sizeof(name)/sizeof(char))-1)

/* moves the active variable watermark (nactvar) to cover the
* local variables in the current declaration. Also
* sets the starting code location (set to current instruction)
* for nvars new local variables
*/
static void adjustlocalvars(LexState *ls, int nvars) {
}

/* intialize var with the variable name - may be local,
* global or upvalue - note that var->k will be set to
* VLOCAL (local var), or VINDEXED or VUPVAL? TODO check
*/
static void singlevar(LexState *ls) {
    TString *varname = check_name_and_next(ls);
}

static void adjust_assign(LexState *ls, int nvars, int nexps, expdesc *e) {
    FuncState *fs = ls->fs;
    int extra = nvars - nexps;
    if (hasmultret(e->k)) {
        extra++;  /* includes call itself */
        if (extra < 0) extra = 0;
        /* following adjusts the C operand in the OP_CALL instruction */
        luaK_setreturns(fs, e, extra);  /* last exp. provides the difference */
        if (extra > 1) luaK_reserveregs(fs, extra - 1);
    }
    else {
        if (e->k != VVOID) luaK_exp2nextreg(fs, e);  /* close last expression */
        if (extra > 0) {
            int reg = fs->freereg;
            luaK_reserveregs(fs, extra);
            /* RAVI TODO for typed variables we should not set to nil? */
            luaK_nil(fs, reg, extra);
        }
    }
    if (nexps > nvars)
        ls->fs->freereg -= nexps - nvars;  /* remove extra values */
}

/*
** create a label named 'break' to resolve break statements
*/
static void breaklabel(LexState *ls) {
}

/*
** generates an error for an undefined 'goto'; choose appropriate
** message when label name is a reserved word (which can only be 'break')
*/
static l_noret undefgoto(LexState *ls, Labeldesc *gt) {
    const char *msg = isreserved(gt->name)
        ? "<%s> at line %d not inside a loop"
        : "no visible label '%s' for <goto> at line %d";
    msg = luaO_pushfstring(ls->L, msg, getstr(gt->name), gt->line);
    semerror(ls, msg);
}


/*============================================================*/
/* GRAMMAR RULES */
/*============================================================*/

static void parse_field_selector(LexState *ls) {
    /* fieldsel -> ['.' | ':'] NAME */
    luaX_next(ls);  /* skip the dot or colon */
    check_name_and_next(ls);
}


static void parse_yindex(LexState *ls) {
    /* index -> '[' expr ']' */
    luaX_next(ls);  /* skip the '[' */
    parse_expression(ls);
    checknext(ls, ']');
}


/*
** {======================================================================
** Rules for Constructors
** =======================================================================
*/


static void parse_recfield(LexState *ls) {
    /* recfield -> (NAME | '['exp1']') = exp1 */
    if (ls->t.token == TK_NAME) {
        check_name_and_next(ls);
    }
    else  /* ls->t.token == '[' */
        parse_yindex(ls);
    checknext(ls, '=');
    parse_expression(ls);
}



static void parse_listfield(LexState *ls) {
    /* listfield -> exp */
    parse_expression(ls);
}


static void parse_field(LexState *ls) {
    /* field -> listfield | recfield */
    switch (ls->t.token) {
    case TK_NAME: {  /* may be 'listfield' or 'recfield' */
        if (luaX_lookahead(ls) != '=')  /* expression? */
            parse_listfield(ls);
        else
            parse_recfield(ls);
        break;
    }
    case '[': {
        parse_recfield(ls);
        break;
    }
    default: {
        parse_listfield(ls);
        break;
    }
    }
}


static void parse_table_constructor(LexState *ls) {
    /* constructor -> '{' [ field { sep field } [sep] ] '}'
    sep -> ',' | ';' */
    int line = ls->linenumber;
    checknext(ls, '{');
    do {
        if (ls->t.token == '}') break;
        parse_field(ls);
    } while (testnext(ls, ',') || testnext(ls, ';'));
    check_match(ls, '}', '{', line);
}

/* }====================================================================== */

/* RAVI Parse
*   name : type
*   where type is 'integer', 'integer[]',
*                 'number', 'number[]'
*/
static void declare_local_variable(LexState *ls) {
    TString *name = check_name_and_next(ls);
    if (testnext(ls, ':')) {
        TString *typename = check_name_and_next(ls); /* we expect a type name */
        const char *str = getstr(typename);
        /* following is not very nice but easy as
        * the lexer doesn't need to be changed
        */
        if (strcmp(str, "integer") == 0)
            ;
        else if (strcmp(str, "number") == 0)
            ;
        else if (strcmp(str, "closure") == 0)
            ;
        else if (strcmp(str, "userdata") == 0)
            ;
        else if (strcmp(str, "table") == 0)
            ;
        else if (strcmp(str, "string") == 0)
            ;
        else if (strcmp(str, "boolean") == 0)
            ;
            /* if we see [] then it is an array type */
            if (testnext(ls, '[')) {
                checknext(ls, ']');
            }
    }
    new_localvar(ls, name);
}

static void parse_parameter_list(LexState *ls) {
    /* parlist -> [ param { ',' param } ] */
    int nparams = 0;
    int is_vararg = 0;
    if (ls->t.token != ')') {  /* is 'parlist' not empty? */
        do {
            switch (ls->t.token) {
            case TK_NAME: {  /* param -> NAME */
                             /* RAVI change - add type */
                declare_local_variable(ls);
                nparams++;
                break;
            }
            case TK_DOTS: {  /* param -> '...' */
                luaX_next(ls);
                is_vararg = 1;  /* declared vararg */
                break;
            }
            default: luaX_syntaxerror(ls, "<name> or '...' expected");
            }
        } while (!is_vararg && testnext(ls, ','));
    }
    adjustlocalvars(ls, nparams);
}


static void parse_function_body(LexState *ls, int ismethod, int line) {
    /* body ->  '(' parlist ')' block END */
    checknext(ls, '(');
    if (ismethod) {
        new_localvarliteral(ls, "self");  /* create 'self' parameter */
        adjustlocalvars(ls, 1);
    }
    parse_parameter_list(ls);
    checknext(ls, ')');
    parse_statement_list(ls);
    check_match(ls, TK_END, TK_FUNCTION, line);
}

/* parse expression list */
static int parse_expression_list(LexState *ls) {
    /* explist -> expr { ',' expr } */
    int n = 1;  /* at least one expression */
    parse_expression(ls);
    while (testnext(ls, ',')) {
        parse_expression(ls);
        n++;
    }
    return n;
}

/* parse function arguments */
static void parse_function_arguments(LexState *ls, int line) {
    int base, nparams;
    switch (ls->t.token) {
    case '(': {  /* funcargs -> '(' [ explist ] ')' */
        luaX_next(ls);
        if (ls->t.token == ')')  /* arg list is empty? */
            // args.k = VVOID;
            ;
        else {
            parse_expression_list(ls);
        }
        check_match(ls, ')', '(', line);
        break;
    }
    case '{': {  /* funcargs -> constructor */
        parse_table_constructor(ls);
        break;
    }
    case TK_STRING: {  /* funcargs -> STRING */
        luaX_next(ls);  /* must use 'seminfo' before 'next' */
        break;
    }
    default: {
        luaX_syntaxerror(ls, "function arguments expected");
    }
    }
}




/*
** {======================================================================
** Expression parsing
** =======================================================================
*/

/* primary expression - name or subexpression */
static void parse_primary_expression(LexState *ls) {
    /* primaryexp -> NAME | '(' expr ')' */
    switch (ls->t.token) {
    case '(': {
        int line = ls->linenumber;
        luaX_next(ls);
        parse_expression(ls);
        check_match(ls, ')', '(', line);
        return;
    }
    case TK_NAME: {
        singlevar(ls);
        return;
    }
    default: {
        luaX_syntaxerror(ls, "unexpected symbol");
    }
    }
}

/* variable or field access or function call */
static void parse_suffixed_expression(LexState *ls) {
    /* suffixedexp ->
    primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
    int line = ls->linenumber;
    parse_primary_expression(ls);
    for (;;) {
        switch (ls->t.token) {
        case '.': {  /* fieldsel */
            parse_field_selector(ls);
            break;
        }
        case '[': {  /* '[' exp1 ']' */
            parse_yindex(ls);
            break;
        }
        case ':': {  /* ':' NAME funcargs */
            luaX_next(ls);
            check_name_and_next(ls);
            parse_function_arguments(ls, line);
            break;
        }
        case '(': case TK_STRING: case '{': {  /* funcargs */
            parse_function_arguments(ls, line);
            break;
        }
        default: return;
        }
    }
}


static void parse_simple_expression(LexState *ls) {
    /* simpleexp -> FLT | INT | STRING | NIL | TRUE | FALSE | ... |
    constructor | FUNCTION body | suffixedexp */
    switch (ls->t.token) {
    case TK_FLT: {
        break;
    }
    case TK_INT: {
        break;
    }
    case TK_STRING: {
        break;
    }
    case TK_NIL: {
        break;
    }
    case TK_TRUE: {
        break;
    }
    case TK_FALSE: {
        break;
    }
    case TK_DOTS: {  /* vararg */
        break;
    }
    case '{': {  /* constructor */
        parse_table_constructor(ls);
        return;
    }
    case TK_FUNCTION: {
        luaX_next(ls);
        parse_function_body(ls, 0, ls->linenumber);
        return;
    }
    default: {
        parse_suffixed_expression(ls);
        return;
    }
    }
    luaX_next(ls);
}


static UnOpr get_unary_opr(int op) {
    switch (op) {
    case TK_NOT: return OPR_NOT;
    case '-': return OPR_MINUS;
    case '~': return OPR_BNOT;
    case '#': return OPR_LEN;
    case TK_TO_INTEGER: return OPR_TO_INTEGER;
    case TK_TO_NUMBER: return OPR_TO_NUMBER;
    case TK_TO_INTARRAY: return OPR_TO_INTARRAY;
    case TK_TO_NUMARRAY: return OPR_TO_NUMARRAY;
    case TK_TO_TABLE: return OPR_TO_TABLE;
    default: return OPR_NOUNOPR;
    }
}


static BinOpr get_binary_opr(int op) {
    switch (op) {
    case '+': return OPR_ADD;
    case '-': return OPR_SUB;
    case '*': return OPR_MUL;
    case '%': return OPR_MOD;
    case '^': return OPR_POW;
    case '/': return OPR_DIV;
    case TK_IDIV: return OPR_IDIV;
    case '&': return OPR_BAND;
    case '|': return OPR_BOR;
    case '~': return OPR_BXOR;
    case TK_SHL: return OPR_SHL;
    case TK_SHR: return OPR_SHR;
    case TK_CONCAT: return OPR_CONCAT;
    case TK_NE: return OPR_NE;
    case TK_EQ: return OPR_EQ;
    case '<': return OPR_LT;
    case TK_LE: return OPR_LE;
    case '>': return OPR_GT;
    case TK_GE: return OPR_GE;
    case TK_AND: return OPR_AND;
    case TK_OR: return OPR_OR;
    default: return OPR_NOBINOPR;
    }
}


static const struct {
    lu_byte left;  /* left priority for each binary operator */
    lu_byte right; /* right priority */
} priority[] = {  /* ORDER OPR */
    { 10, 10 },{ 10, 10 },           /* '+' '-' */
    { 11, 11 },{ 11, 11 },           /* '*' '%' */
    { 14, 13 },                  /* '^' (right associative) */
    { 11, 11 },{ 11, 11 },           /* '/' '//' */
    { 6, 6 },{ 4, 4 },{ 5, 5 },   /* '&' '|' '~' */
    { 7, 7 },{ 7, 7 },           /* '<<' '>>' */
    { 9, 8 },                   /* '..' (right associative) */
    { 3, 3 },{ 3, 3 },{ 3, 3 },   /* ==, <, <= */
    { 3, 3 },{ 3, 3 },{ 3, 3 },   /* ~=, >, >= */
    { 2, 2 },{ 1, 1 }            /* and, or */
};

#define UNARY_PRIORITY	12  /* priority for unary operators */


/*
** subexpr -> (simpleexp | unop subexpr) { binop subexpr }
** where 'binop' is any binary operator with a priority higher than 'limit'
*/
static BinOpr parse_sub_expression(LexState *ls, int limit) {
    BinOpr op;
    UnOpr uop;
    enterlevel(ls);
    uop = get_unary_opr(ls->t.token);
    if (uop != OPR_NOUNOPR) {
        int line = ls->linenumber;
        luaX_next(ls);
        parse_sub_expression(ls, UNARY_PRIORITY);
    }
    else {
        parse_simple_expression(ls);
    }
    /* expand while operators have priorities higher than 'limit' */
    op = get_binary_opr(ls->t.token);
    while (op != OPR_NOBINOPR && priority[op].left > limit) {
        BinOpr nextop;
        int line = ls->linenumber;
        luaX_next(ls);
        /* read sub-expression with higher priority */
        nextop = parse_sub_expression(ls, priority[op].right);
        op = nextop;
    }
    leavelevel(ls);
    return op;  /* return first untreated operator */
}


static void parse_expression(LexState *ls) {
    parse_sub_expression(ls, 0);
}

/* }==================================================================== */



/*
** {======================================================================
** Rules for Statements
** =======================================================================
*/


static void parse_block(LexState *ls) {
    /* block -> statlist */
    parse_statement_list(ls);
}

/* parse assignment (not part of local statement) - for each variable
* on the left and side this is called recursively with increasing nvars.
* The final recursive call parses the rhs.
*/
static void parse_assignment(LexState *ls, int nvars) {
    //check_condition(ls, vkisvar(lh->v.k), "syntax error");
    if (testnext(ls, ',')) {  /* assignment -> ',' suffixedexp assignment */
        parse_suffixed_expression(ls);
        //if (nv.v.k != VINDEXED)
        //    check_conflict(ls, lh, &nv.v);
        //checklimit(ls->fs, nvars + ls->L->nCcalls, LUAI_MAXCCALLS,
        //    "C levels");
        parse_assignment(ls, nvars + 1);
    }
    else {  /* assignment -> '=' explist */
        int nexps;
        checknext(ls, '=');
        nexps = parse_expression_list(ls);
        if (nexps != nvars) {
            //adjust_assign(ls, nvars, nexps, &e);
        }
        else {
            return;  /* avoid default */
        }
    }
}

/* parse condition in a repeat statement or an if control structure
* called by repeatstat(), test_then_block()
*/
static void parse_condition(LexState *ls) {
    /* cond -> exp */
    parse_expression(ls);                   /* read condition */
}


static void parse_goto_statment(LexState *ls) {
    int line = ls->linenumber;
    TString *label;
    int g;
    if (testnext(ls, TK_GOTO))
        label = check_name_and_next(ls);
    else {
        luaX_next(ls);  /* skip break */
        label = luaS_new(ls->L, "break");
    }
}


/* skip no-op statements */
static void skip_noop_statements(LexState *ls) {
    while (ls->t.token == ';' || ls->t.token == TK_DBCOLON)
        parse_statement(ls);
}


static void parse_label_statement(LexState *ls, TString *label, int line) {
    /* label -> '::' NAME '::' */
    checknext(ls, TK_DBCOLON);  /* skip double colon */
                                /* create new entry for this label */
    skip_noop_statements(ls);  /* skip other no-op statements */
}

/* parse a while-do control structure, body processed by block()
* called by statement()
*/
static void parse_while_statement(LexState *ls, int line) {
    /* whilestat -> WHILE cond DO block END */
    int whileinit;
    int condexit;
    luaX_next(ls);  /* skip WHILE */
    parse_condition(ls);
    checknext(ls, TK_DO);
    parse_block(ls);
    check_match(ls, TK_END, TK_WHILE, line);
}

/* parse a repeat-until control structure, body parsed by statlist()
* called by statement()
*/
static void parse_repeat_statement(LexState *ls, int line) {
    /* repeatstat -> REPEAT block UNTIL cond */
    luaX_next(ls);  /* skip REPEAT */
    parse_statement_list(ls);
    check_match(ls, TK_UNTIL, TK_REPEAT, line);
    parse_condition(ls);  /* read condition (inside scope block) */
}

/* parse the single expressions needed in numerical for loops
* called by fornum()
*/
static void parse_exp1(LexState *ls) {
    /* Since the local variable in a fornum loop is local to the loop and does
    * not use any variable in outer scope we don't need to check its
    * type - also the loop is already optimised so no point trying to
    * optimise the iteration variable
    */
    parse_expression(ls);
}

/* parse a for loop body for both versions of the for loop
* called by fornum(), forlist()
*/
static void parse_forbody(LexState *ls, int line, int nvars, int isnum) {
    /* forbody -> DO block */
    OpCode forprep_inst = OP_FORPREP, forloop_inst = OP_FORLOOP;
    int prep, endfor;
    adjustlocalvars(ls, 3);  /* control variables */
    checknext(ls, TK_DO);
    adjustlocalvars(ls, nvars);
    parse_block(ls);
}

/* parse a numerical for loop, calls forbody()
* called from forstat()
*/
static void parse_fornum_statement(LexState *ls, TString *varname, int line) {
    /* fornum -> NAME = exp1,exp1[,exp1] forbody */
    new_localvarliteral(ls, "(for index)");
    new_localvarliteral(ls, "(for limit)");
    new_localvarliteral(ls, "(for step)");
    new_localvar(ls, varname);
    /* The fornum sets up its own variables as above.
    These are expected to hold numeric values - but from Ravi's
    point of view we need to know if the variable is an integer or
    double. So we need to check if this can be determined from the
    fornum expressions. If we can then we will set the
    fornum variables to the type we discover.
    */
    checknext(ls, '=');
    /* get the type of each expression */
    parse_exp1(ls);  /* initial value */
    checknext(ls, ',');
    parse_exp1(ls);  /* limit */
    if (testnext(ls, ','))
        parse_exp1(ls);  /* optional step */
    else {  /* default step = 1 */
    }
    parse_forbody(ls, line, 1, 1);
}

/* parse a generic for loop, calls forbody()
* called from forstat()
*/
static void parse_for_list(LexState *ls, TString *indexname) {
    /* forlist -> NAME {,NAME} IN explist forbody */
    int nvars = 4; /* gen, state, control, plus at least one declared var */
    int line;
    /* create control variables */
    new_localvarliteral(ls, "(for generator)");
    new_localvarliteral(ls, "(for state)");
    new_localvarliteral(ls, "(for control)");
    /* create declared variables */
    new_localvar(ls, indexname); /* RAVI TODO for name:type syntax? */
    while (testnext(ls, ',')) {
        new_localvar(ls, check_name_and_next(ls)); /* RAVI change - add type */
        nvars++;
    }
    checknext(ls, TK_IN);
    line = ls->linenumber;
    parse_forbody(ls, line, nvars - 3, 0);
}

/* initial parsing of a for loop - calls fornum() or forlist()
* called from statement()
*/
static void parse_for_statement(LexState *ls, int line) {
    /* forstat -> FOR (fornum | forlist) END */
    TString *varname;
    luaX_next(ls);  /* skip 'for' */
    varname = check_name_and_next(ls);  /* first variable name */
    switch (ls->t.token) {
    case '=': parse_fornum_statement(ls, varname, line); break;
    case ',': case TK_IN: parse_for_list(ls, varname); break;
    default: luaX_syntaxerror(ls, "'=' or 'in' expected");
    }
    check_match(ls, TK_END, TK_FOR, line);
}

/* parse if cond then block - called from ifstat() */
static void parse_if_cond_then_block(LexState *ls, int *escapelist) {
    /* test_then_block -> [IF | ELSEIF] cond THEN block */
    int jf;         /* instruction to skip 'then' code (if condition is false) */
    luaX_next(ls);  /* skip IF or ELSEIF */
    parse_expression(ls);  /* read condition */
    checknext(ls, TK_THEN);
    if (ls->t.token == TK_GOTO || ls->t.token == TK_BREAK) {
        parse_goto_statment(ls);  /* handle goto/break */
        skip_noop_statements(ls);  /* skip other no-op statements */
        if (block_follow(ls, 0)) {  /* 'goto' is the entire block? */
            return;  /* and that is it */
        }
        else  /* must skip over 'then' part if condition is false */
            ;//jf = luaK_jump(fs);
    }
    else {  /* regular case (not goto/break) */
        //jf = v.f;
    }
    parse_statement_list(ls);  /* 'then' part */
}

/* parse an if control structure - called from statement() */
static void parse_if_statement(LexState *ls, int line) {
    /* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
    int escapelist = NO_JUMP;  /* exit list for finished parts */
    parse_if_cond_then_block(ls, &escapelist);  /* IF cond THEN block */
    while (ls->t.token == TK_ELSEIF)
        parse_if_cond_then_block(ls, &escapelist);  /* ELSEIF cond THEN block */
    if (testnext(ls, TK_ELSE))
        parse_block(ls);  /* 'else' part */
    check_match(ls, TK_END, TK_IF, line);
}

/* parse a local function statement - called from statement() */
static void parse_local_function(LexState *ls) {
    new_localvar(ls, check_name_and_next(ls));  /* new local variable */
    adjustlocalvars(ls, 1);  /* enter its scope */
    parse_function_body(ls, 0, ls->linenumber);  /* function created in next register */
}

/* parse a local variable declaration statement - called from statement() */
static void parse_local_statement(LexState *ls) {
    /* stat -> LOCAL NAME {',' NAME} ['=' explist] */
    int nvars = 0;
    int nexps;
    do {
        /* local name : type = value */
        declare_local_variable(ls);
        nvars++;
        if (nvars >= MAXVARS)
            luaX_syntaxerror(ls, "too many local variables");
    } while (testnext(ls, ','));
    if (testnext(ls, '='))
        nexps = parse_expression_list(ls);
    else {
        nexps = 0;
    }
    adjustlocalvars(ls, nvars);
}

/* parse a function name specification - called from funcstat()
* returns boolean value - true if function is a method
*/
static int parse_function_name(LexState *ls) {
    /* funcname -> NAME {fieldsel} [':' NAME] */
    int ismethod = 0;
    singlevar(ls);
    while (ls->t.token == '.')
        parse_field_selector(ls);
    if (ls->t.token == ':') {
        ismethod = 1;
        parse_field_selector(ls);
    }
    return ismethod;
}

/* parse a function statement - called from statement() */
static void parse_function_statement(LexState *ls, int line) {
    /* funcstat -> FUNCTION funcname body */
    int ismethod;
    luaX_next(ls); /* skip FUNCTION */
    ismethod = parse_function_name(ls);
    parse_function_body(ls, ismethod, line);
}


/* parse function call with no returns or assignment statement - called from statement() */
static void parse_expr_statement(LexState *ls) {
    /* stat -> func | assignment */
    parse_suffixed_expression(ls);
    if (ls->t.token == '=' || ls->t.token == ',') { /* stat -> assignment ? */
        parse_assignment(ls, 1);
    }
    else {  /* stat -> func */
        //check_condition(ls, v.v.k == VCALL, "syntax error");
    }
}

/* parse return statement - called from statement() */
static void parse_return_statement(LexState *ls) {
    /* stat -> RETURN [explist] [';'] */
    int first, nret; /* registers with returned values */
    if (block_follow(ls, 1) || ls->t.token == ';')
        first = nret = 0;  /* return no values */
    else {
        nret = parse_expression_list(ls);  /* optional return values */
//        if (hasmultret(e.k)) {
//            nret = LUA_MULTRET;  /* return all values */
 //       }
//        else {
//        }
    }
    testnext(ls, ';');  /* skip optional semicolon */
}


/* parse a statement */
static void parse_statement(LexState *ls) {
    int line = ls->linenumber;  /* may be needed for error messages */
    enterlevel(ls);
    switch (ls->t.token) {
    case ';': {  /* stat -> ';' (empty statement) */
        luaX_next(ls);  /* skip ';' */
        break;
    }
    case TK_IF: {  /* stat -> ifstat */
        parse_if_statement(ls, line);
        break;
    }
    case TK_WHILE: {  /* stat -> whilestat */
        parse_while_statement(ls, line);
        break;
    }
    case TK_DO: {  /* stat -> DO block END */
        luaX_next(ls);  /* skip DO */
        parse_block(ls);
        check_match(ls, TK_END, TK_DO, line);
        break;
    }
    case TK_FOR: {  /* stat -> forstat */
        parse_for_statement(ls, line);
        break;
    }
    case TK_REPEAT: {  /* stat -> repeatstat */
        parse_repeat_statement(ls, line);
        break;
    }
    case TK_FUNCTION: {  /* stat -> funcstat */
        parse_function_statement(ls, line);
        break;
    }
    case TK_LOCAL: {  /* stat -> localstat */
        luaX_next(ls);  /* skip LOCAL */
        if (testnext(ls, TK_FUNCTION))  /* local function? */
            parse_local_function(ls);
        else
            parse_local_statement(ls);
        break;
    }
    case TK_DBCOLON: {  /* stat -> label */
        luaX_next(ls);  /* skip double colon */
        parse_label_statement(ls, check_name_and_next(ls), line);
        break;
    }
    case TK_RETURN: {  /* stat -> retstat */
        luaX_next(ls);  /* skip RETURN */
        parse_return_statement(ls);
        break;
    }
    case TK_BREAK:   /* stat -> breakstat */
    case TK_GOTO: {  /* stat -> 'goto' NAME */
        parse_goto_statment(ls);
        break;
    }
    default: {  /* stat -> func | assignment */
        parse_expr_statement(ls);
        break;
    }
    }
    leavelevel(ls);
}

static void parse_statement_list(LexState *ls) {
    /* statlist -> { stat [';'] } */
    while (!block_follow(ls, 1)) {
        if (ls->t.token == TK_RETURN) {
            parse_statement(ls);
            return;  /* 'return' must be last statement */
        }
        parse_statement(ls);
    }
}


/* mainfunc() equivalent */
static void parse_chunk(LexState *ls) { 
    luaX_next(ls); /* read first token */
    parse_statement_list(ls);
    check(ls, TK_EOS);
}

/*
** Parse the given source 'chunk' and build an abstract
** syntax tree; return 0 on success / non-zero return code on
** failure
** On success will push a userdata object containing the abstract
** syntax tree.
** On failure push an error message.
*/
int raviY_parse_to_ast(lua_State *L, ZIO *z, Mbuffer *buff, 
                       const char *name, int firstchar) {
  StkId ctop = L->top;
  struct ast_container *container = new_ast_container(L);
  LexState lexstate;
  lexstate.h = luaH_new(L);         /* create table for scanner */
  sethvalue(L, L->top, lexstate.h); /* anchor it */
  luaD_inctop(L);
  TString *src = luaS_new(L, name); /* create and anchor TString */
  setsvalue(L, L->top, src);
  luaD_inctop(L);
  lexstate.buff = buff;
  lexstate.dyd = NULL; /* Unlike standard Lua parser / code generator we do not use this */
  luaX_setinput(L, &lexstate, z, src, firstchar);
  parse_chunk(&lexstate);
  L->top--; /* remove source name */
  L->top--; /* remove scanner's table */
  assert(ctop == L->top-1);
  return 0;
}

static const luaL_Reg container_methods[] = {
	{ NULL, NULL } };

static const luaL_Reg astlib[] = {
	{ NULL, NULL } };

LUAMOD_API int raviopen_ast_library(lua_State *L) {
	raviL_newmetatable(L, AST_type, AST_type);
	lua_pushcfunction(L, collect_ast_container);
	lua_setfield(L, -2, "__gc");
	lua_pushvalue(L, -1);           /* push metatable */
	lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
	luaL_setfuncs(L, container_methods, 0);
	lua_pop(L, 1);

	luaL_newlib(L, astlib);
	return 1;
	return 0;
}