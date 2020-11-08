f = compiler.load([[
function doit(a: number, what)
    if what == 'add' then
        return a+5
    elseif what == 'sub' then
        return a-5
    elseif what == 'mul' then
        return a*5
    elseif what == 'div' then
        return a/5
    else
        return 0
    end
end
]]
)
assert(f and type(f) == 'function')
f()
assert(doit and type(doit) == 'function')
assert(math.abs(doit(11.1, 'add') - 16.1) < 1e-15)
assert(math.abs(doit(1.1, 'mul') - 5.5) < 1e-15)
assert(math.abs(doit(5.5, 'sub') - 0.5) < 1e-15)
assert(math.abs(doit(5.5, 'div') - 1.1) < 1e-15)

f = compiler.load([[
function doit(a: number, what)
    if what == 'add' then
        return 5+a
    elseif what == 'sub' then
        return 5-a
    elseif what == 'mul' then
        return 5*a
    elseif what == 'div' then
        return 5/a
    else
        return 0
    end
end
]]
)
assert(f and type(f) == 'function')
f()
assert(doit and type(doit) == 'function')
assert(math.abs(doit(11.1, 'add') - 16.1) < 1e-15)
assert(math.abs(doit(1.1, 'mul') - 5.5) < 1e-15)
assert(math.abs(doit(5.5, 'sub') + 0.5) < 1e-15)
assert(math.abs(doit(2.5, 'div') - 2.0) < 1e-15)