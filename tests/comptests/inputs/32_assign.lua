f = compiler.load([[
local a, b = 1, 2
return a, b
]]
)
assert(f and type(f) == 'function')
local x,y = f()
assert(x == 1)
assert(y == 2)
print 'Ok'