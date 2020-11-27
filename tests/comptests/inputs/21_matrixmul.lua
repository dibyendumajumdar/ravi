f = compiler.load([[
local matrix = {}

function matrix.T(a: table)
	local m: integer, n: integer, x: table = #a, #a[1], {};
	for i = 1, n do
		local xi: number[] = table.numarray(n, 0.0)
		x[i] = xi
		for j = 1, m do xi[j] = @number (a[j][i]) end
	end
	return x;
end

function matrix.mul(a: table, b: table)
	assert(@integer(#a[1]) == #b);
	local m: integer, n: integer, p: integer, x: table = #a, #a[1], #b[1], {};
	local c: table = matrix.T(b); -- transpose for efficiency
	for i = 1, m do
		local xi: number[] = table.numarray(p, 0.0)
		x[i] = xi
		for j = 1, p do
			local sum: number, ai: number[], cj: number[] = 0.0, @number[](a[i]), @number[](c[j]);
			-- for luajit, caching c[j] or not makes no difference; lua is not so clever
			for k = 1, n do sum = sum + ai[k] * cj[k] end
			xi[j] = sum;
		end
	end
	return x;
end

function matrix.gen(n: integer)
	local a: table, tmp: number = {}, 1. / n / n;
	for i = 1, n do
		local ai: number[] = table.numarray(n, 0.0)
		a[i] = ai
		for j = 1, n do
			ai[j] = tmp * (i - j) * (i + j - 2)
		end
	end
	return a;
end
return matrix
]]
)
assert(f and type(f) == 'function')
matrix = f()
assert(matrix and type(matrix) == 'table')
local n = 1000;
n = math.floor(n/2) * 2;
local t1 = os.clock()
local a = matrix.mul(matrix.gen(n), matrix.gen(n))
local t2 = os.clock()
print("time taken ", t2-t1)
print(a[n/2+1][n//2+1])
print 'Ok'