x = 4
y = 2
f = compiler.load('x,y = y,x')
assert(f and type(f) == 'function')
f()
assert(x == 2)
assert(y == 4)