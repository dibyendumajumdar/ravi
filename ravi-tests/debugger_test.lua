local function func_with_varargs(...)
	local t = { ... }
	print(table.unpack(t))
	return t
end

x = 'global'
local t = func_with_varargs('hello', ' ', 'world!')
local t2 = func_with_varargs(t, 1, 2, 3)

print('Done')