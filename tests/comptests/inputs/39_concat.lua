f = compiler.load([[
  local y
  for i=1,100 do y = "a".."b" end
  return y
]]
)
assert(f and type(f) == 'function')

local s = f()
assert(s == "ab")
print 'Ok'