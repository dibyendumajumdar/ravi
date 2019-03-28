local str = 
[[return function()
    local a = 1

	local function y()
		return function()
			return a
		end
	end

	local a = 5

	local function z()
		return function()
			return a
		end
	end

	return y, z
end
]]
--local y, z = x()

--print(y()())
--print(z()())

--ravi.dumplua(x)
local x, msg = ast.parse(str)
if not x then print(msg) end
print(x:tostring())
--print(x)