-- Written by Attractive Chaos; distributed under the MIT license
-- Adapted to LuaJIT FFI by Mike Pall.

local ffi = require("ffi")

local matrix = {}

function matrix.new(m, n)
	return ffi.new("double["..m.."]["..n.."]")
end

function matrix.T(a, n)
	local y = matrix.new(n, n)
	for i = 0, n - 1 do
		for j = 0, n - 1 do
			y[i][j] = a[j][i]
		end
	end
	return y
end

function matrix.mul(a, b, n)
	local y = matrix.new(n, n)
	local c = matrix.T(b, n)
	for i = 1, n - 1 do
		for j = 1, n - 1 do
			local sum = 0
			for k = 0, n - 1 do sum = sum + a[i][k] * c[j][k] end
			y[i][j] = sum
		end
	end
	return y
end

function matgen(n)
	local y, tmp = matrix.new(n, n), 1 / n / n
	for i = 0, n - 1 do
		for j = 0, n - 1 do
			y[i][j] = tmp * (i - j) * (i + j)
		end
	end
	return y
end

local n = tonumber(arg[1]) or 100
n = math.floor(n/2) * 2

if jit then
  -- LuaJIT - warmup
  matrix.mul(matgen(n), matgen(n), n)
end

local t1 = os.clock()
local a = matrix.mul(matgen(n), matgen(n), n)
local t2 = os.clock()
print("time taken ", t2-t1)
print(a[n/2][n/2])