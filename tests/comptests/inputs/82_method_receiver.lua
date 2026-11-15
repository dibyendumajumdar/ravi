-- In a method call o:m(args) the method is looked up in o, and o is passed as 'self'.
-- Both must use the value o had before the arguments were evaluated, even if
-- evaluating the arguments changes o.
-- See Bug-21 in readthedocs/COMPILER_ISSUES.md

local function make(name)
    local o = { n = name }
    function o:m(x) return self.n end
    function o:two(x, y) return self.n, x, y end
    return o
end

-- receiver is a local that is changed via a closure
local function local_receiver()
    local o1, o2 = make('o1'), make('o2')
    local o = o1
    local function change() o = o2 return 0 end
    return o:m(change())
end
assert(local_receiver() == 'o1')

-- the new value is seen by a later call
local function local_receiver_then_again()
    local o1, o2 = make('o1'), make('o2')
    local o = o1
    local function change() o = o2 return 0 end
    local a = o:m(change())
    local b = o:m(0)
    return a, b
end
do
    local a, b = local_receiver_then_again()
    assert(a == 'o1' and b == 'o2')
end

-- receiver is a parameter
local function param_receiver(o, other)
    local function change() o = other return 0 end
    return o:m(change())
end
assert(param_receiver(make('p1'), make('p2')) == 'p1')

-- receiver is an upvalue
local function upvalue_receiver()
    local o1, o2 = make('o1'), make('o2')
    local o = o1
    local function change() o = o2 return 0 end
    local function f() return o:m(change()) end
    return f()
end
assert(upvalue_receiver() == 'o1')

-- several arguments; only a later one changes the receiver
local function several_args()
    local o1, o2 = make('o1'), make('o2')
    local o = o1
    local function change() o = o2 return 'c' end
    local n, x, y = o:two('a', change())
    return n, x, y
end
do
    local n, x, y = several_args()
    assert(n == 'o1' and x == 'a' and y == 'c')
end

-- string receiver
local function string_receiver()
    local s = 'abc'
    local function change() s = 'xyz' return 2 end
    return s:sub(change())
end
assert(string_receiver() == 'bc')

-- the receiver is changed by a metamethod while an argument is evaluated
local function via_metamethod()
    local o1, o2 = make('o1'), make('o2')
    local o = o1
    local t = setmetatable({}, { __index = function(_, k) o = o2 return 0 end })
    return o:m(t.x)
end
assert(via_metamethod() == 'o1')

-- chained method calls
local function chained()
    local o1, o2 = make('o1'), make('o2')
    function o1:next(x) return o2 end
    local o = o1
    local function change() o = o2 return 0 end
    return o:next(change()):m(0)
end
assert(chained() == 'o2')

-- Controls: receivers that were already evaluated to a value

local function field_receiver()
    local o1, o2 = make('o1'), make('o2')
    local h = { o = o1 }
    local function change() h.o = o2 return 0 end
    return h.o:m(change())
end
assert(field_receiver() == 'o1')

gv82 = make('g1')
local function global_receiver()
    local function change() gv82 = make('g2') return 0 end
    return gv82:m(change())
end
assert(global_receiver() == 'g1')
gv82 = nil

local function call_receiver()
    local o1, o2 = make('o1'), make('o2')
    local cur = o1
    local function get() return cur end
    local function change() cur = o2 return 0 end
    return get():m(change())
end
assert(call_receiver() == 'o1')

local function plain()
    local o = make('o')
    return o:m(1), o:m()
end
do
    local a, b = plain()
    assert(a == 'o' and b == 'o')
end

print '82 Ok'
