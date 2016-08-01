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


print('Done')