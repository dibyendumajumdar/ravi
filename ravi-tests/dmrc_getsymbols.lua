-- Print anything - including nested tables
local function table_print (tt, indent, done)
  done = done or {}
  indent = indent or 0
  if type(tt) == "table" then
    for key, value in pairs (tt) do
      io.write(string.rep (" ", indent)) -- indent it
      if type (value) == "table" and not done [value] then
        done [value] = true
        io.write(string.format("[%s] => table\n", tostring (key)));
        io.write(string.rep (" ", indent+4)) -- indent it
        io.write("(\n");
        table_print (value, indent + 7, done)
        io.write(string.rep (" ", indent+4)) -- indent it
        io.write(")\n");
      else
        io.write(string.format("[%s] => %s\n",
          tostring (key), tostring(value)))
      end
    end
  else
    io.write(tostring(tt) .. "\n")
  end
end


local somecode = [[
extern int adder(int a, int b);

int adder(int a, int b)
{
	return a+b;
}
]]

local symbols = dmrc.getsymbols(somecode)

table_print(symbols)