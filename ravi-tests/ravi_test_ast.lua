# Test AST construction

local function dotest(code)
	local x = ast.parse(code)
	print('--[[' .. code .. '\n]]')
	print(x:tostring())
	print '--'
end

local x

dotest 'return'
dotest 'return 1'
dotest 'return 42, 4.2, true, "hello"'
dotest 'return a'
dotest 'return 1+2'
dotest 'return 2^3-5*4'

dotest 'return 0.0'
dotest 'return 0'
dotest 'return -0//1'
dotest 'return 3^-1'
dotest 'return (1 + 1)^(50 + 50)'
dotest 'return (-2)^(31 - 2)'
dotest 'return (-3^0 + 5) // 3.0'
dotest 'return -3 % 5'
dotest 'return -((2.0^8 + -(-1)) % 8)/2 * 4 - 3'
dotest 'return -((2^8 + -(-1)) % 8)//2 * 4 - 3'
dotest 'return 0xF0.0 | 0xCC.0 ~ 0xAA & 0xFD'
dotest 'return ~(~0xFF0 | 0xFF0)'
dotest 'return ~~-100024.0'
dotest 'return ((100 << 6) << -4) >> 2'

dotest 'return 2^3^2 == 2^(3^2)'
dotest 'return 2^3*4 == (2^3)*4'
dotest 'return 2.0^-2 == 1/4 and -2^- -2 == - - -4'
dotest 'return not nil and 2 and not(2>3 or 3<2)'
dotest 'return -3-1-5 == 0+0-9'
dotest 'return -2^2 == -4 and (-2)^2 == 4 and 2*2-3-1 == 0'
dotest 'return -3%5 == 2 and -3+5 == 2'
dotest 'return 2*1+3/3 == 3 and 1+2 .. 3*1 == "33"'
dotest 'return not(2+1 > 3*1) and "a".."b" > "a"'

dotest 'return "7" .. 3 << 1 == 146'
dotest 'return 10 >> 1 .. "9" == 0'
dotest 'return 10 | 1 .. "9" == 27'

dotest 'return 0xF0 | 0xCC ~ 0xAA & 0xFD == 0xF4'
dotest 'return 0xFD & 0xAA ~ 0xCC | 0xF0 == 0xF4'
dotest 'return 0xF0 & 0x0F + 1 == 0x10'

dotest 'return 3^4//2^3//5 == 2'

dotest 'return -3+4*5//2^3^2//9+4%10/3 == (-3)+(((4*5)//(2^(3^2)))//9)+((4%10)/3)'

dotest 'return not ((true or false) and nil)'
dotest 'return true or false  and nil'

-- old bug
dotest 'return (((1 or false) and true) or false) == true'
dotest 'return (((nil and true) or false) and true) == false'

dotest 'return -(1 or 2) == -1 and (1 and 2)+(-1.25 or -4) == 0.75'
dotest 'return (b or a)+1 == 2 and (10 or a)+1 == 11'
dotest 'return ((2<3) or 1) == true and (2<3 and 4) == 4'

dotest 'return (x>y) and x or y == 2'
dotest 'return (x>y) and x or y == 2'

