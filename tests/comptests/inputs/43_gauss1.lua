f = compiler.load([[
local numarray = table.numarray
function copy(a: number[])
  local c: number[] = numarray(#a, 0.0)
  for i = 1,#a do
    c[i] = a[i]
  end
  return c
end
]]
)
assert(f and type(f) == 'function')
f()

local function comp(a: number[], b: number[])
  local abs = math.abs
  for i = 1, #a do
    if abs(a[i] - b[i]) > 1e-10 then
      return false
    end
  end
  return true
end

local A: number[] = { 4,8,-4; 2,5,1; 1,5,10 }
local B: number[] = copy(A)

assert(comp(A, B))

print 'Ok'