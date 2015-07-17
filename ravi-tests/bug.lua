-- test 40
function x(t) return t; end
function f()
  local tt: integer[] = {1}
  local ss: number[] = { 55.5 }
  tt = x(tt)
  ss = x(ss)
end
assert(ravi.compile(x))
assert(ravi.compile(f))
assert(pcall(f))
function f()
  local tt: integer[] = {1}
  tt = x({})
end
--ravi.dumplua(f)
print'+'
assert(ravi.compile(f))
assert(not pcall(f))
print'+'
function f()
  local tt: integer[] = {1}
  local ss: number[] = { 55.5 }
  ss = x(tt)
end
print'+'
assert(ravi.compile(f))
assert(not pcall(f))

print("test 40 OK")

