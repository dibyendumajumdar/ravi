t = nil   -- 'declare' t
function f(a,b,c) local d = 'a'; t={a,b,c,d} end

f(      -- this line change must be valid
  1,2)
assert(t[1] == 1 and t[2] == 2 and t[3] == nil and t[4] == 'a')
f(1,2,   -- this one too
      3,4)
assert(t[1] == 1 and t[2] == 2 and t[3] == 3 and t[4] == 'a')

print '64 Ok'