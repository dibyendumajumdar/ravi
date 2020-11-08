f = compiler.load("local a,b = 1,2 return a and b");
assert(f and type(f) == 'function')
assert(f() == 2)