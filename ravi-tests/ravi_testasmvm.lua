local tests = {}

tests.RETURN = function()
	return
end

tests.LOADK = function() 
	return 1, 4.2, 'hello'
end

tests.MOVE = function(a, b)
	local c,d = a,b
	return c,d
end 
	

for k,v in pairs(tests) do
	print(k)
	ravi.dumplua(v)
	--v()
end