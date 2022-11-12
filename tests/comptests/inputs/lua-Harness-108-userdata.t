--
-- lua-Harness : <https://fperrad.frama.io/lua-Harness/>
--
-- Copyright (C) 2009-2021, Perrad Francois
--
-- This code is licensed under the terms of the MIT/X11 license,
-- like Lua itself.
--

--[[

=head1 Lua userdata & coercion

=head2 Synopsis

    % prove 108-userdata.t

=head2 Description

=cut

--]]
print '108-userdata'
require'test_assertion'
local has_op53 = _VERSION >= 'Lua 5.3'

plan'no_plan'

local u = io.stdin

error_matches(function () return -u end,
        "^.*: attempt to perform arithmetic on",
        "-u")

error_matches(function () return #u end,
        "^.*: attempt to get length of",
        "#u")

equals(not u, false, "not u")

error_matches(function () return u + 10 end,
        "^.*: attempt to perform arithmetic on",
        "u + 10")

error_matches(function () return u - 2 end,
        "^.*: attempt to perform arithmetic on",
        "u - 2")

error_matches(function () return u * 3.14 end,
        "^.*: attempt to perform arithmetic on",
        "u * 3.14")

error_matches(function () return u / 7 end,
        "^.*: attempt to perform arithmetic on",
        "u / 7")

error_matches(function () return u % 4 end,
        "^.*: attempt to perform arithmetic on",
        "u % 4")

error_matches(function () return u ^ 3 end,
        "^.*: attempt to perform arithmetic on",
        "u ^ 3")

error_matches(function () return u .. 'end' end,
        "^.*: attempt to concatenate",
        "u .. 'end'")

equals(u == u, true, "u == u")

local v = io.stdout
equals(u ~= v, true, "u ~= v")

equals(u == 1, false, "u == 1")

equals(u ~= 1, true, "u ~= 1")

error_matches(function () return u < v end,
        "^.*: attempt to compare two",
        "u < v")

error_matches(function () return u <= v end,
        "^.*: attempt to compare two",
        "u <= v")

error_matches(function () return u > v end,
        "^.*: attempt to compare two",
        "u > v")

error_matches(function () return u >= v end,
        "^.*: attempt to compare two",
        "u >= v")

error_matches(function () return u < 0 end,
        "^.*: attempt to compare",
        "u < 0")

error_matches(function () return u <= 0 end,
        "^.*: attempt to compare",
        "u <= 0")

error_matches(function () return u > 0 end,
        "^.*: attempt to compare",
        "u > 0")

error_matches(function () return u > 0 end,
        "^.*: attempt to compare",
        "u >= 0")

equals(u[1], nil, "index")

error_matches(function () u[1] = 1 end,
        "^.*: attempt to index",
        "index")

local t = {}
t[u] = true
truthy(t[u])

if has_op53 then
    _dofile'inputs/lexico53/userdata.t'
end

done_testing()

-- Local Variables:
--   mode: lua
--   lua-indent-level: 4
--   fill-column: 100
-- End:
-- vim: ft=lua expandtab shiftwidth=4:
