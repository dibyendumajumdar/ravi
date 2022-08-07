---
-- lua-Harness : <https://fperrad.frama.io/lua-Harness/>
---

-- luacheck: globals _VERSION
_VERSION = 'Lua 5.3'    -- instead of 'Ravi 5.3'

local profile = {

--[[ compat 5.0
    has_string_gfind = true,
    has_math_mod = true,
--]]

    compat51 = true,
--[[
    has_unpack = true,
    has_package_loaders = true,
    has_math_log10 = true,
    has_loadstring = true,
    has_table_maxn = true,
    has_module = true,
    has_package_seeall = true,
--]]

    compat52 = true,
--[[
    has_mathx = true,
    has_bit32 = true,
    has_metamethod_ipairs = true,
--]]

    nocvtn2s = false,
    nocvts2n = true,

    compat53 = false,
--[[
    has_mathx = true,
    has_metamethod_ipairs = true,
--]]

--[[ luajit
    luajit_compat52 = true,
    openresty = false,
--]]

}

package.loaded.profile = profile        -- prevents loading of default profile

return profile

--
-- Copyright (c) 2018-2021 Francois Perrad
--
-- This library is licensed under the terms of the MIT/X11 license,
-- like Lua itself.
--
