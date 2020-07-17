These patches are for Lua 5.3 and 5.4.

The 'defer' patch adds the defer statement to Lua.

Note that in Lua 5.4 a deferred closure may be called more than once just as the close method of a to-be-closed variable 
may be called more than once, when exiting the scope. I am checking why Lua 5.4 behaves this way.