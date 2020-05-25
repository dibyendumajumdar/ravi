local function get(t)
    local sum = 0
    for i = 1,1000000 do
        sum = sum + t.a + t.b
    end
    return sum 
end

local function get2(t, x, y)
    local sum = 0
    for i = 1,1000000 do
        sum = sum + t[x] + t[y]
    end
    return sum 
end

local function get3(t: table)
    local sum = 0
    for i = 1,1000000 do
        sum = sum + t.a + t.b
    end
    return sum 
end

local module = { get, get2, get3 }
ravi.compile(module)

local t = {}
t.a = 151
t.b = 152

local t1 = os.clock()
get(t)
local t2 = os.clock()

print('Time taken by get()', t2-t1)

t1 = os.clock()
get2(t, 'a', 'b')
t2 = os.clock()

print('Time taken by get2()', t2-t1)

t1 = os.clock()
get3(t)
t2 = os.clock()

print('Time taken by get3()', t2-t1)

-- ravi.dumplua(get)
-- ravi.dumplua(get2)
-- ravi.dumplua(get3)
-- ravi.dumpllvm(get)