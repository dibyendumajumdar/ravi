-- This program demonstrates creating a 
-- recursive fibonacci calc function in LLVM
-- Example based upon 
-- https://llvm.org/svn/llvm-project/llvm/trunk/examples/Fibonacci/fibonacci.cpp


-- ensure that LLVM bindings are available
assert(llvm)


-- Generate the fibonacci function
local function makefib(context, module, types) 

  -- The goal of this snippet is to create in the memory the LLVM module
  -- consisting of one function as follow:
  --
  --   int fib(int x) {
  --     if(x<=2) return 1;
  --     return fib(x-1)+fib(x-2);
  --   }
  --

  -- Create the fib function and insert it into module M. This function is said
  -- to return an int and take an int parameter.
  local fibtype = context:functiontype(types.int, {types.int})
  local FibF = module:newfunction("fib", fibtype)

  -- Get a new IRBuilder intance
  -- this will be garbage collected by Lua
  local ir = context:irbuilder()

  -- Add a basic block to the function.
  local BB = context:basicblock("EntryBlock");
  FibF:appendblock(BB)
  ir:setinsertpoint(BB)

  -- Get pointers to the constants.
  local One = context:intconstant(1);
  local Two = context:intconstant(2);

  -- Get pointer to the integer argument of the add1 function...
  local ArgX = FibF:arg(1);   -- Get the arg.

  -- Create the true_block.
  local RetBB = context:basicblock("return");
  FibF:appendblock(RetBB)

  -- Create an exit block.
  local RecurseBB = context:basicblock("recurse");
  FibF:appendblock(RecurseBB)

  -- Create the "if (arg <= 2) goto exitbb"
  local CondInst = ir:icmpsle(ArgX, Two);
  ir:condbr(CondInst, RetBB, RecurseBB);

  ir:setinsertpoint(RetBB)
  -- Create: ret int 1
  ir:ret(One);

  -- create fib(x-1)
  ir:setinsertpoint(RecurseBB)
  local Sub = ir:nswsub(ArgX, One);
  local CallFibX1 = ir:call(FibF, {Sub}, {tailcall=true});

  -- create fib(x-2)
  Sub = ir:nswsub(ArgX, Two);
  local CallFibX2 = ir:call(FibF, {Sub}, {tailcall=true});

  -- fib(x-1)+fib(x-2)
  local Sum = ir:nswadd(CallFibX1, CallFibX2);

  -- Create the return instruction and add it to the basic block
  ir:ret(Sum);

  return FibF
end

-- Get the LLVM context - right now this is the
-- global context
local context = llvm.context()

-- The bindings provide a number of predefined types that
-- are Lua specific plus some standard C types such as 'int',
-- 'double', 'int64_t', etc.
local types = context:types()

-- Create a lua_CFunction instance
-- At this stage the function will get a module and 
-- execution engine but no body
local mainfunc = context:lua_CFunction("demofib")

-- Get hold of the module
-- as we will create the fib function as an 
-- iternal function
local module = mainfunc:module()

-- The actual fibonacci function is an internal
-- function so that it is pure native function
local fib = makefib(context, module, types)

-- Get a new IRBuilder
local ir = context:irbuilder()

-- Our main Lua function has only one block
local BB = context:basicblock("entry")
mainfunc:appendblock(BB)
ir:setinsertpoint(BB)

-- Declare prototypes
local luaL_checkinteger = mainfunc:extern("luaL_checkinteger")
local lua_pushinteger = mainfunc:extern("lua_pushinteger")

-- Get lua_State* 
local L = mainfunc:arg(1)

-- We are expecting one integer parameter
local intparam = ir:call(luaL_checkinteger, {L, context:intconstant(1)})
-- cast from 64 to 32 bit
intparam = ir:truncorbitcast(intparam, types.int)

-- Call the fib calculator
local result = ir:call(fib, {intparam})

-- Extend from 32 to 64 bit
result = ir:zext(result, types.lua_Integer)

-- Push the final integer result
ir:call(lua_pushinteger, {L, result})

-- Return 1 
ir:ret(context:intconstant(1))

---- We are done! ---

-- Dump the module for info
module:dump()

-- compile the Lua callable function
local runnable = mainfunc:compile()
assert(runnable(11) == 89)
print 'Ok'