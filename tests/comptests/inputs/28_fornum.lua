-- Test 3 from language tests
f = compiler.load([[
function doit()
    local j:number = 1.5
    for i=1,10 do
      j = j+1
    end
    return j
end
]]
)
assert(f and type(f) == 'function')
f()
assert(doit and type(doit) == 'function')
assert(doit() == 11.5)
print 'Ok'