-- ensure that LLVM bindings are available
assert(llvm)

-- Get the LLVM context - right now this is the
-- global context
context = llvm.context()
assert(getmetatable(context).type == "LLVMcontext")

-- Get a new IRBuilder intance
-- this will be garbage collected by Lua
irbuilder = llvm.irbuilder()
assert(getmetatable(irbuilder).type == "LLVMirbuilder")

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
