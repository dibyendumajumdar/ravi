LUA=$1
if [ "$LUA" = "" ] 
then
  echo "Please pass path to Lua"
  exit 1
fi

# Run the interpreter tests first
$LUA -e"_port=true" all.lua
if [ $? != 0 ] 
then
  echo "all.lua interpreted failed"
  exit 1
fi

# Run tests in partial JIT mode, with line hook
$LUA -e"_port=true; ravi.auto(true)" all.lua
if [ $? != 0 ] 
then
  echo "all.lua partially compiled with tracehook failed"
  exit 1
fi

cd ../ravi-tests 
. ./run_tests.sh "$LUA"
cd -
