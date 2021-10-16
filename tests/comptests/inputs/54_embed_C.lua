-- Test simple data passing

local i: integer = 42
local j: integer

C__unsafe(i,j) [[
    j = i;
]]

assert(j == 42)

print 'Ok'