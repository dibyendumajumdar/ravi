-- Copyright vnmakarov see https://github.com/vnmakarov/mir/issues/2
local function sieve()
	local i: integer, k: integer, prime: integer, count: integer
	local flags: integer[] = table.intarray(8190)

	for iter=0,100000  do
		count = 0
		for i=0,8190 do 
			flags[i] = 1
		end
		for i=0,8190 do
			if flags[i] == 1 then
				prime = i + i + 3;
				for k = i + prime, 8190, prime do
					flags[k] = 0
				end
				count = count + 1
			end
		end
	end
	return count
end

if ravi.jit() then
	print('JIT ON')
	ravi.optlevel(2)
	ravi.compile(sieve, {omitArrayGetRangeCheck=1})
end
-- ravi.dumpir(sieve)

local t1 = os.clock()
local count = sieve()
local t2 = os.clock()
print("time taken ", t2-t1)
print(count)
assert(count == 1899)
