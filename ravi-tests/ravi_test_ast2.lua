local function doast(str)
    local x, msg = ast.parse(str)
    if not x then error(msg) end
    print(x:tostring())
end

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
doast(str)

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
doast(str)

str =
[[return function (a) while a < 10 do a = a + 1 end end
]]
doast(str)

-- Note that the while loop above is supposed to ultimately
-- generate same code as below
str=
[[return function (a) ::L2:: if not(a < 10) then goto L1 end; a = a + 1;
                  goto L2; ::L1:: end
]]
doast(str)

str=
[[return function ()
    (function () end){f()}
  end
]]
doast(str)

str=
[[return function ()
    local sum 
    for j = 1,500 do
        sum = 0.0
        for k = 1,10000 do
            sum = sum + 1.0/(k*k)
        end
    end
    return sum
end
]]
doast(str)

str=
[[local a: integer return a+3
]]
doast(str)

str=
[[local i: integer; return t[i/5]
]]
doast(str)

str=
[[local t: integer[]; return t[0][1]
]]
doast(str)

str=
[[return f()[1]
]]
doast(str)