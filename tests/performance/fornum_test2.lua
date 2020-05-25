local function x()
  local j=0.0
  for i=1,1000000000 do
  	j = j+1.0
  	end
  return j
end

if ravi then
  ravi.optlevel(2)
  ravi.compile(x)
end

if jit then
  -- LuaJIT warmup
  x()
end

local t1 = os.clock()
local y = x(); 
local t2 = os.clock()
print(y)

assert(y == 1000000000.0)
print("time taken ", t2-t1); 

