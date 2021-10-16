-- Test simple data passing

local i: integer = 42
local j: integer

C__decl [[
typedef struct {
    int a,b;
} TestStruct;
]]

local u = C__new('TestStruct', 1)

C__unsafe(i,j,u) [[
    TestStruct *s = (TestStruct*) u.ptr;
    s->b = i;
    j = s->b;
]]

assert(j == 42)

print 'Ok'