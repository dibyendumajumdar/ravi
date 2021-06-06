-- Test 7 from language tests
f = compiler.load([[
function doit(y: integer)
    if y < 1 then
      return 1.0
    elseif y >= 5 then
      return 2.0
    else
      return 3.0
    end
end
]]
)
assert(f and type(f) == 'function')
f()
assert(doit and type(doit) == 'function')
assert(doit(5) == 2.0)
assert(doit(4) == 3.0)
print 'Ok'