function unlpack (t, i)
  i = i or 1
  if (i <= #t) then
    return t[i], unlpack(t, i+1)
  end
end

function equaltab (t1, t2)
  assert(#t1 == #t2)
  local n: integer = #t1
  for i = 1, n do
    assert(t1[i] == t2[i])
  end
end

local pack = table.pack

function f() return 1,2,30,4 end
function ret2 (a,b) return a,b end

local a,b,c,d = unlpack{1,2,3}
--print(a,b,c,d)
assert(a==1 and b==2 and c==3 and d==nil)
a = {1,2,3,4,false,10,'alo',false,assert}
equaltab(pack(unlpack(a)), a)
equaltab(pack(unlpack(a), -1), {1,-1})
a,b,c,d = ret2(f()), ret2(f())
assert(a==1 and b==1 and c==2 and d==nil)
a,b,c,d = unlpack(pack(ret2(f()), ret2(f())))
assert(a==1 and b==1 and c==2 and d==nil)
-- a,b,c,d = unlpack(pack(ret2(f()), (ret2(f()))))
-- assert(a==1 and b==1 and c==nil and d==nil)
--
-- a = ret2{ unlpack{1,2,3}, unlpack{3,2,1}, unlpack{"a", "b"}}
-- assert(a[1] == 1 and a[2] == 3 and a[3] == "a" and a[4] == "b")

print '68 Ok'