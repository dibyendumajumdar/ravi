X86-64 calling sequence
=======================

On Windows 64-bit calling sequence requires caller to reserve 32 bytes (for 4 register parameters). See below sequence for example::

        subq    $32, %rsp
        movq    %rcx, %rsi
        movq    32(%rsi), %rdi
        movq    (%rdi), %rax
        movq    32(%rdi), %rbx
        movq    (%rax), %rax
        movq    24(%rax), %rax
        cmpl    $0, 32(%rax)
        jle     .LBB0_2
        movabsq $luaF_close, %rax
        movq    %rsi, %rcx
        movq    %rbx, %rdx
        callq   *%rax
   .LBB0_2:
        movabsq $luaD_poscall, %rax
        xorl    %r9d, %r9d
        movq    %rsi, %rcx
        movq    %rdi, %rdx
        movq    %rbx, %r8
        addq    $32, %rsp

References
==========
* `X86-64 Calling conventions <https://en.wikipedia.org/wiki/X86_calling_conventions#x86-64_calling_conventions>`_
* `Windows X86-64 Conventions <https://docs.microsoft.com/en-us/cpp/build/x64-software-conventions>`_
