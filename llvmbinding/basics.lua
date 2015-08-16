assert(llvm)

context = llvm.context()

assert(getmetatable(context).type == "LLVMcontext")

irbuilder = llvm.irbuilder()

assert(getmetatable(irbuilder).type == "LLVMirbuilder")

types = llvm.types()

print 'Listing types'
for k,v in pairs(types) do
	print('\t', k, getmetatable(v).type)
	llvm.dump(v)
end

gcobject = llvm.structtype("GCObject")

assert(getmetatable(gcobject).type == "LLVMstructtype")

pgcobject = llvm.pointertype(gcobject)

assert(getmetatable(pgcobject).type == "LLVMpointertype")

llvm.dump(pgcobject)
print(getmetatable(pgcobject).type)

llvm.addmembers(gcobject, {pgcobject, types.lu_byte, types.lu_byte})

llvm.dump(gcobject)
