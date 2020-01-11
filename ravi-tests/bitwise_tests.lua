-- $Id: bitwise.lua,v 1.24 2014/12/26 17:20:53 roberto Exp $

print("testing bitwise operations")

local numbits: integer = string.packsize('j')
numbits = numbits * 8

function toint(x)
	return x
end

function testbits() 
	assert(~0 == -1)

	assert((1 << (numbits - 1)) == math.mininteger)

	-- basic tests for bitwise operators;
	-- use variables to avoid constant folding
	local a : integer, b: integer, c: integer, d: integer
	a = 0xFFFFFFFFFFFFFFFF
	assert(a == -1 and a & -1 == a and a & 35 == 35)
	a = 0xF0F0F0F0F0F0F0F0
	assert(a | -1 == -1)
	assert(a ~ a == 0 and a ~ 0 == a and a ~ ~a == -1)
	assert(a >> 4 == ~a)
	a = 0xF0; b = 0xCC; c = 0xAA; d = 0xFD
	assert(a | b ~ c & d == 0xF4)

	a = 0xF0000000; b = 0xCC000000;
	c = 0xAA000000; d = 0xFD000000
	assert(a | b ~ c & d == 0xF4000000)
	assert(~~a == a and ~a == -1 ~ a and -d == ~d + 1)
    print'+'
	
	a = a << 32
	b = b << 32
	c = c << 32
	d = d << 32
	assert(a | b ~ c & d == 0xF4000000 << 32)
	assert(~~a == a and ~a == -1 ~ a and -d == ~d + 1)
    print'+'

    --a = -1 >> 1
    --b = toint(2^(numbits - 1) - 1)
    --print'+'
    --c = 1 << 31
    --d = 0x80000000
	--assert(a == b and c == d)
	--assert(-1 >> (numbits - 1) == 1)
 
	a = -1 >> numbits
	b = -1 >> -numbits
	c = -1 << numbits
	d = -1 << -numbits
	assert(a == 0 and
	       b == 0 and
	       c == 0 and
	       d == 0)
    print'+'

    a = @integer math.tointeger(2^30) - 1
    b = @integer math.tointeger(2^30) - 1
    c = @integer math.tointeger(2^30)
    d = @integer math.tointeger(2^30)
	assert((a) << c == 0)
	assert((b) >> d == 0)
    print'+'

    a = 1 >> -3 
    b = 1 << 3
    c = 1000 >> 5
    d = 1000 << -5
    print'+'

	assert(a == b and c == d)

end

ravi.dumplua(testbits)
assert(ravi.compile(testbits))

testbits()

print'OK'