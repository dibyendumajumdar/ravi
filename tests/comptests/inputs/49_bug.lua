f = compiler.load([[
do
  local function test_numarray_meta()
    local farray : number[] = {1.1, 2.2, 3.3}
    setmetatable(farray, {
      __name = 'matrix',
      __tostring = function() return '{' .. table.concat(farray, ",") .. '}' end
      })
    assert(ravitype(farray) == 'matrix')
    assert(tostring(farray) == '{1.1,2.2,3.3}')
  end
  assert(pcall(test_numarray_meta));
end
]]
)
assert(f and type(f) == 'function')
f()
print 'Ok'