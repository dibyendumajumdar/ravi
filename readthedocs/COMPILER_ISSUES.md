# Ravi compiler (ravicomp) issues

Tested 2026-11-12 at commit `abe1eebf630971615afa087524c65f703fbcb13c`.
Scope: the new compiler framework in `ravicomp/` (parser, linearizer, C code
generator, embedded chibicc front end) as reached through `compiler.load`,
`compiler.loadfile` and `compiler.compile`. The interpreter and the bytecode
JIT are excluded. No implementation files changed.

## Method

- Built Release (as in `.github/workflows/build.yml`) and Debug with ASAN
  (`-DCMAKE_BUILD_TYPE=Debug -DLTESTS=OFF`), Linux x86-64, gcc, MIR backend.
- Ran `tests/run_tests.sh` on Release: all pass, including
  `tests/comptests/test.lua`.
- Ran `tests/comptests/test.lua` on Debug/ASAN: aborts (Bug-13).
- Differential testing: about 320 small snippets, each run in its own process
  through `load` (reference) and `compiler.load`, comparing returned values or
  whether an error was raised. Run on both Release and Debug/ASAN.
- One AOT round trip (`compiler.compile` → gcc → `package.load_ravi_lib`)
  produced the correct result. AOT was not tested further; every issue below
  was observed on the JIT path, but all except the MIR-specific diagnostics
  originate in the generated C and should affect AOT equally.

"Interpreter" below means the result from `load`; "compiled" means the result
from `compiler.load`. Locations are given where the cause was confirmed from
the generated C or the source; otherwise the issue is marked as not localised.

Priorities: P1 = silent wrong result or memory unsafety in ordinary code;
P2 = crash, hang or wrong result in edge cases, or valid code rejected;
P3 = minor divergence or diagnostic quality.

## Summary

| Id | Pri | Issue |
|---|---|---|
| Bug-1 | P1 | Multi-value `return` overwrites values it has not yet read |
| Bug-2 | P1 | Integer `/` is compiled as C integer division; SIGFPE on zero |
| Bug-3 | P1 | Typed array reads have no bounds check |
| Bug-4 | P1 | String constants are emitted unescaped / corrupt the generated C |
| Bug-5 | P1 | Float assigned to an `integer` local is bit-reinterpreted |
| Bug-6 | P2 | `integer` parameters and upvalues are not converted from float |
| Bug-7 | P2 | `(f())` is not adjusted to one value when `f` returns nothing |
| Bug-8 | P2 | Multiple assignment evaluates target subexpressions too late |
| Bug-9 | P2 | Numeric `for` loop overflows at `math.maxinteger` and never ends |
| Bug-10 | P2 | Constant-folded infinity and NaN are emitted as identifiers |
| Bug-11 | P2 | `and` / `or` with a call operand fails to compile |
| Bug-12 | P2 | Negative constant shift count emits invalid C |
| Bug-13 | P2 | Out-of-bounds read in chibicc `C_equal` (ASAN) |
| Bug-14 | P2 | Assertion failures in the linearizer in Debug builds |
| Bug-15 | P3 | Large decimal integer literals become floats |
| Bug-16 | P3 | `\u{XXX}` escape is rejected by the lexer |
| Bug-17 | P3 | Constant `1//0` and `1%0` are compile-time errors |
| Bug-18 | P3 | `@number` cast of an integer-typed expression fails to compile |
| Lim-1 | — | Recursion depth limited to about 190; no tail calls |
| Lim-2 | — | Numeric `for` loops support integer bounds only |
| Lim-3 | — | Table constructor ordering differs for duplicate keys |

## Findings

### Bug-1 P1 — Multi-value `return` overwrites values it has not yet read

Location: `ravicomp/src/codegen.c`, `emit_op_ret` (line 1350).

Return values are copied one at a time to `S(0)`, `S(1)`, … where
`S(k) = ci->func + k`. Registers are `R(k) = ci->func + 1 + k`, so writing
`S(k)` destroys `R(k-1)`. Any later return value that still lives in a lower
register is read after it has been overwritten.

