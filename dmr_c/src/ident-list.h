
#define IDENT(n) __IDENT(n## _ident, #n, 0)
#define IDENT_RESERVED(n) __IDENT(n## _ident, #n, 1)

/* Basic C reserved words.. */
IDENT_RESERVED(sizeof);
IDENT_RESERVED(if);
IDENT_RESERVED(else);
IDENT_RESERVED(return);
IDENT_RESERVED(switch);
IDENT_RESERVED(case);
IDENT_RESERVED(default);
IDENT_RESERVED(break);
IDENT_RESERVED(continue);
IDENT_RESERVED(for);
IDENT_RESERVED(while);
IDENT_RESERVED(do);
IDENT_RESERVED(goto);

/* C typenames. They get marked as reserved when initialized */
IDENT(struct);
IDENT(union);
IDENT(enum);
IDENT(__attribute); IDENT(__attribute__);
IDENT(volatile); IDENT(__volatile); IDENT(__volatile__);
IDENT(double);

/* C storage classes. They get marked as reserved when initialized */
IDENT(static);

/* C99 keywords */
IDENT(restrict); IDENT(__restrict); IDENT(__restrict__);
IDENT(_Bool);
IDENT_RESERVED(_Complex);
IDENT_RESERVED(_Imaginary);

/* C11 keywords */
IDENT(_Alignas);
IDENT_RESERVED(_Alignof);
IDENT_RESERVED(_Atomic);
IDENT_RESERVED(_Generic);
IDENT(_Noreturn);
IDENT_RESERVED(_Static_assert);
IDENT(_Thread_local);

/* Special case for L'\t' */
IDENT(L);

/* Extended gcc identifiers */
IDENT(asm); IDENT_RESERVED(__asm); IDENT_RESERVED(__asm__);
IDENT(alignof); IDENT_RESERVED(__alignof); IDENT_RESERVED(__alignof__); 
IDENT_RESERVED(__sizeof_ptr__);
IDENT_RESERVED(__builtin_types_compatible_p);
IDENT_RESERVED(__builtin_offsetof);
IDENT_RESERVED(__label__);

/* Attribute names */
IDENT(packed); IDENT(__packed__);
IDENT(aligned); IDENT(__aligned__);
IDENT(nocast);
IDENT(noderef);
IDENT(safe);
IDENT(force);
IDENT(address_space);
IDENT(context);
IDENT(mode); IDENT(__mode__);
IDENT(QI); IDENT(__QI__);
IDENT(HI); IDENT(__HI__);
IDENT(SI); IDENT(__SI__);
IDENT(DI); IDENT(__DI__);
IDENT(word); IDENT(__word__);
IDENT(format); IDENT(__format__);
IDENT(section); IDENT(__section__);
IDENT(unused); IDENT(__unused__);
IDENT(const); IDENT(__const); IDENT(__const__);
IDENT(used); IDENT(__used__);
IDENT(warn_unused_result); IDENT(__warn_unused_result__);
IDENT(noinline); IDENT(__noinline__);
IDENT(deprecated); IDENT(__deprecated__);
IDENT(noreturn); IDENT(__noreturn__);
IDENT(regparm); IDENT(__regparm__);
IDENT(weak); IDENT(__weak__);
IDENT(no_instrument_function); IDENT(__no_instrument_function__);
IDENT(sentinel); IDENT(__sentinel__);
IDENT(alias); IDENT(__alias__);
IDENT(pure); IDENT(__pure__);
IDENT(always_inline); IDENT(__always_inline__);
IDENT(syscall_linkage); IDENT(__syscall_linkage__);
IDENT(visibility); IDENT(__visibility__);
IDENT(bitwise); IDENT(__bitwise__);
IDENT(model); IDENT(__model__);
IDENT(format_arg); IDENT(__format_arg__);
IDENT(nothrow); IDENT(__nothrow); IDENT(__nothrow__);
IDENT(__transparent_union__);
IDENT(malloc);
IDENT(__malloc__);
IDENT(nonnull); IDENT(__nonnull); IDENT(__nonnull__);
IDENT(constructor); IDENT(__constructor__);
IDENT(destructor); IDENT(__destructor__);
IDENT(cold); IDENT(__cold__);
IDENT(hot); IDENT(__hot__);
IDENT(cdecl); IDENT(__cdecl__);
IDENT(stdcall); IDENT(__stdcall__);
IDENT(fastcall); IDENT(__fastcall__);
IDENT(dllimport); IDENT(__dllimport__);
IDENT(dllexport); IDENT(__dllexport__);
IDENT(artificial); IDENT(__artificial__);
IDENT(leaf); IDENT(__leaf__);
IDENT(vector_size); IDENT(__vector_size__);
IDENT(error); IDENT(__error__);


/* Preprocessor idents.  Direct use of __IDENT avoids mentioning the keyword
 * itself by name, preventing these tokens from expanding when compiling
 * sparse. */
IDENT(defined);
IDENT(once);
__IDENT(pragma_ident, "__pragma__", 0);
__IDENT(__VA_ARGS___ident, "__VA_ARGS__", 0);
__IDENT(__LINE___ident, "__LINE__", 0);
__IDENT(__FILE___ident, "__FILE__", 0);
__IDENT(__DATE___ident, "__DATE__", 0);
__IDENT(__TIME___ident, "__TIME__", 0);
__IDENT(__func___ident, "__func__", 0);
__IDENT(__FUNCTION___ident, "__FUNCTION__", 0);
__IDENT(__PRETTY_FUNCTION___ident, "__PRETTY_FUNCTION__", 0);
__IDENT(__COUNTER___ident, "__COUNTER__", 0);

/* Sparse commands */
IDENT_RESERVED(__context__);
IDENT_RESERVED(__range__);

/* Magic function names we recognize */
IDENT(memset); IDENT(memcpy);
IDENT(copy_to_user); IDENT(copy_from_user);
IDENT(main);

#undef __IDENT
#undef IDENT
#undef IDENT_RESERVED
