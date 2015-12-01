local function x()
  local j = 0.0
  for i=2.0,1000000000.0 do
  	j = i
  	for k=3,5 do
  		j = k
  	end
  end
  return j
end

if ravi then
  ravi.compile(x)
end
if jit then
  -- LuaJIT - warmup
  x()
end

local t1 = os.clock()
local y = x(); 
local t2 = os.clock()
print(y)

assert(y == 5.0)
print("time taken ", t2-t1); 

