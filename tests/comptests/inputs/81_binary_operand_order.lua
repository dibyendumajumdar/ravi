-- When the first operand of a binary operator is a table field, an array element
-- or a global, its value must be loaded before the second operand is evaluated,
-- as evaluating the second operand can change it.
-- See Bug-20 in readthedocs/COMPILER_ISSUES.md

-- The value is changed via an upvalue reference to the table
local function arith_field()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 10 end
    local r = {}
    r[1] = t.x + change()   -- 1 + 10
    r[2] = t.x - change()   -- 2 - 10
    r[3] = t.x * change()   -- 3 * 10
    r[4] = t.x // change()  -- 4 // 10
    r[5] = t.x % change()   -- 5 % 10
    r[6] = t.x / change()   -- 6 / 10
    r[7] = t.x & change()   -- 7 & 10
    r[8] = t.x | change()   -- 8 | 10
    r[9] = t.x ~ change()   -- 9 ~ 10
    return r
end
do
    local r = arith_field()
    assert(r[1] == 11)
    assert(r[2] == -8)
    assert(r[3] == 30)
    assert(r[4] == 0)
    assert(r[5] == 5)
    assert(r[6] == 0.6)
    assert(r[7] == 2)
    assert(r[8] == 10)
    assert(r[9] == 3)
end

local function shift_field()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 2 end
    local a = t.x << change()  -- 1 << 2
    local b = t.x >> change()  -- 2 >> 2
    return a, b
end
do
    local a, b = shift_field()
    assert(a == 4 and b == 0)
end

-- The value is changed via the table being passed as an argument
local function arith_field_arg()
    local t = { x = 1 }
    local function bump(u) u.x = u.x + 1 return 10 end
    return t.x + bump(t)
end
assert(arith_field_arg() == 11)

-- Method call on the right, a common pattern
local function method_on_right()
    local obj = { pos = 1 }
    function obj:advance() self.pos = self.pos + 1 return 10 end
    return obj.pos + obj:advance()
end
assert(method_on_right() == 11)

-- Integer key
local function arith_index()
    local t = { 1 }
    local function change() t[1] = t[1] + 1 return 10 end
    local i = 1
    return t[1] + change(), t[i] + change()
end
do
    local a, b = arith_index()
    assert(a == 11 and b == 12)
end

-- Global
gv81 = 1
local function arith_global()
    local function change() gv81 = gv81 + 1 return 10 end
    local a = gv81 + change()
    local b = gv81 - change()
    return a, b
end
do
    local a, b = arith_global()
    assert(a == 11 and b == -8)
    gv81 = nil
end

-- Comparison operators used as values
local function compare_field()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 5 end
    local r = {}
    r[1] = t.x == change() - 4   -- 1 == 1
    r[2] = t.x ~= change() - 3   -- 2 ~= 2
    r[3] = t.x < change() - 1    -- 3 < 4
    r[4] = t.x <= change() - 1   -- 4 <= 4
    r[5] = t.x > change() - 1    -- 5 > 4
    r[6] = t.x >= change() + 1   -- 6 >= 6
    r[7] = t.x > change() + 2    -- 7 > 7
    r[8] = t.x < change() + 3    -- 8 < 8
    return r
end
do
    local r = compare_field()
    assert(r[1] == true)
    assert(r[2] == false)
    assert(r[3] == true)
    assert(r[4] == true)
    assert(r[5] == true)
    assert(r[6] == true)
    assert(r[7] == false)
    assert(r[8] == false)
end

local function compare_global()
    gv81 = 1
    local function change() gv81 = gv81 + 1 return 5 end
    local a = gv81 == change() - 4   -- 1 == 1
    local b = gv81 > change() - 4    -- 2 > 1
    gv81 = nil
    return a, b
end
do
    local a, b = compare_global()
    assert(a == true and b == true)
end

-- Comparison operators used as conditions
local function condition_if()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 2 end
    if t.x < change() then   -- 1 < 2
        return 'lt'
    else
        return 'ge'
    end
end
assert(condition_if() == 'lt')

