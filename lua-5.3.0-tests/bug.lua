function x()
  local IX
  if ((10 or true) and false) then IX = true end; return ((10 or true) and false)
end

--ravi.dumplua(x)
ravi.compile(x,1)
assert(x() == false)