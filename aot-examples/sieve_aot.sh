../build/trun --gen-C -main mymain -f sieve_lib.lua > sieve_lib.c
gcc -O2 -shared -fpic sieve_lib.c -o sieve_lib.so
../../ravi/build/ravi sieve_aot.lua