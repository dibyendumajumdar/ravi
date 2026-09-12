# Ravi interpreter review

Reviewed 2026-09-12 at commit `3370dc030955b2e2ca0c67ebb11ddba1d17ad00c`.
Scope: interpreter, parser/bytecode generation, typed arrays, runtime type checks,
GC/upvalues, defer, and feasibility of updating the standalone Lua defer patches.
JIT implementations and ravicomp are excluded. No implementation files changed.
The pre-existing untracked 5.4.4 patch was included in the patch assessment.

## Findings

### Bug-1 P1 — Coroutine error recovery uses a stale stack pointer after running defer

Location: `src/ldo.c:687–702`, particularly the call at line 694 and subsequent
`luaD_seterrorobj`.

`recover()` restores `oldtop` from `ci->extra`, calls `luaF_close()`, then uses
`oldtop` again. Closing can execute arbitrary deferred Lua code and reallocate
the stack. Unlike `luaD_pcall()` at lines 848–850, this path does not restore the
pointer after closing.

Reproduced in a fresh interpreter-only Release build: yield inside a protected
call, resume it into an error, and let its defer expand the stack using
`table.unpack` with 10,000 arguments. The second resume terminates the process
with Windows access violation `0xC0000005` (exit -1073741819).

```lua
local co = coroutine.create(function()
  local ok, err = pcall(function()
    defer
      local a = {}
      for i = 1, 10000 do a[i] = i end
      local function consume(...) return select('#', ...) end
      assert(consume(table.unpack(a)) == 10000)
    end
    coroutine.yield('ready')
    error('original')
  end)
  print(ok, err)
end)
print(coroutine.resume(co))
print(coroutine.resume(co))
```

Restore `oldtop` from a saved offset after `luaF_close`; audit this recovery
path's frame restoration against the protected-call path as well. Add this
case to defer tests: normal-return stack growth does not exercise it.

### Bug-2 P2 — Fixed-size arrays can grow into unused allocation capacity

Location: `src/ltable.c:867–875` and `890–898`.

The setters accept `u == t->len` and increment the length whenever capacity
remains. They check the fixed-size flag only indirectly when allocation must
grow. Fixed-array construction reserves more capacity than requested for small
arrays, so this is readily observable:

```lua
local a = table.intarray(1, 0)
a[2] = 7
print(#a, a[2]) -- 2, 7; should reject the assignment
```

This violates the documented fixed-length contract and makes acceptance depend
on spare capacity. Check fixed/slice flags before accepting an append, in both
integer and floating-point setters. Existing indices must remain writable.

### Bug-3 P2 — Narrowing indices and sizes before validation silently aliases values

Locations: `src/ltable.h:167,178,189,200`, `src/ltable.c:859,882`, and
`src/ltablib.c:428,436,442–443`.

Array access converts Lua's 64-bit integer to `unsigned int` before checking
bounds. Consequently, values separated by 2^32 address the same element:

```lua
local a: integer[] = {1}
a[4294967297] = 9
print(a[1], a[4294967297]) -- 9, 9
```

The library constructors and slice wrapper similarly narrow arguments to `int`
before the underlying API validates them. `#table.intarray(4294967297, 0)` returns
1 on this build. This should be rejected rather than constructing an unrelated
small array. Validate the full-width value first, preserving documented index
zero access, and narrow only after the representable range is established.

### Bug-4 P2 — Typed integer-array stores silently truncate fractional values

Location: `src/lvm.c:2313–2319`.

`OP_RAVI_IARRAY_SET` directly casts a floating-point value to `lua_Integer`.
The generic array store instead uses checked conversion and rejects a
non-integral value. Merely annotating the array changes runtime semantics:

```lua
local a: integer[] = {1}
local x = 1.5
a[1] = x -- succeeds, stores 1

local b = table.intarray(1, 0)
b[1] = x -- errors: value cannot be converted to integer
```

Use checked integer conversion for the specialized opcode too. Test fractional,
out-of-range and non-finite floats, plus integral floats that should succeed.
The direct C cast also lacks a safe range check for non-finite/out-of-range input.

## GC and upvalue assessment

The implementation is a hybrid, not an unchanged Lua 5.4 collector:

- `UpVal` has a reference count and is not independently managed as a normal
  Lua 5.4 GC object. Closures decrement references when freed; unreferenced open
  upvalues remain owned by the thread until closed.
- `traverseLclosure`, `remarkupvals`, and `traversethread` mark captured values;
  threads additionally mark values referenced by their open-upvalue lists.
- Closed-upvalue writes use `luaC_upvalbarrier_`, which conservatively marks the
  assigned object while maintaining the invariant and promotes young targets to
  `G_OLD0`. This is essential because an upvalue does not have its own GC color
  and may be shared by multiple closures.
