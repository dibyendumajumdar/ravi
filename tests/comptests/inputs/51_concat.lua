f = compiler.load([[
function z()
    local days: table = {"Sunday", "Monday", "Tuesday", "Wednesday",
            "Thursday", "Friday", "Saturda"}
    local t = ''
    for k,v in pairs(days) do
        t = t .. v
        --print(t)
    end
    return t
end
]]
)
assert(f and type(f) == 'function')
f()
assert(z and type(z) == 'function')
assert(z() == "SundayMondayTuesdayWednesdayThursdayFridaySaturda")
print 'Ok'