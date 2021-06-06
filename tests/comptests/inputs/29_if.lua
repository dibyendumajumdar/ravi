-- Test 5 from language tests
f = compiler.load([[
function doit()
    if 1 == 2 then
      return 5.0
    end
    return 1.0
end
]]
)
assert(f and type(f) == 'function')
f()
assert(doit and type(doit) == 'function')
assert(doit() == 1.0)
print 'Ok'