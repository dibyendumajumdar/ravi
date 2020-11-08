local sieve = compiler.load([[
	local i: integer, k: integer, prime: integer, count: integer
	local flags: integer[] = table.intarray(8190)

    local iter: integer = 0
	while iter <= 100000  do
		count = 0
		i = 0
		while i <= 8190 do
			flags[i] = 1
			i = i + 1
		end
		i = 0
		while i <= 8190 do
			if flags[i] == 1 then
				prime = i + i + 3;
				k = i + prime
				while k <= 8190 do
					flags[k] = 0
					k = k + prime
				end
				count = count + 1
			end
			i = i + 1
		end
		iter = iter + 1
	end
	return count
]]
)
assert(sieve and type(sieve) == 'function')

local t1 = os.clock()
local count = sieve()
local t2 = os.clock()
print("time taken ", t2-t1)
print(count)
assert(count == 1899)
