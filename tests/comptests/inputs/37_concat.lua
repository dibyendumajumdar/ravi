f = compiler.load([[
    local x = 1.
    local y = x .. ' test ' .. x
    local z = 2.
    return x, y, z
]]
)
assert(f and type(f) == 'function')

local x, y, z = f()
--print(x,y,z)
assert(x == 1.)
assert(y == "1.0 test 1.0")
assert(z == 2.)
print 'Ok'