
vm.obj:	file format COFF-x86-64

Disassembly of section .text:
ravi_vm_asm_begin:
       0:	8b 03 	movl	(%rbx), %eax
       2:	0f b6 d0 	movzbl	%al, %edx
       5:	48 83 c3 04 	addq	$4, %rbx
       9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADK:
       d:	0f b6 cc 	movzbl	%ah, %ecx
      10:	c1 e8 10 	shrl	$16, %eax
      13:	0f b7 d0 	movzwl	%ax, %edx
      16:	c1 e2 04 	shll	$4, %edx
      19:	49 8d 34 17 	leaq	(%r15,%rdx), %rsi
      1d:	c1 e1 04 	shll	$4, %ecx
      20:	49 8d 3c 08 	leaq	(%r8,%rcx), %rdi
      24:	48 8b 06 	movq	(%rsi), %rax
      27:	44 8b 4e 08 	movl	8(%rsi), %r9d
      2b:	48 89 07 	movq	%rax, (%rdi)
      2e:	44 89 4f 08 	movl	%r9d, 8(%rdi)
      32:	8b 03 	movl	(%rbx), %eax
      34:	0f b6 d0 	movzbl	%al, %edx
      37:	48 83 c3 04 	addq	$4, %rbx
      3b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADKX:
      3f:	8b 03 	movl	(%rbx), %eax
      41:	0f b6 d0 	movzbl	%al, %edx
      44:	48 83 c3 04 	addq	$4, %rbx
      48:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADBOOL:
      4c:	8b 03 	movl	(%rbx), %eax
      4e:	0f b6 d0 	movzbl	%al, %edx
      51:	48 83 c3 04 	addq	$4, %rbx
      55:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADNIL:
      59:	8b 03 	movl	(%rbx), %eax
      5b:	0f b6 d0 	movzbl	%al, %edx
      5e:	48 83 c3 04 	addq	$4, %rbx
      62:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETUPVAL:
      66:	8b 03 	movl	(%rbx), %eax
      68:	0f b6 d0 	movzbl	%al, %edx
      6b:	48 83 c3 04 	addq	$4, %rbx
      6f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABUP:
      73:	8b 03 	movl	(%rbx), %eax
      75:	0f b6 d0 	movzbl	%al, %edx
      78:	48 83 c3 04 	addq	$4, %rbx
      7c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE:
      80:	8b 03 	movl	(%rbx), %eax
      82:	0f b6 d0 	movzbl	%al, %edx
      85:	48 83 c3 04 	addq	$4, %rbx
      89:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABUP:
      8d:	8b 03 	movl	(%rbx), %eax
      8f:	0f b6 d0 	movzbl	%al, %edx
      92:	48 83 c3 04 	addq	$4, %rbx
      96:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVAL:
      9a:	8b 03 	movl	(%rbx), %eax
      9c:	0f b6 d0 	movzbl	%al, %edx
      9f:	48 83 c3 04 	addq	$4, %rbx
      a3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE:
      a7:	8b 03 	movl	(%rbx), %eax
      a9:	0f b6 d0 	movzbl	%al, %edx
      ac:	48 83 c3 04 	addq	$4, %rbx
      b0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWTABLE:
      b4:	8b 03 	movl	(%rbx), %eax
      b6:	0f b6 d0 	movzbl	%al, %edx
      b9:	48 83 c3 04 	addq	$4, %rbx
      bd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF:
      c1:	8b 03 	movl	(%rbx), %eax
      c3:	0f b6 d0 	movzbl	%al, %edx
      c6:	48 83 c3 04 	addq	$4, %rbx
      ca:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADD:
      ce:	8b 03 	movl	(%rbx), %eax
      d0:	0f b6 d0 	movzbl	%al, %edx
      d3:	48 83 c3 04 	addq	$4, %rbx
      d7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUB:
      db:	8b 03 	movl	(%rbx), %eax
      dd:	0f b6 d0 	movzbl	%al, %edx
      e0:	48 83 c3 04 	addq	$4, %rbx
      e4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MUL:
      e8:	8b 03 	movl	(%rbx), %eax
      ea:	0f b6 d0 	movzbl	%al, %edx
      ed:	48 83 c3 04 	addq	$4, %rbx
      f1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOD:
      f5:	8b 03 	movl	(%rbx), %eax
      f7:	0f b6 d0 	movzbl	%al, %edx
      fa:	48 83 c3 04 	addq	$4, %rbx
      fe:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_POW:
     102:	8b 03 	movl	(%rbx), %eax
     104:	0f b6 d0 	movzbl	%al, %edx
     107:	48 83 c3 04 	addq	$4, %rbx
     10b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIV:
     10f:	8b 03 	movl	(%rbx), %eax
     111:	0f b6 d0 	movzbl	%al, %edx
     114:	48 83 c3 04 	addq	$4, %rbx
     118:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_IDIV:
     11c:	8b 03 	movl	(%rbx), %eax
     11e:	0f b6 d0 	movzbl	%al, %edx
     121:	48 83 c3 04 	addq	$4, %rbx
     125:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BAND:
     129:	8b 03 	movl	(%rbx), %eax
     12b:	0f b6 d0 	movzbl	%al, %edx
     12e:	48 83 c3 04 	addq	$4, %rbx
     132:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BOR:
     136:	8b 03 	movl	(%rbx), %eax
     138:	0f b6 d0 	movzbl	%al, %edx
     13b:	48 83 c3 04 	addq	$4, %rbx
     13f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BXOR:
     143:	8b 03 	movl	(%rbx), %eax
     145:	0f b6 d0 	movzbl	%al, %edx
     148:	48 83 c3 04 	addq	$4, %rbx
     14c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHL:
     150:	8b 03 	movl	(%rbx), %eax
     152:	0f b6 d0 	movzbl	%al, %edx
     155:	48 83 c3 04 	addq	$4, %rbx
     159:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHR:
     15d:	8b 03 	movl	(%rbx), %eax
     15f:	0f b6 d0 	movzbl	%al, %edx
     162:	48 83 c3 04 	addq	$4, %rbx
     166:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNM:
     16a:	8b 03 	movl	(%rbx), %eax
     16c:	0f b6 d0 	movzbl	%al, %edx
     16f:	48 83 c3 04 	addq	$4, %rbx
     173:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BNOT:
     177:	8b 03 	movl	(%rbx), %eax
     179:	0f b6 d0 	movzbl	%al, %edx
     17c:	48 83 c3 04 	addq	$4, %rbx
     180:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NOT:
     184:	8b 03 	movl	(%rbx), %eax
     186:	0f b6 d0 	movzbl	%al, %edx
     189:	48 83 c3 04 	addq	$4, %rbx
     18d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LEN:
     191:	8b 03 	movl	(%rbx), %eax
     193:	0f b6 d0 	movzbl	%al, %edx
     196:	48 83 c3 04 	addq	$4, %rbx
     19a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CONCAT:
     19e:	8b 03 	movl	(%rbx), %eax
     1a0:	0f b6 d0 	movzbl	%al, %edx
     1a3:	48 83 c3 04 	addq	$4, %rbx
     1a7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_JMP:
     1ab:	8b 03 	movl	(%rbx), %eax
     1ad:	0f b6 d0 	movzbl	%al, %edx
     1b0:	48 83 c3 04 	addq	$4, %rbx
     1b4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ:
     1b8:	8b 03 	movl	(%rbx), %eax
     1ba:	0f b6 d0 	movzbl	%al, %edx
     1bd:	48 83 c3 04 	addq	$4, %rbx
     1c1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT:
     1c5:	8b 03 	movl	(%rbx), %eax
     1c7:	0f b6 d0 	movzbl	%al, %edx
     1ca:	48 83 c3 04 	addq	$4, %rbx
     1ce:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE:
     1d2:	8b 03 	movl	(%rbx), %eax
     1d4:	0f b6 d0 	movzbl	%al, %edx
     1d7:	48 83 c3 04 	addq	$4, %rbx
     1db:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TEST:
     1df:	8b 03 	movl	(%rbx), %eax
     1e1:	0f b6 d0 	movzbl	%al, %edx
     1e4:	48 83 c3 04 	addq	$4, %rbx
     1e8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TESTSET:
     1ec:	8b 03 	movl	(%rbx), %eax
     1ee:	0f b6 d0 	movzbl	%al, %edx
     1f1:	48 83 c3 04 	addq	$4, %rbx
     1f5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CALL:
     1f9:	8b 03 	movl	(%rbx), %eax
     1fb:	0f b6 d0 	movzbl	%al, %edx
     1fe:	48 83 c3 04 	addq	$4, %rbx
     202:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TAILCALL:
     206:	8b 03 	movl	(%rbx), %eax
     208:	0f b6 d0 	movzbl	%al, %edx
     20b:	48 83 c3 04 	addq	$4, %rbx
     20f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_RETURN:
     213:	49 89 c7 	movq	%rax, %r15
     216:	49 8b 75 18 	movq	24(%r13), %rsi
     21a:	83 7e 20 00 	cmpl	$0, 32(%rsi)
     21e:	74 11 	je	17 <ravi_BC_RETURN+0x1E>
     220:	4d 89 c5 	movq	%r8, %r13
     223:	48 89 e9 	movq	%rbp, %rcx
     226:	4c 89 c2 	movq	%r8, %rdx
     229:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x1B>
     22e:	4d 89 e8 	movq	%r13, %r8
     231:	49 89 5c 24 28 	movq	%rbx, 40(%r12)
     236:	4c 89 f8 	movq	%r15, %rax
     239:	0f b6 cc 	movzbl	%ah, %ecx
     23c:	c1 e8 10 	shrl	$16, %eax
     23f:	0f b6 d4 	movzbl	%ah, %edx
     242:	c1 e1 04 	shll	$4, %ecx
     245:	4d 8d 04 08 	leaq	(%r8,%rcx), %r8
     249:	85 d2 	testl	%edx, %edx
     24b:	74 07 	je	7 <ravi_BC_RETURN+0x41>
     24d:	ff ca 	decl	%edx
     24f:	41 89 d1 	movl	%edx, %r9d
     252:	eb 0b 	jmp	11 <ravi_BC_RETURN+0x4C>
     254:	4c 8b 4d 10 	movq	16(%rbp), %r9
     258:	4d 29 c1 	subq	%r8, %r9
     25b:	49 c1 e9 04 	shrq	$4, %r9
     25f:	48 89 e9 	movq	%rbp, %rcx
     262:	4c 89 e2 	movq	%r12, %rdx
     265:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x57>
     26a:	41 f7 44 24 42 08 00 00 00 	testl	$8, 66(%r12)
     273:	0f 85 3f 04 00 00 	jne	1087 <ravi_vm_return>
     279:	4c 8b 65 20 	movq	32(%rbp), %r12
     27d:	85 c0 	testl	%eax, %eax
     27f:	74 09 	je	9 <ravi_BC_RETURN+0x77>
     281:	4d 8b 6c 24 08 	movq	8(%r12), %r13
     286:	4c 89 6d 10 	movq	%r13, 16(%rbp)
     28a:	e9 0b 04 00 00 	jmp	1035 <ravi_new_frame>

