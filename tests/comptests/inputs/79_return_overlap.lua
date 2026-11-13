-- Return values are copied to the stack starting at the function's slot, which overlaps
-- the registers of the function. These tests check that values being returned are
-- not overwritten before they are copied.
-- See Bug-1 in readthedocs/COMPILER_ISSUES.md

local function check(t, expected)
    assert(#t == #expected, 'expected ' .. #expected .. ' values but got ' .. #t)
    for i = 1, #expected do
        assert(t[i] == expected[i], 'value ' .. i .. ' expected ' .. tostring(expected[i]) .. ' got ' .. tostring(t[i]))
    end
end

-- locals returned in a different order to their registers
local function reversed()
    local a, b, c = 'A', 'B', 'C'
    return c, b, a
end
check({reversed()}, {'C', 'B', 'A'})

local function rotated()
    local a, b, c = 'A', 'B', 'C'
    return b, c, a
end
check({rotated()}, {'B', 'C', 'A'})

local function reversed5()
    local a, b, c, d, e = 1, 2, 3, 4, 5
    return e, d, c, b, a
end
check({reversed5()}, {5, 4, 3, 2, 1})

local function inorder()
    local a, b, c = 'A', 'B', 'C'
    return a, b, c
end
check({inorder()}, {'A', 'B', 'C'})

local function duplicated()
    local a, b = 'A', 'B'
    return b, b, a, a, b, a
end
check({duplicated()}, {'B', 'B', 'A', 'A', 'B', 'A'})

-- parameters
local function params_shifted(a, b, c)
    return 0, 0, a, b, c
end
check({params_shifted(1, 2, 3)}, {0, 0, 1, 2, 3})

local function params_reversed(a, b, c)
    return c, b, a
end
check({params_reversed(1, 2, 3)}, {3, 2, 1})

local function params_swapped(a, b)
    return 0, b, a
end
check({params_swapped(1, 2)}, {0, 2, 1})

-- constants followed by a temp on the Lua stack, function has no locals on the stack
local holder = { x = 'X', y = 'Y' }
local function consts_then_temp()
    return 'p', 'q', holder.x
end
check({consts_then_temp()}, {'p', 'q', 'X'})

local function consts_then_temps()
    return 1.5, 2.5, 3, holder.x, holder.y
end
check({consts_then_temps()}, {1.5, 2.5, 3, 'X', 'Y'})

-- typed temps followed by a temp on the Lua stack
local function typed_then_temp()
    local a: number, b: number = 7.5, 2.0
    return a - b, a * b, a // b
end
check({typed_then_temp()}, {5.5, 15.0, 3.0})

local function typed_ints_then_temp(t)
    local a: integer, b: integer = 7, 3
    return a + b, a - b, a * b, t.x
end
check({typed_ints_then_temp(holder)}, {10, 4, 21, 'X'})

-- trailing function call results that have to be moved up the stack
local function none() end
local function one() return 7 end
local function two() return 7, 8 end
local function five() return 7, 8, 9, 10, 11 end

local function range_up_none() return 1, 2, 3, 4, none() end
check({range_up_none()}, {1, 2, 3, 4})

local function range_up_one() return 1, 2, 3, 4, one() end
check({range_up_one()}, {1, 2, 3, 4, 7})

local function range_up_two() return 1, 2, 3, 4, two() end
check({range_up_two()}, {1, 2, 3, 4, 7, 8})

local function range_up_five() return 1, 2, 3, 4, five() end
check({range_up_five()}, {1, 2, 3, 4, 7, 8, 9, 10, 11})

local function range_up_with_locals(a, b)
    return b, a, 3, 4, 5, b, a, five()
end
check({range_up_with_locals('A', 'B')}, {'B', 'A', 3, 4, 5, 'B', 'A', 7, 8, 9, 10, 11})

local function range_up_with_temps()
    return 'p', 'q', holder.x, holder.y, 'r', 's', two()
end
check({range_up_with_temps()}, {'p', 'q', 'X', 'Y', 'r', 's', 7, 8})

-- range moved up far enough that the stack may need to grow
local function range_up_far()
    return 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
           21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
           41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
           five()
end
do
    local t = {range_up_far()}
    assert(#t == 65)
    for i = 1, 60 do assert(t[i] == i) end
    for i = 61, 65 do assert(t[i] == i - 54) end
end

-- trailing function call results that stay in place or move down
local function range_only() return five() end
check({range_only()}, {7, 8, 9, 10, 11})

local function range_after_one(a) return a, five() end
check({range_after_one('A')}, {'A', 7, 8, 9, 10, 11})

local function range_after_two(a, b) return b, a, two() end
check({range_after_two('A', 'B')}, {'B', 'A', 7, 8})

local function range_down(a, b, c, d)
    local e, f = 5, 6
    return two()
end
check({range_down(1, 2, 3, 4)}, {7, 8})

-- caller wants fewer or more values than returned
do
    local x = reversed()
    assert(x == 'C')
    local p, q = reversed()
    assert(p == 'C' and q == 'B')
    local a, b, c, d, e = reversed()
    assert(a == 'C' and b == 'B' and c == 'A' and d == nil and e == nil)
    local r1, r2, r3, r4, r5, r6, r7, r8 = range_up_two()
    assert(r1 == 1 and r2 == 2 and r3 == 3 and r4 == 4 and r5 == 7 and r6 == 8 and r7 == nil and r8 == nil)
    local s1, s2, s3, s4, s5 = range_up_five()
    assert(s1 == 1 and s2 == 2 and s3 == 3 and s4 == 4 and s5 == 7)
    local u1, u2, u3 = params_shifted(1, 2, 3)
    assert(u1 == 0 and u2 == 0 and u3 == 1)
end

-- returned values that are captured as upvalues
local function with_closure()
    local a, b, c = 'A', 'B', 'C'
    local function f() return a .. b .. c end
    return c, b, a, f
end
do
    local t = {with_closure()}
    assert(t[1] == 'C' and t[2] == 'B' and t[3] == 'A' and t[4]() == 'ABC')
end

print '79 Ok'
