f = compiler.load([[
a = {}
i = 3
i, a[i] = i+1, 20
]]
)
assert(f and type(f) == 'function')
f()
assert(a[3] == 20)
assert(a[4] == nil)
assert(i == 4)
print 'Ok'