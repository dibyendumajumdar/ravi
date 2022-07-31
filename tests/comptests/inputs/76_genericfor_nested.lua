local values = {}

for k, v in pairs({name='Dibyendu'}) do
    for k, v in pairs({surname='Majumdar'}) do
        assert(k == 'surname')
        assert(v == 'Majumdar')
    end
    assert(k == 'name')
    assert(v == 'Dibyendu')
end

print '76 Ok'