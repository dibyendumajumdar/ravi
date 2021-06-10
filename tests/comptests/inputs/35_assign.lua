f = compiler.load([[
x[1],y[1],c,d = 1,z()
]]
)
assert(f and type(f) == 'function')

function z()
  return 2,3,41
end
x = {}
y = {}
f()
assert(x[1] == 1)
assert(y[1] == 2)
assert(c == 3)
assert(d == 41)
print 'Ok'