local function x()
  local j = 0.0
  for i=2.0,1000000000.0 do
  	j = i
  end
  return j
end

x()

local t1 = os.clock()
local y = x(); 
local t2 = os.clock()
print(y)

assert(y == 1000000000.0)
print("time taken ", t2-t1); 

