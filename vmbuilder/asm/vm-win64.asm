
vm.obj:	file format COFF-x86-64

Disassembly of section .text:
ravi_vm_asm_begin:
       0:	0f b6 cc 	movzbl	%ah, %ecx
       3:	c1 e8 10 	shrl	$16, %eax
       6:	0f b6 d4 	movzbl	%ah, %edx
       9:	c1 e2 04 	shll	$4, %edx
       c:	4d 8d 14 10 	leaq	(%r8,%rdx), %r10
      10:	c1 e1 04 	shll	$4, %ecx
      13:	4d 8d 1c 08 	leaq	(%r8,%rcx), %r11
      17:	49 8b 02 	movq	(%r10), %rax
      1a:	45 8b 4a 08 	movl	8(%r10), %r9d
      1e:	49 89 03 	movq	%rax, (%r11)
      21:	45 89 4b 08 	movl	%r9d, 8(%r11)
      25:	8b 03 	movl	(%rbx), %eax
      27:	0f b6 d0 	movzbl	%al, %edx
      2a:	48 83 c3 04 	addq	$4, %rbx
      2e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADK:
      32:	0f b6 cc 	movzbl	%ah, %ecx
      35:	c1 e8 10 	shrl	$16, %eax
      38:	0f b7 d0 	movzwl	%ax, %edx
      3b:	c1 e2 04 	shll	$4, %edx
      3e:	4d 8d 14 17 	leaq	(%r15,%rdx), %r10
      42:	c1 e1 04 	shll	$4, %ecx
      45:	4d 8d 1c 08 	leaq	(%r8,%rcx), %r11
      49:	49 8b 02 	movq	(%r10), %rax
      4c:	45 8b 4a 08 	movl	8(%r10), %r9d
      50:	49 89 03 	movq	%rax, (%r11)
      53:	45 89 4b 08 	movl	%r9d, 8(%r11)
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
     238:	4d 8b 55 18 	movq	24(%r13), %r10
     23c:	41 83 7a 20 00 	cmpl	$0, 32(%r10)
     241:	74 17 	je	23 <ravi_BC_RETURN+0x22>
     243:	49 89 c7 	movq	%rax, %r15
     246:	4d 89 c5 	movq	%r8, %r13
     249:	48 89 e9 	movq	%rbp, %rcx
     24c:	4c 89 c2 	movq	%r8, %rdx
     24f:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x1C>
     254:	4d 89 e8 	movq	%r13, %r8
     257:	4c 89 f8 	movq	%r15, %rax
     25a:	49 89 5c 24 28 	movq	%rbx, 40(%r12)
     25f:	0f b6 cc 	movzbl	%ah, %ecx
     262:	c1 e8 10 	shrl	$16, %eax
     265:	0f b6 d4 	movzbl	%ah, %edx
     268:	41 89 ca 	movl	%ecx, %r10d
     26b:	85 d2 	testl	%edx, %edx
     26d:	74 0f 	je	15 <ravi_BC_RETURN+0x46>
     26f:	ff ca 	decl	%edx
     271:	41 89 d1 	movl	%edx, %r9d
     274:	41 c1 e2 04 	shll	$4, %r10d
     278:	4f 8d 04 10 	leaq	(%r8,%r10), %r8
     27c:	eb 13 	jmp	19 <ravi_BC_RETURN+0x59>
     27e:	41 c1 e2 04 	shll	$4, %r10d
     282:	4f 8d 04 10 	leaq	(%r8,%r10), %r8
     286:	4c 8b 4d 10 	movq	16(%rbp), %r9
     28a:	4d 29 c1 	subq	%r8, %r9
     28d:	49 c1 e9 04 	shrq	$4, %r9
     291:	48 89 e9 	movq	%rbp, %rcx
     294:	4c 89 e2 	movq	%r12, %rdx
     297:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x64>
     29c:	66 41 f7 44 24 42 08 00 	testw	$8, 66(%r12)
     2a4:	0f 85 fc 04 00 00 	jne	1276 <ravi_vm_return>
     2aa:	4c 8b 65 20 	movq	32(%rbp), %r12
     2ae:	85 c0 	testl	%eax, %eax
     2b0:	74 09 	je	9 <ravi_BC_RETURN+0x83>
     2b2:	4d 8b 6c 24 08 	movq	8(%r12), %r13
     2b7:	4c 89 6d 10 	movq	%r13, 16(%rbp)
     2bb:	e9 c0 04 00 00 	jmp	1216 <ravi_new_frame>

