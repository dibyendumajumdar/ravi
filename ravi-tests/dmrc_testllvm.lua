local testfunc = [[
struct lua_State;
extern int puts(const char *);
extern int TestFunc(struct lua_State *L);

int TestFunc(struct lua_State *L) 
{
	puts("hello world!\n");
	return 0;
}
]]


local ctx = llvm.context()
local m = ctx:newmodule()
m:compileC(testfunc)
local f = m:getfunction("TestFunc")
local callable = f:compile()

callable()