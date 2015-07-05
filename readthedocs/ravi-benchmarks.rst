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
|mandel(4000)   | 21.247  | 2.936      | 1.633      |
+---------------+---------+------------+------------+
|fannkuchen(11) | 63.446  | 8.317      | 4.751      |
+---------------+---------+------------+------------+
|matmul(1000)   | 34.604  | 2.942      | 0.968      |
+---------------+---------+------------+------------+

Following points are worth bearing in mind when looking at above benchmarks.

1. Luajit uses an optimized representation of double values. In Lua 5.3 and
   in Ravi, a value is 16 bytes - and floating point operations require two loads
   / two stores. Luajit has a performance advantage when it comes to floating 
   point operations due to this.

2. More work is needed to optimize numeric operations in Ravi.

3. Luajit compilation approach ensures that it can use information about 
   the actual execution path taken by the code at runtime whereas Ravi
   compiles each function as a whole regardless of how it will be used.

4. For Ravi the timings above do not include the LLVM compilation time.
   But LuaJIT timings include the JIT compilation times, so they show
   incredible performance.

5. The benchmarks were run on Windows 8.1 64-bit. LLVM version 3.7 was used.

6. The Ravi benchmarks are based on code that uses optional static types.

Ideas
-----
There are a number of improvements possible. Below are some of my thoughts.

Allocating variables on C stack
-------------------------------
Certain local and temporary variables that hold numeric values could be allocated on the C stack avoiding the overhead of accessing the Lua stack. This requires implementing escape analysis to determine which variables are safe to be allocated on the C stack.

Optimizing Fornum loops
-----------------------
The Lua fornum loops create an `extra "external" variable <http://www.lua.org/manual/5.3/manual.html#3.3.5>`_ that has the name given by the user. 
However an internal variable is actually used as the loop index. The external variable is updated at every iteration - this entails several IR 
instructions. The obvious optimization is to eliminate this variable by making the loop index available as a readonly value. If for backward 
compatiblity it is necessary to allow updates to the external variable then a compromise would be analyse the Lua program and only create the
external variable if necessary.

The Fornum loop needs to handle four different scenarios, resulting from the type of the index variable and whether the loop increments or decrements. 
The generated code is not very efficient due to branching. The common case of integer index with constant step can be specialized for greater
performance. I have implemented the case when index is an integer and the step size is a positive constant. This seems to be the most common case.

The Value Storage
-----------------
In Lua the type of the value and the data associated with a value are stored in separate fields. Luajit however overlays the storage by utilizing
the `technique known as NaN tagging <http://lua-users.org/lists/lua-l/2009-11/msg00089.html>`_. The Luajit model is not suited for Lua 5.3 as in this version 64-int integers are natively supported by Lua. 

There is however still a possibility that NaN tagging can be used to improve performance of values that hold doubles. The following scheme should work.

Let the first 8 bytes hold a double value. And let the other values be held in the second 8 bytes.
Then the NaN tagging technique can be used to overlay the type information with the double part.
This would allow operations involving doubles to be faster as an extra step to set the type can be avoided. This would mean greater
performance in floating point operations which are important in many domains.

Above scheme has the additional advantage that it can be extended to support complex numbers.

* First 8 bytes could be a double representing the real part.
* Second 8 bytes could be a double representing the imaginary part.

If a value is a not a complex number then the real part will either be
NaN, or if the real part is a double then the imaginary part will be a
NaN.

The problem of course is that NaN tagging may not be viable in mainstream Lua as it is probably a non-portable technique. It could also 
introduce incompatibility between Lua and Ravi especially if Ravi supported complex numbers.

