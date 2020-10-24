function g() return 3, 4.2 end
f = compiler.load('return g()')
assert(f and type(f) == 'function')
local a, b = f()
assert(a == 3)
assert(b == 4.2)