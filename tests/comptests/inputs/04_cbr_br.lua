f = compiler.load("local a = 2 return 1 or a");
assert(f and type(f) == 'function')
assert(f() == 1)
print 'Ok'