LUA=$1
if [ "$LUA" = "" ] 
then
  echo "Please pass path to Lua"
  exit 1
fi

$LUA -e"_U=true" all.lua
if [ $? != 0 ] 
then
  echo "all.lua interpreted failed"
  exit 1
fi

$LUA -e"_U=true; ravi.auto(true,1)" all.lua
if [ $? != 0 ] 
then
  echo "all.lua compiled failed"
  exit 1
fi

$LUA -e"_U=true; ravi.auto(true)" all.lua
if [ $? != 0 ] 
then
  echo "all.lua part compiled failed"
  exit 1
fi

cd ../ravi-tests 
. ./run_tests.sh "$LUA"
cd -
