f = compiler.load([[
    for k,v in pairs(t) do
        print(k,v)
    end
]]
)
assert(f and type(f) == 'function')
t = { a = 1, b = 2, c = 3 }
f()
print 'Ok'