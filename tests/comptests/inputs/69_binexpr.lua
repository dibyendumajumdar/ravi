local b: number = 0.5
local i: integer = 1
local j: integer = 2
local a:number[] = { 4.0, 6.0 }
a[i]=a[i]+b*a[j]
assert(a[i] == 7.0)