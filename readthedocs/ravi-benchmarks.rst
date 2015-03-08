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

