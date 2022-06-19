local function g()
    return 1,2,3
end
local function f(a,b,c)
    return 0,a,b,c,4
end

local a,b,c,d,e = f(g())
assert(a == 0)
assert(b == 1)
assert(c == 2)
assert(d == 3)
assert(e == 4)