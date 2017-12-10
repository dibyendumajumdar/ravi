
vm.obj:	file format COFF-x86-64

Disassembly of section .text:
ravi_vm_asm_begin:
       0:	0f b6 cc 	movzbl	%ah, %ecx
       3:	c1 e8 10 	shrl	$16, %eax
       6:	0f b6 d4 	movzbl	%ah, %edx
       9:	c1 e2 04 	shll	$4, %edx
       c:	49 8d 34 10 	leaq	(%r8,%rdx), %rsi
      10:	c1 e1 04 	shll	$4, %ecx
      13:	49 8d 3c 08 	leaq	(%r8,%rcx), %rdi
      17:	48 8b 06 	movq	(%rsi), %rax
      1a:	44 8b 4e 08 	movl	8(%rsi), %r9d
      1e:	48 89 07 	movq	%rax, (%rdi)
      21:	44 89 4f 08 	movl	%r9d, 8(%rdi)
      25:	8b 03 	movl	(%rbx), %eax
      27:	0f b6 d0 	movzbl	%al, %edx
      2a:	48 83 c3 04 	addq	$4, %rbx
      2e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADK:
      32:	0f b6 cc 	movzbl	%ah, %ecx
      35:	c1 e8 10 	shrl	$16, %eax
      38:	0f b7 d0 	movzwl	%ax, %edx
      3b:	c1 e2 04 	shll	$4, %edx
      3e:	49 8d 34 17 	leaq	(%r15,%rdx), %rsi
      42:	c1 e1 04 	shll	$4, %ecx
      45:	49 8d 3c 08 	leaq	(%r8,%rcx), %rdi
      49:	48 8b 06 	movq	(%rsi), %rax
      4c:	44 8b 4e 08 	movl	8(%rsi), %r9d
      50:	48 89 07 	movq	%rax, (%rdi)
      53:	44 89 4f 08 	movl	%r9d, 8(%rdi)
      57:	8b 03 	movl	(%rbx), %eax
      59:	0f b6 d0 	movzbl	%al, %edx
      5c:	48 83 c3 04 	addq	$4, %rbx
      60:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADKX:
      64:	8b 03 	movl	(%rbx), %eax
      66:	0f b6 d0 	movzbl	%al, %edx
      69:	48 83 c3 04 	addq	$4, %rbx
      6d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADBOOL:
      71:	8b 03 	movl	(%rbx), %eax
      73:	0f b6 d0 	movzbl	%al, %edx
      76:	48 83 c3 04 	addq	$4, %rbx
      7a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADNIL:
      7e:	8b 03 	movl	(%rbx), %eax
      80:	0f b6 d0 	movzbl	%al, %edx
      83:	48 83 c3 04 	addq	$4, %rbx
      87:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETUPVAL:
      8b:	8b 03 	movl	(%rbx), %eax
      8d:	0f b6 d0 	movzbl	%al, %edx
      90:	48 83 c3 04 	addq	$4, %rbx
      94:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABUP:
      98:	8b 03 	movl	(%rbx), %eax
      9a:	0f b6 d0 	movzbl	%al, %edx
      9d:	48 83 c3 04 	addq	$4, %rbx
      a1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE:
      a5:	8b 03 	movl	(%rbx), %eax
      a7:	0f b6 d0 	movzbl	%al, %edx
      aa:	48 83 c3 04 	addq	$4, %rbx
      ae:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABUP:
      b2:	8b 03 	movl	(%rbx), %eax
      b4:	0f b6 d0 	movzbl	%al, %edx
      b7:	48 83 c3 04 	addq	$4, %rbx
      bb:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVAL:
      bf:	8b 03 	movl	(%rbx), %eax
      c1:	0f b6 d0 	movzbl	%al, %edx
      c4:	48 83 c3 04 	addq	$4, %rbx
      c8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE:
      cc:	8b 03 	movl	(%rbx), %eax
      ce:	0f b6 d0 	movzbl	%al, %edx
      d1:	48 83 c3 04 	addq	$4, %rbx
      d5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWTABLE:
      d9:	8b 03 	movl	(%rbx), %eax
      db:	0f b6 d0 	movzbl	%al, %edx
      de:	48 83 c3 04 	addq	$4, %rbx
      e2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF:
      e6:	8b 03 	movl	(%rbx), %eax
      e8:	0f b6 d0 	movzbl	%al, %edx
      eb:	48 83 c3 04 	addq	$4, %rbx
      ef:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADD:
      f3:	8b 03 	movl	(%rbx), %eax
      f5:	0f b6 d0 	movzbl	%al, %edx
      f8:	48 83 c3 04 	addq	$4, %rbx
      fc:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUB:
     100:	8b 03 	movl	(%rbx), %eax
     102:	0f b6 d0 	movzbl	%al, %edx
     105:	48 83 c3 04 	addq	$4, %rbx
     109:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MUL:
     10d:	8b 03 	movl	(%rbx), %eax
     10f:	0f b6 d0 	movzbl	%al, %edx
     112:	48 83 c3 04 	addq	$4, %rbx
     116:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOD:
     11a:	8b 03 	movl	(%rbx), %eax
     11c:	0f b6 d0 	movzbl	%al, %edx
     11f:	48 83 c3 04 	addq	$4, %rbx
     123:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_POW:
     127:	8b 03 	movl	(%rbx), %eax
     129:	0f b6 d0 	movzbl	%al, %edx
     12c:	48 83 c3 04 	addq	$4, %rbx
     130:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIV:
     134:	8b 03 	movl	(%rbx), %eax
     136:	0f b6 d0 	movzbl	%al, %edx
     139:	48 83 c3 04 	addq	$4, %rbx
     13d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_IDIV:
     141:	8b 03 	movl	(%rbx), %eax
     143:	0f b6 d0 	movzbl	%al, %edx
     146:	48 83 c3 04 	addq	$4, %rbx
     14a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BAND:
     14e:	8b 03 	movl	(%rbx), %eax
     150:	0f b6 d0 	movzbl	%al, %edx
     153:	48 83 c3 04 	addq	$4, %rbx
     157:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BOR:
     15b:	8b 03 	movl	(%rbx), %eax
     15d:	0f b6 d0 	movzbl	%al, %edx
     160:	48 83 c3 04 	addq	$4, %rbx
     164:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BXOR:
     168:	8b 03 	movl	(%rbx), %eax
     16a:	0f b6 d0 	movzbl	%al, %edx
     16d:	48 83 c3 04 	addq	$4, %rbx
     171:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHL:
     175:	8b 03 	movl	(%rbx), %eax
     177:	0f b6 d0 	movzbl	%al, %edx
     17a:	48 83 c3 04 	addq	$4, %rbx
     17e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHR:
     182:	8b 03 	movl	(%rbx), %eax
     184:	0f b6 d0 	movzbl	%al, %edx
     187:	48 83 c3 04 	addq	$4, %rbx
     18b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNM:
     18f:	8b 03 	movl	(%rbx), %eax
     191:	0f b6 d0 	movzbl	%al, %edx
     194:	48 83 c3 04 	addq	$4, %rbx
     198:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BNOT:
     19c:	8b 03 	movl	(%rbx), %eax
     19e:	0f b6 d0 	movzbl	%al, %edx
     1a1:	48 83 c3 04 	addq	$4, %rbx
     1a5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NOT:
     1a9:	8b 03 	movl	(%rbx), %eax
     1ab:	0f b6 d0 	movzbl	%al, %edx
     1ae:	48 83 c3 04 	addq	$4, %rbx
     1b2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LEN:
     1b6:	8b 03 	movl	(%rbx), %eax
     1b8:	0f b6 d0 	movzbl	%al, %edx
     1bb:	48 83 c3 04 	addq	$4, %rbx
     1bf:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CONCAT:
     1c3:	8b 03 	movl	(%rbx), %eax
     1c5:	0f b6 d0 	movzbl	%al, %edx
     1c8:	48 83 c3 04 	addq	$4, %rbx
     1cc:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_JMP:
     1d0:	8b 03 	movl	(%rbx), %eax
     1d2:	0f b6 d0 	movzbl	%al, %edx
     1d5:	48 83 c3 04 	addq	$4, %rbx
     1d9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ:
     1dd:	8b 03 	movl	(%rbx), %eax
     1df:	0f b6 d0 	movzbl	%al, %edx
     1e2:	48 83 c3 04 	addq	$4, %rbx
     1e6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT:
     1ea:	8b 03 	movl	(%rbx), %eax
     1ec:	0f b6 d0 	movzbl	%al, %edx
     1ef:	48 83 c3 04 	addq	$4, %rbx
     1f3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE:
     1f7:	8b 03 	movl	(%rbx), %eax
     1f9:	0f b6 d0 	movzbl	%al, %edx
     1fc:	48 83 c3 04 	addq	$4, %rbx
     200:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TEST:
     204:	8b 03 	movl	(%rbx), %eax
     206:	0f b6 d0 	movzbl	%al, %edx
     209:	48 83 c3 04 	addq	$4, %rbx
     20d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TESTSET:
     211:	8b 03 	movl	(%rbx), %eax
     213:	0f b6 d0 	movzbl	%al, %edx
     216:	48 83 c3 04 	addq	$4, %rbx
     21a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CALL:
     21e:	8b 03 	movl	(%rbx), %eax
     220:	0f b6 d0 	movzbl	%al, %edx
     223:	48 83 c3 04 	addq	$4, %rbx
     227:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TAILCALL:
     22b:	8b 03 	movl	(%rbx), %eax
     22d:	0f b6 d0 	movzbl	%al, %edx
     230:	48 83 c3 04 	addq	$4, %rbx
     234:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_RETURN:
     238:	49 89 c7 	movq	%rax, %r15
     23b:	49 8b 75 18 	movq	24(%r13), %rsi
     23f:	83 7e 20 00 	cmpl	$0, 32(%rsi)
     243:	74 11 	je	17 <ravi_BC_RETURN+0x1E>
     245:	4d 89 c5 	movq	%r8, %r13
     248:	48 89 e9 	movq	%rbp, %rcx
     24b:	4c 89 c2 	movq	%r8, %rdx
     24e:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x1B>
     253:	4d 89 e8 	movq	%r13, %r8
     256:	49 89 5c 24 28 	movq	%rbx, 40(%r12)
     25b:	4c 89 f8 	movq	%r15, %rax
     25e:	0f b6 cc 	movzbl	%ah, %ecx
     261:	c1 e8 10 	shrl	$16, %eax
     264:	0f b6 d4 	movzbl	%ah, %edx
     267:	c1 e1 04 	shll	$4, %ecx
     26a:	4d 8d 04 08 	leaq	(%r8,%rcx), %r8
     26e:	85 d2 	testl	%edx, %edx
     270:	74 07 	je	7 <ravi_BC_RETURN+0x41>
     272:	ff ca 	decl	%edx
     274:	41 89 d1 	movl	%edx, %r9d
     277:	eb 0b 	jmp	11 <ravi_BC_RETURN+0x4C>
     279:	4c 8b 4d 10 	movq	16(%rbp), %r9
     27d:	4d 29 c1 	subq	%r8, %r9
     280:	49 c1 e9 04 	shrq	$4, %r9
     284:	48 89 e9 	movq	%rbp, %rcx
     287:	4c 89 e2 	movq	%r12, %rdx
     28a:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x57>
     28f:	66 41 f7 44 24 42 08 00 	testw	$8, 66(%r12)
     297:	0f 85 fa 04 00 00 	jne	1274 <ravi_vm_return>
     29d:	4c 8b 65 20 	movq	32(%rbp), %r12
     2a1:	85 c0 	testl	%eax, %eax
     2a3:	74 09 	je	9 <ravi_BC_RETURN+0x76>
     2a5:	4d 8b 6c 24 08 	movq	8(%r12), %r13
     2aa:	4c 89 6d 10 	movq	%r13, 16(%rbp)
     2ae:	e9 be 04 00 00 	jmp	1214 <ravi_new_frame>

