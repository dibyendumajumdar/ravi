a = 5
b = 66
f = compiler.load('return a < b')
assert(f and type(f) == 'function')
assert(f() == true)
a = 9.66
b = 4.2
assert(f() == false)
a = 'a'
b = 'apple'
assert(f() == true)

f = compiler.load('return b >= a')
assert(f and type(f) == 'function')
assert(f() == true)
print 'Ok'