ravi_BC_FORLOOP:
     28f:	8b 03 	movl	(%rbx), %eax
     291:	0f b6 d0 	movzbl	%al, %edx
     294:	48 83 c3 04 	addq	$4, %rbx
     298:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP:
     29c:	8b 03 	movl	(%rbx), %eax
     29e:	0f b6 d0 	movzbl	%al, %edx
     2a1:	48 83 c3 04 	addq	$4, %rbx
     2a5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORCALL:
     2a9:	8b 03 	movl	(%rbx), %eax
     2ab:	0f b6 d0 	movzbl	%al, %edx
     2ae:	48 83 c3 04 	addq	$4, %rbx
     2b2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORLOOP:
     2b6:	8b 03 	movl	(%rbx), %eax
     2b8:	0f b6 d0 	movzbl	%al, %edx
     2bb:	48 83 c3 04 	addq	$4, %rbx
     2bf:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETLIST:
     2c3:	8b 03 	movl	(%rbx), %eax
     2c5:	0f b6 d0 	movzbl	%al, %edx
     2c8:	48 83 c3 04 	addq	$4, %rbx
     2cc:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CLOSURE:
     2d0:	8b 03 	movl	(%rbx), %eax
     2d2:	0f b6 d0 	movzbl	%al, %edx
     2d5:	48 83 c3 04 	addq	$4, %rbx
     2d9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_VARARG:
     2dd:	8b 03 	movl	(%rbx), %eax
     2df:	0f b6 d0 	movzbl	%al, %edx
     2e2:	48 83 c3 04 	addq	$4, %rbx
     2e6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EXTRAARG:
     2ea:	8b 03 	movl	(%rbx), %eax
     2ec:	0f b6 d0 	movzbl	%al, %edx
     2ef:	48 83 c3 04 	addq	$4, %rbx
     2f3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYI:
     2f7:	8b 03 	movl	(%rbx), %eax
     2f9:	0f b6 d0 	movzbl	%al, %edx
     2fc:	48 83 c3 04 	addq	$4, %rbx
     300:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYF:
     304:	8b 03 	movl	(%rbx), %eax
     306:	0f b6 d0 	movzbl	%al, %edx
     309:	48 83 c3 04 	addq	$4, %rbx
     30d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADIZ:
     311:	8b 03 	movl	(%rbx), %eax
     313:	0f b6 d0 	movzbl	%al, %edx
     316:	48 83 c3 04 	addq	$4, %rbx
     31a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADFZ:
     31e:	8b 03 	movl	(%rbx), %eax
     320:	0f b6 d0 	movzbl	%al, %edx
     323:	48 83 c3 04 	addq	$4, %rbx
     327:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMF:
     32b:	8b 03 	movl	(%rbx), %eax
     32d:	0f b6 d0 	movzbl	%al, %edx
     330:	48 83 c3 04 	addq	$4, %rbx
     334:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMI:
     338:	8b 03 	movl	(%rbx), %eax
     33a:	0f b6 d0 	movzbl	%al, %edx
     33d:	48 83 c3 04 	addq	$4, %rbx
     341:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFF:
     345:	8b 03 	movl	(%rbx), %eax
     347:	0f b6 d0 	movzbl	%al, %edx
     34a:	48 83 c3 04 	addq	$4, %rbx
     34e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFI:
     352:	8b 03 	movl	(%rbx), %eax
     354:	0f b6 d0 	movzbl	%al, %edx
     357:	48 83 c3 04 	addq	$4, %rbx
     35b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDII:
     35f:	8b 03 	movl	(%rbx), %eax
     361:	0f b6 d0 	movzbl	%al, %edx
     364:	48 83 c3 04 	addq	$4, %rbx
     368:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFF:
     36c:	8b 03 	movl	(%rbx), %eax
     36e:	0f b6 d0 	movzbl	%al, %edx
     371:	48 83 c3 04 	addq	$4, %rbx
     375:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFI:
     379:	8b 03 	movl	(%rbx), %eax
     37b:	0f b6 d0 	movzbl	%al, %edx
     37e:	48 83 c3 04 	addq	$4, %rbx
     382:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBIF:
     386:	8b 03 	movl	(%rbx), %eax
     388:	0f b6 d0 	movzbl	%al, %edx
     38b:	48 83 c3 04 	addq	$4, %rbx
     38f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBII:
     393:	8b 03 	movl	(%rbx), %eax
     395:	0f b6 d0 	movzbl	%al, %edx
     398:	48 83 c3 04 	addq	$4, %rbx
     39c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFF:
     3a0:	8b 03 	movl	(%rbx), %eax
     3a2:	0f b6 d0 	movzbl	%al, %edx
     3a5:	48 83 c3 04 	addq	$4, %rbx
     3a9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFI:
     3ad:	8b 03 	movl	(%rbx), %eax
     3af:	0f b6 d0 	movzbl	%al, %edx
     3b2:	48 83 c3 04 	addq	$4, %rbx
     3b6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULII:
     3ba:	8b 03 	movl	(%rbx), %eax
     3bc:	0f b6 d0 	movzbl	%al, %edx
     3bf:	48 83 c3 04 	addq	$4, %rbx
     3c3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFF:
     3c7:	8b 03 	movl	(%rbx), %eax
     3c9:	0f b6 d0 	movzbl	%al, %edx
     3cc:	48 83 c3 04 	addq	$4, %rbx
     3d0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFI:
     3d4:	8b 03 	movl	(%rbx), %eax
     3d6:	0f b6 d0 	movzbl	%al, %edx
     3d9:	48 83 c3 04 	addq	$4, %rbx
     3dd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVIF:
     3e1:	8b 03 	movl	(%rbx), %eax
     3e3:	0f b6 d0 	movzbl	%al, %edx
     3e6:	48 83 c3 04 	addq	$4, %rbx
     3ea:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVII:
     3ee:	8b 03 	movl	(%rbx), %eax
     3f0:	0f b6 d0 	movzbl	%al, %edx
     3f3:	48 83 c3 04 	addq	$4, %rbx
     3f7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOINT:
     3fb:	8b 03 	movl	(%rbx), %eax
     3fd:	0f b6 d0 	movzbl	%al, %edx
     400:	48 83 c3 04 	addq	$4, %rbx
     404:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOFLT:
     408:	8b 03 	movl	(%rbx), %eax
     40a:	0f b6 d0 	movzbl	%al, %edx
     40d:	48 83 c3 04 	addq	$4, %rbx
     411:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYI:
     415:	8b 03 	movl	(%rbx), %eax
     417:	0f b6 d0 	movzbl	%al, %edx
     41a:	48 83 c3 04 	addq	$4, %rbx
     41e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYF:
     422:	8b 03 	movl	(%rbx), %eax
     424:	0f b6 d0 	movzbl	%al, %edx
     427:	48 83 c3 04 	addq	$4, %rbx
     42b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTAB:
     42f:	8b 03 	movl	(%rbx), %eax
     431:	0f b6 d0 	movzbl	%al, %edx
     434:	48 83 c3 04 	addq	$4, %rbx
     438:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOSTRING:
     43c:	8b 03 	movl	(%rbx), %eax
     43e:	0f b6 d0 	movzbl	%al, %edx
     441:	48 83 c3 04 	addq	$4, %rbx
     445:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOCLOSURE:
     449:	8b 03 	movl	(%rbx), %eax
     44b:	0f b6 d0 	movzbl	%al, %edx
     44e:	48 83 c3 04 	addq	$4, %rbx
     452:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTYPE:
     456:	8b 03 	movl	(%rbx), %eax
     458:	0f b6 d0 	movzbl	%al, %edx
     45b:	48 83 c3 04 	addq	$4, %rbx
     45f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEI:
     463:	8b 03 	movl	(%rbx), %eax
     465:	0f b6 d0 	movzbl	%al, %edx
     468:	48 83 c3 04 	addq	$4, %rbx
     46c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEF:
     470:	8b 03 	movl	(%rbx), %eax
     472:	0f b6 d0 	movzbl	%al, %edx
     475:	48 83 c3 04 	addq	$4, %rbx
     479:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAI:
     47d:	8b 03 	movl	(%rbx), %eax
     47f:	0f b6 d0 	movzbl	%al, %edx
     482:	48 83 c3 04 	addq	$4, %rbx
     486:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAF:
     48a:	8b 03 	movl	(%rbx), %eax
     48c:	0f b6 d0 	movzbl	%al, %edx
     48f:	48 83 c3 04 	addq	$4, %rbx
     493:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVETAB:
     497:	8b 03 	movl	(%rbx), %eax
     499:	0f b6 d0 	movzbl	%al, %edx
     49c:	48 83 c3 04 	addq	$4, %rbx
     4a0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AI:
     4a4:	8b 03 	movl	(%rbx), %eax
     4a6:	0f b6 d0 	movzbl	%al, %edx
     4a9:	48 83 c3 04 	addq	$4, %rbx
     4ad:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AF:
     4b1:	8b 03 	movl	(%rbx), %eax
     4b3:	0f b6 d0 	movzbl	%al, %edx
     4b6:	48 83 c3 04 	addq	$4, %rbx
     4ba:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AI:
     4be:	8b 03 	movl	(%rbx), %eax
     4c0:	0f b6 d0 	movzbl	%al, %edx
     4c3:	48 83 c3 04 	addq	$4, %rbx
     4c7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AF:
     4cb:	8b 03 	movl	(%rbx), %eax
     4cd:	0f b6 d0 	movzbl	%al, %edx
     4d0:	48 83 c3 04 	addq	$4, %rbx
     4d4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AII:
     4d8:	8b 03 	movl	(%rbx), %eax
     4da:	0f b6 d0 	movzbl	%al, %edx
     4dd:	48 83 c3 04 	addq	$4, %rbx
     4e1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AFF:
     4e5:	8b 03 	movl	(%rbx), %eax
     4e7:	0f b6 d0 	movzbl	%al, %edx
     4ea:	48 83 c3 04 	addq	$4, %rbx
     4ee:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_IP:
     4f2:	8b 03 	movl	(%rbx), %eax
     4f4:	0f b6 d0 	movzbl	%al, %edx
     4f7:	48 83 c3 04 	addq	$4, %rbx
     4fb:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_I1:
     4ff:	8b 03 	movl	(%rbx), %eax
     501:	0f b6 d0 	movzbl	%al, %edx
     504:	48 83 c3 04 	addq	$4, %rbx
     508:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_IP:
     50c:	8b 03 	movl	(%rbx), %eax
     50e:	0f b6 d0 	movzbl	%al, %edx
     511:	48 83 c3 04 	addq	$4, %rbx
     515:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_I1:
     519:	8b 03 	movl	(%rbx), %eax
     51b:	0f b6 d0 	movzbl	%al, %edx
     51e:	48 83 c3 04 	addq	$4, %rbx
     522:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALI:
     526:	8b 03 	movl	(%rbx), %eax
     528:	0f b6 d0 	movzbl	%al, %edx
     52b:	48 83 c3 04 	addq	$4, %rbx
     52f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALF:
     533:	8b 03 	movl	(%rbx), %eax
     535:	0f b6 d0 	movzbl	%al, %edx
     538:	48 83 c3 04 	addq	$4, %rbx
     53c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAI:
     540:	8b 03 	movl	(%rbx), %eax
     542:	0f b6 d0 	movzbl	%al, %edx
     545:	48 83 c3 04 	addq	$4, %rbx
     549:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAF:
     54d:	8b 03 	movl	(%rbx), %eax
     54f:	0f b6 d0 	movzbl	%al, %edx
     552:	48 83 c3 04 	addq	$4, %rbx
     556:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALT:
     55a:	8b 03 	movl	(%rbx), %eax
     55c:	0f b6 d0 	movzbl	%al, %edx
     55f:	48 83 c3 04 	addq	$4, %rbx
     563:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BAND_II:
     567:	8b 03 	movl	(%rbx), %eax
     569:	0f b6 d0 	movzbl	%al, %edx
     56c:	48 83 c3 04 	addq	$4, %rbx
     570:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BOR_II:
     574:	8b 03 	movl	(%rbx), %eax
     576:	0f b6 d0 	movzbl	%al, %edx
     579:	48 83 c3 04 	addq	$4, %rbx
     57d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BXOR_II:
     581:	8b 03 	movl	(%rbx), %eax
     583:	0f b6 d0 	movzbl	%al, %edx
     586:	48 83 c3 04 	addq	$4, %rbx
     58a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHL_II:
     58e:	8b 03 	movl	(%rbx), %eax
     590:	0f b6 d0 	movzbl	%al, %edx
     593:	48 83 c3 04 	addq	$4, %rbx
     597:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHR_II:
     59b:	8b 03 	movl	(%rbx), %eax
     59d:	0f b6 d0 	movzbl	%al, %edx
     5a0:	48 83 c3 04 	addq	$4, %rbx
     5a4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BNOT_I:
     5a8:	8b 03 	movl	(%rbx), %eax
     5aa:	0f b6 d0 	movzbl	%al, %edx
     5ad:	48 83 c3 04 	addq	$4, %rbx
     5b1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_II:
     5b5:	8b 03 	movl	(%rbx), %eax
     5b7:	0f b6 d0 	movzbl	%al, %edx
     5ba:	48 83 c3 04 	addq	$4, %rbx
     5be:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_FF:
     5c2:	8b 03 	movl	(%rbx), %eax
     5c4:	0f b6 d0 	movzbl	%al, %edx
     5c7:	48 83 c3 04 	addq	$4, %rbx
     5cb:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_II:
     5cf:	8b 03 	movl	(%rbx), %eax
     5d1:	0f b6 d0 	movzbl	%al, %edx
     5d4:	48 83 c3 04 	addq	$4, %rbx
     5d8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_FF:
     5dc:	8b 03 	movl	(%rbx), %eax
     5de:	0f b6 d0 	movzbl	%al, %edx
     5e1:	48 83 c3 04 	addq	$4, %rbx
     5e5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_II:
     5e9:	8b 03 	movl	(%rbx), %eax
     5eb:	0f b6 d0 	movzbl	%al, %edx
     5ee:	48 83 c3 04 	addq	$4, %rbx
     5f2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_FF:
     5f6:	8b 03 	movl	(%rbx), %eax
     5f8:	0f b6 d0 	movzbl	%al, %edx
     5fb:	48 83 c3 04 	addq	$4, %rbx
     5ff:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_S:
     603:	8b 03 	movl	(%rbx), %eax
     605:	0f b6 d0 	movzbl	%al, %edx
     608:	48 83 c3 04 	addq	$4, %rbx
     60c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_S:
     610:	8b 03 	movl	(%rbx), %eax
     612:	0f b6 d0 	movzbl	%al, %edx
     615:	48 83 c3 04 	addq	$4, %rbx
     619:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_S:
     61d:	8b 03 	movl	(%rbx), %eax
     61f:	0f b6 d0 	movzbl	%al, %edx
     622:	48 83 c3 04 	addq	$4, %rbx
     626:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_I:
     62a:	8b 03 	movl	(%rbx), %eax
     62c:	0f b6 d0 	movzbl	%al, %edx
     62f:	48 83 c3 04 	addq	$4, %rbx
     633:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_I:
     637:	8b 03 	movl	(%rbx), %eax
     639:	0f b6 d0 	movzbl	%al, %edx
     63c:	48 83 c3 04 	addq	$4, %rbx
     640:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_SK:
     644:	8b 03 	movl	(%rbx), %eax
     646:	0f b6 d0 	movzbl	%al, %edx
     649:	48 83 c3 04 	addq	$4, %rbx
     64d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_SK:
     651:	8b 03 	movl	(%rbx), %eax
     653:	0f b6 d0 	movzbl	%al, %edx
     656:	48 83 c3 04 	addq	$4, %rbx
     65a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_SK:
     65e:	8b 03 	movl	(%rbx), %eax
     660:	0f b6 d0 	movzbl	%al, %edx
     663:	48 83 c3 04 	addq	$4, %rbx
     667:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABUP_SK:
     66b:	8b 03 	movl	(%rbx), %eax
     66d:	0f b6 d0 	movzbl	%al, %edx
     670:	48 83 c3 04 	addq	$4, %rbx
     674:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_luaV_interp:
     678:	55 	pushq	%rbp
     679:	57 	pushq	%rdi
     67a:	56 	pushq	%rsi
     67b:	53 	pushq	%rbx
     67c:	41 54 	pushq	%r12
     67e:	41 55 	pushq	%r13
     680:	41 56 	pushq	%r14
     682:	41 57 	pushq	%r15
     684:	48 83 ec 28 	subq	$40, %rsp
     688:	48 89 cd 	movq	%rcx, %rbp
     68b:	4c 8b 75 18 	movq	24(%rbp), %r14
     68f:	49 81 c6 40 05 00 00 	addq	$1344, %r14
     696:	4c 8b 65 20 	movq	32(%rbp), %r12

