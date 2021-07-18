f = compiler.load([[
do
  local x = function()
    local function tryme()
      local i,j = 5,6
      return i,j
    end
    local i:integer, j:integer = tryme()
    assert(i+j == 11)
  end
  x()
end
]]
)
assert(f and type(f) == 'function')
f()
print 'Ok'