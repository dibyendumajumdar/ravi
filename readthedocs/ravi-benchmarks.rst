Ravi Performance Benchmarks
===========================

Ravi's reason for existence is to achieve greater performance than standard Lua 5.3. Hence performance benchmarks are of interest.

The programs used in the performance testing can be found at `Ravi Tests <https://github.com/dibyendumajumdar/ravi/tree/master/ravi-tests>`_ folder.

+-------------+---------+----------+-----------+
| Program     | Lua5.3  | Ravi     | Luajit 2.1|
+=============+=========+==========+===========+
|fornum_test1 | 9.187   | 2.765    | 0.309     |
+-------------+---------+----------+-----------+
|fornum_test2 | 9.57    | 2.782    | 0.93      |
+-------------+---------+----------+-----------+
|fornum_test3 | 53.932  | 15.92    | 7.806     |
+-------------+---------+----------+-----------+
|mandel       | 21.247  | 5.79     | 1.633     |
+-------------+---------+----------+-----------+

There are a number of reasons why Ravi's performance is not as good as Luajit.

1. Luajit uses an optimized representation of values. In Lua 5.3 and
   in Ravi, the value is 16 bytes - and many operations require two loads
   / two stores. Luajit therefore will always have an advantage here.

2. More work is needed to optimize fornum loops in Ravi. I have some
   ideas on what can be improved but have parked this for now as I want
   to get more coverage of Lua bytecodes first.

3. Luajit obviously is significant smaller in size when LLVM is added
   to the comparison. Using LLVM will tend to preclude Ravi's JIT engine
   from being useful in constrained devices - although ahead of time
   compilation could be used in such cases.

Ideas
-----
There are a number of improvements possible. Below are some of my thoughts.

Optimizing Fornum loops
-----------------------
The Lua fornum loops create an `extra "external" variable <http://www.lua.org/manual/5.3/manual.html#3.3.5>`_ that has the name given by the user. 
However an internal variable is actually used as the loop index. The external variable is updated at every iteration - this entails several IR 
instructions. The obvious optimization is to eliminate this variable by making the loop index available as a readonly value. If for backward 
compatiblity it is necessary to allow updates to the external variable then a compromise would be analyse the Lua program and only create the
external variable if necessary.

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