ravi_new_frame:
     69a:	49 8b 34 24 	movq	(%r12), %rsi
     69e:	4c 8b 2e 	movq	(%rsi), %r13
     6a1:	4d 8b 44 24 20 	movq	32(%r12), %r8
     6a6:	49 8b 5c 24 28 	movq	40(%r12), %rbx
     6ab:	8b 03 	movl	(%rbx), %eax
     6ad:	0f b6 d0 	movzbl	%al, %edx
     6b0:	48 83 c3 04 	addq	$4, %rbx
     6b4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_vm_return:
     6b8:	48 83 c4 28 	addq	$40, %rsp
     6bc:	41 5f 	popq	%r15
     6be:	41 5e 	popq	%r14
     6c0:	41 5d 	popq	%r13
     6c2:	41 5c 	popq	%r12
     6c4:	5b 	popq	%rbx
     6c5:	5e 	popq	%rsi
     6c6:	5f 	popq	%rdi
     6c7:	5d 	popq	%rbp
     6c8:	c3 	retq
SYMBOL TABLE:
[ 0](sec -1)(fl 0x00)(ty   0)(scl   3) (nx 0) 0x00000001 @feat.00
[ 1](sec  1)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .text
AUX scnlen 0x6c9 nreloc 2 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 3](sec  0)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 luaF_close
[ 4](sec  0)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 luaD_poscall
[ 5](sec  2)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .pdata
AUX scnlen 0xc nreloc 3 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 7](sec  3)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .xdata
AUX scnlen 0x18 nreloc 0 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 9](sec  1)(fl 0x00)(ty   0)(scl   2) (nx 0) 0x00000000 ravi_vm_asm_begin
[10](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 ravi_BC_MOVE
[11](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000000d ravi_BC_LOADK
[12](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000003f ravi_BC_LOADKX
[13](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000004c ravi_BC_LOADBOOL
[14](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000059 ravi_BC_LOADNIL
[15](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000066 ravi_BC_GETUPVAL
[16](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000073 ravi_BC_GETTABUP
[17](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000080 ravi_BC_GETTABLE
[18](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000008d ravi_BC_SETTABUP
[19](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000009a ravi_BC_SETUPVAL
[20](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000a7 ravi_BC_SETTABLE
[21](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000b4 ravi_BC_NEWTABLE
[22](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000c1 ravi_BC_SELF
[23](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000ce ravi_BC_ADD
[24](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000db ravi_BC_SUB
[25](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000e8 ravi_BC_MUL
[26](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000f5 ravi_BC_MOD
[27](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000102 ravi_BC_POW
[28](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000010f ravi_BC_DIV
[29](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000011c ravi_BC_IDIV
[30](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000129 ravi_BC_BAND
[31](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000136 ravi_BC_BOR
[32](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000143 ravi_BC_BXOR
[33](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000150 ravi_BC_SHL
[34](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000015d ravi_BC_SHR
[35](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000016a ravi_BC_UNM
[36](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000177 ravi_BC_BNOT
[37](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000184 ravi_BC_NOT
[38](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000191 ravi_BC_LEN
[39](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000019e ravi_BC_CONCAT
[40](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001ab ravi_BC_JMP
[41](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001b8 ravi_BC_EQ
[42](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001c5 ravi_BC_LT
[43](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001d2 ravi_BC_LE
[44](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001df ravi_BC_TEST
[45](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001ec ravi_BC_TESTSET
[46](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001f9 ravi_BC_CALL
[47](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000206 ravi_BC_TAILCALL
[48](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000213 ravi_BC_RETURN
[49](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000028f ravi_BC_FORLOOP
[50](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000029c ravi_BC_FORPREP
[51](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002a9 ravi_BC_TFORCALL
[52](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002b6 ravi_BC_TFORLOOP
[53](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002c3 ravi_BC_SETLIST
[54](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002d0 ravi_BC_CLOSURE
[55](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002dd ravi_BC_VARARG
[56](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002ea ravi_BC_EXTRAARG
[57](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002f7 ravi_BC_NEWARRAYI
[58](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000304 ravi_BC_NEWARRAYF
[59](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000311 ravi_BC_LOADIZ
[60](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000031e ravi_BC_LOADFZ
[61](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000032b ravi_BC_UNMF
[62](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000338 ravi_BC_UNMI
[63](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000345 ravi_BC_ADDFF
[64](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000352 ravi_BC_ADDFI
[65](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000035f ravi_BC_ADDII
[66](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000036c ravi_BC_SUBFF
[67](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000379 ravi_BC_SUBFI
[68](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000386 ravi_BC_SUBIF
[69](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000393 ravi_BC_SUBII
[70](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003a0 ravi_BC_MULFF
[71](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003ad ravi_BC_MULFI
[72](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003ba ravi_BC_MULII
[73](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003c7 ravi_BC_DIVFF
[74](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003d4 ravi_BC_DIVFI
[75](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003e1 ravi_BC_DIVIF
[76](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003ee ravi_BC_DIVII
[77](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003fb ravi_BC_TOINT
[78](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000408 ravi_BC_TOFLT
[79](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000415 ravi_BC_TOARRAYI
[80](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000422 ravi_BC_TOARRAYF
[81](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000042f ravi_BC_TOTAB
[82](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000043c ravi_BC_TOSTRING
[83](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000449 ravi_BC_TOCLOSURE
[84](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000456 ravi_BC_TOTYPE
[85](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000463 ravi_BC_MOVEI
[86](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000470 ravi_BC_MOVEF
[87](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000047d ravi_BC_MOVEAI
[88](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000048a ravi_BC_MOVEAF
[89](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000497 ravi_BC_MOVETAB
[90](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004a4 ravi_BC_GETTABLE_AI
[91](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004b1 ravi_BC_GETTABLE_AF
[92](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004be ravi_BC_SETTABLE_AI
[93](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004cb ravi_BC_SETTABLE_AF
[94](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004d8 ravi_BC_SETTABLE_AII
[95](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004e5 ravi_BC_SETTABLE_AFF
[96](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004f2 ravi_BC_FORLOOP_IP
[97](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004ff ravi_BC_FORLOOP_I1
[98](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000050c ravi_BC_FORPREP_IP
[99](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000519 ravi_BC_FORPREP_I1
[100](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000526 ravi_BC_SETUPVALI
[101](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000533 ravi_BC_SETUPVALF
[102](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000540 ravi_BC_SETUPVALAI
[103](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000054d ravi_BC_SETUPVALAF
[104](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000055a ravi_BC_SETUPVALT
[105](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000567 ravi_BC_BAND_II
[106](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000574 ravi_BC_BOR_II
[107](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000581 ravi_BC_BXOR_II
[108](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000058e ravi_BC_SHL_II
[109](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000059b ravi_BC_SHR_II
[110](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005a8 ravi_BC_BNOT_I
[111](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005b5 ravi_BC_EQ_II
[112](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005c2 ravi_BC_EQ_FF
[113](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005cf ravi_BC_LT_II
[114](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005dc ravi_BC_LT_FF
[115](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005e9 ravi_BC_LE_II
[116](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005f6 ravi_BC_LE_FF
[117](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000603 ravi_BC_GETTABLE_S
[118](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000610 ravi_BC_SETTABLE_S
[119](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000061d ravi_BC_SELF_S
[120](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000062a ravi_BC_GETTABLE_I
[121](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000637 ravi_BC_SETTABLE_I
[122](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000644 ravi_BC_GETTABLE_SK
[123](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000651 ravi_BC_SELF_SK
[124](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000065e ravi_BC_SETTABLE_SK
[125](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000066b ravi_BC_GETTABUP_SK
[126](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000678 ravi_luaV_interp
[127](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000069a ravi_new_frame
[128](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000006b8 ravi_vm_return
[129](sec  4)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .rdata$Z
AUX scnlen 0xd nreloc 0 nlnno 0 checksum 0x0 assoc 0 comdat 0
Unwind info:

Function Table:
  Start Address: ravi_vm_asm_begin
  End Address: ravi_vm_asm_begin + 0x06c9
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

