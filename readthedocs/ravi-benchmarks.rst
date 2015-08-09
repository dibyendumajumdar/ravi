Ravi Performance Benchmarks
===========================
Ravi's reason for existence is to achieve greater performance than standard Lua 5.3. Hence performance benchmarks are of interest.

The programs used in the performance testing can be found at `Ravi Tests <https://github.com/dibyendumajumdar/ravi/tree/master/ravi-tests>`_ folder.

+---------------+---------+------------+------------+
| Program       | Lua5.3  | Ravi(LLVM) | Luajit 2.1 |
+===============+=========+============+============+
|fornum_test1   | 9.187   | 0.31       | 0.309      |
+---------------+---------+------------+------------+
|fornum_test2   | 9.57    | 0.917      | 0.906      |
+---------------+---------+------------+------------+
|fornum_test3   | 53.932  | 4.598      | 7.778      |
+---------------+---------+------------+------------+
|mandel(4000)   | 21.247  | 1.582      | 1.633      |
+---------------+---------+------------+------------+
|fannkuchen(11) | 63.446  | 4.55       | 4.751      |
+---------------+---------+------------+------------+
|matmul(1000)   | 34.604  | 1.018      | 0.968      |
+---------------+---------+------------+------------+

Following points are worth bearing in mind when looking at above benchmarks.

1. For Ravi the timings above do not include the LLVM compilation time.
   But LuaJIT timings include the JIT compilation times, so they show
   incredible performance.

2. The benchmarks were run on Windows 8.1 64-bit. A snapshot of upcoming 
   LLVM version 3.7 was used.

3. The Ravi benchmarks are based on code that uses optional static types;
   additionally for the matmul benchmark a setting was used to disable
   array bounds checks for array read operations.

4. Above benchmarks are primarily numerical. In real life scenarios there
   are other factors that affect performance. For instance, via FFI LuaJIT 
   is able to make efficient calls to external C functions, but Ravi does
   not have a similar FFI interface. 
   