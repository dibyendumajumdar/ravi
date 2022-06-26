a = function()
  return function (x)
    return function (y)
     return function (z)
       return x+y+z
     end
   end
  end
end
assert(a()(2)(3)(10) == 15)

print '65 Ok'