```lua
local a, b, c = "A", "B", "C"
return c, b, a
-- interpreter: C B A      compiled: C B B

local function f(a, b, c) return 0, 0, a, b, c end
return f(1, 2, 3)
-- interpreter: 0 0 1 2 3  compiled: 0 0 0 0 0
```

Generated C for `return Tflt(2), Tflt(3), T(0)`:

```c
{ TValue *dst_reg = S(0); setfltvalue(dst_reg, raviX__f_2); }
{ TValue *dst_reg = S(1); setfltvalue(dst_reg, raviX__f_3); }   /* S(1) == R(0) */
{ const TValue *src_reg = R(0); TValue *dst_reg = S(2); ... }    /* reads clobbered R(0) */
```

The hazard is not limited to locals. The `RET` instruction accepts any pseudo
as an operand, and stack temps and a trailing multi-result range are
overwritten in the same way:

```lua
gt = {x = "X"}
return "p", "q", gt.x
-- IR: RET {'p' Ks, 'q' Ks, T(0)}; no locals, so T(0) is R(0) == S(1)
-- interpreter: p q X      compiled: p q q

local function g() return 7, 8 end
return 1, 2, 3, 4, g()
-- IR: RET {1, 2, 3, 4, T(0..)}; the range starts at R(1) == S(2)
-- interpreter: 1 2 3 4 7 8   compiled: 1 2 3 4 3 4
```

Operand `j` (0-based) held in register `r` is overwritten before it is read
when `r <= j - 2`. For a trailing range starting at register `r` the copy is
additionally an overlapping ascending copy with the destination above the
source when `r < j - 1`.

Returning two values, or values already in ascending register order
(`return a, b, c`), happens to work, which is why the existing tests pass.

#### Fix

Status: implemented in `emit_op_ret` (`ravicomp/src/codegen.c`) with tests in
`tests/comptests/inputs/79_return_overlap.lua`; awaiting review. The design
follows.

Implementation note: the upward range copy recomputes both pointers from the
remaining count on each iteration. A first version that pre-decremented two
pointers inside `while (nrange-- > 0)` returned `1 2 3 4 8 8` for
`return 1, 2, 3, 4, g()` under the MIR C compiler, although the C is valid.
The cause was not investigated; it may be a separate MIR/c2mir problem.

Fix in the code generator only: `emit_op_ret` in `ravicomp/src/codegen.c`.
The IR and the linearizer are unchanged; `RET` keeps accepting arbitrary
operands.

Why not the linearizer: forcing operands into temps is only correct if every
operand, including constants and typed C temps, is moved into consecutive
stack temps. That adds a move per returned value in the common case and gives
up returning typed values straight from C variables. The overlap is a
consequence of where `emit_op_ret` places results, so it is handled there.

Why not reorder the copies: no order is safe in general. `return c, b, a`
with `a, b, c` in `R(0), R(1), R(2)` fails ascending (writing `S(1)` destroys
`a`) and descending (writing `S(2)` destroys `b`). A temporary is required.

Definitions. `n` is the number of `RET` operands. The fixed operands are those
that are not a trailing `PSEUDO_RANGE`. An operand is *stack-resident* if it is
a local `PSEUDO_SYMBOL`, `PSEUDO_TEMP_ANY` or `PSEUDO_RANGE_SELECT`; its
register `r` is `compute_register_from_base()`. Upvalue symbols, constants,
`nil`/`true`/`false` and typed temps (`PSEUDO_TEMP_INT/FLT/BOOL`) are never
stack-resident in this frame and need no handling. All registers are known at
code-generation time, so every decision below is static.

1. **Fixed operands.** Fixed operand `j` (0-based) must be saved if it is
   stack-resident and `r <= j - 2`. For each such operand emit, before the
   first write to any `S(k)`, a C local holding a copy of the value:

   ```c
   TValue ret_save_2; ret_save_2.tt_ = R(0)->tt_; ret_save_2.value_.gc = R(0)->value_.gc;
   ```

   and later write `S(j)` from the saved copy instead of calling `emit_move`
   on the register. Member-wise copy is used, as elsewhere in the generated
   code, rather than struct assignment. No allocation or GC can happen between
   the save and the write (`luaF_close` runs before the block), so a plain C
   local is safe. Operands that do not meet the condition are emitted exactly
   as today, so the common cases generate identical code.

