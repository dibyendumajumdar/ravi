# Defer for Lua 5.5.1

Patch: [defer_statement_patch_for_Lua_5_5_1.patch](defer_statement_patch_for_Lua_5_5_1.patch)

Base: the Lua repository's `v5.5.1` tag, commit
`7579fc9d7ed90240487251dfb69168f8e64e9294`.
This is a source port of the earlier defer extension, not a port of Ravi's
interpreter, reference-counted upvalues, or garbage collector.
Application to other Lua releases has not been verified.

## Applying

From a clean Lua repository checked out at `v5.5.1`:

```sh
git apply --check /path/to/defer_statement_patch_for_Lua_5_5_1.patch
git apply /path/to/defer_statement_patch_for_Lua_5_5_1.patch
```

Build Lua normally for the target platform. Rebuild dependent components too.
The patch adds a reserved word, an opcode, and internal close-list metadata.
It also updates the repository's tests and adds two defer regression files.

For a release archive without the repository's `testes` directory, apply from
its `src` directory, excluding the two existing-suite updates:

```sh
git apply --check --exclude=testes/all.lua --exclude=testes/calls.lua /path/to/defer_statement_patch_for_Lua_5_5_1.patch
git apply --exclude=testes/all.lua --exclude=testes/calls.lua /path/to/defer_statement_patch_for_Lua_5_5_1.patch
```

These exclusions retain the new standalone tests. The release-archive procedure
has not been tested against a downloaded archive; the verified base is the Git tag.

## Syntax and behavior

```lua
local function example()
  local messages = {}
  do
    defer messages[#messages + 1] = 'first' end
    defer messages[#messages + 1] = 'second' end
  end
  assert(table.concat(messages, ',') == 'second,first')
end
```

`defer ... end` creates an anonymous, zero-parameter closure. It captures lexical
variables as an ordinary Lua closure does. The body executes when its enclosing
scope closes, including return, break, goto and error unwinding. It does not
evaluate the body's expressions at registration time.

Deferred closures and native `<close>` variables share Lua 5.5's close list and
run in reverse registration order. Entries are removed before invocation, so an
error in cleanup does not invoke the same entry again. Lua 5.5's native handling
of cleanup errors is retained: a later cleanup error can replace an earlier
error. This differs from Ravi's policy of retaining the original error in some
unwinding paths.

Defer bodies cannot yield, matching the earlier standalone patch. Ordinary
`<close>` methods retain Lua 5.5's yield rules. Suspending a coroutine does not
run defer; explicit `coroutine.close` runs pending cleanup. Garbage collection
of an abandoned coroutine does not run its deferred bodies. Use Lua's normal
explicit coroutine-closing discipline.

The runtime passes an error object, or nil on normal exit, to the deferred
closure. The syntax declares no parameter to receive it and does not provide
access to an enclosing function's `...`.

## Implementation choices

- `OP_CLOSURE` creates the closure before `OP_DEFER` registers it. Registration
  does not allocate. A failed closure allocation therefore cannot leave an old
  register value registered as a deferred call.
- Lua 5.5's native close-list traversal, stack relocation, coroutine handling,
  GC and upvalue implementation remain in place.
- Native close-list entries are explicitly marked as non-deferred, including
  entries created through the C API and generic-for loops. Dummy delta nodes
  are initialized too.
- The parser uses `marktobeclosed` to arrange scope closing and suppress tail
  calls where cleanup is pending. It uses the closure's allocated register,
  rather than assuming the number of locals equals the register number.
- Binary chunks use `LUAC_FORMAT = 1` because opcode numbering differs from
  stock Lua. Stock chunks are rejected, and stock Lua rejects patched chunks.
  Recompile source files instead of exchanging binary chunks between them.

## Tests

The patch adds `testes/defer.lua` and registers it in `testes/all.lua`. It covers:

- Captured variables, LIFO order, mixed `<close>`/defer, and return values.
- Compile-time constants, loops, break, goto, and tail-call suppression.
- Multiple cleanup errors and non-yielding defer versus yieldable `<close>`.
- Explicit coroutine close and stack growth during error recovery after yield.
- Stripped binary chunks and rejection of stock-format chunks.
- Close-list dummy nodes across more than 65,535 stack slots.
- Captured values under incremental and generational collection.

Run the standalone Lua regression from the source root:

```sh
./lua testes/defer.lua
```

`testes/defer_alloc.c` runs 300 allocation budgets, checking memory-error
unwinding and that cleanup entries are not invoked twice. Link it against the
patched library; for example on Linux:

```sh
cc -I. testes/defer_alloc.c liblua.a -lm -ldl -o defer_alloc
./defer_alloc
```

Local validation used MSVC on Windows with `LUA_USE_APICHECK` and `LUAI_ASSERT`:

- Debug and Release builds and standalone defer tests passed.
- Allocation tests passed: 218 memory-error outcomes and 82 successful runs.
- The upstream user suite (`_U=true`) passed with `files.lua` excluded in a
  temporary local runner. That file unconditionally accesses `/dev/null` and
  `/dev/full`; the patch does not alter or skip it in the upstream runner.
- The patch applies cleanly to a fresh export of the exact base tag. Applied
  files were compared against the tested source files.

The upstream user suite skips internal tests and some expensive/nonportable
cases. Linux/macOS builds, computed-goto dispatch, and the full internal Lua
test configuration still need validation on those platforms.
