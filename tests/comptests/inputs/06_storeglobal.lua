f = compiler.load('x=1')
assert(f and type(f) == 'function')
f()
assert(x==1)
print 'Ok'