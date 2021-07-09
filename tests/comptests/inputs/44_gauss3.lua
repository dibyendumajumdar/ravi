f = compiler.load([[
local slice = table.slice
function buildtable(A: number[], b: number[], m: integer, n: integer)
  local columns: table = {}
  -- first get the column slices
  for j = 1,n do
    columns[j] = slice(A, (j-1)*m+1, m)
  end
  columns[n+1] = b
  return columns
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

local function dump_matrix(columns: table, m: integer, n: integer, nrow: integer[])
  local write = io.write
  for i = 1,m do
    for j = 1,n do
      write(columns[j][nrow[i]], ' ')
    end
    write("\n")
  end
end


local function copy(a: number[])
  local c: number[] = table.numarray(#a, 0.0)
  for i = 1,#a do
    c[i] = a[i]
  end
  return c
end

local A: number[] = { 4,8,-4; 2,5,1; 1,5,10 }
local b: number[] = { 3,8,5 }
local nrow: integer[] = {1,2,3}

local X: table = buildtable(copy(A), copy(b), 3, 3)

assert(comp(X[1], @number[]{4,8,-4}))
assert(comp(X[2], @number[]{2,5,1}))
assert(comp(X[3], @number[]{1,5,10}))
assert(comp(X[4], @number[]{3,8,5}))

dump_matrix(X, 3, 4, nrow)

print 'Ok'