2. **Trailing range starting at register `rr`.**
   - `rr >= n - 2`: results move down or stay in place. The existing
     ascending copy after the fixed writes is correct; no change.
   - `rr < n - 2`: the range has to move *up* by `d = n - 2 - rr` slots.
     Emit, after the saves from step 1 and before the fixed writes:
     - a stack check for the `d` extra slots, in the same form already used
       by `emit_op_call`:
       `if (stackoverflow(L, d)) { luaD_growstack(L, d); base = ci->u.l.base; }`
       and assign `stackbase = ci->func` after it rather than before;
     - a descending copy of the `L->top - R(rr)` range values to
       `S(n - 1)` onwards, last element first.

     The fixed writes then follow and only touch `S(0) .. S(n - 2)`, which is
     below the moved range. Because the moved range's upper bound is only
     known at run time, in this case save *every* stack-resident fixed
     operand in step 1, not just those with `r <= j - 2`.

   The stack check also closes a latent overflow: today
   `return 1, 2, …, k, g()` writes up to `d` slots above `L->top` without
   checking for space.

3. Everything after the copies (`wanted` nil-fill, `L->top`, `L->ci`) is
   unchanged.

Tests: add `tests/comptests/inputs/79_return_overlap.lua` and register it in
`tests/comptests/test.lua`. Each case asserts the exact returned values:

- locals in reversed and rotated order: `return c, b, a`, `return b, c, a`;
- parameters: `function f(a, b, c) return 0, 0, a, b, c end`;
- constants followed by a stack temp with no locals: `return "p", "q", gt.x`;
- typed temps followed by a stack temp:
  `local a: number, b: number = 7.5, 2.0 return a-b, a*b, a//b`;
- trailing range moving up: `return 1, 2, 3, 4, g()` with `g` returning
  zero, one and several values;
- trailing range moving up far enough to need the stack check (a long
  constant list before the call);
- trailing range moving down or in place (`return g()`, `return a, g()`) to
  confirm the unchanged path;
- callers that want fewer and more results than are returned
  (`local x = f()`, `local a, b, c, d, e, f2 = f()`).

The fix also needs a run of the full `tests/run_tests.sh`, since every
compiled function return goes through this code.
This bug hides other results; the remaining findings were confirmed with the
return list wrapped in `table.pack(...)` or reduced to a single value.

### Bug-2 P1 — Integer `/` is compiled as C integer division; SIGFPE on zero

Location: `ravicomp/src/codegen.c`, `emit_bin_ii` (line 1708); `op_divii`
shares the `"/"` operator string with `op_divff` (lines 1743–1746).

The linearizer correctly types the result as float (`DIVii {Tint, Tint}
{Tflt}`), but the generated C is `raviX__f_0 = raviX__i_0 / raviX__i_1;` with
no cast, so the division truncates. Triggered whenever both operands are known
integers, including untyped locals initialised from integer literals and
elements of `integer[]`.

```lua
local a, b = 7, 3
return a / b
-- interpreter: 2.3333333333333   compiled: 2.0

local a: integer, b: integer = 1, 0
return a / b
-- interpreter: inf               compiled: process killed by SIGFPE (exit 136)
```

`math.mininteger / -1` with typed operands also dies with SIGFPE.

### Bug-3 P1 — Typed array reads have no bounds check

Location: `ravicomp/src/codegen.c`, `emit_op_arrayget_ikey` (line 1892).

The emitted code is `iptr[ukey]` with no comparison against `arr->len`. Reads
past the end return whatever is in memory; far indices fault. Stores are
checked correctly.

```lua
local a: integer[] = {1, 2, 3}
local i: integer = 4
return a[i]
-- interpreter: error "array out of bounds"   compiled: 0 (garbage)

local a: number[] = {1.0}
local i: integer = 2
return a[i]
-- interpreter: error                         compiled: 2.4703282292062e-322

local a: integer[] = {1, 2, 3}
local i: integer = -100000000
return a[i]
-- interpreter: error                         compiled: SIGSEGV (exit 139)
```

