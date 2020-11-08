x = {}
f = compiler.load("x.a = 'Dibyendu' x[1] = 2")
assert(f and type(f) == 'function')
f()
assert(x.a == 'Dibyendu')
assert(x[1] == 2)