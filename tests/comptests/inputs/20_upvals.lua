g = compiler.load([[
local a: integer = 1
function f()
  return function()
     return function()
       a = a + 1
       return a
     end
  end, a
end
]]
)
assert(g and type(g) == 'function')
g()
assert(f and type(f) == 'function')
h, b = f()
assert(h and type(h) == 'function')
assert(b == 1)
k = h()
assert(k and type(k) == 'function')
b = k()
assert(b == 2)
b = k()
assert(b == 3)
l = h()
assert(l and type(l) == 'function')
b = l()
assert(b == 4)
assert('integer' == math.type(b))