Also reproduces through a typed parameter
(`local function g(a: integer[], i: integer) return a[i] end`).

### Bug-4 P1 — String constants are emitted unescaped / corrupt the generated C

Location: `ravicomp/src/codegen.c`, `output_string_literal` (line 3374) and the
per-instruction `//` comment that echoes the IR text.

Three separate effects:

1. `"` and `\` are not escaped in the C string literal passed to
   `luaS_newlstr`. A backslash followed by a valid C escape silently changes
   the string; otherwise the C fails to compile.
2. A newline inside a string constant is copied verbatim into the `// <IR>`
   comment preceding each instruction, ending the comment early. Any string
   containing a newline therefore fails to compile.
3. Bytes ≥ 0x80 (for example `"\200\255"`) fail to compile.

```lua
local s = "a\\b" return s
-- interpreter: a\b            compiled: "a", backspace, NUL  (silent)

local s = "a\\nb" return s
-- interpreter: a\nb (4 chars) compiled: "a", newline, "b", NUL

print("hello\n")               -- compiled: fails to compile (unterminated char)
return "q\"x"                  -- compiled: fails to compile (unterminated string)
local s = "a\\" return s       -- compiled: fails to compile
return "\200\255"              -- compiled: fails to compile
```

Generated C for the quote case: `luaS_newlstr(L, "q"x", 3)`. Because the
content is not escaped, a Lua string literal can inject arbitrary C into the
generated module.

Tab, `\r`, embedded NUL, `'` and `*/` were handled correctly.

### Bug-5 P1 — Float assigned to an `integer` local is bit-reinterpreted

Location: not localised; the store into the typed local after a dynamic value.

When a value of unknown type that is a float with an integral value is
assigned to an existing `integer` local, the double's bit pattern is stored as
the integer.

```lua
local x: integer = 1
local function f() return 2.0 end
x = f()
return x
-- interpreter: 2     compiled: 4611686018427387904  (0x4000000000000000)

local x: integer = 1
local t = {2.0}
x = t[1]
return x
-- interpreter: 2     compiled: 4611686018427387904
```

Non-integral floats (`2.5`) and strings correctly raise an error, and the
initialising form `local x: integer = f()` behaved correctly in the cases
tried, so the type check runs but the conversion does not.

### Bug-6 P2 — `integer` parameters and upvalues are not converted from float

Location: not localised.

Same family as Bug-5, but the value is left as a float instead of being
converted.

```lua
local function f(a: integer) return a end
return f(3.0)
-- interpreter: 3 (integer)    compiled: 3.0 (float)

local x: integer = 1
local function set(v) x = v end
set(2.0)
return x, math.type(x)
-- interpreter: 2 integer      compiled: 2.0 float
```

Code that then relies on the declared type (integer arithmetic, array
indexing) operates on a mistyped value.

### Bug-7 P2 — `(f())` is not adjusted to one value when `f` returns nothing

Location: not localised; the IR is `CALL {T(0)} {T(0), -1 Kint(0)}`, i.e. the
parenthesised call still requests multiple results.

With zero results nothing is written to the target register, which still holds
the function being called.

```lua
local function f() end
return (f())                 -- interpreter: nil     compiled: function
return tostring((f()))       -- interpreter: "nil"   compiled: "function: 0x…"
local t = {(f())} return #t  -- interpreter: 0       compiled: 1
```

`local x = (f())` and `f() == nil` are correct.

### Bug-8 P2 — Multiple assignment evaluates target subexpressions too late

Location: not localised (linearizer assignment handling).

The Lua manual requires all expressions, including table and key expressions
on the left, to be evaluated before any assignment is performed.

```lua
local t = {}
local i = 1
i, t[i] = i + 1, 20
return t
-- interpreter: t[1] == 20     compiled: t[2] == 20
```

### Bug-9 P2 — Numeric `for` loop overflows at `math.maxinteger` and never ends

Location: linearizer `for` loop lowering (`ravicomp/src/linearizer.c`, near
line 2600). The IR pre-decrements the index, then on each iteration does
`ADDii idx, step` followed by `LIii limit, idx`.

When the limit is `math.maxinteger` the increment overflows (signed overflow,
undefined behaviour in C) and the exit test never succeeds. PUC Lua avoids
this by precomputing the iteration count.

```lua
local n = 0
for i = math.maxinteger - 2, math.maxinteger do n = n + 1 end
return n
-- interpreter: 3     compiled: does not terminate
```

Variants of the same loop with a `break` or `return` in the body terminated
with the correct count, which is consistent with the overflow being undefined
behaviour rather than a deterministic wrap. The symmetric case toward
`math.mininteger` with a negative step was only tested with an early exit.

### Bug-10 P2 — Constant-folded infinity and NaN are emitted as identifiers

Location: float constant formatting in `ravicomp/src/codegen.c`; folding in
`ravicomp/src/ast_simplify.c`.

When constant folding yields ±inf or NaN, the generated C contains the bare
tokens `Infinity` or `NaN`, and compilation fails.

```lua
return 1/0          -- undeclared identifier Infinity
return 0/0          -- undeclared identifier NaN
return 2^1024       -- undeclared identifier Infinity
return 1e308 * 10   -- undeclared identifier Infinity
local x = -(1/0)    -- undeclared identifier Infinity
```

`math.huge` and non-constant expressions (`local a = 1.0 return a / 0.0`) work.

### Bug-11 P2 — `and` / `or` with a call operand fails to compile

Location: `ravicomp/src/codegen.c`, `emit_move` (lines 1286, 1292), reached
with a range pseudo `T(n..)` as the source.

```lua
local function t() return true end
local x = t() and t()   -- emit_move: Unexpected src pseudo: T(1..)
local y = t() or t()    -- emit_move: Unexpected src pseudo: T(1..)
while lt(i, 5) and (lt(i, 3) or lt(0, 1)) do … end   -- same error
```

The existing `74_luajit_andor.lua` / `77_luajit_andor_mult.lua` tests cover
`and`/`or` over variables and constants, not calls.

### Bug-12 P2 — Negative constant shift count emits invalid C

Location: `ravicomp/src/codegen.c` lines 1786–1791. `op_shrii` is emitted as
`luaV_shiftl(x, -<operand>)`; with a negative constant operand this produces
`--4`.

```lua
local a: integer = 1
return a >> -4
-- generated: raviX__i_1 = luaV_shiftl(raviX__i_0, --4);
-- "lvalue required as left operand of assignment"; fails to compile
```

`a << -4`, and shifts by ±63 / ±64 or by a variable count, are correct.

### Bug-13 P2 — Out-of-bounds read in chibicc `C_equal` (ASAN)

Location: `ravicomp/src/chibicc/chibicc_tokenize.c:139`.

```c
bool C_equal(C_Token *tok, char *op) {
  return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}
```

`memcmp` reads `tok->len` bytes from `op` even when `op` is a shorter string
literal. ASAN reports a global-buffer-overflow (read of size 7) via
`C_consume` ← `parse_typedef` (`chibicc_parse.c:3148`) ← `analyze_C_code`
(`codegen.c:2818`) ← `emit_op_C__unsafe`.

Reproduce: run `tests/comptests/test.lua` with a Debug/ASAN build; it aborts
at the first embedded-C test (`54_embed_C.lua`). As a consequence the compiler
test suite cannot currently complete under ASAN.

### Bug-14 P2 — Assertion failures in the linearizer in Debug builds

Release builds produce the correct result for all three; Debug builds abort.

```lua
local a: integer[] = {1, 2, 3}
return a.x
-- linearizer.c:1351: indexed_load: Assertion
-- `container_type != RAVI_TARRAYINT && container_type != RAVI_TARRAYFLT' failed

local function f() local _ENV = {y = 5} return y end
local function f(_ENV) return a + b end
-- linearizer.c:456: allocate_symbol_pseudo: Assertion
-- `sym->variable.pseudo == NULL' failed
```

