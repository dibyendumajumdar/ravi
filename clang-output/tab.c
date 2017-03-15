#include "lua_hdr.h"

int tablevalue(struct TValue *v)
{
	return (hvalue(v))->sizearray;
}