-- source https://github.com/LuaJIT/LuaJIT-test-cleanup/blob/master/test/lang/andor.lua

do --- allcases
  local basiccases = {
    {"nil", nil},
    {"false", false},
    {"true", true},
    {"10", 10},
  }

  local mem = {basiccases}    -- for memoization

  local function allcases (n: integer)
    if mem[n] then return mem[n] end
    local res = {}
    -- include all smaller cases
    for _, v in ipairs(allcases(n - 1)) do
      res[#res + 1] = v
    end
    for i = 1, n - 1 do
      for _, v1 in ipairs(allcases(i)) do
        for _, v2 in ipairs(allcases(n - i)) do
    res[#res + 1] = {
      "(" .. v1[1] .. " and " .. v2[1] .. ")",
      v1[2] and v2[2]
    }
    res[#res + 1] = {
      "(" .. v1[1] .. " or " .. v2[1] .. ")",
      v1[2] or v2[2]
    }
        end
      end
    end
    mem[n] = res   -- memoize
    return res
  end

  for _, v in pairs(allcases(4)) do
    local code = "return " .. v[1]
    --print('evaluating ' .. code)
    local res = compiler.load(code)()
    if res ~= v[2] then
      --error(string.format("bad conditional eval\n%s\nexpected: %s\ngot: %s",
      --  v[1], tostring(v[2]), tostring(res)))
      error("bad condition eval " .. v[1] .. " expected " .. tostring(v[2]) .. " got " .. tostring(res))
    end
  end
end


print '77 Ok'