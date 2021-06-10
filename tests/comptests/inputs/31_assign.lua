f = compiler.load([[
local a, b = {}, 10
a[b], b = 20, 30
return a[10], a[30]
]]
)
assert(f and type(f) == 'function')
local x,y = f()
assert(x == 20)
assert(y == nil)
print 'Ok'