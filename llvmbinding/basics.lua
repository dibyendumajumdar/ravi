assert(llvm)

context = llvm.context()

print(getmetatable(context).type)

irbuilder = llvm.irbuilder()

print(getmetatable(irbuilder).type)

types = llvm.types()

print 'Listing types'
for k,v in pairs(types) do
	print('\t', k, getmetatable(v).type)
end
