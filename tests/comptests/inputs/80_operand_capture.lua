-- When a local or upvalue is used in a return list, as a function argument or as a
-- concatenation operand, its value must be taken when it is evaluated, and not
-- after later expressions in the list have been evaluated, as these can change it.
-- See Bug-19 in readthedocs/COMPILER_ISSUES.md

local function check(t, expected)
    assert(#t == #expected, 'expected ' .. #expected .. ' values but got ' .. #t)
    for i = 1, #expected do
        assert(t[i] == expected[i], 'value ' .. i .. ' expected ' .. tostring(expected[i]) .. ' got ' .. tostring(t[i]))
    end
end

local function pack(a, b, c, d, e)
    return {a, b, c, d, e}
end

-- return list, local captured by a closure
local function ret_local()
    local a = 1
    local function change() a = a + 1 return 10 end
    return a, change()
end
check({ret_local()}, {1, 10})

local function ret_local_paren()
    local a = 1
    local function change() a = a + 1 return 10 end
    return a, (change())
end
check({ret_local_paren()}, {1, 10})

local function ret_local_many()
    local a = 1
    local function change() a = a + 1 return 10 end
    return a, change(), a, change(), a
end
check({ret_local_many()}, {1, 10, 2, 10, 3})

local function ret_param(a)
    local function change() a = a + 1 return 10 end
    return a, change()
end
check({ret_param(1)}, {1, 10})

local function ret_typed_int()
    local a: integer = 1
    local function change() a = a + 1 return 10 end
    return a, change()
end
check({ret_typed_int()}, {1, 10})

local function ret_typed_num()
    local a: number = 1.5
    local function change() a = a + 1.0 return 10 end
    return a, change()
end
check({ret_typed_num()}, {1.5, 10})

-- return list, upvalue
local function ret_upvalue()
    local a = 1
    local function change() a = a + 1 return 10 end
    local function f() return a, change(), a end
    return f()
end
check({ret_upvalue()}, {1, 10, 2})

local function ret_typed_upvalue()
    local a: integer = 1
    local function change() a = a + 1 return 10 end
    local function f() return a, change(), a end
    return f()
end
check({ret_typed_upvalue()}, {1, 10, 2})

-- function arguments
local function args_local()
    local a = 1
    local function change() a = a + 1 return 10 end
    return pack(a, change(), a, change(), a)
end
check(args_local(), {1, 10, 2, 10, 3})

local function args_upvalue()
    local a = 1
    local function change() a = a + 1 return 10 end
    local function f() return pack(a, change(), a, change(), a) end
    return f()
end
check(args_upvalue(), {1, 10, 2, 10, 3})

local function args_const_first()
    local a = 1
    local function change() a = a + 1 return 10 end
    return pack('x', a, change(), a)
end
check(args_const_first(), {'x', 1, 10, 2})

local function args_nested()
    local a = 1
    local function change() a = a + 1 return 10 end
    local function first(x, y) return x end
    return first(a, first(change(), 0))
end
assert(args_nested() == 1)

local function args_library()
    local a = 1
    local function change() a = a + 1 return 10 end
    return string.format('%d %d %d', a, change(), a)
end
assert(args_library() == '1 10 2')

-- concatenation
local function concat_local()
    local a = 1
    local function change() a = a + 1 return 10 end
    return a .. change() .. a
end
assert(concat_local() == '1102')

local function concat_upvalue()
    local a = 1
    local function change() a = a + 1 return 10 end
    local function f() return a .. change() .. a end
    return f()
end
assert(concat_upvalue() == '1102')

-- the value is changed by a metamethod rather than a direct call
local function via_index_metamethod()
    local a = 1
    local mt = setmetatable({}, { __index = function(t, k) a = a + 1 return 10 end })
    return pack(a, mt.x, a)
end
check(via_index_metamethod(), {1, 10, 2})

local function via_arith_metamethod()
    local a = 1
    local o = setmetatable({}, { __add = function(x, y) a = a + 1 return 10 end })
    return a, o + 1, a
end
check({via_arith_metamethod()}, {1, 10, 2})

-- upvalue as the first operand of a binary operator: the value is taken before
-- the second operand is evaluated
local function binary_upvalue()
    local a = 1
    local function change() a = a + 1 return 10 end
    local function add() return a + change() end
    local function sub() return a - change() end
    local function band() return a & change() end
    local function eq() return a == change() - 7 end
    local function lt() return a < change() - 6 end
    return pack(add(), sub(), band(), eq(), lt())
end
-- a is 1, 2, 3, 4, 5 at the start of each function
check(binary_upvalue(), {11, -8, 2, false, false})

local function binary_typed_upvalue()
    local a: integer = 1
    local function change() a = a + 1 return 10 end
    local function add() return a + change() end
    return add()
end
assert(binary_typed_upvalue() == 11)

-- Controls: behaviour that must stay as it is

-- a local that is the first operand of a binary operator is read after
-- the second operand has been evaluated, as in Lua
local function binary_local()
    local a = 1
    local function change() a = a + 1 return 10 end
    return a + change()
end
assert(binary_local() == 12)

-- locals that are not captured by any closure
local function plain_locals()
    local a, b, c = 'A', 'B', 'C'
    local function g() return 10 end
    return c, b, a, g()
end
check({plain_locals()}, {'C', 'B', 'A', 10})

-- captured locals, but nothing later in the list can change them
local function captured_no_calls()
    local a, b, c = 'A', 'B', 'C'
    local function f() return a .. b .. c end
    return c, b, a, f
end
do
    local t = {captured_no_calls()}
    assert(t[1] == 'C' and t[2] == 'B' and t[3] == 'A' and t[4]() == 'ABC')
end

-- last expression changes nothing that precedes it being read
local function last_is_symbol()
    local a = 1
    local function change() a = a + 1 return 10 end
    return change(), a
end
check({last_is_symbol()}, {10, 2})

print '80 Ok'