- Defer entries use the open-upvalue list with flags and zero closure references.
  `luaF_close` removes/frees the entry before calling it, preventing that same
  entry from being invoked again during error unwinding. Ordinary captured values
  are moved to closed storage and passed through the upvalue barrier.
- Slice parents are explicitly retained; numeric element storage is not scanned.

Focused checks passed in both incremental and generational modes: repeated
closed-upvalue replacement with newly allocated nested tables; captured values
surviving abandonment of suspended coroutines; slices surviving collection of
their original variable; and a stripped bytecode round trip with a typed upvalue.
These checks did not reveal an additional GC defect. They are not exhaustive
verification of generational invariants, allocation failure, finalizers or every
collector transition. The confirmed defer crash is stack relocation, not evidence
that reference counting itself must be replaced.

## Standalone defer patch migration

The current targets are Lua 5.4.9 (final 5.4 release) and Lua 5.5.1, per the
[official version history](https://www.lua.org/versions.html). Both tags exist in
the local Lua repository. Its checkout was left at v5.3.6; patch checks used
separate exported source directories.

The latest supplied patch, `patches/defer_statement_patch_for_Lua_5_4_4.patch`,
does **not** apply cleanly to either target:

| Target | `git apply --check --verbose` result | Assessment |
| --- | --- | --- |
| 5.4.9 | Rejected changes in lfunc.c, lapi.c and lfunc.h; other files accepted, some with offsets | A relatively contained manual port appears feasible. |
| 5.5.1 | Rejected changes in lexer, parser, VM, opcode tables, lfunc and lapi | Feasible in principle, but requires an explicit source port and semantic testing. |

Patch application alone would not validate either port. Even accepted hunks can
contain obsolete code. In particular, current stack fields use relocatable
representations such as `L->top.p` and `L->tbclist.p`; the patch's inserted helper
still uses raw `L->top` arithmetic.

Preserve the target's native to-be-closed list and closing machinery. There is
no need to transplant Ravi's reference-counted upvalue scheme into current Lua.
Port the defer syntax, anonymous closure creation, marker/opcode and closing
dispatch onto that infrastructure. Account for these details:

1. Keep native close-list removal before invocation, stack relocation handling,
   protected closing, return handling and coroutine closing behavior.
2. Lua 5.5's `prepcallclosemth` uses a null error pointer on ordinary closing;
   the old deferred-call helper unconditionally dereferences its error argument.
   Define whether defer retains an explicit nil argument and handle null safely.
3. The old helper deliberately uses `luaD_callnoyield` and ignores native `yy`.
   Preserve that policy explicitly or implement yielding with the target's
   continuation machinery; do not change it accidentally while rebasing.
4. The old patch registers a defer slot before creating its closure, without
   initializing the slot to nil. Ravi's current opcode does initialize it.
   Carry that initialization into the port so allocation failure cannot expose
   a stale function value to the close dispatcher.
5. Update all opcode metadata/jump tables and every native caller of
   `luaF_newtbcupval`, including C API and generic-for paths. Preserve ordinary
   `<close>` behavior alongside defer. Review parser register allocation around
   compile-time constants and newer 5.5 syntax rather than trusting hunk offsets.
6. Test mixed `<close>`/defer LIFO order, errors in multiple handlers, early
   returns, tail calls, break/goto/repeat exits, yield/resume/error recovery,
   explicit coroutine close, stack growth, allocation failure and binary chunks.

This is a feasibility assessment, not a completed or validated patch upgrade.

## Validation and artifacts

Fresh build: `build/interpreter-review/Release/ravi_s.exe`, configured with
`NO_JIT=ON`, `RAVICOMP=OFF`, `ASAN=OFF`, `COMPUTED_GOTO=OFF`, `STATIC_BUILD=ON`.
The `ravi_s` target was used throughout. CMake's `LTESTS=OFF` option does not
propagate to that static target; its version banner still reports ltests/assertions.
No sanitizer coverage is claimed.

Passed unchanged: `ravi_tests1.ravi`, `ravi_tests2.ravi`, `ravi_tests3.ravi`,
`ravi_errors.ravi`, `defer_tests.ravi`.
`basics.lua` and `bitwise_tests.lua` stop at assertions that `ravi.compile`
succeeds, which is incompatible with this no-JIT build; their remaining tests
were not executed.

Reproducers and logs are in `build/interpreter-review/`: `probes.ravi` reproduces
all four findings (and intentionally crashes last); `gc-probes.ravi` contains the
passing GC checks. Existing untracked user files were left unchanged.
