local function f()
   local tm = os.clock()
   local o: integer = 0
   for j = 1, 10000 do
      local x: integer = 0
      for k = 1, 100000 do
         x = x ~ (j + k)
      end
      o = o | x
   end
   print("   Result = "..o)
   print("   CPU time = "..(os.clock() - tm))
end
print"Benchmarking non-compiled function"
f()
print"Compiling the function"
assert(ravi.compile(f))
print"Benchmarking compiled function"
f() 