#!/bin/sh

LUA=$1
RUN_TRACEHOOK_TESTS=$2

if [ "$LUA" = "" ]
then
  echo "Please pass path to Ravi executable"
  exit 1
fi

run_lua53_tests() {
  arg=$1
  errmsg=$2
  cd lua53
  $LUA -e"$arg" all.lua
  rc=$?
  cd ..
  if [ $rc != 0 ]
  then
    echo "$errmsg"
    exit 1
  fi
}

run_ravi_tests() {
  dir=$1
  script=$2
  arg=$3
  cd $dir
  $LUA -e"$arg" $script
  rc=$?
  cd ..
  if [ $rc != 0 ]
  then
    echo "Test $script failed"
    exit 1
  fi
}

run_lua53_tests "_port=true ravi.jit(false)" "Lua53 interpreter test failed"
run_lua53_tests "_port=true ravi.auto(true)" "Lua53 auto JIT test failed"
run_lua53_tests "_port=true ravi.auto(true,1)" "Lua53 auto JIT all test failed"

run_ravi_tests language ravi_tests1.ravi "ravi.jit(false)"
run_ravi_tests language ravi_tests1.ravi "ravi.auto(true,1)"
run_ravi_tests language ravi_tests2.ravi "ravi.jit(false)"
run_ravi_tests language ravi_tests2.ravi "ravi.auto(true,1)"
run_ravi_tests language defer_tests.ravi "ravi.jit(false)"
run_ravi_tests language defer_tests.ravi "ravi.auto(true,1)"
run_ravi_tests language ravi_tests3.ravi "ravi.auto(true,1)"
run_ravi_tests language ravi_errors.ravi "ravi.auto(true,1)"
run_ravi_tests language basics.lua "ravi.auto(true,1)"
run_ravi_tests extra gaussian2.lua "ravi.auto(true,1)"
run_ravi_tests extra bittest.lua "ravi.auto(true,1)"
run_ravi_tests performance fannkuchen.ravi "ravi.auto(true,1)"
run_ravi_tests performance mandel1.ravi "ravi.auto(true,1)"
run_ravi_tests performance matmul1_ravi.lua "ravi.auto(true,1)"
run_ravi_tests performance sieve.ravi "ravi.auto(true,1)"
run_ravi_tests performance pisum.ravi "ravi.auto(true,1)"
run_ravi_tests performance md5test.lua "ravi.auto(true,1)"

if [ "$RUN_TRACEHOOK_TESTS" = "" ]
then
  exit 0
fi

# The following tests require JIT code to execute a hook at each instruction
# and are only supported by the LLVM backend
run_lua53_tests "_port=true ravi.tracehook(true) ravi.auto(true)" "Lua53 tracehook test failed"
run_lua53_tests "_port=true ravi.tracehook(true) ravi.auto(true,1)" "Lua53 tracehook test failed"
