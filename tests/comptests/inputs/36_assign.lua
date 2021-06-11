f = compiler.load([[
    local x = 1.
    local y = 2.
    x, y = y, x -- swap
    return x, y
]]
)
assert(f and type(f) == 'function')

local x, y = f()
print(x, y)
assert(x == 2.)
assert(y == 1.)
print 'Ok'