-- ensure that LLVM bindings are available
assert(llvm)

-- Get the LLVM context - right now this is the
-- global context
context = llvm.context()
assert(getmetatable(context).type == "LLVMcontext")

-- The bindings provide a number of predefined types that
-- are Lua specific plus some standard C types such as 'int',
-- 'double', 'int64_t', etc.
types = llvm.types()
print 'Listing types'
for k,v in pairs(types) do
	print('\t', k, getmetatable(v).type)
	llvm.dump(v)
end

-- ***************************************************\
-- Test creating a struct type
-- Just as an exercise create a struct type
-- Initially we have an opaque struct
gcobject = llvm.structtype("GCObject")
assert(getmetatable(gcobject).type == "LLVMstructtype")

-- Create pointer to GCObject
gcobject_p = llvm.pointertype(gcobject)
assert(getmetatable(gcobject_p).type == "LLVMpointertype")

-- Add the members of the struct
-- Following demonstrates the use of predefined Lua types 
llvm.setbody(gcobject, {gcobject_p, types.lu_byte, types.lu_byte})

-- Display structure of gcobject
llvm.dump(gcobject)

-- ****************************************************/

-- ****************************************************\
-- Test building a Lua C function without writing C code!
--
-- Create a lua_CFunction instance
-- At this stage the function will get a module and 
-- execution engine but no body
myfunc = llvm.lua_CFunction("myfunc")
assert(getmetatable(myfunc).type == "LLVMfunction")

-- Get a new IRBuilder intance
-- this will be garbage collected by Lua
irbuilder = llvm.irbuilder()
assert(getmetatable(irbuilder).type == "LLVMirbuilder")

-- Create a basic block
block = llvm.basicblock("entry")
-- Add it to the end of the function
llvm.appendblock(myfunc, block)
-- Set this as the next instruction point
llvm.setinsertpoint(irbuilder, block)

-----------------------------------------------
-- Test calling a predefined extern function
-- Get printf decl
-----------------------------------------------
printf = llvm.extern(myfunc, "printf")
assert(getmetatable(printf).type == "LLVMexternfunc")

luaL_checklstring = llvm.extern(myfunc, "luaL_checklstring")
assert(getmetatable(luaL_checklstring).type == "LLVMexternfunc")

hellostr = llvm.stringconstant(irbuilder, "hello world!\n")
llvm.call(irbuilder, printf, { hellostr })

-------------------------------------------------
-- Test calling a random function
------------------------------------------------- 
puts_type = llvm.functiontype(types.int, {types.pchar}, {vararg=false})
assert(getmetatable(puts_type).type == "LLVMfunctiontype")

-- Declare extern puts()
puts = llvm.extern(myfunc, "puts", puts_type);
assert(getmetatable(puts).type == "LLVMconstant")

-- Get the L parameter of myfunc
L = llvm.arg(myfunc, 1)

-- get the first argument as a string
str = llvm.call(irbuilder, luaL_checklstring, {L, llvm.intconstant(1), llvm.nullconstant(types.psize_t)} )

-- Call puts
llvm.call(irbuilder, puts, { str })

-- add CreateRet(0)
llvm.ret(irbuilder, llvm.intconstant(0))
-- **************************************************/

-- what did we get?
llvm.dump(myfunc)

-- JIT compile the function
-- returns a C Closure
runnable = llvm.compile(myfunc)

print('Type of compiled function is', type(runnable))
assert(not runnable('ok\n'))

