f = compiler.load([[
function z()
    local t = ''
    --print(t)
    return t
end
]]
)
assert(f and type(f) == 'function')
f()
assert(z and type(z) == 'function')
assert(z() == "")
print 'Ok'