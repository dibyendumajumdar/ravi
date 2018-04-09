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