### Bug-15 P3 — Large decimal integer literals become floats

Location: not localised (`ravicomp/src/lexer.c` numeral handling or constant
emission).

```lua
return 9223372036854775807   -- interpreter: integer   compiled: 9.2233720368548e+18
return 9223372036854775806   -- interpreter: integer   compiled: 9.2233720368548e+18
```

`0x7fffffffffffffff` and `math.maxinteger` are correct. The smallest affected
magnitude was not determined. A side effect is that
`for i = 9223372036854775805, 9223372036854775807` is rejected as a
non-integer loop.

### Bug-16 P3 — `\u{XXX}` escape is rejected by the lexer

Location: `ravicomp/src/lexer.c:757` (`case 'u'` is commented out).

```lua
return "\u{48}"   -- input(1): invalid escape sequence near '"\u'
```

### Bug-17 P3 — Constant `1//0` and `1%0` are compile-time errors

Location: `ravicomp/src/ast_simplify.c` constant folding.

```lua
return 1 // 0   -- compiled: "attempt to divide by zero", chunk does not compile
return 1 % 0    -- compiled: "attempt to perform 'n%%0'", chunk does not compile
```

The interpreter loads the chunk and raises at run time, so the error can be
caught by `pcall` or never reached. The second message also contains a stray
`%%`.

### Bug-18 P3 — `@number` cast of an integer-typed expression fails to compile

Location: `ravicomp/src/codegen.c:1178`.

```lua
local x = 5
local y: number = @number x
-- "Unexpected src pseudo: Tint(0)"
```

Related: because untyped locals initialised from literals are given an
inferred static type, casts the interpreter accepts or rejects at run time are
rejected at compile time (`local x = 5.0 local y: integer = @integer x` →
"Cannot convert type to integer"). This may be intended, but it differs from
the interpreter, where the first of these succeeds.

## Limitations not listed in `ravi-compiler.rst`

### Lim-1 — Recursion depth limited to about 190; no tail calls

Each compiled-to-compiled call is a C call and counts against the C call
limit. Tail calls are compiled as ordinary calls.

```lua
local function d(n) if n == 0 then return 0 end return 1 + d(n - 1) end
return d(190)     -- interpreter: 190   compiled: error "stack overflow"

local function loop(n, acc) if n == 0 then return acc end return loop(n - 1, acc + n) end
return loop(150, 0)   -- interpreter: 11325   compiled: error "stack overflow"
```

`fib(20)`-style recursion (shallow, wide) works.

### Lim-2 — Numeric `for` loops support integer bounds only

Float literal bounds or step are rejected at compile time ("Only for loops
with integer expressions currently supported"). With untyped bounds the chunk
compiles, and a float or string bound raises "integer expected" at run time,
including cases the interpreter accepts with an integer loop variable
(`for i = 1, 3.5`). A string literal bound produces an internal message
(`emit_move_inttemp: Unexpected pseudo: '1' Ks(0)`).

### Lim-3 — Table constructor ordering differs for duplicate keys

```lua
local t = {1, 2, [1] = 'ov'}
return t[1]   -- interpreter: 1   compiled: "ov"
```

Lua leaves the order of assignments in a constructor unspecified, so this is a
divergence rather than a bug.

## Areas that matched the interpreter

No differences were found in: integer and float `+ - * // %` including
negative operands and `mininteger // -1`; bitwise operators and variable
shifts; comparisons including NaN at run time and integer/float edge cases;
string coercion in arithmetic; concatenation and `__concat`; all arithmetic,
comparison, index, newindex, call and len metamethods; generic `for` including
`__pairs`; `while`, `repeat`, `goto`, `break`; closures and upvalue sharing
across loops and scopes; `pcall` / `error` / `xpcall` with non-string error
values; typed array stores, growth, slices, parameters and type checks;
GC stress with tables, closures and strings.

## Not covered

Var args, coroutines and `defer` (documented as unsupported); embedded C
beyond the existing tests; AOT beyond one round trip; macOS, Windows and
non-MIR builds; the standalone AST/IR dump options.
