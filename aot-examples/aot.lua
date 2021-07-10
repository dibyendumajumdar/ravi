local function readall(file)
    local f = assert(io.open(file, "rb"))
    local content = f:read("*all")
    f:close()
    return content
end

local function writestring(file,str)
    local f = io.open(file,'w')
    f:write(str)
    f:close()
end

local function comptoC(inputfile, outputfile)
    local chunk = readall(inputfile)
    local compiled = compiler.compile(chunk, 'mymain')
    if not compiled then
        error ('Failed to compile')
    end
    writestring(outputfile, compiled)
end

return {
    comptoC = comptoC
}

