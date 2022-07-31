local values = {}
for k,v in pairs({ name='Dibyendu', surname='Majumdar' }) do
    local key = k
    local value = v
    values[key] = value
end

assert(values.name == 'Dibyendu')
assert(values.surname == 'Majumdar')

print '75 Ok'