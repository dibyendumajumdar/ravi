-- The Computer Language Benchmarks Game
-- http://benchmarksgame.alioth.debian.org/
-- contributed by Mike Pall


local function domandel(pfunc)
  local width = 4000
  local height, wscale = width, 2.0/width
  local m, limit2 = 50, 4.0
  --local write, char = io.write, string.char

  for y=0,height-1 do
    local Ci = 2.0*y / height - 1
    for xb=0,width-1,8 do
      local bits = 0
      local xbb = xb+7
      local xblimit 
      if xbb < width then
        xblimit = xbb
      else
        xblimit = width-1
      end
      for x=xb,xblimit do
        bits = bits + bits
        local Zr, Zi, Zrq, Ziq = 0.0, 0.0, 0.0, 0.0
        local Cr = x * wscale - 1.5
        for i=1,m do
          local Zri = Zr*Zi
          Zr = Zrq - Ziq + Cr
          Zi = Zri + Zri + Ci
          Zrq = Zr*Zr
          Ziq = Zi*Zi
          if Zrq + Ziq > limit2 then
            bits = bits + 1
            break
          end
        end
      end
      if xbb >= width then
        for x=width,xbb do bits = bits + bits + 1 end
     end
     --pfunc(bits)
    end
  end
end

if ravi then
  ravi.compile(domandel)
end
if jit then
  -- LuaJIT warmup
  domandel(print)  
end

t1 = os.clock()
domandel(print)
t2 = os.clock()
print(t2-t1)
