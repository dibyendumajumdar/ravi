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
[[local t: integer[]; return t[0]
]]
doast(str)

str=
[[return f()[1]
]]
doast(str)

str=
[[return x.y[1]
]]
doast(str)

str=
[[local t: integer[]
if (t[1] == 5) then
    return true
end
return false
]]
doast(str)

str=
[[function matmul(a: table, b: table)
  	assert(@integer(#a[1]) == #b);
  	local m: integer, n: integer, p: integer, x: table = #a, #a[1], #b[1], {};
  	local c: table = matrix.T(b); -- transpose for efficiency
  	for i = 1, m do
  		local xi: number[] = table.numarray(p, 0.0)
  		x[i] = xi
  		for j = 1, p do
  			local sum: number, ai: number[], cj: number[] = 0.0, @number[](a[i]), @number[](c[j]);
  			-- for luajit, caching c[j] or not makes no difference; lua is not so clever
  			for k = 1, n do sum = sum + ai[k] * cj[k] end
  			xi[j] = sum;
  		end
  	end
  	return x
  end
return matmul
]]
doast(str)