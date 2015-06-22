-- TEST OP_RETURN
x=function() return; end
ravi.dumplua(x)
assert(ravi.compile(x))
assert(not x())
print('test 1 ok')

-- TEST OP_RETURN and OP_LOADK
x=function() return 42; end
ravi.dumplua(x)
assert(ravi.compile(x))
assert(x() == 42)
print('test 2 ok')

-- Test OP_RETURN, OP_LOADK, OP_MOVE
-- OP_RAVI_FORPREP_I1, OP_RAVI_FORLOOP_I1
x=function() 
  local j = 0
  for i=1,1234 do
    j = i
  end
  return j
end
ravi.dumplua(x)
assert(ravi.compile(x))
assert(x() == 1234)
print('test 3 ok')

-- Test OP_RETURN, OP_LOADK, OP_MOVE
-- OP_RAVI_FORPREP_I1, OP_RAVI_FORLOOP_I1
-- OP_RAVI_ADDFN, OP_RAVI_LOADFZ
x=function() 
  local j:number 
  for i=1,1234 do
    j = j + 1
  end
  return j
end
ravi.dumplua(x)
assert(ravi.compile(x))
assert(x() == 1234.0)
print('test 4 ok')

-- Test OP_CALL
y=function() return x(); end
ravi.dumplua(y)
assert(ravi.compile(y))
assert(y() == 1234.0)
print('test 5 ok')

x=function(a,b)
  if a == b then 
    return "foo" 
  else 
    return "bar" 
  end
end

ravi.dumplua(x)
assert(ravi.compile(x))
assert(x(1,1) == "foo")
assert(x(1,2) == "bar")
print('test 6 ok')

x=function(a,b)
  if a ~= b then 
    return "foo" 
  else 
    return "bar" 
  end
end

ravi.dumplua(x)
assert(ravi.compile(x))
assert(x(1,1) == "bar")
assert(x(1,2) == "foo")
print('test 7 ok')




