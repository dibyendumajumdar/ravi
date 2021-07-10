local function comp(a: number[], b: number[])
  local abs = math.abs
  for i = 1, #a do
    if abs(a[i] - b[i]) > 1e-10 then
      return false
    end
  end
  return true
end

local computils = assert(require('aot'))

computils.comptoC('gaussian2_lib.lua', 'gaussian2_lib.c')
assert(os.execute("gcc -O2 -shared -fpic gaussian2_lib.c -o gaussian2_lib.so"))
local f = package.load_ravi_lib('gaussian2_lib.so', 'mymain')
assert(f and type(f) == 'function')
local glib  = f()
assert(glib and type(glib) == 'table')

local gaussian_solve = glib.gaussian_solve
assert(gaussian_solve and type(gaussian_solve) == 'function')
local A: number[] = { 4,8,-4; 2,5,1; 1,5,10 }
local b: number[] = { 3,8,5 }

-- control (LAPACK solve)
local expectedx: number[] = { 1,-1,1 }
local x:number[] = gaussian_solve(A, b, 3, 3)

print('expected ', table.unpack(expectedx))
print('got      ', table.unpack(x))
assert(comp(x, expectedx))

local A: number[] = { 2,6,4; 1,-1,3; -1,-9,1 }
local b: number[] = { 3,7,5 }

local expectedx: number[] = { 0,2,-1 }
x = gaussian_solve(A, b, 3, 3)

print('expected ', table.unpack(expectedx))
print('got      ', table.unpack(x))
assert(comp(x, expectedx))

local A: number[] = { 0,1,2,1; 1,1,2,2; 1,2,4,1; 1,1,0,1 }
local b: number[] = { 1,-1,-1,2 }

local expectedx: number[] = { -1.25, 2.25, -0.75, -0.5 }
x = gaussian_solve(A, b, 4, 4)

print('expected ', table.unpack(expectedx))
print('got      ', table.unpack(x))
assert(comp(x, expectedx))

print 'Ok'
--ravi.dumplua(gaussian_solve)
--ravi.dumplua(partial_pivot)
