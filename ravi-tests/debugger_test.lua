local t = table.numarray(10, 4.2)
setmetatable(t, {
  __name = 'matrix',
  __tostring = function() return '{' .. table.concat(t, ",") .. '}' end 
  })

print(t)

local atable = { t }

defer 
   print('deferred')
end

local function func_with_varargs(...)
	local t = { ... }
	print(table.unpack(t))
	return t
end

x = 'global'
local t = func_with_varargs('hello', ' ', 'world!')
local t2 = func_with_varargs(t, 1, 2, 3)

code1 = 'return function (y) y() end'
local code2 = 'print "hello"'

local f1 = load(code1)
local f2 = load(code2)

local f3 = f1()
f3(f2)

local t3 = {}
for i = 1,249 do
	t3[i] = i
end
t3[250] = 250
t3[251] = 251
t3[252] = 252

-- coroutine test copied
local function eqtab (t1, t2)
  assert(#t1 == #t2)
  for i = 1, #t1 do
    local v = t1[i]
    assert(t2[i] == v)
  end
end

_G.x = nil   -- declare x
function foo (a, ...)
  local x, y = coroutine.running()
  assert(x == f and y == false)
  -- next call should not corrupt coroutine (but must fail,
  -- as it attempts to resume the running coroutine)
  assert(coroutine.resume(f) == false)
  assert(coroutine.status(f) == "running")
  local arg = {...}
  assert(coroutine.isyieldable())
  for i=1,#arg do
    _G.x = {coroutine.yield(table.unpack(arg[i]))}
  end
  return table.unpack(a)
end

f = coroutine.create(foo)
assert(type(f) == "thread" and coroutine.status(f) == "suspended")
assert(string.find(tostring(f), "thread"))
local s,a,b,c,d
s,a,b,c,d = coroutine.resume(f, {1,2,3}, {}, {1}, {'a', 'b', 'c'})
assert(s and a == nil and coroutine.status(f) == "suspended")
s,a,b,c,d = coroutine.resume(f)
eqtab(_G.x, {})
assert(s and a == 1 and b == nil)
s,a,b,c,d = coroutine.resume(f, 1, 2, 3)
eqtab(_G.x, {1, 2, 3})
assert(s and a == 'a' and b == 'b' and c == 'c' and d == nil)
s,a,b,c,d = coroutine.resume(f, "xuxu")
eqtab(_G.x, {"xuxu"})
assert(s and a == 1 and b == 2 and c == 3 and d == nil)
assert(coroutine.status(f) == "dead")
s, a = coroutine.resume(f, "xuxu")
assert(not s and string.find(a, "dead") and coroutine.status(f) == "dead")


-- pcall tests
-- =================== start copy from Lua testsuite ===============
local function checkerr (msg, f, ...)
  local st, err = pcall(f, ...)
  assert(not st and string.find(err, msg))
end

local function doit (s)
  local f, msg = load(s)
  if f == nil then return msg end
  local cond, msg = pcall(f)
  return (not cond) and msg
end

local function checkmessage (prog, msg)
  local m = doit(prog)
  --print(m)
  assert(string.find(m, msg, 1, true))
end

local function checksyntax (prog, extra, token, line)
  local msg = doit(prog)
  if not string.find(token, "^<%a") and not string.find(token, "^char%(")
    then token = "'"..token.."'" end
  token = string.gsub(token, "(%p)", "%%%1")
  local pt = string.format([[^%%[string ".*"%%]:%d: .- near %s$]],
                           line, token)
  assert(string.find(msg, pt))
  assert(string.find(msg, msg, 1, true))
end
-- =================== end copy from Lua testsuite ===============

checkmessage('local t: table = {}; local t2: table = {}; t=t2[1]', 'Invalid assignment: table expected')
checkmessage('local t: table = {}; t=1', 'Invalid assignment: table expected')
checkmessage('local t: table = {}; t=function() end', 'table expected')
checkmessage('local t: table = {}; local t2: number[] = {}; t=t2', 'Invalid assignment: table expected')
checkmessage('local t: table = {}; local t2: table = { data={} }; t=t2.data', 'Invalid assignment: table expected')
checkmessage('local t: table = {}; local f=function() t = 1 end; f()', 'upvalue of table type, cannot be set to non table value')
checkmessage('local t: table = {}; local x=function() return 0 end; local f=function() t = x() end; f()', 'upvalue of table type, cannot be set to non table value')
checkmessage('local t: table = {}; local x=function() t[1] = 1; local f=function() t = 1 end; f(); end; x()', 'upvalue of table type, cannot be set to non table value')
checkmessage('local i: integer, t: table, j: number', 'uninitialized table in local variable')


print('Done')