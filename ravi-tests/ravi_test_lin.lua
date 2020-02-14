-- This script contains tests for the new code generator
-- specifically the linearizer part of the code generator.

local function gen(chunk: string)

	print('Testing ' .. chunk)
	local parsed, err = ast.parse(chunk)
	assert(parsed)
	parsed:linearize()
	print(parsed:showlinear())

end

gen 'return'
gen 'return 1'
gen 'return 42, 4.2, true, "hello"'
gen 'return a'
gen 'return 1+2'
gen 'return 2^3-5*4'
gen 'return 1+1'
gen 'return 1+1+1'
gen 'return 2-3/5*4'
gen 'return 4.2//5'	

gen 'return 0.0'
gen 'return 0'
gen 'return -0//1'
gen 'return 3^-1'
gen 'return (1 + 1)^(50 + 50)'
gen 'return (-2)^(31 - 2)'
gen 'return (-3^0 + 5) // 3.0'
gen 'return -3 % 5'
gen 'return -((2.0^8 + -(-1)) % 8)/2 * 4 - 3'
gen 'return -((2^8 + -(-1)) % 8)//2 * 4 - 3'
gen 'return 0xF0.0 | 0xCC.0 ~ 0xAA & 0xFD'
gen 'return ~(~0xFF0 | 0xFF0)'
gen 'return ~~-100024.0'
gen 'return ((100 << 6) << -4) >> 2'

gen 'return 2^3^2 == 2^(3^2)'
gen 'return 2^3*4 == (2^3)*4'
gen 'return 2.0^-2 == 1/4, -2^- -2 == - - -4'
gen 'return -3-1-5 == 0+0-9'
gen 'return -2^2 == -4, (-2)^2 == 4, 2*2-3-1 == 0'
gen 'return -3%5 == 2, -3+5 == 2'

gen 'return 0xF0 | 0xCC ~ 0xAA & 0xFD == 0xF4'
gen 'return 0xFD & 0xAA ~ 0xCC | 0xF0 == 0xF4'
gen 'return 0xF0 & 0x0F + 1 == 0x10'

gen 'return 3^4//2^3//5 == 2'

gen 'return -3+4*5//2^3^2//9+4%10/3 == (-3)+(((4*5)//(2^(3^2)))//9)+((4%10)/3)'

gen 'return @integer 1'
gen 'return @string "hello"'
gen 'return @table {}'
gen 'return @integer[] {}'
gen 'return @number[] {}'
gen 'return @closure function() end'
gen 'return @number 54.4'
gen 'return @User.Type a'

gen 'return {1,2,3}'
gen 'return {[1] = a}'
gen 'return {a = b}'
gen 'return @integer[]{[1] = 5.5, [2] = 4}'
gen 'return @number[] {[1] = 4, [2] = 5.4}'