ravi_BC_FORLOOP:
     2b3:	8b 03 	movl	(%rbx), %eax
     2b5:	0f b6 d0 	movzbl	%al, %edx
     2b8:	48 83 c3 04 	addq	$4, %rbx
     2bc:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP:
     2c0:	8b 03 	movl	(%rbx), %eax
     2c2:	0f b6 d0 	movzbl	%al, %edx
     2c5:	48 83 c3 04 	addq	$4, %rbx
     2c9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORCALL:
     2cd:	8b 03 	movl	(%rbx), %eax
     2cf:	0f b6 d0 	movzbl	%al, %edx
     2d2:	48 83 c3 04 	addq	$4, %rbx
     2d6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORLOOP:
     2da:	8b 03 	movl	(%rbx), %eax
     2dc:	0f b6 d0 	movzbl	%al, %edx
     2df:	48 83 c3 04 	addq	$4, %rbx
     2e3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETLIST:
     2e7:	8b 03 	movl	(%rbx), %eax
     2e9:	0f b6 d0 	movzbl	%al, %edx
     2ec:	48 83 c3 04 	addq	$4, %rbx
     2f0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CLOSURE:
     2f4:	8b 03 	movl	(%rbx), %eax
     2f6:	0f b6 d0 	movzbl	%al, %edx
     2f9:	48 83 c3 04 	addq	$4, %rbx
     2fd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_VARARG:
     301:	8b 03 	movl	(%rbx), %eax
     303:	0f b6 d0 	movzbl	%al, %edx
     306:	48 83 c3 04 	addq	$4, %rbx
     30a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EXTRAARG:
     30e:	8b 03 	movl	(%rbx), %eax
     310:	0f b6 d0 	movzbl	%al, %edx
     313:	48 83 c3 04 	addq	$4, %rbx
     317:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYI:
     31b:	8b 03 	movl	(%rbx), %eax
     31d:	0f b6 d0 	movzbl	%al, %edx
     320:	48 83 c3 04 	addq	$4, %rbx
     324:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYF:
     328:	8b 03 	movl	(%rbx), %eax
     32a:	0f b6 d0 	movzbl	%al, %edx
     32d:	48 83 c3 04 	addq	$4, %rbx
     331:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADIZ:
     335:	8b 03 	movl	(%rbx), %eax
     337:	0f b6 d0 	movzbl	%al, %edx
     33a:	48 83 c3 04 	addq	$4, %rbx
     33e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADFZ:
     342:	8b 03 	movl	(%rbx), %eax
     344:	0f b6 d0 	movzbl	%al, %edx
     347:	48 83 c3 04 	addq	$4, %rbx
     34b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMF:
     34f:	8b 03 	movl	(%rbx), %eax
     351:	0f b6 d0 	movzbl	%al, %edx
     354:	48 83 c3 04 	addq	$4, %rbx
     358:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMI:
     35c:	8b 03 	movl	(%rbx), %eax
     35e:	0f b6 d0 	movzbl	%al, %edx
     361:	48 83 c3 04 	addq	$4, %rbx
     365:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFF:
     369:	8b 03 	movl	(%rbx), %eax
     36b:	0f b6 d0 	movzbl	%al, %edx
     36e:	48 83 c3 04 	addq	$4, %rbx
     372:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFI:
     376:	8b 03 	movl	(%rbx), %eax
     378:	0f b6 d0 	movzbl	%al, %edx
     37b:	48 83 c3 04 	addq	$4, %rbx
     37f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDII:
     383:	8b 03 	movl	(%rbx), %eax
     385:	0f b6 d0 	movzbl	%al, %edx
     388:	48 83 c3 04 	addq	$4, %rbx
     38c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFF:
     390:	8b 03 	movl	(%rbx), %eax
     392:	0f b6 d0 	movzbl	%al, %edx
     395:	48 83 c3 04 	addq	$4, %rbx
     399:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFI:
     39d:	8b 03 	movl	(%rbx), %eax
     39f:	0f b6 d0 	movzbl	%al, %edx
     3a2:	48 83 c3 04 	addq	$4, %rbx
     3a6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBIF:
     3aa:	8b 03 	movl	(%rbx), %eax
     3ac:	0f b6 d0 	movzbl	%al, %edx
     3af:	48 83 c3 04 	addq	$4, %rbx
     3b3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBII:
     3b7:	8b 03 	movl	(%rbx), %eax
     3b9:	0f b6 d0 	movzbl	%al, %edx
     3bc:	48 83 c3 04 	addq	$4, %rbx
     3c0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFF:
     3c4:	8b 03 	movl	(%rbx), %eax
     3c6:	0f b6 d0 	movzbl	%al, %edx
     3c9:	48 83 c3 04 	addq	$4, %rbx
     3cd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFI:
     3d1:	8b 03 	movl	(%rbx), %eax
     3d3:	0f b6 d0 	movzbl	%al, %edx
     3d6:	48 83 c3 04 	addq	$4, %rbx
     3da:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULII:
     3de:	8b 03 	movl	(%rbx), %eax
     3e0:	0f b6 d0 	movzbl	%al, %edx
     3e3:	48 83 c3 04 	addq	$4, %rbx
     3e7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFF:
     3eb:	8b 03 	movl	(%rbx), %eax
     3ed:	0f b6 d0 	movzbl	%al, %edx
     3f0:	48 83 c3 04 	addq	$4, %rbx
     3f4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFI:
     3f8:	8b 03 	movl	(%rbx), %eax
     3fa:	0f b6 d0 	movzbl	%al, %edx
     3fd:	48 83 c3 04 	addq	$4, %rbx
     401:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVIF:
     405:	8b 03 	movl	(%rbx), %eax
     407:	0f b6 d0 	movzbl	%al, %edx
     40a:	48 83 c3 04 	addq	$4, %rbx
     40e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVII:
     412:	8b 03 	movl	(%rbx), %eax
     414:	0f b6 d0 	movzbl	%al, %edx
     417:	48 83 c3 04 	addq	$4, %rbx
     41b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOINT:
     41f:	8b 03 	movl	(%rbx), %eax
     421:	0f b6 d0 	movzbl	%al, %edx
     424:	48 83 c3 04 	addq	$4, %rbx
     428:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOFLT:
     42c:	8b 03 	movl	(%rbx), %eax
     42e:	0f b6 d0 	movzbl	%al, %edx
     431:	48 83 c3 04 	addq	$4, %rbx
     435:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYI:
     439:	8b 03 	movl	(%rbx), %eax
     43b:	0f b6 d0 	movzbl	%al, %edx
     43e:	48 83 c3 04 	addq	$4, %rbx
     442:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYF:
     446:	8b 03 	movl	(%rbx), %eax
     448:	0f b6 d0 	movzbl	%al, %edx
     44b:	48 83 c3 04 	addq	$4, %rbx
     44f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTAB:
     453:	8b 03 	movl	(%rbx), %eax
     455:	0f b6 d0 	movzbl	%al, %edx
     458:	48 83 c3 04 	addq	$4, %rbx
     45c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOSTRING:
     460:	8b 03 	movl	(%rbx), %eax
     462:	0f b6 d0 	movzbl	%al, %edx
     465:	48 83 c3 04 	addq	$4, %rbx
     469:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOCLOSURE:
     46d:	8b 03 	movl	(%rbx), %eax
     46f:	0f b6 d0 	movzbl	%al, %edx
     472:	48 83 c3 04 	addq	$4, %rbx
     476:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTYPE:
     47a:	8b 03 	movl	(%rbx), %eax
     47c:	0f b6 d0 	movzbl	%al, %edx
     47f:	48 83 c3 04 	addq	$4, %rbx
     483:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEI:
     487:	8b 03 	movl	(%rbx), %eax
     489:	0f b6 d0 	movzbl	%al, %edx
     48c:	48 83 c3 04 	addq	$4, %rbx
     490:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEF:
     494:	8b 03 	movl	(%rbx), %eax
     496:	0f b6 d0 	movzbl	%al, %edx
     499:	48 83 c3 04 	addq	$4, %rbx
     49d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAI:
     4a1:	8b 03 	movl	(%rbx), %eax
     4a3:	0f b6 d0 	movzbl	%al, %edx
     4a6:	48 83 c3 04 	addq	$4, %rbx
     4aa:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAF:
     4ae:	8b 03 	movl	(%rbx), %eax
     4b0:	0f b6 d0 	movzbl	%al, %edx
     4b3:	48 83 c3 04 	addq	$4, %rbx
     4b7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVETAB:
     4bb:	8b 03 	movl	(%rbx), %eax
     4bd:	0f b6 d0 	movzbl	%al, %edx
     4c0:	48 83 c3 04 	addq	$4, %rbx
     4c4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AI:
     4c8:	8b 03 	movl	(%rbx), %eax
     4ca:	0f b6 d0 	movzbl	%al, %edx
     4cd:	48 83 c3 04 	addq	$4, %rbx
     4d1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AF:
     4d5:	8b 03 	movl	(%rbx), %eax
     4d7:	0f b6 d0 	movzbl	%al, %edx
     4da:	48 83 c3 04 	addq	$4, %rbx
     4de:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AI:
     4e2:	8b 03 	movl	(%rbx), %eax
     4e4:	0f b6 d0 	movzbl	%al, %edx
     4e7:	48 83 c3 04 	addq	$4, %rbx
     4eb:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AF:
     4ef:	8b 03 	movl	(%rbx), %eax
     4f1:	0f b6 d0 	movzbl	%al, %edx
     4f4:	48 83 c3 04 	addq	$4, %rbx
     4f8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AII:
     4fc:	8b 03 	movl	(%rbx), %eax
     4fe:	0f b6 d0 	movzbl	%al, %edx
     501:	48 83 c3 04 	addq	$4, %rbx
     505:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AFF:
     509:	8b 03 	movl	(%rbx), %eax
     50b:	0f b6 d0 	movzbl	%al, %edx
     50e:	48 83 c3 04 	addq	$4, %rbx
     512:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_IP:
     516:	0f b6 cc 	movzbl	%ah, %ecx
     519:	c1 e1 04 	shll	$4, %ecx
     51c:	4d 8d 0c 08 	leaq	(%r8,%rcx), %r9
     520:	49 8b 31 	movq	(%r9), %rsi
     523:	49 03 71 20 	addq	32(%r9), %rsi
     527:	49 3b 71 10 	cmpq	16(%r9), %rsi
     52b:	7f 1e 	jg	30 <ravi_BC_FORLOOP_IP+0x35>
     52d:	49 89 31 	movq	%rsi, (%r9)
     530:	49 8d 79 30 	leaq	48(%r9), %rdi
     534:	48 89 37 	movq	%rsi, (%rdi)
     537:	66 c7 47 08 13 00 	movw	$19, 8(%rdi)
     53d:	c1 e8 10 	shrl	$16, %eax
     540:	0f b7 d0 	movzwl	%ax, %edx
     543:	48 8d 9c 93 00 00 fe ff 	leaq	-131072(%rbx,%rdx,4), %rbx
     54b:	8b 03 	movl	(%rbx), %eax
     54d:	0f b6 d0 	movzbl	%al, %edx
     550:	48 83 c3 04 	addq	$4, %rbx
     554:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_I1:
     558:	0f b6 cc 	movzbl	%ah, %ecx
     55b:	c1 e1 04 	shll	$4, %ecx
     55e:	4d 8d 0c 08 	leaq	(%r8,%rcx), %r9
     562:	49 8b 31 	movq	(%r9), %rsi
     565:	48 ff c6 	incq	%rsi
     568:	49 3b 71 10 	cmpq	16(%r9), %rsi
     56c:	7f 1e 	jg	30 <ravi_BC_FORLOOP_I1+0x34>
     56e:	49 89 31 	movq	%rsi, (%r9)
     571:	49 8d 79 30 	leaq	48(%r9), %rdi
     575:	48 89 37 	movq	%rsi, (%rdi)
     578:	66 c7 47 08 13 00 	movw	$19, 8(%rdi)
     57e:	c1 e8 10 	shrl	$16, %eax
     581:	0f b7 d0 	movzwl	%ax, %edx
     584:	48 8d 9c 93 00 00 fe ff 	leaq	-131072(%rbx,%rdx,4), %rbx
     58c:	8b 03 	movl	(%rbx), %eax
     58e:	0f b6 d0 	movzbl	%al, %edx
     591:	48 83 c3 04 	addq	$4, %rbx
     595:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_IP:
     599:	0f b6 cc 	movzbl	%ah, %ecx
     59c:	c1 e8 10 	shrl	$16, %eax
     59f:	0f b7 d0 	movzwl	%ax, %edx
     5a2:	c1 e1 04 	shll	$4, %ecx
     5a5:	49 8d 04 08 	leaq	(%r8,%rcx), %rax
     5a9:	48 8b 30 	movq	(%rax), %rsi
     5ac:	48 2b 70 20 	subq	32(%rax), %rsi
     5b0:	48 89 30 	movq	%rsi, (%rax)
     5b3:	48 8d 9c 93 00 00 fe ff 	leaq	-131072(%rbx,%rdx,4), %rbx
     5bb:	8b 03 	movl	(%rbx), %eax
     5bd:	0f b6 d0 	movzbl	%al, %edx
     5c0:	48 83 c3 04 	addq	$4, %rbx
     5c4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_I1:
     5c8:	0f b6 cc 	movzbl	%ah, %ecx
     5cb:	c1 e8 10 	shrl	$16, %eax
     5ce:	0f b7 d0 	movzwl	%ax, %edx
     5d1:	c1 e1 04 	shll	$4, %ecx
     5d4:	49 8d 04 08 	leaq	(%r8,%rcx), %rax
     5d8:	48 8b 30 	movq	(%rax), %rsi
     5db:	48 ff ce 	decq	%rsi
     5de:	48 89 30 	movq	%rsi, (%rax)
     5e1:	48 8d 9c 93 00 00 fe ff 	leaq	-131072(%rbx,%rdx,4), %rbx
     5e9:	8b 03 	movl	(%rbx), %eax
     5eb:	0f b6 d0 	movzbl	%al, %edx
     5ee:	48 83 c3 04 	addq	$4, %rbx
     5f2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALI:
     5f6:	8b 03 	movl	(%rbx), %eax
     5f8:	0f b6 d0 	movzbl	%al, %edx
     5fb:	48 83 c3 04 	addq	$4, %rbx
     5ff:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALF:
     603:	8b 03 	movl	(%rbx), %eax
     605:	0f b6 d0 	movzbl	%al, %edx
     608:	48 83 c3 04 	addq	$4, %rbx
     60c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAI:
     610:	8b 03 	movl	(%rbx), %eax
     612:	0f b6 d0 	movzbl	%al, %edx
     615:	48 83 c3 04 	addq	$4, %rbx
     619:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAF:
     61d:	8b 03 	movl	(%rbx), %eax
     61f:	0f b6 d0 	movzbl	%al, %edx
     622:	48 83 c3 04 	addq	$4, %rbx
     626:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALT:
     62a:	8b 03 	movl	(%rbx), %eax
     62c:	0f b6 d0 	movzbl	%al, %edx
     62f:	48 83 c3 04 	addq	$4, %rbx
     633:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BAND_II:
     637:	8b 03 	movl	(%rbx), %eax
     639:	0f b6 d0 	movzbl	%al, %edx
     63c:	48 83 c3 04 	addq	$4, %rbx
     640:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BOR_II:
     644:	8b 03 	movl	(%rbx), %eax
     646:	0f b6 d0 	movzbl	%al, %edx
     649:	48 83 c3 04 	addq	$4, %rbx
     64d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BXOR_II:
     651:	8b 03 	movl	(%rbx), %eax
     653:	0f b6 d0 	movzbl	%al, %edx
     656:	48 83 c3 04 	addq	$4, %rbx
     65a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHL_II:
     65e:	8b 03 	movl	(%rbx), %eax
     660:	0f b6 d0 	movzbl	%al, %edx
     663:	48 83 c3 04 	addq	$4, %rbx
     667:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHR_II:
     66b:	8b 03 	movl	(%rbx), %eax
     66d:	0f b6 d0 	movzbl	%al, %edx
     670:	48 83 c3 04 	addq	$4, %rbx
     674:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BNOT_I:
     678:	8b 03 	movl	(%rbx), %eax
     67a:	0f b6 d0 	movzbl	%al, %edx
     67d:	48 83 c3 04 	addq	$4, %rbx
     681:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_II:
     685:	8b 03 	movl	(%rbx), %eax
     687:	0f b6 d0 	movzbl	%al, %edx
     68a:	48 83 c3 04 	addq	$4, %rbx
     68e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_FF:
     692:	8b 03 	movl	(%rbx), %eax
     694:	0f b6 d0 	movzbl	%al, %edx
     697:	48 83 c3 04 	addq	$4, %rbx
     69b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_II:
     69f:	8b 03 	movl	(%rbx), %eax
     6a1:	0f b6 d0 	movzbl	%al, %edx
     6a4:	48 83 c3 04 	addq	$4, %rbx
     6a8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_FF:
     6ac:	8b 03 	movl	(%rbx), %eax
     6ae:	0f b6 d0 	movzbl	%al, %edx
     6b1:	48 83 c3 04 	addq	$4, %rbx
     6b5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_II:
     6b9:	8b 03 	movl	(%rbx), %eax
     6bb:	0f b6 d0 	movzbl	%al, %edx
     6be:	48 83 c3 04 	addq	$4, %rbx
     6c2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_FF:
     6c6:	8b 03 	movl	(%rbx), %eax
     6c8:	0f b6 d0 	movzbl	%al, %edx
     6cb:	48 83 c3 04 	addq	$4, %rbx
     6cf:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_S:
     6d3:	8b 03 	movl	(%rbx), %eax
     6d5:	0f b6 d0 	movzbl	%al, %edx
     6d8:	48 83 c3 04 	addq	$4, %rbx
     6dc:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_S:
     6e0:	8b 03 	movl	(%rbx), %eax
     6e2:	0f b6 d0 	movzbl	%al, %edx
     6e5:	48 83 c3 04 	addq	$4, %rbx
     6e9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_S:
     6ed:	8b 03 	movl	(%rbx), %eax
     6ef:	0f b6 d0 	movzbl	%al, %edx
     6f2:	48 83 c3 04 	addq	$4, %rbx
     6f6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_I:
     6fa:	8b 03 	movl	(%rbx), %eax
     6fc:	0f b6 d0 	movzbl	%al, %edx
     6ff:	48 83 c3 04 	addq	$4, %rbx
     703:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_I:
     707:	8b 03 	movl	(%rbx), %eax
     709:	0f b6 d0 	movzbl	%al, %edx
     70c:	48 83 c3 04 	addq	$4, %rbx
     710:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_SK:
     714:	8b 03 	movl	(%rbx), %eax
     716:	0f b6 d0 	movzbl	%al, %edx
     719:	48 83 c3 04 	addq	$4, %rbx
     71d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_SK:
     721:	8b 03 	movl	(%rbx), %eax
     723:	0f b6 d0 	movzbl	%al, %edx
     726:	48 83 c3 04 	addq	$4, %rbx
     72a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_SK:
     72e:	8b 03 	movl	(%rbx), %eax
     730:	0f b6 d0 	movzbl	%al, %edx
     733:	48 83 c3 04 	addq	$4, %rbx
     737:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABUP_SK:
     73b:	8b 03 	movl	(%rbx), %eax
     73d:	0f b6 d0 	movzbl	%al, %edx
     740:	48 83 c3 04 	addq	$4, %rbx
     744:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_luaV_interp:
     748:	55 	pushq	%rbp
     749:	57 	pushq	%rdi
     74a:	56 	pushq	%rsi
     74b:	53 	pushq	%rbx
     74c:	41 54 	pushq	%r12
     74e:	41 55 	pushq	%r13
     750:	41 56 	pushq	%r14
     752:	41 57 	pushq	%r15
     754:	48 83 ec 28 	subq	$40, %rsp
     758:	48 89 cd 	movq	%rcx, %rbp
     75b:	4c 8b 75 18 	movq	24(%rbp), %r14
     75f:	49 81 c6 40 05 00 00 	addq	$1344, %r14
     766:	4c 8b 65 20 	movq	32(%rbp), %r12
     76a:	66 41 83 4c 24 42 08 	orw	$8, 66(%r12)

