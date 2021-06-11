f = compiler.load([[
    local x = 1.
    local y = x .. ' test ' .. x
    local z = 2.
    return x+1 .. ' ' .. y .. ' ' .. z+2
]]
)
assert(f and type(f) == 'function')

local s = f()
--print(s)
assert(s == "2.0 1.0 test 1.0 4.0")
print 'Ok'