ravi_BC_FORLOOP:
     2c0:	8b 03 	movl	(%rbx), %eax
     2c2:	0f b6 d0 	movzbl	%al, %edx
     2c5:	48 83 c3 04 	addq	$4, %rbx
     2c9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP:
     2cd:	8b 03 	movl	(%rbx), %eax
     2cf:	0f b6 d0 	movzbl	%al, %edx
     2d2:	48 83 c3 04 	addq	$4, %rbx
     2d6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORCALL:
     2da:	8b 03 	movl	(%rbx), %eax
     2dc:	0f b6 d0 	movzbl	%al, %edx
     2df:	48 83 c3 04 	addq	$4, %rbx
     2e3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORLOOP:
     2e7:	8b 03 	movl	(%rbx), %eax
     2e9:	0f b6 d0 	movzbl	%al, %edx
     2ec:	48 83 c3 04 	addq	$4, %rbx
     2f0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETLIST:
     2f4:	8b 03 	movl	(%rbx), %eax
     2f6:	0f b6 d0 	movzbl	%al, %edx
     2f9:	48 83 c3 04 	addq	$4, %rbx
     2fd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CLOSURE:
     301:	8b 03 	movl	(%rbx), %eax
     303:	0f b6 d0 	movzbl	%al, %edx
     306:	48 83 c3 04 	addq	$4, %rbx
     30a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_VARARG:
     30e:	8b 03 	movl	(%rbx), %eax
     310:	0f b6 d0 	movzbl	%al, %edx
     313:	48 83 c3 04 	addq	$4, %rbx
     317:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EXTRAARG:
     31b:	8b 03 	movl	(%rbx), %eax
     31d:	0f b6 d0 	movzbl	%al, %edx
     320:	48 83 c3 04 	addq	$4, %rbx
     324:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYI:
     328:	8b 03 	movl	(%rbx), %eax
     32a:	0f b6 d0 	movzbl	%al, %edx
     32d:	48 83 c3 04 	addq	$4, %rbx
     331:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYF:
     335:	8b 03 	movl	(%rbx), %eax
     337:	0f b6 d0 	movzbl	%al, %edx
     33a:	48 83 c3 04 	addq	$4, %rbx
     33e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADIZ:
     342:	8b 03 	movl	(%rbx), %eax
     344:	0f b6 d0 	movzbl	%al, %edx
     347:	48 83 c3 04 	addq	$4, %rbx
     34b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADFZ:
     34f:	8b 03 	movl	(%rbx), %eax
     351:	0f b6 d0 	movzbl	%al, %edx
     354:	48 83 c3 04 	addq	$4, %rbx
     358:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMF:
     35c:	8b 03 	movl	(%rbx), %eax
     35e:	0f b6 d0 	movzbl	%al, %edx
     361:	48 83 c3 04 	addq	$4, %rbx
     365:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMI:
     369:	8b 03 	movl	(%rbx), %eax
     36b:	0f b6 d0 	movzbl	%al, %edx
     36e:	48 83 c3 04 	addq	$4, %rbx
     372:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFF:
     376:	8b 03 	movl	(%rbx), %eax
     378:	0f b6 d0 	movzbl	%al, %edx
     37b:	48 83 c3 04 	addq	$4, %rbx
     37f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFI:
     383:	8b 03 	movl	(%rbx), %eax
     385:	0f b6 d0 	movzbl	%al, %edx
     388:	48 83 c3 04 	addq	$4, %rbx
     38c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDII:
     390:	8b 03 	movl	(%rbx), %eax
     392:	0f b6 d0 	movzbl	%al, %edx
     395:	48 83 c3 04 	addq	$4, %rbx
     399:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFF:
     39d:	8b 03 	movl	(%rbx), %eax
     39f:	0f b6 d0 	movzbl	%al, %edx
     3a2:	48 83 c3 04 	addq	$4, %rbx
     3a6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFI:
     3aa:	8b 03 	movl	(%rbx), %eax
     3ac:	0f b6 d0 	movzbl	%al, %edx
     3af:	48 83 c3 04 	addq	$4, %rbx
     3b3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBIF:
     3b7:	8b 03 	movl	(%rbx), %eax
     3b9:	0f b6 d0 	movzbl	%al, %edx
     3bc:	48 83 c3 04 	addq	$4, %rbx
     3c0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBII:
     3c4:	8b 03 	movl	(%rbx), %eax
     3c6:	0f b6 d0 	movzbl	%al, %edx
     3c9:	48 83 c3 04 	addq	$4, %rbx
     3cd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFF:
     3d1:	8b 03 	movl	(%rbx), %eax
     3d3:	0f b6 d0 	movzbl	%al, %edx
     3d6:	48 83 c3 04 	addq	$4, %rbx
     3da:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFI:
     3de:	8b 03 	movl	(%rbx), %eax
     3e0:	0f b6 d0 	movzbl	%al, %edx
     3e3:	48 83 c3 04 	addq	$4, %rbx
     3e7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULII:
     3eb:	8b 03 	movl	(%rbx), %eax
     3ed:	0f b6 d0 	movzbl	%al, %edx
     3f0:	48 83 c3 04 	addq	$4, %rbx
     3f4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFF:
     3f8:	8b 03 	movl	(%rbx), %eax
     3fa:	0f b6 d0 	movzbl	%al, %edx
     3fd:	48 83 c3 04 	addq	$4, %rbx
     401:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFI:
     405:	8b 03 	movl	(%rbx), %eax
     407:	0f b6 d0 	movzbl	%al, %edx
     40a:	48 83 c3 04 	addq	$4, %rbx
     40e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVIF:
     412:	8b 03 	movl	(%rbx), %eax
     414:	0f b6 d0 	movzbl	%al, %edx
     417:	48 83 c3 04 	addq	$4, %rbx
     41b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVII:
     41f:	8b 03 	movl	(%rbx), %eax
     421:	0f b6 d0 	movzbl	%al, %edx
     424:	48 83 c3 04 	addq	$4, %rbx
     428:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOINT:
     42c:	8b 03 	movl	(%rbx), %eax
     42e:	0f b6 d0 	movzbl	%al, %edx
     431:	48 83 c3 04 	addq	$4, %rbx
     435:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOFLT:
     439:	8b 03 	movl	(%rbx), %eax
     43b:	0f b6 d0 	movzbl	%al, %edx
     43e:	48 83 c3 04 	addq	$4, %rbx
     442:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYI:
     446:	8b 03 	movl	(%rbx), %eax
     448:	0f b6 d0 	movzbl	%al, %edx
     44b:	48 83 c3 04 	addq	$4, %rbx
     44f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYF:
     453:	8b 03 	movl	(%rbx), %eax
     455:	0f b6 d0 	movzbl	%al, %edx
     458:	48 83 c3 04 	addq	$4, %rbx
     45c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTAB:
     460:	8b 03 	movl	(%rbx), %eax
     462:	0f b6 d0 	movzbl	%al, %edx
     465:	48 83 c3 04 	addq	$4, %rbx
     469:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOSTRING:
     46d:	8b 03 	movl	(%rbx), %eax
     46f:	0f b6 d0 	movzbl	%al, %edx
     472:	48 83 c3 04 	addq	$4, %rbx
     476:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOCLOSURE:
     47a:	8b 03 	movl	(%rbx), %eax
     47c:	0f b6 d0 	movzbl	%al, %edx
     47f:	48 83 c3 04 	addq	$4, %rbx
     483:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTYPE:
     487:	8b 03 	movl	(%rbx), %eax
     489:	0f b6 d0 	movzbl	%al, %edx
     48c:	48 83 c3 04 	addq	$4, %rbx
     490:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEI:
     494:	8b 03 	movl	(%rbx), %eax
     496:	0f b6 d0 	movzbl	%al, %edx
     499:	48 83 c3 04 	addq	$4, %rbx
     49d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEF:
     4a1:	8b 03 	movl	(%rbx), %eax
     4a3:	0f b6 d0 	movzbl	%al, %edx
     4a6:	48 83 c3 04 	addq	$4, %rbx
     4aa:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAI:
     4ae:	8b 03 	movl	(%rbx), %eax
     4b0:	0f b6 d0 	movzbl	%al, %edx
     4b3:	48 83 c3 04 	addq	$4, %rbx
     4b7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAF:
     4bb:	8b 03 	movl	(%rbx), %eax
     4bd:	0f b6 d0 	movzbl	%al, %edx
     4c0:	48 83 c3 04 	addq	$4, %rbx
     4c4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVETAB:
     4c8:	8b 03 	movl	(%rbx), %eax
     4ca:	0f b6 d0 	movzbl	%al, %edx
     4cd:	48 83 c3 04 	addq	$4, %rbx
     4d1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AI:
     4d5:	8b 03 	movl	(%rbx), %eax
     4d7:	0f b6 d0 	movzbl	%al, %edx
     4da:	48 83 c3 04 	addq	$4, %rbx
     4de:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AF:
     4e2:	8b 03 	movl	(%rbx), %eax
     4e4:	0f b6 d0 	movzbl	%al, %edx
     4e7:	48 83 c3 04 	addq	$4, %rbx
     4eb:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AI:
     4ef:	8b 03 	movl	(%rbx), %eax
     4f1:	0f b6 d0 	movzbl	%al, %edx
     4f4:	48 83 c3 04 	addq	$4, %rbx
     4f8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AF:
     4fc:	8b 03 	movl	(%rbx), %eax
     4fe:	0f b6 d0 	movzbl	%al, %edx
     501:	48 83 c3 04 	addq	$4, %rbx
     505:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AII:
     509:	8b 03 	movl	(%rbx), %eax
     50b:	0f b6 d0 	movzbl	%al, %edx
     50e:	48 83 c3 04 	addq	$4, %rbx
     512:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AFF:
     516:	8b 03 	movl	(%rbx), %eax
     518:	0f b6 d0 	movzbl	%al, %edx
     51b:	48 83 c3 04 	addq	$4, %rbx
     51f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_IP:
     523:	0f b6 cc 	movzbl	%ah, %ecx
     526:	c1 e1 04 	shll	$4, %ecx
     529:	4d 8d 0c 08 	leaq	(%r8,%rcx), %r9
     52d:	4d 8b 11 	movq	(%r9), %r10
     530:	4d 03 51 20 	addq	32(%r9), %r10
     534:	4d 3b 51 10 	cmpq	16(%r9), %r10
     538:	7f 1f 	jg	31 <ravi_BC_FORLOOP_IP+0x36>
     53a:	4d 89 11 	movq	%r10, (%r9)
     53d:	4d 8d 59 30 	leaq	48(%r9), %r11
     541:	4d 89 13 	movq	%r10, (%r11)
     544:	66 41 c7 43 08 13 00 	movw	$19, 8(%r11)
     54b:	c1 e8 10 	shrl	$16, %eax
     54e:	0f b7 d0 	movzwl	%ax, %edx
     551:	48 8d 9c 93 00 00 fe ff 	leaq	-131072(%rbx,%rdx,4), %rbx
     559:	8b 03 	movl	(%rbx), %eax
     55b:	0f b6 d0 	movzbl	%al, %edx
     55e:	48 83 c3 04 	addq	$4, %rbx
     562:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_I1:
     566:	0f b6 cc 	movzbl	%ah, %ecx
     569:	c1 e1 04 	shll	$4, %ecx
     56c:	4d 8d 0c 08 	leaq	(%r8,%rcx), %r9
     570:	4d 8b 11 	movq	(%r9), %r10
     573:	49 ff c2 	incq	%r10
     576:	4d 3b 51 10 	cmpq	16(%r9), %r10
     57a:	7f 1f 	jg	31 <ravi_BC_FORLOOP_I1+0x35>
     57c:	4d 89 11 	movq	%r10, (%r9)
     57f:	4d 8d 59 30 	leaq	48(%r9), %r11
     583:	4d 89 13 	movq	%r10, (%r11)
     586:	66 41 c7 43 08 13 00 	movw	$19, 8(%r11)
     58d:	c1 e8 10 	shrl	$16, %eax
     590:	0f b7 d0 	movzwl	%ax, %edx
     593:	48 8d 9c 93 00 00 fe ff 	leaq	-131072(%rbx,%rdx,4), %rbx
     59b:	8b 03 	movl	(%rbx), %eax
     59d:	0f b6 d0 	movzbl	%al, %edx
     5a0:	48 83 c3 04 	addq	$4, %rbx
     5a4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_IP:
     5a8:	0f b6 cc 	movzbl	%ah, %ecx
     5ab:	c1 e8 10 	shrl	$16, %eax
     5ae:	0f b7 d0 	movzwl	%ax, %edx
     5b1:	c1 e1 04 	shll	$4, %ecx
     5b4:	49 8d 04 08 	leaq	(%r8,%rcx), %rax
     5b8:	4c 8b 10 	movq	(%rax), %r10
     5bb:	4c 2b 50 20 	subq	32(%rax), %r10
     5bf:	4c 89 10 	movq	%r10, (%rax)
     5c2:	48 8d 9c 93 00 00 fe ff 	leaq	-131072(%rbx,%rdx,4), %rbx
     5ca:	8b 03 	movl	(%rbx), %eax
     5cc:	0f b6 d0 	movzbl	%al, %edx
     5cf:	48 83 c3 04 	addq	$4, %rbx
     5d3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_I1:
     5d7:	0f b6 cc 	movzbl	%ah, %ecx
     5da:	c1 e8 10 	shrl	$16, %eax
     5dd:	0f b7 d0 	movzwl	%ax, %edx
     5e0:	c1 e1 04 	shll	$4, %ecx
     5e3:	49 8d 04 08 	leaq	(%r8,%rcx), %rax
     5e7:	4c 8b 10 	movq	(%rax), %r10
     5ea:	49 ff ca 	decq	%r10
     5ed:	4c 89 10 	movq	%r10, (%rax)
     5f0:	48 8d 9c 93 00 00 fe ff 	leaq	-131072(%rbx,%rdx,4), %rbx
     5f8:	8b 03 	movl	(%rbx), %eax
     5fa:	0f b6 d0 	movzbl	%al, %edx
     5fd:	48 83 c3 04 	addq	$4, %rbx
     601:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALI:
     605:	8b 03 	movl	(%rbx), %eax
     607:	0f b6 d0 	movzbl	%al, %edx
     60a:	48 83 c3 04 	addq	$4, %rbx
     60e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALF:
     612:	8b 03 	movl	(%rbx), %eax
     614:	0f b6 d0 	movzbl	%al, %edx
     617:	48 83 c3 04 	addq	$4, %rbx
     61b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAI:
     61f:	8b 03 	movl	(%rbx), %eax
     621:	0f b6 d0 	movzbl	%al, %edx
     624:	48 83 c3 04 	addq	$4, %rbx
     628:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAF:
     62c:	8b 03 	movl	(%rbx), %eax
     62e:	0f b6 d0 	movzbl	%al, %edx
     631:	48 83 c3 04 	addq	$4, %rbx
     635:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALT:
     639:	8b 03 	movl	(%rbx), %eax
     63b:	0f b6 d0 	movzbl	%al, %edx
     63e:	48 83 c3 04 	addq	$4, %rbx
     642:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BAND_II:
     646:	8b 03 	movl	(%rbx), %eax
     648:	0f b6 d0 	movzbl	%al, %edx
     64b:	48 83 c3 04 	addq	$4, %rbx
     64f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BOR_II:
     653:	8b 03 	movl	(%rbx), %eax
     655:	0f b6 d0 	movzbl	%al, %edx
     658:	48 83 c3 04 	addq	$4, %rbx
     65c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BXOR_II:
     660:	8b 03 	movl	(%rbx), %eax
     662:	0f b6 d0 	movzbl	%al, %edx
     665:	48 83 c3 04 	addq	$4, %rbx
     669:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHL_II:
     66d:	8b 03 	movl	(%rbx), %eax
     66f:	0f b6 d0 	movzbl	%al, %edx
     672:	48 83 c3 04 	addq	$4, %rbx
     676:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHR_II:
     67a:	8b 03 	movl	(%rbx), %eax
     67c:	0f b6 d0 	movzbl	%al, %edx
     67f:	48 83 c3 04 	addq	$4, %rbx
     683:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BNOT_I:
     687:	8b 03 	movl	(%rbx), %eax
     689:	0f b6 d0 	movzbl	%al, %edx
     68c:	48 83 c3 04 	addq	$4, %rbx
     690:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_II:
     694:	8b 03 	movl	(%rbx), %eax
     696:	0f b6 d0 	movzbl	%al, %edx
     699:	48 83 c3 04 	addq	$4, %rbx
     69d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_FF:
     6a1:	8b 03 	movl	(%rbx), %eax
     6a3:	0f b6 d0 	movzbl	%al, %edx
     6a6:	48 83 c3 04 	addq	$4, %rbx
     6aa:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_II:
     6ae:	8b 03 	movl	(%rbx), %eax
     6b0:	0f b6 d0 	movzbl	%al, %edx
     6b3:	48 83 c3 04 	addq	$4, %rbx
     6b7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_FF:
     6bb:	8b 03 	movl	(%rbx), %eax
     6bd:	0f b6 d0 	movzbl	%al, %edx
     6c0:	48 83 c3 04 	addq	$4, %rbx
     6c4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_II:
     6c8:	8b 03 	movl	(%rbx), %eax
     6ca:	0f b6 d0 	movzbl	%al, %edx
     6cd:	48 83 c3 04 	addq	$4, %rbx
     6d1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_FF:
     6d5:	8b 03 	movl	(%rbx), %eax
     6d7:	0f b6 d0 	movzbl	%al, %edx
     6da:	48 83 c3 04 	addq	$4, %rbx
     6de:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_S:
     6e2:	8b 03 	movl	(%rbx), %eax
     6e4:	0f b6 d0 	movzbl	%al, %edx
     6e7:	48 83 c3 04 	addq	$4, %rbx
     6eb:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_S:
     6ef:	8b 03 	movl	(%rbx), %eax
     6f1:	0f b6 d0 	movzbl	%al, %edx
     6f4:	48 83 c3 04 	addq	$4, %rbx
     6f8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_S:
     6fc:	8b 03 	movl	(%rbx), %eax
     6fe:	0f b6 d0 	movzbl	%al, %edx
     701:	48 83 c3 04 	addq	$4, %rbx
     705:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_I:
     709:	8b 03 	movl	(%rbx), %eax
     70b:	0f b6 d0 	movzbl	%al, %edx
     70e:	48 83 c3 04 	addq	$4, %rbx
     712:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_I:
     716:	8b 03 	movl	(%rbx), %eax
     718:	0f b6 d0 	movzbl	%al, %edx
     71b:	48 83 c3 04 	addq	$4, %rbx
     71f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_SK:
     723:	8b 03 	movl	(%rbx), %eax
     725:	0f b6 d0 	movzbl	%al, %edx
     728:	48 83 c3 04 	addq	$4, %rbx
     72c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_SK:
     730:	8b 03 	movl	(%rbx), %eax
     732:	0f b6 d0 	movzbl	%al, %edx
     735:	48 83 c3 04 	addq	$4, %rbx
     739:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_SK:
     73d:	8b 03 	movl	(%rbx), %eax
     73f:	0f b6 d0 	movzbl	%al, %edx
     742:	48 83 c3 04 	addq	$4, %rbx
     746:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABUP_SK:
     74a:	8b 03 	movl	(%rbx), %eax
     74c:	0f b6 d0 	movzbl	%al, %edx
     74f:	48 83 c3 04 	addq	$4, %rbx
     753:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_luaV_interp:
     757:	55 	pushq	%rbp
     758:	57 	pushq	%rdi
     759:	56 	pushq	%rsi
     75a:	53 	pushq	%rbx
     75b:	41 54 	pushq	%r12
     75d:	41 55 	pushq	%r13
     75f:	41 56 	pushq	%r14
     761:	41 57 	pushq	%r15
     763:	48 83 ec 28 	subq	$40, %rsp
     767:	48 89 cd 	movq	%rcx, %rbp
     76a:	4c 8b 75 18 	movq	24(%rbp), %r14
     76e:	49 81 c6 40 05 00 00 	addq	$1344, %r14
     775:	4c 8b 65 20 	movq	32(%rbp), %r12
     779:	66 41 83 4c 24 42 08 	orw	$8, 66(%r12)

