f = compiler.load("local a,b = 1,2 return a or b");
assert(f and type(f) == 'function')
assert(f() == 1)
print 'Ok'