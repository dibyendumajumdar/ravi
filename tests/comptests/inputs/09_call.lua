function g() return 4.2 end
f = compiler.load('return g()')
assert(f and type(f) == 'function')
local a = f()
assert(a == 4.2)
print 'Ok'