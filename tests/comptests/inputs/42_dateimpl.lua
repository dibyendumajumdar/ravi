f = compiler.load([[
function from_dmy1(y: integer, m: integer, d: integer)
    if m <= 2 then
      y = y - 1
    end
    local era: integer
    if y >= 0 then
      era = y // 400
    else
      era = (y-399) // 400
    end
    local yoe: integer = y - era * 400
    local tmp: integer
    if m > 2 then
      tmp = -3
    else
      tmp = 9
    end
    local doy: integer = (153 * (m  + tmp) + 2)//5 + d-1
    local doe: integer = yoe * 365 + yoe//4 - yoe//100 + doy
    return era * 146097 + doe - 719468
end
function from_dmy2(y, m, d)
    if m <= 2 then
      y = y - 1
    end
    local era
    if y >= 0 then
      era = y // 400
    else
      era = (y-399) // 400
    end
    local yoe = y - era * 400
    local tmp
    if m > 2 then
      tmp = -3
    else
      tmp = 9
    end
    local doy = (153 * (m  + tmp) + 2)//5 + d-1
    local doe = yoe * 365 + yoe//4 - yoe//100 + doy
    return era * 146097 + doe - 719468
end
]]
)
assert(f and type(f) == 'function')
f()
assert(from_dmy2(1900, 1, 1) == -25567)
assert(from_dmy1(1900, 1, 1) == -25567)
assert(from_dmy1(2000, 1, 1) == from_dmy2(2000, 1, 1))
print 'Ok'