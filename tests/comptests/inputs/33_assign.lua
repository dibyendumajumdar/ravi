f = compiler.load([[
a, b = 1, 2
]]
)
assert(f and type(f) == 'function')
f()
assert(a == 1)
assert(b == 2)
print 'Ok'