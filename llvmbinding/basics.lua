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

ptr_gcobject = llvm.pointertype(gcobject)

assert(getmetatable(ptr_gcobject).type == "LLVMpointertype")

llvm.dump(ptr_gcobject)