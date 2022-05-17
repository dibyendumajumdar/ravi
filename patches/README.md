These patches are for Lua 5.3 and 5.4.

The 'defer' patch adds the defer statement to Lua.

Note that in Lua 5.4 versions prior to 5.4.3 a deferred closure may be called more than once
just as the close method of a to-be-closed variable may be called more than once, when exiting the scope.

I think this is fixed in Lua 5.4.3.

The original patch for 5.4 is applicable to versions prior to 5.4.3.
The 5.4.3 version has a new approach to the implementation of toclose values, hence a new patch had to be created.
It seems that the 5.4.3 patch also works for 5.4.4.
