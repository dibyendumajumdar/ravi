Ravi
====

Experimental derivative of Lua. Ravi is a Sanskrit word that means the Sun.

Lua is perfect as a small embeddable dynamic language. So why a derivative? The reason is primarily to extend Lua with static typing for greater efficiency in performance. However, at the same time I would like to retain compatibility with Lua to the degree possible.

There are other attempts to add static typing to Lua but these efforts are mostly about adding static type checks in the language while leaving the VM unmodified. So the static typing is merely to aid programming - the code is eventually translated to Lua and executed in the VM.

My motivation is somewhat different - I want to enhance the VM to support more efficient operations when types are known. I would also like to add one additional type - an array. This is to allow more efficient array operations.

Status
------
The project is being kicked off in January 2015. I expect it will be a while before there is any code that runs. However my intention is start small and grow incrementally.

License
-------
Will be MIT similar to Lua.

Language Syntax
---------------
I hope to enhance the language to support following types:
