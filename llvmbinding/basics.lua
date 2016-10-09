-- ensure that LLVM bindings are available
assert(llvm)

-- Get the LLVM context - right now this is the
-- global context
context = llvm.context()
assert(ravitype(context) == "LLVMcontext")

-- The bindings provide a number of predefined types that
-- are Lua specific plus some standard C types such as 'int',
-- 'double', 'int64_t', etc.
types = context:types()
print 'Listing types'
for k,v in pairs(types) do
	print('\t', k, ravitype(v))
	llvm.dump(v)
end

-- ***************************************************\
-- Test creating a struct type
-- Just as an exercise create a struct type
-- Initially we have an opaque struct
gcobject = context:structtype("GCObject")
assert(ravitype(gcobject) == "LLVMstructtype")

-- Create pointer to GCObject
gcobject_p = context:pointertype(gcobject)
assert(ravitype(gcobject_p) == "LLVMpointertype")

-- Add the members of the struct
-- Following demonstrates the use of predefined Lua types 
gcobject:setbody {gcobject_p, types.lu_byte, types.lu_byte}

-- Display structure of gcobject
llvm.dump(gcobject)

-- ****************************************************/

-- ****************************************************\
-- Test building a Lua C function without writing C code!
--
-- Create a lua_CFunction instance
-- At this stage the function will get a module and 
-- execution engine but no body
myfunc = context:lua_CFunction("myfunc")
assert(ravitype(myfunc) == "LLVMmainfunction")

-- Get a new IRBuilder intance
-- this will be garbage collected by Lua
irbuilder = context:irbuilder()
assert(ravitype(irbuilder) == "LLVMirbuilder")

-- Create a basic block
block = context:basicblock("entry")
-- Add it to the end of the function
myfunc:appendblock(block)
-- Set this as the next instruction point
irbuilder:setinsertpoint(block)

-----------------------------------------------
-- Test calling a predefined extern function
-- Get printf decl
-----------------------------------------------
printf = myfunc:extern("printf")
assert(ravitype(printf) == "LLVMconstant")

luaL_checklstring = myfunc:extern("luaL_checklstring")
assert(ravitype(luaL_checklstring) == "LLVMconstant")

hellostr = irbuilder:stringconstant("hello world!\n")
irbuilder:call(printf, { hellostr })

-------------------------------------------------
-- Test calling a random function
------------------------------------------------- 
puts_type = context:functiontype(types.int, {types.pchar}, {vararg=false})
assert(ravitype(puts_type) == "LLVMfunctiontype")

-- Declare extern puts()
puts = myfunc:extern("puts", puts_type);
assert(ravitype(puts) == "LLVMconstant")

-- Get the L parameter of myfunc
L = myfunc:arg(1)

-- get the first argument as a string
str = irbuilder:call(luaL_checklstring, {L, context:intconstant(1), 
	                 context:nullconstant(types.psize_t)} )

-- Call puts
irbuilder:call(puts, { str })

-- add CreateRet(0)
inst = irbuilder:ret(context:intconstant(0))
assert(ravitype(inst) == "LLVMinstruction")
-- **************************************************/

-- what did we get?
llvm.dump(myfunc)

-- JIT compile the function
-- returns a C Closure
runnable = myfunc:compile()

print('Type of compiled function is', type(runnable))
assert(not runnable('ok\n'))

