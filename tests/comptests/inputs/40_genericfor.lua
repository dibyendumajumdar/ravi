f = compiler.load([[
    local r = {}
    for k,v in pairs(t) do
        print(k,v)
        r[k] = v
    end
    return r
]]
)
assert(f and type(f) == 'function')
t = { a = 1, b = 2, c = 3 }
x = f()
assert(t.a == x.a)
assert(t.b == x.b)
assert(t.c == x.c)

print 'Ok'