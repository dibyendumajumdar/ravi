local z,x,y,tryme

-- test 1
do
  local z = function(a)
    return a+1
  end
  local x = function(f)
    local j = 5
    j = f(j)
    return j
  end
  local y = x(z)
  assert(x(z) == 6 and y == 6)
end
print("Test 1 OK")

-- test 2
do
  local x = function ()
    local i, j:integer
    j=0
    for i=1,10 do
  	  j = j+1
    end
    return j
  end
  assert(x() == 10)
end
print("Test 2 OK")

-- test 3
do
  local x = function ()
    local j:number
    for i=1,10 do
      j = j+1
    end
    return j
  end
  assert(x() == 10.0)
end
print("Test 3 OK")

-- test 4
do
  local x = function ()
    local j = 0
    for i=2,6,3 do
      j = i
    end
    return j
  end
  assert(x() == 5)
end
print("Test 4 OK")

-- test 5
do
  local x = function ()
    if 1 == 2 then
      return 5.0
    end
    return 1.0
  end
  assert(x() == 1.0)
end
print("Test 5 OK")

-- test 6
do
  local x = function (a: integer, b: integer)
    if a == b then
      return 5.0
    end
    return 1.0
  end
  assert(x(1,2) == 1.0)
end
print("Test 6 OK")

-- test 7
do
  local x = function (y: integer)
    if y < 1 then
      return 1.0
    elseif y >= 5 then
      return 2.0
    else
      return 3.0
    end
  end
  assert(x(5) == 2.0)
  assert(x(4) == 3.0)
end
print("Test 7 OK")

-- test 8
do
  local x = function (y: number)
    if y < 1.0 then
      return 1.0
    elseif y >= 5.0 then
      return 2.0
    else
      return 3.0
    end
  end
  assert(x(5.1) == 2.0)
  assert(x(4.0) == 3.0)
end
print("Test 8 OK")

-- test 9
do
  local x = function (y: integer, z)
    if y == 1 then
      if z == 1 then
        return 99.0
      else
        return z
      end
    elseif y >= 5 then
      return 2.0
    else
      return 3.0
    end
  end
  assert(x(1,1) == 99.0)
  assert(x(1,2) == 2)
  assert(x(1,5.3) == 5.3)
  assert(x(5) == 2.0)
  assert(x(4) == 3.0)
end
print("Test 9 OK")

-- test 10
do
  local x = function()
    local function tryme()
      local i,j = 5,6
      return i,j
    end
    local i:integer, j:integer = tryme()
    assert(i+j == 11)
  end
  x()
end
print("Test 10 OK")

-- test 11
do
  local function x()
    local a : number[], j:number = {}
    for i=1,10 do
      a[i] = i
      j = j + a[i]
    end
    return j
  end
  assert(x() == 55.0)
end
print("Test 11 OK")

-- test 12
do
  local function pisum()
    local sum : number
    for j = 1,500 do
        sum = 0.0
        for k = 1,10000 do
            sum = sum + 1.0/(k*k)
        end
    end
    return sum
  end
  assert(math.abs(pisum()-1.644834071848065) < 1e-12)
end
print("Test 12 OK")

-- test 13
do
  local function y()
    local i,j = 5.1,6.2
    return i,j
  end
  local function x(f)
    local i:number, j:number = f()
    return i+j
  end
  assert(math.abs(x(y) - 11.3) < 0.0001)
end
print("Test 13 OK")

-- test 14
do
  local function tryme(x,y)
    if x < y then
      return 1
    else
      return 0
    end
  end
  assert(tryme(1,2) == 1)
  assert(tryme(2,1) == 0)
end
print("Test 14 OK")

-- test 15
do
  local function tryme(x,y)
    return x < y
  end
  assert(tryme(1,2))
  assert(not tryme(2,1))
end
print("Test 15 OK")

-- test 16
do
  local function tabtest(x)
    x[1] = 5
    return x[1]
  end
  assert(tabtest({}) == 5)
end
print("Test 16 OK")

-- test 17
do
  local function optest()
    local a,b,c = 1, 5
    c = a and b
    return c
  end
  assert(optest() == 5)
end
print("Test 17 OK")

-- test 18
do
  local function optest()
    local a,b,c = 1, 5
    c = a or b
    return c
  end
  assert(optest() == 1)
end
print("Test 18 OK")

-- test 19
do
  local function optest()
    local a,b = 1, 5
    if a and b then
      return b
    end
    return a
  end
  assert(optest() == 5)
end
print("Test 19 OK")

-- test 20
do
  local function optest()
    local a,b = nil, 5
    if a or b then
      return b
    end
    return a
  end
  assert(optest() == 5)
end
print("Test 20 OK")

