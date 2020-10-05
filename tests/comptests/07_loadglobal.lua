x = 1
y = 2
f = compiler.load('return x,y')
assert(f and type(f) == 'function')
local a,b = f()
assert(a == 1)
assert(b == 2)