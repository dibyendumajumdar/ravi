-- Gaussian elimination
if ravi.jit then
  ravi.auto(true,1)
end

local assert = assert
local slice, numarray, intarray = table.slice, table.numarray, table.intarray
local write = io.write

local function comp(a: number[], b: number[])
  local abs = math.abs
  for i = 1, #a do
    if abs(a[i] - b[i]) > 1e-10 then
      return false
    end
  end
  return true
end

local function copy(a: number[])
  local c: number[] = numarray(#a, 0.0)
  for i = 1,#a do
    c[i] = a[i]
  end
  return c
end

-- i = column
local function partial_pivot(columns: table, nrow: integer[], i: integer, n: integer)
  local p: integer = i
  local max: number = 0.0
  local a: number[] = @number[]( columns[i] )
  local max_set = false

  -- find the row from i to n that has
  -- max absolute value in column[i]
  for row=i, n do
    local value: number = a[nrow[row]]
    if value < 0.0 then value = -value end
    if not max_set then 
      max = value
      max_set = true
      p = row
    elseif value > max then
      p = row
      max = value
    end
  end
  if a[p] == 0.0 then 
    error("no unique solution exists")
  end
  if nrow[i] ~= nrow[p] then
    write('Performing row interchange ', i, ' will be swapped with ', p, "\n")
    local temp: integer = nrow[i]
    nrow[i] = nrow[p]
    nrow[p] = temp
  end
end

local function dump_matrix(columns: table, m: integer, n: integer, nrow: integer[])
  for i = 1,m do
    for j = 1,n do
      write(columns[j][nrow[i]], ' ')
    end
    write("\n")
  end
end

local function gaussian_solve(A: number[], b: number[], m: integer, n: integer)

  -- make copies
  A = copy(A)
  b = copy(b)

  assert(m == n)
  assert(#b == m)

  -- nrow will hold the order of the rows allowing
  -- easy interchange of rows
  local nrow: integer[] = intarray(n)

  -- As ravi matrices are column major we 
  -- create slices for each column for easy access
  -- the vector b can also be treated as an additional
  -- column thereby creating the augmented matrix 
  local columns: table = {}

  -- we use i as the row and j a the column

  -- first get the column slices 
  for j = 1,n do
    columns[j] = slice(A, (j-1)*m+1, m)
  end
  columns[n+1] = b

  -- initialize the nrow vector
  for i = 1,n do
    nrow[i] = i
  end

  for j = 1,n-1 do -- j is the column
    partial_pivot(columns, nrow, j, m)

    dump_matrix(columns, n, n+1, nrow)

    for i = j+1,m do -- i is the row
      -- obtain the column j
      local column: number[] = @number[]( columns[j] ) 
      local multiplier: number = column[nrow[i]]/column[nrow[j]]
      write('m(' .. i .. ',' .. j .. ') = ', column[nrow[i]], ' / ', column[nrow[j]], "\n")
      write('Performing R(' .. i .. ') = R(' .. i .. ') - m(' .. i .. ',' .. j .. ') * R(' .. j .. ')\n')
      -- For the row i, we need to 
      -- do row(i) = row(i) - multipler * row(j)
      for q = j,n+1 do
        local col: number[] = @number[]( columns[q] )
        col[nrow[i]] = col[nrow[i]] - multiplier*col[nrow[j]]
      end
    end

    write("Post elimination column ", j, "\n")
    dump_matrix(columns, n, n+1, nrow)
  end

  if columns[n][nrow[n]] == 0.0 then
    error("no unique solution exists")
  end


  -- Now we do the back substitution
  local x: number[] = numarray(n, 0.0)
  local a: number[] = @number[]( columns[n] )

  write('Performing back substitution\n')
  x[n] = b[nrow[n]] / a[nrow[n]]
  write('x[', n, '] = b[', n, '] / a[', n, '] = ', x[n], "\n")
  for i = n-1,1,-1 do
    local sum: number
    for j = i+1, n do
      a = @number[]( columns[j] )
      sum = sum + a[nrow[i]] * x[j]
      if j == i+1 then
        write('sum = ')
      else 
        write('sum = sum + ')  
      end
      write('a[', i, ', ', j, '] * x[', j, ']', "\n")
    end
    write('sum = ', sum, '\n')
    a = @number[]( columns[i] )
    x[i] = (b[nrow[i]] - sum) / a[nrow[i]]
    write('x[',i,'] = (b[', i, '] - sum) / a[', i, ', ', i, '] = ', x[i], "\n")  
  end  

  return x
end

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