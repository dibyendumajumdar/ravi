adder = compiler.load('local a: number, b: number = 1.1, 2.2 return a+b')
assert(adder and type(adder) == 'function')
assert(math.abs(adder() - 3.3) < 1e-15)

mult = compiler.load('local a: number, b: number = 1.1, 2.2 return a*b')
assert(mult and type(mult) == 'function')
assert(math.abs(mult() - 2.42) < 1e-15)