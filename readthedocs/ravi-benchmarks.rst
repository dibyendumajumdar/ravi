Ravi Performance Benchmarks
===========================
Ravi's reason for existence is to achieve greater performance than standard Lua 5.3. Hence performance benchmarks are of interest.

The programs used in the performance testing can be found at `Ravi Tests <https://github.com/dibyendumajumdar/ravi/tree/master/ravi-tests>`_ folder.

+--------------------+-----------+----------+------------+---------------+-----------+
| Program            | Lua5.3.2  | Ravi Int | Ravi(LLVM) | LuaJIT2.1 Int | LuaJIT2.1 |
+====================+===========+==========+============+===============+===========+
|fornum_test1.lua    | 8.952     | 8.327    | 0.321      | 3.516         | 0.312     |
+--------------------+-----------+----------+------------+---------------+-----------+
|fornum_test2.lua    | 9.195     | 9.205    | 4.73       | 3.75          | 0.922     |
+--------------------+-----------+----------+------------+---------------+-----------+
|fornum_test3.lua    | 52.494    | 47.367   | 4.722      | 16.74         | 7.75      |
+--------------------+-----------+----------+------------+---------------+-----------+
|mandel1.lua(4000)   | 20.324    | 20.436   | 8.186      | 8.469         | 1.594     |
+--------------------+-----------+----------+------------+---------------+-----------+
|mandel1.ravi(4000)  | n/a       | 16.67    | 1.572      | n/a           | n/a       |
+--------------------+-----------+----------+------------+---------------+-----------+
|fannkuchen.lua(11)  | 46.203    | 48.199   | 30.586     | 20.6          | 4.672     |
+--------------------+-----------+----------+------------+---------------+-----------+
|fannkuchen.ravi(11) | n/a       | 35.797   | 4.639      | n/a           | n/a       |
+--------------------+-----------+----------+------------+---------------+-----------+
|matmul1.lua(1000)   | 26.672    | 27.426   | 17.869     | 12.594        | 1.078     |
+--------------------+-----------+----------+------------+---------------+-----------+
|matmul1.ravi(1000)  | n/a       | 24.862   | 0.995      | n/a           | n/a       |
+--------------------+-----------+----------+------------+---------------+-----------+

Following points are worth bearing in mind when looking at above benchmarks.

1. For Ravi the timings above do not include the LLVM compilation time.

2. The benchmarks were run on Windows 10 64-bit. LLVM version 3.7 was used.
   Ravi and Lua 5.3.2 were compiled using Visual C++ 2015.

3. Some of the Ravi benchmarks are based on code that uses optional static types;
   additionally for the matmul benchmark a setting was used to disable
   array bounds checks for array read operations.

4. Above benchmarks are primarily numerical. In real life scenarios there
   are other factors that affect performance. For instance, via FFI LuaJIT 
   is able to make efficient calls to external C functions, but Ravi does
   not have a similar FFI interface. 

5. Performance of Lua 5.3.2 is better than 5.3.0 or 5.3.1, thanks to the 
   table optimizations in this version.