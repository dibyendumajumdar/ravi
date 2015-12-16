LUA=$1
if [ "$LUA" = "" ] 
then
  echo "Please pass path to Lua"
  exit 1
fi

$LUA fornum_test1.lua
if [ $? != 0 ] 
then
  echo "fornum_test1 failed"
  exit 1
fi

$LUA fornum_test2.ravi
if [ $? != 0 ] 
then
  echo "fornum_test2 failed"
  exit 1
fi

$LUA mandel1.ravi
if [ $? != 0 ] 
then
  echo "mandel1 failed"
  exit 1
fi

$LUA fannkuchen.ravi
if [ $? != 0 ] 
then
  echo "fannkuchen failed"
  exit 1
fi

$LUA matmul1.ravi
if [ $? != 0 ] 
then
  echo "matmul1 failed"
  exit 1
fi

$LUA basics.lua
if [ $? != 0 ] 
then
  echo "basics failed"
  exit 1
fi

$LUA ravi_tests1.ravi
if [ $? != 0 ] 
then
  echo "ravi_tests1 failed"
  exit 1
fi

$LUA fornum_test3.lua
if [ $? != 0 ] 
then
  echo "fornum_test3 failed"
  exit 1
fi

$LUA bitwise_tests.lua
if [ $? != 0 ] 
then
  echo "bitwise failed"
  exit 1
fi

$LUA ravi_errors.ravi
if [ $? != 0 ] 
then
  echo "ravi_errors failed"
  exit 1
fi

$LUA -e"ravi.auto(true,1)" ravi_errors.ravi
if [ $? != 0 ] 
then
  echo "ravi_errors failed"
  exit 1
fi

$LUA ravi_tests2.ravi
if [ $? != 0 ] 
then
  echo "ravi_tests2 failed"
  exit 1
fi

$LUA -e"ravi.auto(true,1)" ravi_tests2.ravi
if [ $? != 0 ] 
then
  echo "ravi_tests2 failed"
  exit 1
fi

$LUA gaussian2.lua
if [ $? != 0 ] 
then
  echo "gaussian2 failed"
  exit 1
fi

$LUA -e"ravi.auto(true,1)" gaussian2.lua
if [ $? != 0 ] 
then
  echo "gaussian2 failed"
  exit 1
fi
