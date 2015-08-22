-- This small demo creates a compiled function
-- takes a string parameter and calls puts() to 
-- print this

-- ensure that LLVM bindings are available
assert(llvm)

-- Get the LLVM context - right now this is the
-- global context
context = llvm.context()

-- The bindings provide a number of predefined types that
-- are Lua specific plus some standard C types such as 'int',
-- 'double', 'int64_t', etc.
types = llvm.types()

-- Create a lua_CFunction instance
-- At this stage the function will get a module and 
-- execution engine but no body
myfunc = llvm.lua_CFunction("myfunc")

-- Get a new IRBuilder intance
-- this will be garbage collected by Lua
irbuilder = llvm.irbuilder()

-- Create a basic block
block = llvm.basicblock("entry")
-- Add it to the end of the function
myfunc:appendblock(block)
-- Set this as the next instruction point
irbuilder:setinsertpoint(block)

-- get declaration for luaL_checklstring
luaL_checklstring = myfunc:extern("luaL_checklstring")

-- Get the L parameter of myfunc
L = myfunc:arg(1)

-- get the first argument as a string
str = irbuilder:call(luaL_checklstring, {L, llvm.intconstant(1), llvm.nullconstant(types.psize_t)} )

-- declare puts
puts_type = llvm.functiontype(types.int, {types.pchar})
puts = myfunc:extern("puts", puts_type);

-- Call puts
irbuilder:call(puts, {str})

-- add CreateRet(0)
irbuilder:ret(llvm.intconstant(0))
-- **************************************************/

-- what did we get?
llvm.dump(myfunc)

-- JIT compile the function
-- returns a C Closure
runnable = myfunc:compile()

assert(not runnable('hello world\n'))

