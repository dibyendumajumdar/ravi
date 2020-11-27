f = compiler.load("local a = 2 return 1 and a");
assert(f and type(f) == 'function')
assert(f() == 2)
print 'Ok'