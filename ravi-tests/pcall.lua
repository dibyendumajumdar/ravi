-- This is a copy of snippet from
-- calls.lua 

local function rais(n, f)
  --local x = n == 0 and error()
  --rais(n-1)
  print("called")
  if n == 0 then f()
  else rais(n-1, f)
  end
  print("normal exit")
end
ravi.dumplua(rais)
ravi.compile(rais)

function caller(n)
  if n > 0 then
    pcall(rais,n,error)
    caller(n-1)
  end
end
ravi.compile(caller)

caller(10)
print("OK")