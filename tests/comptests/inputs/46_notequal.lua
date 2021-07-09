f = compiler.load([[
function comp(a, b)
  return a ~= b
end
]]
)
assert(f and type(f) == 'function')
f()

assert(comp(1, 2))
assert(not comp(1, 1))

print 'Ok'