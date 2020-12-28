f = compiler.load([[
function doit(a: integer, what)
    local f: integer = ~0
    if what == '&' then
        return a&f
    elseif what == '|' then
        return a|f
    elseif what == '<<' then
        return a<<1
    elseif what == '>>' then
        return a>>1
    elseif what == '~' then
        return ~a
    else
        return 0
    end
end
]]
)
assert(f and type(f) == 'function')
f()
assert(doit and type(doit) == 'function')
assert(math.abs(doit(16, '&') - 16) < 1e-15)
assert(math.abs(doit(16, '|') - ~0) < 1e-15)
assert(math.abs(doit(16, '<<') - 32) < 1e-15)
assert(math.abs(doit(16, '>>') - 8) < 1e-15)
assert(math.abs(doit(16, '~') - ~16) < 1e-15)
print 'Ok'