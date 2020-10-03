f = compiler.load("local a, b, c = 4.2, true, 'hi' return a, b, c")
assert(f and type(f) == 'function')
local z = 62
local a,b,c,d = f()
assert(z == 62)
assert(a == 4.2)
assert(b == true)
assert(c == 'hi')