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
   not have a similar FFI interface. LuaJIT can also inline Lua function calls
   but Ravi does not have this ability and hence function calls go via the
   Lua infrastructure and are therefore expensive. Ravi's code generation is best
   when types are annotated as otherwise the dynamic type checks kill performance
   as above benchmarks show. Finally LLVM is a slow compiler relative to LuaJIT's
   JIT compiler which is very very fast.

5. Performance of Lua 5.3.2 is better than 5.3.0 or 5.3.1, thanks to the 
   table optimizations in this version.

In general to obtain the best performanc with Ravi, following steps are necessary.

1. Annotate types as much as possible.

2. Use fornum loops with integer counters.

3. Avoid function calls inside loop bodies.

4. Do not assume that JIT compilation is beneficial - benchmark code with and without
   JIT compilation.

5. Try to compile a set of functions (in a table) preferably at program startup.
   This way you pay for the JIT compilation cost only once.

6. Dump the generated Lua bytecode to see if specialised Ravi bytecodes are being 
   generated or not. If not you may be missing type annotations.

7. Avoid using globals.

8. Note that only functions executing in the main Lua thread are run in JIT mode.
   Coroutines in particular are always interpreted.

9. Also note that tail calls are expensive in JIT mode as they are treated as 
   normal function calls; so it is better to avoid JIT compilation of code that
   relies upon tail calls.