f = compiler.load([[
local intarray = table.intarray
function testme(n: integer)
  local nrow: integer[] = intarray(n)
  for i = 1,n do
    nrow[i] = i
  end
  return nrow
end
]]
)
assert(f and type(f) == 'function')
f()

local function comp(a: integer[], b: integer[])
  for i = 1, #a do
    if a[i] - b[i] ~= 0 then
      return false
    end
  end
  return true
end

local A: integer[] = { 1,2,3 }
local B: integer[] = testme(3)

assert(comp(A, B))

print 'Ok'