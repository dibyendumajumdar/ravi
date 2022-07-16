local t1 = { text = 'hello' }
function t1:hello()
    return self.text
end

local t2 = { t1 }
local t3 = { t2 }

assert('hello' == t3[1][1]:hello())

local function foo()
    return t3
end

local function index()
    return 1
end

assert('hello' == foo()[index()][index()]:hello())
print '71 Ok'