ravi_new_frame:
     780:	4d 8b 14 24 	movq	(%r12), %r10
     784:	4d 8b 2a 	movq	(%r10), %r13
     787:	4d 8b 44 24 20 	movq	32(%r12), %r8
     78c:	49 8b 5d 18 	movq	24(%r13), %rbx
     790:	4c 8b 7b 30 	movq	48(%rbx), %r15
     794:	49 8b 5c 24 28 	movq	40(%r12), %rbx
     799:	8b 03 	movl	(%rbx), %eax
     79b:	0f b6 d0 	movzbl	%al, %edx
     79e:	48 83 c3 04 	addq	$4, %rbx
     7a2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_vm_return:
     7a6:	48 83 c4 28 	addq	$40, %rsp
     7aa:	41 5f 	popq	%r15
     7ac:	41 5e 	popq	%r14
     7ae:	41 5d 	popq	%r13
     7b0:	41 5c 	popq	%r12
     7b2:	5b 	popq	%rbx
     7b3:	5e 	popq	%rsi
     7b4:	5f 	popq	%rdi
     7b5:	5d 	popq	%rbp
     7b6:	c3 	retq
SYMBOL TABLE:
[ 0](sec -1)(fl 0x00)(ty   0)(scl   3) (nx 0) 0x00000001 @feat.00
[ 1](sec  1)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .text
AUX scnlen 0x7b7 nreloc 2 nlnno 0 checksum 0x0 assoc 0 comdat 0
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
[49](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002c0 ravi_BC_FORLOOP
[50](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002cd ravi_BC_FORPREP
[51](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002da ravi_BC_TFORCALL
[52](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002e7 ravi_BC_TFORLOOP
[53](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002f4 ravi_BC_SETLIST
[54](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000301 ravi_BC_CLOSURE
[55](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000030e ravi_BC_VARARG
[56](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000031b ravi_BC_EXTRAARG
[57](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000328 ravi_BC_NEWARRAYI
[58](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000335 ravi_BC_NEWARRAYF
[59](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000342 ravi_BC_LOADIZ
[60](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000034f ravi_BC_LOADFZ
[61](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000035c ravi_BC_UNMF
[62](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000369 ravi_BC_UNMI
[63](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000376 ravi_BC_ADDFF
[64](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000383 ravi_BC_ADDFI
[65](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000390 ravi_BC_ADDII
[66](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000039d ravi_BC_SUBFF
[67](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003aa ravi_BC_SUBFI
[68](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003b7 ravi_BC_SUBIF
[69](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003c4 ravi_BC_SUBII
[70](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003d1 ravi_BC_MULFF
[71](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003de ravi_BC_MULFI
[72](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003eb ravi_BC_MULII
[73](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003f8 ravi_BC_DIVFF
[74](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000405 ravi_BC_DIVFI
[75](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000412 ravi_BC_DIVIF
[76](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000041f ravi_BC_DIVII
[77](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000042c ravi_BC_TOINT
[78](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000439 ravi_BC_TOFLT
[79](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000446 ravi_BC_TOARRAYI
[80](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000453 ravi_BC_TOARRAYF
[81](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000460 ravi_BC_TOTAB
[82](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000046d ravi_BC_TOSTRING
[83](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000047a ravi_BC_TOCLOSURE
[84](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000487 ravi_BC_TOTYPE
[85](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000494 ravi_BC_MOVEI
[86](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004a1 ravi_BC_MOVEF
[87](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004ae ravi_BC_MOVEAI
[88](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004bb ravi_BC_MOVEAF
[89](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004c8 ravi_BC_MOVETAB
[90](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004d5 ravi_BC_GETTABLE_AI
[91](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004e2 ravi_BC_GETTABLE_AF
[92](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004ef ravi_BC_SETTABLE_AI
[93](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004fc ravi_BC_SETTABLE_AF
[94](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000509 ravi_BC_SETTABLE_AII
[95](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000516 ravi_BC_SETTABLE_AFF
[96](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000523 ravi_BC_FORLOOP_IP
[97](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000566 ravi_BC_FORLOOP_I1
[98](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005a8 ravi_BC_FORPREP_IP
[99](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005d7 ravi_BC_FORPREP_I1
[100](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000605 ravi_BC_SETUPVALI
[101](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000612 ravi_BC_SETUPVALF
[102](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000061f ravi_BC_SETUPVALAI
[103](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000062c ravi_BC_SETUPVALAF
[104](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000639 ravi_BC_SETUPVALT
[105](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000646 ravi_BC_BAND_II
[106](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000653 ravi_BC_BOR_II
[107](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000660 ravi_BC_BXOR_II
[108](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000066d ravi_BC_SHL_II
[109](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000067a ravi_BC_SHR_II
[110](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000687 ravi_BC_BNOT_I
[111](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000694 ravi_BC_EQ_II
[112](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006a1 ravi_BC_EQ_FF
[113](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006ae ravi_BC_LT_II
[114](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006bb ravi_BC_LT_FF
[115](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006c8 ravi_BC_LE_II
[116](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006d5 ravi_BC_LE_FF
[117](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006e2 ravi_BC_GETTABLE_S
[118](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006ef ravi_BC_SETTABLE_S
[119](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006fc ravi_BC_SELF_S
[120](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000709 ravi_BC_GETTABLE_I
[121](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000716 ravi_BC_SETTABLE_I
[122](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000723 ravi_BC_GETTABLE_SK
[123](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000730 ravi_BC_SELF_SK
[124](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000073d ravi_BC_SETTABLE_SK
[125](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000074a ravi_BC_GETTABUP_SK
[126](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000757 ravi_luaV_interp
[127](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000780 ravi_new_frame
[128](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000007a6 ravi_vm_return
[129](sec  4)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .rdata$Z
AUX scnlen 0xd nreloc 0 nlnno 0 checksum 0x0 assoc 0 comdat 0
Unwind info:

Function Table:
  Start Address: ravi_vm_asm_begin
  End Address: ravi_vm_asm_begin + 0x07b7
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

