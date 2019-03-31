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

local x, msg = ast.parse(str)
if not x then print(msg) end
print(x:tostring())

str =
[[return function (a, b, c, d, e)
    if a == b then goto l1
    elseif a == c then goto l2
    elseif a == d then goto l2
    else if a == e then goto l3
        else goto l3
        end
    end
    ::l1:: ::l2:: ::l3:: ::l4::
end
]]

x, msg = ast.parse(str)
if not x then print(msg) end
print(x:tostring())