-- test 21
do
  local y
  local z = function()
    local x=function()
      local j:number[] = {}
      return j
    end
    y=x()
    y[1] = 99.67
    assert(y[1], 99.67)
    assert(@integer (#y) == 1)
  end
  z()
end
print("Test 21 OK")

-- test 22
do
  local x
  local z = function()
    local days: table = {"Sunday", "Monday", "Tuesday", "Wednesday",
            "Thursday", "Friday", "Saturda"}
    assert(days[1] == 'Sunday')
    assert(#days == 7)
    assert(days[3] == 'Tuesday')
    days[7] = days[7] .. 'y'
    x = function()
      local t = ''
      for k,v in pairs(days) do
        t = t .. v
      end
      return t
    end
    assert(x() == "SundayMondayTuesdayWednesdayThursdayFridaySaturday")
  end
  z()
end
print("Test 22 OK")

-- test 23
do
  local x = function(a)
    return not a
  end
  local y = function(a)
    return a
  end
  assert(y(x()))
  assert(y(x(false)))
  assert(not y(x(true)))
end
print("Test 23 OK")

-- test 24
do
  local t = { name_ = "ravi" }
  function t:name()
    return self.name_
  end
  local function z(t)
    return t:name()
  end
  assert(z(t) == "ravi")
end
print("Test 24 OK")

-- test 25
do
  -- test ravi integer array
  local function f()
    local x: integer[] = { 1, 5 }
    local y: integer

    x[3] = x[1] + x[2]
    y = x[3]

    return y
  end
  assert(f() == 6)
end
print("Test 25 OK")

-- test 26
do
  local function f()
    local x: number[] = { 5.0, 6.1 }
    x[3] = x[1] + x[2]
    return x[3]
  end
  assert(math.abs(f()-11.1) < 1e-12)
end
print("Test 26 OK")

-- test 27
do
  -- Ravi arrays support for ipairs()
  -- Plus special slot at [0]
  local x = function()
    local nums: integer[] = {1, 2, 3, 4, 5, 6, 7}
    local t = 0
    assert(#nums == 7)
    nums[0] = 558
    for k,v in ipairs(nums) do
      t = t + v
    end
    assert(nums[0] == 558)
    return t
  end
  assert(x() == 28)
end
print("Test 27 OK")

-- test 28
do
  -- Ravi arrays support for pairs()
  -- Plus special slot at [0]
  local x = function()
    local nums: integer[] = {1, 2, 3, 4, 5, 6, 7}
    local t = 0
    assert(#nums == 7)
    nums[0] = 558
    for k,v in pairs(nums) do
      t = t + v
    end
    assert(nums[0] == 558)
    return t
  end
  assert(x() == 28)
end
print("Test 28 OK")

-- test 29
-- test creation of arrays and slice
do
  local x = function()
    local zeros: integer[] = table.intarray(10, 0)
    local ones: integer[] = table.intarray(10, 1)

    local a : integer[] = table.slice(ones, 1, 10)
    assert(#zeros == 10)
    assert(#ones == 10)
    assert(#a == 10)

    local i = 0
    for k,v in ipairs(a) do
      i = i + v
    end
    assert(i == 10)
    local y = table.slice(ones, 10, 1)
    y[1] = 115
    return ones[10] == 115
  end
  assert(x() == true)
end
print("Test 29 OK")

-- test 30
do
  local matrix = {}
  matrix.new = function (m, n)
    local t = {m, n, table.numarray(m*n, 0)}
    return t
  end

  matrix.getcol = function(m, col)
    local rows = m[1]
    local cols = m[2]
    local data = m[3]
    assert(col > 0 and col <= cols)
    return table.slice(data, (col-1)*rows+1, rows)
  end

  matrix.getdata = function(m)
    return m[3]
  end

  matrix.datalen = function(data)
    return #data
  end

  local x = function()
    local m = matrix.new(5,5)
    local data: number[] = matrix.getdata(m)
    local data_len: integer = matrix.datalen(data)
    for i = 1,data_len do
      data[i] = i
    end
    -- get third col
    local col3 = matrix.getcol(m, 3)
    -- should be 11,12,13,14,15
    local n = 0
    for k,v in ipairs(col3) do
      n = n + v
    end
    -- should be 65
    return n
  end

  assert(x() == 65)
end
print("Test 30 OK")

-- test 31
do
  local function testadd(a,b)
    return a+b
  end
  assert(testadd(1,1) == 2)
  assert(testadd(1.5,1.6) == 3.1)
  assert(testadd("1.5",1.6) == 3.1)
  assert(testadd("1.5","1.6") == 3.1)
end
print("Test 31 OK")

-- test 32
do
  local function testsub(a,b)
    return a-b
  end
  assert(testsub(1,1) == 0)
  assert(math.abs(testsub(1.5,1.6)-(-0.1)) < 1e-6)
  assert(math.abs(testsub("1.5",1.6)-(-0.1)) < 1e-6)
  assert(math.abs(testsub("1.5","1.6")-(-0.1)) < 1e-6)
end
print("Test 32 OK")

-- test 33
do
  local function testmul(a,b)
    return a*b
  end
  assert(testmul(2,2) == 4)
  assert(math.abs(testmul(1.5,1.6)-2.4) < 1e-12)
  assert(math.abs(testmul("1.5",1.6)-2.4) < 1e-12)
  assert(math.abs(testmul("1.5","1.6")-2.4) < 1e-12)
end
print("Test 33 OK")


-- test 34
do
  local function testdiv(a,b)
    return a/b
  end
  assert(testdiv(2,2) == 1.0)
  assert(math.abs(testdiv(1.5,1.6)-0.9375) < 1e-12)
  assert(math.abs(testdiv("1.5",1.6)-0.9375) < 1e-12)
  assert(math.abs(testdiv("1.5","1.6")-0.9375) < 1e-12)
end
print("Test 34 OK")


-- test 35
do
  -- this tests that within the for loop
  -- the locals get mapped correctly to upvalues
  local function f()
    local a={}
    for i=1,2 do
      local y:integer = 0
      a[i] = function()
        y = y + 10
        return y;
      end
    end
    return a
  end
  local t = f()
  assert(#t == 2)
  assert(t[1]() == 10)
  assert(t[2]() == 10)
  assert(t[1]() == 20)
  assert(t[2]() == 20)
end
print("Test 35 OK")

-- test 36
do
  -- upvalues
  local x = 1
  local y=function()
    local f = function()
               return x
            end
    return f
  end
  local f = y()
  assert(f() == 1)
  x=5
  assert(f() == 5)
end
print("Test 36 OK")

-- test 37
do
  -- upvalues - TODO is this test worth keeping?
  g = 3 -- global
  local y=function()
    local f = function()
               return g
            end
    return f
  end
  local f = y()
  assert(f() == 3)
  g=5
  assert(f() == 5)
end
print("Test 37 OK")

-- test 38
do
  local function x()
    local x = 1
    local f = function()
      x=x+1
      return x
    end
    return f
  end
  local f=x()
  assert(f() == 2)
  assert(f() == 3)
end
print("Test 38 OK")

-- test setupval, getupval
do
  local function x()
    local a = 0
    return function(x) a=a+x; return a; end
  end
  local y=x()
  assert(y(2) == 2)
  assert(y(2) == 4)
  assert(y(3) == 7)
end
print('Test 39 OK')

-- test 40
do
  local x=function(a,b)
    return a%b
  end
  -- When using 32-bit numbers we don't jit mod
  assert(x(5,2) == 1)
  assert(math.abs(x(5.1,2.1)-0.9) < 1e-6)
end
print("Test 40 OK")

-- test 41
do
  local x=function(a,b)
    return a//b
  end
  -- When using 32-bit numbers we don't jit idiv
  assert(x(5,2) == 2)
  assert(math.abs(x(5.5,2.1)-2.0) < 1e-6)
end
print("Test 41 OK")

-- test 42
--do
--  local x = function ()
--    local j = 0
--    for i=2.0,6.0,3.1 do
--      j = i
--    end
--    return j
--  end
--  assert(x() == 5.1)
--end
--print("Test 42 OK")

-- test parameter types
do
  local x = function (a: integer, b: number)
    local i: integer = a
    local j: number = b
    return i+j
  end
  assert(x(1,5.5) == 6.5)
end
print'Test 43 OK'

do
  local x=function (a:number[], b:integer)
    local j: number = a[b]
    return j
  end
  local function y()
    local t: number[] = { 4.2 }
    return x(t, 1)
  end
  assert(y() == 4.2)
end
print'Test 44 OK'

-- test 45
do
  local function x(t) return t; end
  local function f()
    local tt: integer[] = {1}
    local ss: number[] = { 55.5 }
    tt = x(tt)
    ss = x(ss)
  end
  assert(pcall(f))
  local function f()
    local tt: integer[] = {1}
    tt = x({})
  end
  assert(not pcall(f))
  local function f()
    local tt: integer[] = {1}
    local ss: number[] = { 55.5 }
    ss = x(tt)
  end
  assert(not pcall(f))
end
print("Test 45 OK")

-- test 47
do
  local function test_idiv()
    local t = {}
    t.__idiv = function(...) return 'idiv' end
    local t1=setmetatable({1,2,3}, t)
    local t2=setmetatable({4,5,6}, t)
    local x= function() return t1//t2 end
    local s1=x()
    assert(s1 == 'idiv')
    local s2=x()
    assert(s1 == s2)
    local x= function(t1: number, t2: number) return t1//t2 end
    local s1=x(4.1,2.0)
    assert(s1 == 2)
    local s2=x(4.1,2.0)
    assert(s1 == s2)
  end
  test_idiv()
end
print'Test 47 (IDIV) OK'

do
  local function test_tableaccess()
    -- Test specialised version of GETTABLE and SETTABLE
    -- when local variable is known to be of table type
    -- and key is a string
    local f = function()
      local t : table = {}
      t.name = 'dibyendu'
      t.data = {}
      t.data.city = 'london'
      return t.name, t.data.city
    end
    local a,b = f()
    assert(a == 'dibyendu')
    assert(b == 'london')

    -- Test specialised version of GETTABLE and SETTABLE
    -- when local variable is known to be of table type
    -- and key is a integer
    local f = function()
      local t : table = {}
      t[1] = 'dibyendu'
      t[2] = {}
      t[2][1] = 'london'
      return t[1], t[2][1]
    end
    local a,b = f()
    assert(a == 'dibyendu')
    assert(b == 'london')
  end
  test_tableaccess()
end
print 'Test 48 OK'

do
  local function test_self_s()
    local t : table = {}
    t.name_ = 'dibyendu majumdar'
    t.name = function (t:table) return t.name_ end
    assert(t:name())
    return t:name()
  end
  assert(test_self_s() == 'dibyendu majumdar')
end
print 'Test 49 OK'

-- issue #73
do
  local function bug_index_event()
    local t1 = { name='dibyendu' }
    local t2 = { surname='majumdar' }
    t2.__index = t2
    setmetatable(t1, t2)
    assert(t1.name == 'dibyendu')
    assert(t1.surname == 'majumdar') -- index event
    assert(t1.surname == 'majumdar') -- index event

    local t3: table = { name='dibyendu' }
    setmetatable(t3, t2)
    assert(t3.name == 'dibyendu')
    assert(t3.surname == 'majumdar') -- index event
    assert(t3.surname == 'majumdar') -- index event

    t1 = { [1]='dibyendu' }
    t2 = { [2]='majumdar' }
    t2.__index = t2
    setmetatable(t1, t2)
    assert(t1[1] == 'dibyendu')
    assert(t1[2] == 'majumdar') -- index event
    assert(t1[2] == 'majumdar') -- index event

    local t3: table = { [1]='dibyendu' }
    setmetatable(t3, t2)
    assert(t3[1] == 'dibyendu')
    assert(t3[2] == 'majumdar') -- index event
    assert(t3[2] == 'majumdar') -- index event
  end
  bug_index_event()
end
print 'Test 50 OK'

do
  local function event_test()
    local a: table = {}
    setmetatable(a, {__metatable = "xuxu",
                    __tostring=function(x: table) return x.name end})
    assert(getmetatable(a) == "xuxu")
    --assert(tostring(a) == nil)
    -- cannot change a protected metatable
    assert(pcall(setmetatable, a, {}) == false)
    a.name = "gororoba"
    assert(tostring(a) == "gororoba")
  end
  event_test();
end
print 'Test 51 OK'

do
  local function test_intarray_as_table(t)
    t.a = 'a'
    --print(ravitype(t))
  end
  local function do_test_intarray_as_table()
    local iarray: integer[] = {1}
    test_intarray_as_table(iarray);
  end
  assert(not pcall(do_test_intarray_as_table))
end
print 'Test 52 OK'

do
  local function test_numarray_meta()
    local farray : number[] = {1.1, 2.2, 3.3}
    setmetatable(farray, {
      __name = 'matrix',
      __tostring = function() return '{' .. table.concat(farray, ",") .. '}' end
      })
    assert(ravitype(farray) == 'matrix')
    assert(tostring(farray) == '{1.1,2.2,3.3}')
  end
  assert(pcall(test_numarray_meta));
end
print 'Test 53 OK'

do
  local function test_longkey()
    local t: table = {}
    t._01234567890123456789012345678901234567890123456789 = 4.2
    assert(t._01234567890123456789012345678901234567890123456789 == 4.2)
    t._01234567890 = 4.3
    assert(t._01234567890 == 4.3)
    t["_01234567890"] = 4.4
    assert(t["_01234567890"] == 4.4)
    local s = "_01234567890"
    assert(t[s] == 4.4)
    assert(t.s == nil)
  end
  assert(pcall(test_longkey));
end
print 'Test 54 OK'

do
  local function test_yields_in_metamethods()
    --print"testing yields inside metamethods"
    local mt = {
      __eq = function(a:table,b:table) coroutine.yield(nil, "eq"); return a.x == b.x end,
      __lt = function(a:table,b:table) coroutine.yield(nil, "lt"); return a.x < b.x end,
      __le = function(a:table,b:table) coroutine.yield(nil, "le"); return a - b <= 0 end,
      __add = function(a:table,b:table) coroutine.yield(nil, "add"); return a.x + b.x end,
      __sub = function(a:table,b:table) coroutine.yield(nil, "sub"); return a.x - b.x end,
      __mod = function(a:table,b:table) coroutine.yield(nil, "mod"); return a.x % b.x end,
      __unm = function(a:table,b) coroutine.yield(nil, "unm"); return -a.x end,
      __bnot = function(a:table,b) coroutine.yield(nil, "bnot"); return ~a.x end,
      __shl = function(a:table,b:table) coroutine.yield(nil, "shl"); return a.x << b.x end,
      __shr = function(a:table,b:table) coroutine.yield(nil, "shr"); return a.x >> b.x end,
      __band = function(a,b)
               a = type(a) == "table" and a.x or a
               b = type(b) == "table" and b.x or b
               coroutine.yield(nil, "band")
               return a & b
             end,
      __bor = function(a:table,b:table) coroutine.yield(nil, "bor"); return a.x | b.x end,
      __bxor = function(a:table,b:table) coroutine.yield(nil, "bxor"); return a.x ~ b.x end,

      __concat = function(a,b)
                 coroutine.yield(nil, "concat");
                 a = type(a) == "table" and a.x or a
                 b = type(b) == "table" and b.x or b
                 return a .. b
               end,
      __index = function (t:table,k) coroutine.yield(nil, "idx"); return t.k[k] end,
      __newindex = function (t:table,k,v) coroutine.yield(nil, "nidx"); t.k[k] = v end,
    }

    local function new (x)
      return setmetatable({x = x, k = {}}, mt)
    end

    local a : table = new(10)
    local b : table = new(12)
    local c : table = new"hello"

    local function run (f, t)
      local i = 1
      local c = coroutine.wrap(f)
      while true do
        local res, stat = c()
        if res then assert(t[i] == nil); return res, t end
        assert(stat == t[i])
        i = i + 1
      end
    end

    assert(run(function () if (a>=b) then return '>=' else return '<' end end,
         {"le", "sub"}) == "<")
    -- '<=' using '<'
    mt.__le = nil
    assert(run(function () if (a<=b) then return '<=' else return '>' end end,
         {"lt"}) == "<=")
    assert(run(function () if (a==b) then return '==' else return '~=' end end,
         {"eq"}) == "~=")

    assert(run(function () return a & b + a end, {"add", "band"}) == 2)

    assert(run(function () return a % b end, {"mod"}) == 10)

    assert(run(function () return ~a & b end, {"bnot", "band"}) == ~10 & 12)
    assert(run(function () return a | b end, {"bor"}) == 10 | 12)
    assert(run(function () return a ~ b end, {"bxor"}) == 10 ~ 12)
    assert(run(function () return a << b end, {"shl"}) == 10 << 12)
    assert(run(function () return a >> b end, {"shr"}) == 10 >> 12)

    assert(run(function () return a..b end, {"concat"}) == "1012")

    assert(run(function() return a .. b .. c .. a end,
         {"concat", "concat", "concat"}) == "1012hello10")

    assert(run(function() return "a" .. "b" .. a .. "c" .. c .. b .. "x" end,
         {"concat", "concat", "concat"}) == "ab10chello12x")


    do   -- a few more tests for comparison operators
      local mt1 = {
        __le = function (a,b)
          coroutine.yield(10)
          return
            (type(a) == "table" and a.x or a) <= (type(b) == "table" and b.x or b)
        end,
        __lt = function (a,b)
          coroutine.yield(10)
          return
            (type(a) == "table" and a.x or a) < (type(b) == "table" and b.x or b)
        end,
      }
      local mt2 = { __lt = mt1.__lt }   -- no __le

      local function run (f)
        local co = coroutine.wrap(f)
        local res
        repeat
          res = co()
        until res ~= 10
        return res
      end
    
      local function test ()
        local a1 : table = setmetatable({x=1}, mt1)
        local a2 : table = setmetatable({x=2}, mt2)
        assert(a1 < a2)
        assert(a1 <= a2)
        assert(1 < a2)
        assert(1 <= a2)
        assert(2 > a1)
        assert(2 >= a2)
        return true
      end
      run(test)
    end

    assert(run(function ()
             local a: table = a
             a.BB = print
             return a.BB
           end, {"nidx", "idx"}) == print)
  end
  assert(pcall(test_yields_in_metamethods));
end
print 'Test 55 OK'

do
  local function test_upvaluejoin()
    local debug = require'debug'
    local foo1, foo2, foo3, foo4
    do
      local a:integer, b:integer = 3, 5
      local c:number = 7.1
      foo1 = function() return a+b end
      foo2 = function() return b+a end
      foo4 = function() return c end
      do
        local a: integer = 10
        foo3 = function() return a+b end
      end
    end

    assert(debug.upvalueid(foo1, 1))
    assert(debug.upvalueid(foo1, 2))
    assert(not pcall(debug.upvalueid, foo1, 3))
    assert(debug.upvalueid(foo1, 1) == debug.upvalueid(foo2, 2))
    assert(debug.upvalueid(foo1, 2) == debug.upvalueid(foo2, 1))
    assert(debug.upvalueid(foo3, 1))
    assert(debug.upvalueid(foo1, 1) ~= debug.upvalueid(foo3, 1))
    assert(debug.upvalueid(foo1, 2) == debug.upvalueid(foo3, 2))

    assert(debug.upvalueid(string.gmatch("x", "x"), 1) ~= nil)

    assert(foo1() == 3 + 5 and foo2() == 5 + 3)
    debug.upvaluejoin(foo1, 2, foo2, 2)
    assert(foo1() == 3 + 3 and foo2() == 5 + 3)
    assert(foo3() == 10 + 5)
    debug.upvaluejoin(foo3, 2, foo2, 1)
    assert(foo3() == 10 + 5)
    debug.upvaluejoin(foo3, 2, foo2, 2)
    assert(foo3() == 10 + 3)
    -- Following will fail as typeof(foo4,1) is not same as typeof(foo1,1)
    debug.upvaluejoin(foo4, 1, foo1, 1)
    assert(foo4() == 7.1)
    assert(not pcall(debug.upvaluejoin, foo1, 3, foo2, 1))
    assert(not pcall(debug.upvaluejoin, foo1, 1, foo2, 3))
    assert(not pcall(debug.upvaluejoin, foo1, 0, foo2, 1))
    assert(not pcall(debug.upvaluejoin, print, 1, foo2, 1))
    assert(not pcall(debug.upvaluejoin, {}, 1, foo2, 1))
    assert(not pcall(debug.upvaluejoin, foo1, 1, print, 1))
  end
  assert(pcall(test_upvaluejoin));
end
print 'Test 56 OK'

do
  local function test_idiv(y: integer)
    local era: integer
    era = y // 400
    return era
  end
  assert(test_idiv(1900) == 4)
end
print 'Test 57 OK'

do
  local function from_dmy1(y: integer, m: integer, d: integer)
    if m <= 2 then
      y = y - 1
    end
    local era: integer
    if y >= 0 then
      era = y // 400
    else
      era = (y-399) // 400
    end
    local yoe: integer = y - era * 400
    local tmp: integer
    if m > 2 then
      tmp = -3
    else
      tmp = 9
    end
    local doy: integer = (153 * (m  + tmp) + 2)//5 + d-1
    local doe: integer = yoe * 365 + yoe//4 - yoe//100 + doy
    return era * 146097 + doe - 719468
  end
  local function from_dmy2(y, m, d)
    if m <= 2 then
      y = y - 1
    end
    local era
    if y >= 0 then
      era = y // 400
    else
      era = (y-399) // 400
    end
    local yoe = y - era * 400
    local tmp
    if m > 2 then
      tmp = -3
    else
      tmp = 9
    end
    local doy = (153 * (m  + tmp) + 2)//5 + d-1
    local doe = yoe * 365 + yoe//4 - yoe//100 + doy
    return era * 146097 + doe - 719468
  end
  assert(from_dmy1(1900, 1, 1) == -25567)
  assert(from_dmy1(1900, 1, 1) == from_dmy2(1900, 1, 1))
end
print 'Test 58 OK'

do
  local function x(s1: string, s2: string)
    return @string( s1 .. s2 )
  end
  assert(x('a', 'b') == 'ab')
end
print 'Test 59 OK'

do
  local function x(f: closure)
    local g = @closure f
    return g()
  end
  local called = 0
  local function y()
    called = called + 1
  end
  x(y)
  assert(called == 1)
  x(y)
  assert(called == 2)
end
print 'Test 60 OK'

do
  local mt = { __name='MyType'}
  debug.getregistry().MyType = mt
  local t = {}
  setmetatable(t, mt)
  local function x(s: MyType)
    local assert = assert
    assert(@MyType s == @MyType t)
    assert(@MyType t == t)
  end
  x(t)
end
print 'Test 61 OK'

do
  local mt = { __name='MyType'}
  debug.getregistry().MyType = mt
  local t = {}
  setmetatable(t, mt)
  local function x()
    local a: MyType
    return function (b) a = b end
  end
  local f = x()
  assert(pcall(f, t)) -- t is of MyType so okay
  assert(pcall(f, nil)) -- nil is okay
  assert(not pcall(f, 'hello')) -- string not okay
end
print 'Test 62 OK'

do
  local function x()
    local f: closure = function() end
    f = nil
    local f
    f = 'a'
    return f
  end
  assert(x() == 'a')
end
print 'Test 63 OK'

-- Codegen bug #issue 148
do
  local function x()
    assert(({pcall(function() comp 'x for __result' end)})[2]
       :find'not contain __ prefix')
  end
end
print 'Test 64 OK'

do
  local x = function()
    local i: integer = @integer 1
    local n: number = @number 1.2
    assert(i == 1 and n == 1.2)
    local j = { 1, 2, 3, 4.2, 5 }
    i = @integer( @integer (j[2]) )
    n = @number( @number (j[4]) )
    assert(i == 2)
    assert(n == 4.2)
  end
  x()
end
print 'Test 65 OK'

do
  local function x()
    local a,b = 1,2
    local c: integer,d: integer = @integer a, @integer b
    assert(c == a and b == d)
  end
  x()
end
print 'Test 66 OK'

do
  g = { 1, 5.5 } -- global
  local h = { 10, 4.2 }
  local function x()
    local a: integer = 11
    local b: number = 6.5
    local c = 6
    a = @integer( g[1] ) + @integer( h[1] ) * @integer( c )
    c = 3.2
    b = @number( g[2] ) * @number( h[2] ) - @number( c )
    return a,b
  end
  local y,z = x()
  assert(y == 61 and math.abs(z - 19.9) < 1e-5)
end
print 'Test 67 OK'

do
  local x=load 'local t: number[] = {}'
end
print 'Test 68 OK'

do
  local x=load 'local t = @number[] {}'
end
print 'Test 69 OK'

do
  local x=load 'local t: number[] = @number[] ( @number[] {} )'
end
print 'Test 70 OK'

do
  local function x()
    return { @integer[] {1,2}, @number[] {42.0} }
  end
  local y = x()
  assert(ravitype(y[1]) == 'integer[]')
  assert(ravitype(y[2]) == 'number[]')
end
print 'Test 71 OK'

do
  local function x()
    return @integer[] @integer[] @integer[]{1}
  end
  assert(ravitype(x()) == 'integer[]')
end
print 'Test 72 OK'

do
  local function x()
    return nil or @integer[] {1}
  end
  assert(ravitype(x()) == 'integer[]')
end
print 'Test 73 OK'

do
  local function x()
    return @number[]( @integer[] {1} and @number[] {42.0} )
  end
  assert(ravitype(x()) == 'number[]')
  assert(x()[1] == 42.0)
end
print 'Test 74 OK'

do
  local function x()
    assert(math.exp(2) == ravi.exp(2))
    assert(math.log(2) == ravi.ln(2))
    assert(math.exp(math.log(2)) == ravi.exp(ravi.ln(2)))
  end
  x()
end
print 'Test 75 OK'

-- Test that % of ints results in int
do
  local function x(a: integer, b:integer)
    return a%b == 0
  end
  assert(x(10,5))
end
print 'Test 76 OK'

-- Test that ^ applied to numbers results in a number
do
  local function x(a:number, b:number)
    local c:number = a^b
    return c
  end
  assert(x(2,3) == 2^3)
end
print 'Test 77 OK'


-- Create some slices and invoke GC
do
  local x = function ()
    local function createarray(values: number[])
        local arr: number[] = table.numarray(#values, 0)
        for i=1,#values do
            arr[i] = values[i]
        end
        return arr
    end

    local slice1, slice2, slice3
    local finalizer_called = false
    do
      local arr: number[] = createarray (@number[]{ 1.1, 2.2, 3.3, 4.4, 5.5, 6.6 })
      setmetatable(arr, { __gc = function() finalizer_called = true end })
      assert(#arr == 6)
      slice1 = table.slice(arr, 1, 4)
      collectgarbage()
      assert(not finalizer_called)
      assert(#arr == 6)
      assert(#slice1 == 4)
      slice2 = table.slice(slice1, 2, 2)
      collectgarbage()
      assert(not finalizer_called)
      assert(#arr == 6)
      assert(#slice1 == 4)
      assert(#slice2 == 2)
    end
    collectgarbage()
    assert(not finalizer_called)
    slice3 = table.slice(slice2, 1, 1)
    assert(#slice1 == 4)
    assert(#slice2 == 2)
    assert(slice3[1] == 2.2)
    slice1 = nil
    collectgarbage()
    assert(not finalizer_called)
    assert(#slice2 == 2)
    assert(slice3[1] == 2.2)
    slice2 = nil
    collectgarbage()
    assert(not finalizer_called)
    assert(#slice3 == 1)
    assert(slice3[1] == 2.2)
    slice3 = nil
    collectgarbage()
    assert(finalizer_called)
  end
  x()
end
print 'Test 83 OK'

-- Test that ~ applied to non integer result in any type
-- issue #209
do
  local function x(x:number)
    return (~x)+1
  end
  assert(x(1.0) == -1)
  local function x(x:table)
    return (~x)+1
  end
end
print 'Test 84 OK'

-- Test that #() applied to non integer type produces any type
-- issue #210
do
  local function x(x:integer[])
    return #(x[1])+1
  end
end
print 'Test 85 OK'

-- Test that #() applied to non integer type produces any type
-- issue #210
do
  local function x(x:integer[])
    return #x[1]+1
  end
end
print 'Test 86 OK'

-- An upvalue that is an array will yield a primitive value
-- issue #211
do
    local x: integer[] = {4}
    local y: number[] = {4.2}
    local function f()
        return x[1] + 1, y[1] + 2.2
    end
    local a,b = f()
    assert(a == 5)
    assert(b == 6.4)
end
print 'Test 87 OK'

do
 local function doit(a: integer, what)
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
 assert(doit and type(doit) == 'function')
 assert(math.abs(doit(16, '&') - 16) < 1e-15)
 assert(math.abs(doit(16, '|') - ~0) < 1e-15)
 assert(math.abs(doit(16, '<<') - 32) < 1e-15)
 assert(math.abs(doit(16, '>>') - 8) < 1e-15)
 assert(math.abs(doit(16, '~') - ~16) < 1e-15)
end
print 'Test 88 OK'

do
  local data = { "a", 5.5, false }
  for i = 1,4 do -- check nil too
    local y = data[i]
    local function x()
      return @integer y
    end
    assert(not pcall(x))
  end
end
print 'Test 89 OK'

do
  local y = 56.0
  local function x()
    return @integer y
  end
  local _,z = pcall(x)
  assert(_)
  assert(math.type(z) == 'integer')
end
print 'Test 90 OK'

do
  local data = { "a", 5.5, false }
  for i = 1,4 do -- check nil too
    local y = data[i]
    local function x(y: integer)
      return y
    end
    assert(not pcall(x))
  end
end
print 'Test 91 OK'

do
  local function x(y: integer)
    return y
  end
  local _,z = pcall(x, 56.0)
  assert(_)
  assert(math.type(z) == 'integer')
end
print 'Test 92 OK'


do
  local data = { "a", function() end, false }
  for i = 1,4 do -- check nil too
    local y = data[i]
    local function x()
      return @number y
    end
    assert(not pcall(x))
  end
end
print 'Test 93 OK'

do
  local y = 56
  local function x()
    return @number y
  end
  local _,z = pcall(x)
  assert(_)
  assert(math.type(z) == 'float')
end
print 'Test 94 OK'

do
  local data = { "a", function() end, false }
  for i = 1,4 do -- check nil too
    local y = data[i]
    local function x(y: number)
      return y
    end
    assert(not pcall(x))
  end
end
print 'Test 95 OK'

do
  local function x(y: number)
    return y
  end
  local _,z = pcall(x, 56)
  assert(_)
  assert(math.type(z) == 'float')
end
print 'Test 96 OK'

do
  local data = { 1, 1.5, function() end, false }
  for i = 1,@integer(#data) do
    local y = data[i]
    local function x()
      return @string y
    end
    assert(not pcall(x))
  end
end
print 'Test 97 OK'

do
  local data = { 1, 1.5, function() end, false }
  for i = 1,@integer(#data) do
    local y = data[i]
    local function x(y: string)
      return y
    end
    assert(not pcall(x, y))
  end
end
print 'Test 98 OK'

do
  local data = { 'hello' }
  for i = 1,2 do -- check nil too
    local y = data[i]
    local function x()
      return @string y
    end
    assert(pcall(x))
  end
end
print 'Test 99 OK'

do
  local data = { 'hello' }
  for i = 1,2 do -- check nil too
    local y = data[i]
    local function x(y: string)
      return y
    end
    assert(pcall(x, y))
  end
end
print 'Test 100 OK'

do
  local function x(a, b)
    local z : integer[] =  { @integer(a), @integer(b) }
    return z
  end
  assert(pcall(x, 1, 2.0))
  assert(not pcall(x, 1.5, 2))
  assert(not pcall(x, '1.5', 2))
  assert(not pcall(x, false, 2))
  assert(not pcall(x, nil, 2))
end
print 'Test 101 OK'

do
  local function x(a, b)
    local z : number[] =  { @number(a), @number(b) }
    return z
  end
  assert(pcall(x, 42, 2.2))
  assert(not pcall(x, '1.5', 2))
  assert(not pcall(x, false, 2))
  assert(not pcall(x, nil, 2))
end
print 'Test 102 OK'

do
  local data = { 'hello', 0, false, 5.6 }
  for i = 1,@integer(#data) do
    local y = data[i]
    local function x()
      return @closure y
    end
    assert(not pcall(x))
  end
end
print 'Test 103 OK'

do
  local data = { 'hello', 0, false, 5.6 }
  for i = 1,@integer(#data) do
    local y = data[i]
    local function x(z: closure)
      return z
    end
    assert(not pcall(x, y))
  end
end
print 'Test 104 OK'

-- 
do
  local function x()
    local a, b = {}, 10
    a[b], b = 20, 30
    return a[10], a[30]
  end
  local a,b = x()
  assert(a == 20)
  assert(b == nil)
end
print 'Test 105 OK'


print 'OK'
