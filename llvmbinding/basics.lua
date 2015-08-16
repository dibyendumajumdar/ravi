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

-- Just as an exercise create a struct type
-- Initially we have an opaque struct
gcobject = llvm.structtype("GCObject")
assert(getmetatable(gcobject).type == "LLVMstructtype")

-- Create pointer to GCObject
gcobject_p = llvm.pointertype(gcobject)
assert(getmetatable(gcobject_p).type == "LLVMpointertype")

-- Add the members of the struct
-- Following demonstrates the use of predefined Lua types 
llvm.addmembers(gcobject, {gcobject_p, types.lu_byte, types.lu_byte})

-- Display structure of gcobject
llvm.dump(gcobject)

-- Create a function type
-- int testfunc(GCObject *obj)
testfunc = llvm.functiontype(types.int, {gcobject_p}, {vararg=false})
assert(getmetatable(testfunc).type == "LLVMfunctiontype")

llvm.dump(testfunc)

-- Create a lua_CFunction instance
-- At this stage the function will get a module and 
-- execution engine but no body
myfunc = llvm.Cfunction("myfunc")
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
llvm.setcurrent(irbuilder, block)
-- add CreateRet(0)
llvm.retval(irbuilder, llvm.intconstant(0))

-- what did we get?
llvm.dump(myfunc)

-- JIT compile the function
-- returns a C Closure
runnable = llvm.compile(myfunc)

print('Type of compiled function is', type(runnable))
assert(not runnable())

