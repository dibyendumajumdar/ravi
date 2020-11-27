f = compiler.load([[
    local arr: integer[] = table.intarray(10)
    arr[0] = 10
    arr[4] = 2
    arr[11] = 6
    local sum: integer = 0
    for i=0,11 do
        print(arr[i])
        sum = sum + arr[i]
    end
    return sum, arr
]]
)

assert(f and type(f) == 'function')
local s,arr = f()
assert(s == 18)
assert(arr[0] == 10)
assert(arr[4] == 2)
assert(arr[11] == 6)
assert(#arr == 11)
print 'Ok'