local function condition_if_swapped()
    local t = { x = 3 }
    local function change() t.x = t.x - 1 return 2 end
    if t.x > change() then   -- 3 > 2
        return 'gt'
    end
    return 'le'
end
assert(condition_if_swapped() == 'gt')

local function condition_while()
    local t = { x = 1 }
    local n = 0
    local function change() t.x = t.x + 1 return n + 1 end
    while t.x == change() do  -- 1 == 1, then 2 == 2, then 3 == 3 ..
        n = n + 1
        if n == 3 then break end
    end
    return n
end
assert(condition_while() == 3)

local function condition_repeat()
    local t = { x = 1 }
    local n = 0
    local function change() t.x = t.x + 1 return 1 end
    repeat
        n = n + 1
    until t.x ~= change() or n == 5   -- 1 ~= 1 is false, 2 ~= 1 is true
    return n
end
assert(condition_repeat() == 2)

-- Typed arrays
local function typed_integer_array()
    local a: integer[] = { 1 }
    local function change() a[1] = a[1] + 1 return 10 end
    local x = a[1] + change()   -- 1 + 10
    local y = a[1] < change() - 8   -- 2 < 2
    return x, y
end
do
    local x, y = typed_integer_array()
    assert(x == 11 and y == false)
end

local function typed_number_array()
    local a: number[] = { 1.0 }
    local function change() a[1] = a[1] + 1.0 return 10.0 end
    local x = a[1] * change()   -- 1.0 * 10.0
    local y = a[1] < change() - 7.5   -- 2.0 < 2.5
    return x, y
end
do
    local x, y = typed_number_array()
    assert(x == 10.0 and y == true)
end

-- Nested and chained operators
local function nested()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 10 end
    return t.x + (t.x + change())   -- 1 + (1 + 10)
end
assert(nested() == 12)

local function chained()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 10 end
    return t.x + change() + t.x   -- (1 + 10) + 2
end
assert(chained() == 13)

local function chained_three()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 10 end
    return t.x * change() - t.x * change()   -- 1 * 10 - 2 * 10
end
assert(chained_three() == -10)

-- Binary operator inside other constructs
local function in_store()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 10 end
    local r = {}
    r.v = t.x * change()
    return r.v
end
assert(in_store() == 10)

local function in_call_arg()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 10 end
    local function id(v) return v end
    return id(t.x + change())
end
assert(in_call_arg() == 11)

local function in_fornum_step()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 0 end
    local n = 0
    for i = 1, 6, t.x + change() do   -- step is 1 + 0
        n = n + 1
    end
    return n
end
assert(in_fornum_step() == 6)

-- The value is changed by a metamethod when the second operand is evaluated
local function via_metamethod()
    local t = { x = 1 }
    local o = setmetatable({}, { __index = function(_, k) t.x = t.x + 1 return 10 end })
    local function id(v) return v end
    return t.x + id(o.y)
end
assert(via_metamethod() == 11)

-- Controls: behaviour that must stay as it is

local function no_call()
    local t = { x = 1, y = 2 }
    return t.x + t.y, t.x + 1, 1 + t.x, t.x < t.y, t.x == 1
end
do
    local a, b, c, d, e = no_call()
    assert(a == 3 and b == 2 and c == 2 and d == true and e == true)
end

-- both operands indexed, second one runs a metamethod; loads are in operand order
local function both_indexed()
    local t = { x = 1 }
    local o = setmetatable({}, { __index = function(_, k) t.x = t.x + 1 return 10 end })
    return t.x + o.y
end
assert(both_indexed() == 11)

-- the second operand is evaluated after the first
local function second_operand_sees_change()
    local t = { x = 1 }
    local function change() t.x = t.x + 1 return 10 end
    return change() + t.x
end
assert(second_operand_sees_change() == 12)

-- a local as the first operand is read after the second operand
-- has been evaluated, as in Lua
local function local_first_operand()
    local a = 1
    local function change() a = a + 1 return 10 end
    return a + change()
end
assert(local_first_operand() == 12)

print '81 Ok'