ravi_new_frame:
     771:	49 8b 34 24 	movq	(%r12), %rsi
     775:	4c 8b 2e 	movq	(%rsi), %r13
     778:	4d 8b 44 24 20 	movq	32(%r12), %r8
     77d:	49 8b 5d 18 	movq	24(%r13), %rbx
     781:	4c 8b 7b 30 	movq	48(%rbx), %r15
     785:	49 8b 5c 24 28 	movq	40(%r12), %rbx
     78a:	8b 03 	movl	(%rbx), %eax
     78c:	0f b6 d0 	movzbl	%al, %edx
     78f:	48 83 c3 04 	addq	$4, %rbx
     793:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_vm_return:
     797:	48 83 c4 28 	addq	$40, %rsp
     79b:	41 5f 	popq	%r15
     79d:	41 5e 	popq	%r14
     79f:	41 5d 	popq	%r13
     7a1:	41 5c 	popq	%r12
     7a3:	5b 	popq	%rbx
     7a4:	5e 	popq	%rsi
     7a5:	5f 	popq	%rdi
     7a6:	5d 	popq	%rbp
     7a7:	c3 	retq
SYMBOL TABLE:
[ 0](sec -1)(fl 0x00)(ty   0)(scl   3) (nx 0) 0x00000001 @feat.00
[ 1](sec  1)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .text
AUX scnlen 0x7a8 nreloc 2 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 3](sec  0)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 luaF_close
[ 4](sec  0)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 luaD_poscall
[ 5](sec  2)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .pdata
AUX scnlen 0xc nreloc 3 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 7](sec  3)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .xdata
AUX scnlen 0x18 nreloc 0 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 9](sec  1)(fl 0x00)(ty   0)(scl   2) (nx 0) 0x00000000 ravi_vm_asm_begin
[10](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 ravi_BC_MOVE
[11](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000032 ravi_BC_LOADK
[12](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000064 ravi_BC_LOADKX
[13](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000071 ravi_BC_LOADBOOL
[14](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000007e ravi_BC_LOADNIL
[15](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000008b ravi_BC_GETUPVAL
[16](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000098 ravi_BC_GETTABUP
[17](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000a5 ravi_BC_GETTABLE
[18](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000b2 ravi_BC_SETTABUP
[19](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000bf ravi_BC_SETUPVAL
[20](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000cc ravi_BC_SETTABLE
[21](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000d9 ravi_BC_NEWTABLE
[22](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000e6 ravi_BC_SELF
[23](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000f3 ravi_BC_ADD
[24](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000100 ravi_BC_SUB
[25](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000010d ravi_BC_MUL
[26](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000011a ravi_BC_MOD
[27](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000127 ravi_BC_POW
[28](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000134 ravi_BC_DIV
[29](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000141 ravi_BC_IDIV
[30](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000014e ravi_BC_BAND
[31](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000015b ravi_BC_BOR
[32](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000168 ravi_BC_BXOR
[33](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000175 ravi_BC_SHL
[34](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000182 ravi_BC_SHR
[35](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000018f ravi_BC_UNM
[36](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000019c ravi_BC_BNOT
[37](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001a9 ravi_BC_NOT
[38](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001b6 ravi_BC_LEN
[39](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001c3 ravi_BC_CONCAT
[40](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001d0 ravi_BC_JMP
[41](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001dd ravi_BC_EQ
[42](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001ea ravi_BC_LT
[43](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001f7 ravi_BC_LE
[44](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000204 ravi_BC_TEST
[45](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000211 ravi_BC_TESTSET
[46](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000021e ravi_BC_CALL
[47](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000022b ravi_BC_TAILCALL
[48](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000238 ravi_BC_RETURN
[49](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002b3 ravi_BC_FORLOOP
[50](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002c0 ravi_BC_FORPREP
[51](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002cd ravi_BC_TFORCALL
[52](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002da ravi_BC_TFORLOOP
[53](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002e7 ravi_BC_SETLIST
[54](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002f4 ravi_BC_CLOSURE
[55](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000301 ravi_BC_VARARG
[56](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000030e ravi_BC_EXTRAARG
[57](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000031b ravi_BC_NEWARRAYI
[58](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000328 ravi_BC_NEWARRAYF
[59](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000335 ravi_BC_LOADIZ
[60](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000342 ravi_BC_LOADFZ
[61](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000034f ravi_BC_UNMF
[62](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000035c ravi_BC_UNMI
[63](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000369 ravi_BC_ADDFF
[64](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000376 ravi_BC_ADDFI
[65](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000383 ravi_BC_ADDII
[66](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000390 ravi_BC_SUBFF
[67](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000039d ravi_BC_SUBFI
[68](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003aa ravi_BC_SUBIF
[69](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003b7 ravi_BC_SUBII
[70](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003c4 ravi_BC_MULFF
[71](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003d1 ravi_BC_MULFI
[72](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003de ravi_BC_MULII
[73](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003eb ravi_BC_DIVFF
[74](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003f8 ravi_BC_DIVFI
[75](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000405 ravi_BC_DIVIF
[76](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000412 ravi_BC_DIVII
[77](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000041f ravi_BC_TOINT
[78](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000042c ravi_BC_TOFLT
[79](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000439 ravi_BC_TOARRAYI
[80](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000446 ravi_BC_TOARRAYF
[81](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000453 ravi_BC_TOTAB
[82](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000460 ravi_BC_TOSTRING
[83](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000046d ravi_BC_TOCLOSURE
[84](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000047a ravi_BC_TOTYPE
[85](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000487 ravi_BC_MOVEI
[86](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000494 ravi_BC_MOVEF
[87](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004a1 ravi_BC_MOVEAI
[88](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004ae ravi_BC_MOVEAF
[89](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004bb ravi_BC_MOVETAB
[90](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004c8 ravi_BC_GETTABLE_AI
[91](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004d5 ravi_BC_GETTABLE_AF
[92](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004e2 ravi_BC_SETTABLE_AI
[93](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004ef ravi_BC_SETTABLE_AF
[94](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004fc ravi_BC_SETTABLE_AII
[95](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000509 ravi_BC_SETTABLE_AFF
[96](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000516 ravi_BC_FORLOOP_IP
[97](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000558 ravi_BC_FORLOOP_I1
[98](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000599 ravi_BC_FORPREP_IP
[99](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005c8 ravi_BC_FORPREP_I1
[100](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005f6 ravi_BC_SETUPVALI
[101](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000603 ravi_BC_SETUPVALF
[102](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000610 ravi_BC_SETUPVALAI
[103](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000061d ravi_BC_SETUPVALAF
[104](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000062a ravi_BC_SETUPVALT
[105](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000637 ravi_BC_BAND_II
[106](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000644 ravi_BC_BOR_II
[107](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000651 ravi_BC_BXOR_II
[108](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000065e ravi_BC_SHL_II
[109](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000066b ravi_BC_SHR_II
[110](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000678 ravi_BC_BNOT_I
[111](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000685 ravi_BC_EQ_II
[112](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000692 ravi_BC_EQ_FF
[113](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000069f ravi_BC_LT_II
[114](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006ac ravi_BC_LT_FF
[115](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006b9 ravi_BC_LE_II
[116](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006c6 ravi_BC_LE_FF
[117](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006d3 ravi_BC_GETTABLE_S
[118](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006e0 ravi_BC_SETTABLE_S
[119](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006ed ravi_BC_SELF_S
[120](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006fa ravi_BC_GETTABLE_I
[121](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000707 ravi_BC_SETTABLE_I
[122](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000714 ravi_BC_GETTABLE_SK
[123](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000721 ravi_BC_SELF_SK
[124](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000072e ravi_BC_SETTABLE_SK
[125](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000073b ravi_BC_GETTABUP_SK
[126](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000748 ravi_luaV_interp
[127](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000771 ravi_new_frame
[128](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000797 ravi_vm_return
[129](sec  4)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .rdata$Z
AUX scnlen 0xd nreloc 0 nlnno 0 checksum 0x0 assoc 0 comdat 0
Unwind info:

Function Table:
  Start Address: ravi_vm_asm_begin
  End Address: ravi_vm_asm_begin + 0x07a8
  Unwind Info Address: .xdata
    Version: 1
    Flags: 0
    Size of prolog: 0
    Number of Codes: 9
    No frame pointer used
    Unwind Codes:
      0x00: UOP_AllocSmall 40
      0x00: UOP_PushNonVol R15
      0x00: UOP_PushNonVol R14
      0x00: UOP_PushNonVol R13
      0x00: UOP_PushNonVol R12
      0x00: UOP_PushNonVol RBX
      0x00: UOP_PushNonVol RSI
      0x00: UOP_PushNonVol RDI
      0x00: UOP_PushNonVol RBP

