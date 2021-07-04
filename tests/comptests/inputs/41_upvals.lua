f = compiler.load([[
local assert = assert
return function(m: integer, n: integer)
  assert(m == n)
  print('testing')
end
]]
)
assert(f and type(f) == 'function')
local g = f()
g(1,1)
print 'Ok'
