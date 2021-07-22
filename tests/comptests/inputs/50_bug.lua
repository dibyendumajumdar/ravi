f = compiler.load([[
local function test()
    return 1.0
end
return test()
]]
)
assert(f and type(f) == 'function')
local v = f()
print(v)
assert(v == 1.0)

print 'Ok'