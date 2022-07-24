local function f() return 1,2,3 end
local t = { f() }
assert( t[1] == 1 and t[2] == 2 and t[3] == 3 )
assert( #t == 3 )

print '72 Ok'