f = compiler.load([[
    return function(arr: number[], i: integer, value: number)
        arr[i] = value
    end
]]
)
assert(f and type(f) == 'function')
z = f()
assert(z and type(z) == 'function')
x = table.numarray(10)
z(x, 1, 1.1)
assert(x[1] == 1.1)
z(x, 2, 2.2)
assert(x[1] == 1.1)
assert(x[2] == 2.2)
z(x, 11, 11.11)
assert(x[1] == 1.1)
assert(x[2] == 2.2)
assert(x[11] == 11.11)
z(x, 12, 12)
assert(x[12] == 12.0)
print 'Ok'