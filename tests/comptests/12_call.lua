function g() return 3, 4.2 end
f = compiler.load('return 0, g(), 1')
assert(f and type(f) == 'function')
local a, b, c = f()
assert(a == 0)
assert(b == 3)
assert(c == 1)