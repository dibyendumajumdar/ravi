-- source https://github.com/LuaJIT/LuaJIT-test-cleanup/blob/master/test/lang/andor.lua

do --- smoke
  local x = ((1 or false) and true) or false
  assert(x == true)
end

print '74 Ok'