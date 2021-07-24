# AOT Examples

This folder contains examples of code that can be compiled AOT and run via Ravi.
Assuming you have `gcc` available, you can run these examples as follows on a Linux host:

```
ravi gaussian2_aot.lua
ravi matrixmul_aot.lua
ravi sieve_aot.lua
```

Have a look at [aot.lua](https://github.com/dibyendumajumdar/ravi/blob/master/aot-examples/aot.lua) to see how the AOT C code is generated.

## Notes

Note that these examples can also be JIT compiled - for an example of how that works,
please see [21_matrixmul.lua](https://github.com/dibyendumajumdar/ravi/blob/master/tests/comptests/inputs/21_matrixmul.lua).


