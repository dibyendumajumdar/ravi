
vm.obj:	file format COFF-x86-64

Disassembly of section .text:
ravi_vm_asm_begin:
       0:	8b 03 	movl	(%rbx), %eax
       2:	0f b6 d0 	movzbl	%al, %edx
       5:	48 83 c3 08 	addq	$8, %rbx
       9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADK:
       d:	8b 03 	movl	(%rbx), %eax
       f:	0f b6 d0 	movzbl	%al, %edx
      12:	48 83 c3 08 	addq	$8, %rbx
      16:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADKX:
      1a:	8b 03 	movl	(%rbx), %eax
      1c:	0f b6 d0 	movzbl	%al, %edx
      1f:	48 83 c3 08 	addq	$8, %rbx
      23:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADBOOL:
      27:	8b 03 	movl	(%rbx), %eax
      29:	0f b6 d0 	movzbl	%al, %edx
      2c:	48 83 c3 08 	addq	$8, %rbx
      30:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADNIL:
      34:	8b 03 	movl	(%rbx), %eax
      36:	0f b6 d0 	movzbl	%al, %edx
      39:	48 83 c3 08 	addq	$8, %rbx
      3d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETUPVAL:
      41:	8b 03 	movl	(%rbx), %eax
      43:	0f b6 d0 	movzbl	%al, %edx
      46:	48 83 c3 08 	addq	$8, %rbx
      4a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABUP:
      4e:	8b 03 	movl	(%rbx), %eax
      50:	0f b6 d0 	movzbl	%al, %edx
      53:	48 83 c3 08 	addq	$8, %rbx
      57:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE:
      5b:	8b 03 	movl	(%rbx), %eax
      5d:	0f b6 d0 	movzbl	%al, %edx
      60:	48 83 c3 08 	addq	$8, %rbx
      64:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABUP:
      68:	8b 03 	movl	(%rbx), %eax
      6a:	0f b6 d0 	movzbl	%al, %edx
      6d:	48 83 c3 08 	addq	$8, %rbx
      71:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVAL:
      75:	8b 03 	movl	(%rbx), %eax
      77:	0f b6 d0 	movzbl	%al, %edx
      7a:	48 83 c3 08 	addq	$8, %rbx
      7e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE:
      82:	8b 03 	movl	(%rbx), %eax
      84:	0f b6 d0 	movzbl	%al, %edx
      87:	48 83 c3 08 	addq	$8, %rbx
      8b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWTABLE:
      8f:	8b 03 	movl	(%rbx), %eax
      91:	0f b6 d0 	movzbl	%al, %edx
      94:	48 83 c3 08 	addq	$8, %rbx
      98:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF:
      9c:	8b 03 	movl	(%rbx), %eax
      9e:	0f b6 d0 	movzbl	%al, %edx
      a1:	48 83 c3 08 	addq	$8, %rbx
      a5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADD:
      a9:	8b 03 	movl	(%rbx), %eax
      ab:	0f b6 d0 	movzbl	%al, %edx
      ae:	48 83 c3 08 	addq	$8, %rbx
      b2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUB:
      b6:	8b 03 	movl	(%rbx), %eax
      b8:	0f b6 d0 	movzbl	%al, %edx
      bb:	48 83 c3 08 	addq	$8, %rbx
      bf:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MUL:
      c3:	8b 03 	movl	(%rbx), %eax
      c5:	0f b6 d0 	movzbl	%al, %edx
      c8:	48 83 c3 08 	addq	$8, %rbx
      cc:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOD:
      d0:	8b 03 	movl	(%rbx), %eax
      d2:	0f b6 d0 	movzbl	%al, %edx
      d5:	48 83 c3 08 	addq	$8, %rbx
      d9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_POW:
      dd:	8b 03 	movl	(%rbx), %eax
      df:	0f b6 d0 	movzbl	%al, %edx
      e2:	48 83 c3 08 	addq	$8, %rbx
      e6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIV:
      ea:	8b 03 	movl	(%rbx), %eax
      ec:	0f b6 d0 	movzbl	%al, %edx
      ef:	48 83 c3 08 	addq	$8, %rbx
      f3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_IDIV:
      f7:	8b 03 	movl	(%rbx), %eax
      f9:	0f b6 d0 	movzbl	%al, %edx
      fc:	48 83 c3 08 	addq	$8, %rbx
     100:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BAND:
     104:	8b 03 	movl	(%rbx), %eax
     106:	0f b6 d0 	movzbl	%al, %edx
     109:	48 83 c3 08 	addq	$8, %rbx
     10d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BOR:
     111:	8b 03 	movl	(%rbx), %eax
     113:	0f b6 d0 	movzbl	%al, %edx
     116:	48 83 c3 08 	addq	$8, %rbx
     11a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BXOR:
     11e:	8b 03 	movl	(%rbx), %eax
     120:	0f b6 d0 	movzbl	%al, %edx
     123:	48 83 c3 08 	addq	$8, %rbx
     127:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHL:
     12b:	8b 03 	movl	(%rbx), %eax
     12d:	0f b6 d0 	movzbl	%al, %edx
     130:	48 83 c3 08 	addq	$8, %rbx
     134:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHR:
     138:	8b 03 	movl	(%rbx), %eax
     13a:	0f b6 d0 	movzbl	%al, %edx
     13d:	48 83 c3 08 	addq	$8, %rbx
     141:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNM:
     145:	8b 03 	movl	(%rbx), %eax
     147:	0f b6 d0 	movzbl	%al, %edx
     14a:	48 83 c3 08 	addq	$8, %rbx
     14e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BNOT:
     152:	8b 03 	movl	(%rbx), %eax
     154:	0f b6 d0 	movzbl	%al, %edx
     157:	48 83 c3 08 	addq	$8, %rbx
     15b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NOT:
     15f:	8b 03 	movl	(%rbx), %eax
     161:	0f b6 d0 	movzbl	%al, %edx
     164:	48 83 c3 08 	addq	$8, %rbx
     168:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LEN:
     16c:	8b 03 	movl	(%rbx), %eax
     16e:	0f b6 d0 	movzbl	%al, %edx
     171:	48 83 c3 08 	addq	$8, %rbx
     175:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CONCAT:
     179:	8b 03 	movl	(%rbx), %eax
     17b:	0f b6 d0 	movzbl	%al, %edx
     17e:	48 83 c3 08 	addq	$8, %rbx
     182:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_JMP:
     186:	8b 03 	movl	(%rbx), %eax
     188:	0f b6 d0 	movzbl	%al, %edx
     18b:	48 83 c3 08 	addq	$8, %rbx
     18f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ:
     193:	8b 03 	movl	(%rbx), %eax
     195:	0f b6 d0 	movzbl	%al, %edx
     198:	48 83 c3 08 	addq	$8, %rbx
     19c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT:
     1a0:	8b 03 	movl	(%rbx), %eax
     1a2:	0f b6 d0 	movzbl	%al, %edx
     1a5:	48 83 c3 08 	addq	$8, %rbx
     1a9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE:
     1ad:	8b 03 	movl	(%rbx), %eax
     1af:	0f b6 d0 	movzbl	%al, %edx
     1b2:	48 83 c3 08 	addq	$8, %rbx
     1b6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TEST:
     1ba:	8b 03 	movl	(%rbx), %eax
     1bc:	0f b6 d0 	movzbl	%al, %edx
     1bf:	48 83 c3 08 	addq	$8, %rbx
     1c3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TESTSET:
     1c7:	8b 03 	movl	(%rbx), %eax
     1c9:	0f b6 d0 	movzbl	%al, %edx
     1cc:	48 83 c3 08 	addq	$8, %rbx
     1d0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CALL:
     1d4:	8b 03 	movl	(%rbx), %eax
     1d6:	0f b6 d0 	movzbl	%al, %edx
     1d9:	48 83 c3 08 	addq	$8, %rbx
     1dd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TAILCALL:
     1e1:	8b 03 	movl	(%rbx), %eax
     1e3:	0f b6 d0 	movzbl	%al, %edx
     1e6:	48 83 c3 08 	addq	$8, %rbx
     1ea:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_RETURN:
     1ee:	49 89 c7 	movq	%rax, %r15
     1f1:	49 8b 75 18 	movq	24(%r13), %rsi
     1f5:	83 7e 20 00 	cmpl	$0, 32(%rsi)
     1f9:	74 11 	je	17 <ravi_BC_RETURN+0x1E>
     1fb:	4d 89 c5 	movq	%r8, %r13
     1fe:	48 89 e9 	movq	%rbp, %rcx
     201:	4c 89 c2 	movq	%r8, %rdx
     204:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x1B>
     209:	4d 89 e8 	movq	%r13, %r8
     20c:	49 89 5c 24 28 	movq	%rbx, 40(%r12)
     211:	4c 89 f8 	movq	%r15, %rax
     214:	0f b6 cc 	movzbl	%ah, %ecx
     217:	c1 e8 10 	shrl	$16, %eax
     21a:	0f b6 d4 	movzbl	%ah, %edx
     21d:	4d 8b 04 c8 	movq	(%r8,%rcx,8), %r8
     221:	85 d2 	testl	%edx, %edx
     223:	74 08 	je	8 <ravi_BC_RETURN+0x3F>
     225:	ff ca 	decl	%edx
     227:	4c 0f b7 ca 	movzwq	%dx, %r9
     22b:	eb 0b 	jmp	11 <ravi_BC_RETURN+0x4A>
     22d:	4c 8b 4d 10 	movq	16(%rbp), %r9
     231:	4d 29 c1 	subq	%r8, %r9
     234:	49 c1 e9 04 	shrq	$4, %r9
     238:	48 89 e9 	movq	%rbp, %rcx
     23b:	4c 89 e2 	movq	%r12, %rdx
     23e:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x55>
     243:	41 f7 44 24 42 08 00 00 00 	testl	$8, 66(%r12)
     24c:	0f 85 3f 04 00 00 	jne	1087 <ravi_vm_return>
     252:	4c 8b 65 20 	movq	32(%rbp), %r12
     256:	85 c0 	testl	%eax, %eax
     258:	74 09 	je	9 <ravi_BC_RETURN+0x75>
     25a:	4d 8b 6c 24 08 	movq	8(%r12), %r13
     25f:	4c 89 6d 10 	movq	%r13, 16(%rbp)
     263:	e9 0b 04 00 00 	jmp	1035 <ravi_new_frame>

ravi_BC_FORLOOP:
     268:	8b 03 	movl	(%rbx), %eax
     26a:	0f b6 d0 	movzbl	%al, %edx
     26d:	48 83 c3 08 	addq	$8, %rbx
     271:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP:
     275:	8b 03 	movl	(%rbx), %eax
     277:	0f b6 d0 	movzbl	%al, %edx
     27a:	48 83 c3 08 	addq	$8, %rbx
     27e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORCALL:
     282:	8b 03 	movl	(%rbx), %eax
     284:	0f b6 d0 	movzbl	%al, %edx
     287:	48 83 c3 08 	addq	$8, %rbx
     28b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORLOOP:
     28f:	8b 03 	movl	(%rbx), %eax
     291:	0f b6 d0 	movzbl	%al, %edx
     294:	48 83 c3 08 	addq	$8, %rbx
     298:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETLIST:
     29c:	8b 03 	movl	(%rbx), %eax
     29e:	0f b6 d0 	movzbl	%al, %edx
     2a1:	48 83 c3 08 	addq	$8, %rbx
     2a5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CLOSURE:
     2a9:	8b 03 	movl	(%rbx), %eax
     2ab:	0f b6 d0 	movzbl	%al, %edx
     2ae:	48 83 c3 08 	addq	$8, %rbx
     2b2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_VARARG:
     2b6:	8b 03 	movl	(%rbx), %eax
     2b8:	0f b6 d0 	movzbl	%al, %edx
     2bb:	48 83 c3 08 	addq	$8, %rbx
     2bf:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EXTRAARG:
     2c3:	8b 03 	movl	(%rbx), %eax
     2c5:	0f b6 d0 	movzbl	%al, %edx
     2c8:	48 83 c3 08 	addq	$8, %rbx
     2cc:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYI:
     2d0:	8b 03 	movl	(%rbx), %eax
     2d2:	0f b6 d0 	movzbl	%al, %edx
     2d5:	48 83 c3 08 	addq	$8, %rbx
     2d9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYF:
     2dd:	8b 03 	movl	(%rbx), %eax
     2df:	0f b6 d0 	movzbl	%al, %edx
     2e2:	48 83 c3 08 	addq	$8, %rbx
     2e6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADIZ:
     2ea:	8b 03 	movl	(%rbx), %eax
     2ec:	0f b6 d0 	movzbl	%al, %edx
     2ef:	48 83 c3 08 	addq	$8, %rbx
     2f3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADFZ:
     2f7:	8b 03 	movl	(%rbx), %eax
     2f9:	0f b6 d0 	movzbl	%al, %edx
     2fc:	48 83 c3 08 	addq	$8, %rbx
     300:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMF:
     304:	8b 03 	movl	(%rbx), %eax
     306:	0f b6 d0 	movzbl	%al, %edx
     309:	48 83 c3 08 	addq	$8, %rbx
     30d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMI:
     311:	8b 03 	movl	(%rbx), %eax
     313:	0f b6 d0 	movzbl	%al, %edx
     316:	48 83 c3 08 	addq	$8, %rbx
     31a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFF:
     31e:	8b 03 	movl	(%rbx), %eax
     320:	0f b6 d0 	movzbl	%al, %edx
     323:	48 83 c3 08 	addq	$8, %rbx
     327:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFI:
     32b:	8b 03 	movl	(%rbx), %eax
     32d:	0f b6 d0 	movzbl	%al, %edx
     330:	48 83 c3 08 	addq	$8, %rbx
     334:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDII:
     338:	8b 03 	movl	(%rbx), %eax
     33a:	0f b6 d0 	movzbl	%al, %edx
     33d:	48 83 c3 08 	addq	$8, %rbx
     341:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFF:
     345:	8b 03 	movl	(%rbx), %eax
     347:	0f b6 d0 	movzbl	%al, %edx
     34a:	48 83 c3 08 	addq	$8, %rbx
     34e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFI:
     352:	8b 03 	movl	(%rbx), %eax
     354:	0f b6 d0 	movzbl	%al, %edx
     357:	48 83 c3 08 	addq	$8, %rbx
     35b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBIF:
     35f:	8b 03 	movl	(%rbx), %eax
     361:	0f b6 d0 	movzbl	%al, %edx
     364:	48 83 c3 08 	addq	$8, %rbx
     368:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBII:
     36c:	8b 03 	movl	(%rbx), %eax
     36e:	0f b6 d0 	movzbl	%al, %edx
     371:	48 83 c3 08 	addq	$8, %rbx
     375:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFF:
     379:	8b 03 	movl	(%rbx), %eax
     37b:	0f b6 d0 	movzbl	%al, %edx
     37e:	48 83 c3 08 	addq	$8, %rbx
     382:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFI:
     386:	8b 03 	movl	(%rbx), %eax
     388:	0f b6 d0 	movzbl	%al, %edx
     38b:	48 83 c3 08 	addq	$8, %rbx
     38f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULII:
     393:	8b 03 	movl	(%rbx), %eax
     395:	0f b6 d0 	movzbl	%al, %edx
     398:	48 83 c3 08 	addq	$8, %rbx
     39c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFF:
     3a0:	8b 03 	movl	(%rbx), %eax
     3a2:	0f b6 d0 	movzbl	%al, %edx
     3a5:	48 83 c3 08 	addq	$8, %rbx
     3a9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFI:
     3ad:	8b 03 	movl	(%rbx), %eax
     3af:	0f b6 d0 	movzbl	%al, %edx
     3b2:	48 83 c3 08 	addq	$8, %rbx
     3b6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVIF:
     3ba:	8b 03 	movl	(%rbx), %eax
     3bc:	0f b6 d0 	movzbl	%al, %edx
     3bf:	48 83 c3 08 	addq	$8, %rbx
     3c3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVII:
     3c7:	8b 03 	movl	(%rbx), %eax
     3c9:	0f b6 d0 	movzbl	%al, %edx
     3cc:	48 83 c3 08 	addq	$8, %rbx
     3d0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOINT:
     3d4:	8b 03 	movl	(%rbx), %eax
     3d6:	0f b6 d0 	movzbl	%al, %edx
     3d9:	48 83 c3 08 	addq	$8, %rbx
     3dd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOFLT:
     3e1:	8b 03 	movl	(%rbx), %eax
     3e3:	0f b6 d0 	movzbl	%al, %edx
     3e6:	48 83 c3 08 	addq	$8, %rbx
     3ea:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYI:
     3ee:	8b 03 	movl	(%rbx), %eax
     3f0:	0f b6 d0 	movzbl	%al, %edx
     3f3:	48 83 c3 08 	addq	$8, %rbx
     3f7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYF:
     3fb:	8b 03 	movl	(%rbx), %eax
     3fd:	0f b6 d0 	movzbl	%al, %edx
     400:	48 83 c3 08 	addq	$8, %rbx
     404:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTAB:
     408:	8b 03 	movl	(%rbx), %eax
     40a:	0f b6 d0 	movzbl	%al, %edx
     40d:	48 83 c3 08 	addq	$8, %rbx
     411:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOSTRING:
     415:	8b 03 	movl	(%rbx), %eax
     417:	0f b6 d0 	movzbl	%al, %edx
     41a:	48 83 c3 08 	addq	$8, %rbx
     41e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOCLOSURE:
     422:	8b 03 	movl	(%rbx), %eax
     424:	0f b6 d0 	movzbl	%al, %edx
     427:	48 83 c3 08 	addq	$8, %rbx
     42b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTYPE:
     42f:	8b 03 	movl	(%rbx), %eax
     431:	0f b6 d0 	movzbl	%al, %edx
     434:	48 83 c3 08 	addq	$8, %rbx
     438:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEI:
     43c:	8b 03 	movl	(%rbx), %eax
     43e:	0f b6 d0 	movzbl	%al, %edx
     441:	48 83 c3 08 	addq	$8, %rbx
     445:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEF:
     449:	8b 03 	movl	(%rbx), %eax
     44b:	0f b6 d0 	movzbl	%al, %edx
     44e:	48 83 c3 08 	addq	$8, %rbx
     452:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAI:
     456:	8b 03 	movl	(%rbx), %eax
     458:	0f b6 d0 	movzbl	%al, %edx
     45b:	48 83 c3 08 	addq	$8, %rbx
     45f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAF:
     463:	8b 03 	movl	(%rbx), %eax
     465:	0f b6 d0 	movzbl	%al, %edx
     468:	48 83 c3 08 	addq	$8, %rbx
     46c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVETAB:
     470:	8b 03 	movl	(%rbx), %eax
     472:	0f b6 d0 	movzbl	%al, %edx
     475:	48 83 c3 08 	addq	$8, %rbx
     479:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AI:
     47d:	8b 03 	movl	(%rbx), %eax
     47f:	0f b6 d0 	movzbl	%al, %edx
     482:	48 83 c3 08 	addq	$8, %rbx
     486:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AF:
     48a:	8b 03 	movl	(%rbx), %eax
     48c:	0f b6 d0 	movzbl	%al, %edx
     48f:	48 83 c3 08 	addq	$8, %rbx
     493:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AI:
     497:	8b 03 	movl	(%rbx), %eax
     499:	0f b6 d0 	movzbl	%al, %edx
     49c:	48 83 c3 08 	addq	$8, %rbx
     4a0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AF:
     4a4:	8b 03 	movl	(%rbx), %eax
     4a6:	0f b6 d0 	movzbl	%al, %edx
     4a9:	48 83 c3 08 	addq	$8, %rbx
     4ad:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AII:
     4b1:	8b 03 	movl	(%rbx), %eax
     4b3:	0f b6 d0 	movzbl	%al, %edx
     4b6:	48 83 c3 08 	addq	$8, %rbx
     4ba:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AFF:
     4be:	8b 03 	movl	(%rbx), %eax
     4c0:	0f b6 d0 	movzbl	%al, %edx
     4c3:	48 83 c3 08 	addq	$8, %rbx
     4c7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_IP:
     4cb:	8b 03 	movl	(%rbx), %eax
     4cd:	0f b6 d0 	movzbl	%al, %edx
     4d0:	48 83 c3 08 	addq	$8, %rbx
     4d4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_I1:
     4d8:	8b 03 	movl	(%rbx), %eax
     4da:	0f b6 d0 	movzbl	%al, %edx
     4dd:	48 83 c3 08 	addq	$8, %rbx
     4e1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_IP:
     4e5:	8b 03 	movl	(%rbx), %eax
     4e7:	0f b6 d0 	movzbl	%al, %edx
     4ea:	48 83 c3 08 	addq	$8, %rbx
     4ee:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_I1:
     4f2:	8b 03 	movl	(%rbx), %eax
     4f4:	0f b6 d0 	movzbl	%al, %edx
     4f7:	48 83 c3 08 	addq	$8, %rbx
     4fb:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALI:
     4ff:	8b 03 	movl	(%rbx), %eax
     501:	0f b6 d0 	movzbl	%al, %edx
     504:	48 83 c3 08 	addq	$8, %rbx
     508:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALF:
     50c:	8b 03 	movl	(%rbx), %eax
     50e:	0f b6 d0 	movzbl	%al, %edx
     511:	48 83 c3 08 	addq	$8, %rbx
     515:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAI:
     519:	8b 03 	movl	(%rbx), %eax
     51b:	0f b6 d0 	movzbl	%al, %edx
     51e:	48 83 c3 08 	addq	$8, %rbx
     522:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAF:
     526:	8b 03 	movl	(%rbx), %eax
     528:	0f b6 d0 	movzbl	%al, %edx
     52b:	48 83 c3 08 	addq	$8, %rbx
     52f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALT:
     533:	8b 03 	movl	(%rbx), %eax
     535:	0f b6 d0 	movzbl	%al, %edx
     538:	48 83 c3 08 	addq	$8, %rbx
     53c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BAND_II:
     540:	8b 03 	movl	(%rbx), %eax
     542:	0f b6 d0 	movzbl	%al, %edx
     545:	48 83 c3 08 	addq	$8, %rbx
     549:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BOR_II:
     54d:	8b 03 	movl	(%rbx), %eax
     54f:	0f b6 d0 	movzbl	%al, %edx
     552:	48 83 c3 08 	addq	$8, %rbx
     556:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BXOR_II:
     55a:	8b 03 	movl	(%rbx), %eax
     55c:	0f b6 d0 	movzbl	%al, %edx
     55f:	48 83 c3 08 	addq	$8, %rbx
     563:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHL_II:
     567:	8b 03 	movl	(%rbx), %eax
     569:	0f b6 d0 	movzbl	%al, %edx
     56c:	48 83 c3 08 	addq	$8, %rbx
     570:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHR_II:
     574:	8b 03 	movl	(%rbx), %eax
     576:	0f b6 d0 	movzbl	%al, %edx
     579:	48 83 c3 08 	addq	$8, %rbx
     57d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BNOT_I:
     581:	8b 03 	movl	(%rbx), %eax
     583:	0f b6 d0 	movzbl	%al, %edx
     586:	48 83 c3 08 	addq	$8, %rbx
     58a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_II:
     58e:	8b 03 	movl	(%rbx), %eax
     590:	0f b6 d0 	movzbl	%al, %edx
     593:	48 83 c3 08 	addq	$8, %rbx
     597:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_FF:
     59b:	8b 03 	movl	(%rbx), %eax
     59d:	0f b6 d0 	movzbl	%al, %edx
     5a0:	48 83 c3 08 	addq	$8, %rbx
     5a4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_II:
     5a8:	8b 03 	movl	(%rbx), %eax
     5aa:	0f b6 d0 	movzbl	%al, %edx
     5ad:	48 83 c3 08 	addq	$8, %rbx
     5b1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_FF:
     5b5:	8b 03 	movl	(%rbx), %eax
     5b7:	0f b6 d0 	movzbl	%al, %edx
     5ba:	48 83 c3 08 	addq	$8, %rbx
     5be:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_II:
     5c2:	8b 03 	movl	(%rbx), %eax
     5c4:	0f b6 d0 	movzbl	%al, %edx
     5c7:	48 83 c3 08 	addq	$8, %rbx
     5cb:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_FF:
     5cf:	8b 03 	movl	(%rbx), %eax
     5d1:	0f b6 d0 	movzbl	%al, %edx
     5d4:	48 83 c3 08 	addq	$8, %rbx
     5d8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_S:
     5dc:	8b 03 	movl	(%rbx), %eax
     5de:	0f b6 d0 	movzbl	%al, %edx
     5e1:	48 83 c3 08 	addq	$8, %rbx
     5e5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_S:
     5e9:	8b 03 	movl	(%rbx), %eax
     5eb:	0f b6 d0 	movzbl	%al, %edx
     5ee:	48 83 c3 08 	addq	$8, %rbx
     5f2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_S:
     5f6:	8b 03 	movl	(%rbx), %eax
     5f8:	0f b6 d0 	movzbl	%al, %edx
     5fb:	48 83 c3 08 	addq	$8, %rbx
     5ff:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_I:
     603:	8b 03 	movl	(%rbx), %eax
     605:	0f b6 d0 	movzbl	%al, %edx
     608:	48 83 c3 08 	addq	$8, %rbx
     60c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_I:
     610:	8b 03 	movl	(%rbx), %eax
     612:	0f b6 d0 	movzbl	%al, %edx
     615:	48 83 c3 08 	addq	$8, %rbx
     619:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_SK:
     61d:	8b 03 	movl	(%rbx), %eax
     61f:	0f b6 d0 	movzbl	%al, %edx
     622:	48 83 c3 08 	addq	$8, %rbx
     626:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_SK:
     62a:	8b 03 	movl	(%rbx), %eax
     62c:	0f b6 d0 	movzbl	%al, %edx
     62f:	48 83 c3 08 	addq	$8, %rbx
     633:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_SK:
     637:	8b 03 	movl	(%rbx), %eax
     639:	0f b6 d0 	movzbl	%al, %edx
     63c:	48 83 c3 08 	addq	$8, %rbx
     640:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABUP_SK:
     644:	8b 03 	movl	(%rbx), %eax
     646:	0f b6 d0 	movzbl	%al, %edx
     649:	48 83 c3 08 	addq	$8, %rbx
     64d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_luaV_interp:
     651:	55 	pushq	%rbp
     652:	57 	pushq	%rdi
     653:	56 	pushq	%rsi
     654:	53 	pushq	%rbx
     655:	41 54 	pushq	%r12
     657:	41 55 	pushq	%r13
     659:	41 56 	pushq	%r14
     65b:	41 57 	pushq	%r15
     65d:	48 83 ec 28 	subq	$40, %rsp
     661:	48 89 cd 	movq	%rcx, %rbp
     664:	4c 8b 75 18 	movq	24(%rbp), %r14
     668:	49 81 c6 40 05 00 00 	addq	$1344, %r14
     66f:	4c 8b 65 20 	movq	32(%rbp), %r12

ravi_new_frame:
     673:	49 8b 34 24 	movq	(%r12), %rsi
     677:	4c 8b 2e 	movq	(%rsi), %r13
     67a:	4d 8b 44 24 20 	movq	32(%r12), %r8
     67f:	49 8b 5c 24 28 	movq	40(%r12), %rbx
     684:	8b 03 	movl	(%rbx), %eax
     686:	0f b6 d0 	movzbl	%al, %edx
     689:	48 83 c3 08 	addq	$8, %rbx
     68d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_vm_return:
     691:	48 83 c4 28 	addq	$40, %rsp
     695:	41 5f 	popq	%r15
     697:	41 5e 	popq	%r14
     699:	41 5d 	popq	%r13
     69b:	41 5c 	popq	%r12
     69d:	5b 	popq	%rbx
     69e:	5e 	popq	%rsi
     69f:	5f 	popq	%rdi
     6a0:	5d 	popq	%rbp
     6a1:	c3 	retq
SYMBOL TABLE:
[ 0](sec -1)(fl 0x00)(ty   0)(scl   3) (nx 0) 0x00000001 @feat.00
[ 1](sec  1)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .text
AUX scnlen 0x6a2 nreloc 2 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 3](sec  0)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 luaF_close
[ 4](sec  0)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 luaD_poscall
[ 5](sec  2)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .pdata
AUX scnlen 0xc nreloc 3 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 7](sec  3)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .xdata
AUX scnlen 0x18 nreloc 0 nlnno 0 checksum 0x0 assoc 0 comdat 0
[ 9](sec  1)(fl 0x00)(ty   0)(scl   2) (nx 0) 0x00000000 ravi_vm_asm_begin
[10](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000000 ravi_BC_MOVE
[11](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000000d ravi_BC_LOADK
[12](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000001a ravi_BC_LOADKX
[13](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000027 ravi_BC_LOADBOOL
[14](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000034 ravi_BC_LOADNIL
[15](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000041 ravi_BC_GETUPVAL
[16](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000004e ravi_BC_GETTABUP
[17](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000005b ravi_BC_GETTABLE
[18](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000068 ravi_BC_SETTABUP
[19](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000075 ravi_BC_SETUPVAL
[20](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000082 ravi_BC_SETTABLE
[21](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000008f ravi_BC_NEWTABLE
[22](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000009c ravi_BC_SELF
[23](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000a9 ravi_BC_ADD
[24](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000b6 ravi_BC_SUB
[25](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000c3 ravi_BC_MUL
[26](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000d0 ravi_BC_MOD
[27](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000dd ravi_BC_POW
[28](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000ea ravi_BC_DIV
[29](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000000f7 ravi_BC_IDIV
[30](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000104 ravi_BC_BAND
[31](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000111 ravi_BC_BOR
[32](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000011e ravi_BC_BXOR
[33](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000012b ravi_BC_SHL
[34](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000138 ravi_BC_SHR
[35](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000145 ravi_BC_UNM
[36](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000152 ravi_BC_BNOT
[37](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000015f ravi_BC_NOT
[38](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000016c ravi_BC_LEN
[39](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000179 ravi_BC_CONCAT
[40](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000186 ravi_BC_JMP
[41](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000193 ravi_BC_EQ
[42](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001a0 ravi_BC_LT
[43](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001ad ravi_BC_LE
[44](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001ba ravi_BC_TEST
[45](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001c7 ravi_BC_TESTSET
[46](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001d4 ravi_BC_CALL
[47](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001e1 ravi_BC_TAILCALL
[48](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000001ee ravi_BC_RETURN
[49](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000268 ravi_BC_FORLOOP
[50](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000275 ravi_BC_FORPREP
[51](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000282 ravi_BC_TFORCALL
[52](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000028f ravi_BC_TFORLOOP
[53](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000029c ravi_BC_SETLIST
[54](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002a9 ravi_BC_CLOSURE
[55](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002b6 ravi_BC_VARARG
[56](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002c3 ravi_BC_EXTRAARG
[57](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002d0 ravi_BC_NEWARRAYI
[58](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002dd ravi_BC_NEWARRAYF
[59](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002ea ravi_BC_LOADIZ
[60](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002f7 ravi_BC_LOADFZ
[61](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000304 ravi_BC_UNMF
[62](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000311 ravi_BC_UNMI
[63](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000031e ravi_BC_ADDFF
[64](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000032b ravi_BC_ADDFI
[65](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000338 ravi_BC_ADDII
[66](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000345 ravi_BC_SUBFF
[67](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000352 ravi_BC_SUBFI
[68](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000035f ravi_BC_SUBIF
[69](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000036c ravi_BC_SUBII
[70](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000379 ravi_BC_MULFF
[71](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000386 ravi_BC_MULFI
[72](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000393 ravi_BC_MULII
[73](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003a0 ravi_BC_DIVFF
[74](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003ad ravi_BC_DIVFI
[75](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003ba ravi_BC_DIVIF
[76](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003c7 ravi_BC_DIVII
[77](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003d4 ravi_BC_TOINT
[78](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003e1 ravi_BC_TOFLT
[79](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003ee ravi_BC_TOARRAYI
[80](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003fb ravi_BC_TOARRAYF
[81](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000408 ravi_BC_TOTAB
[82](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000415 ravi_BC_TOSTRING
[83](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000422 ravi_BC_TOCLOSURE
[84](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000042f ravi_BC_TOTYPE
[85](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000043c ravi_BC_MOVEI
[86](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000449 ravi_BC_MOVEF
[87](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000456 ravi_BC_MOVEAI
[88](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000463 ravi_BC_MOVEAF
[89](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000470 ravi_BC_MOVETAB
[90](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000047d ravi_BC_GETTABLE_AI
[91](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000048a ravi_BC_GETTABLE_AF
[92](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000497 ravi_BC_SETTABLE_AI
[93](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004a4 ravi_BC_SETTABLE_AF
[94](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004b1 ravi_BC_SETTABLE_AII
[95](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004be ravi_BC_SETTABLE_AFF
[96](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004cb ravi_BC_FORLOOP_IP
[97](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004d8 ravi_BC_FORLOOP_I1
[98](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004e5 ravi_BC_FORPREP_IP
[99](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004f2 ravi_BC_FORPREP_I1
[100](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004ff ravi_BC_SETUPVALI
[101](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000050c ravi_BC_SETUPVALF
[102](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000519 ravi_BC_SETUPVALAI
[103](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000526 ravi_BC_SETUPVALAF
[104](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000533 ravi_BC_SETUPVALT
[105](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000540 ravi_BC_BAND_II
[106](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000054d ravi_BC_BOR_II
[107](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000055a ravi_BC_BXOR_II
[108](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000567 ravi_BC_SHL_II
[109](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000574 ravi_BC_SHR_II
[110](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000581 ravi_BC_BNOT_I
[111](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000058e ravi_BC_EQ_II
[112](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000059b ravi_BC_EQ_FF
[113](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005a8 ravi_BC_LT_II
[114](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005b5 ravi_BC_LT_FF
[115](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005c2 ravi_BC_LE_II
[116](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005cf ravi_BC_LE_FF
[117](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005dc ravi_BC_GETTABLE_S
[118](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005e9 ravi_BC_SETTABLE_S
[119](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005f6 ravi_BC_SELF_S
[120](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000603 ravi_BC_GETTABLE_I
[121](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000610 ravi_BC_SETTABLE_I
[122](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000061d ravi_BC_GETTABLE_SK
[123](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000062a ravi_BC_SELF_SK
[124](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000637 ravi_BC_SETTABLE_SK
[125](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000644 ravi_BC_GETTABUP_SK
[126](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000651 ravi_luaV_interp
[127](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000673 ravi_new_frame
[128](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000691 ravi_vm_return
[129](sec  4)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .rdata$Z
AUX scnlen 0xd nreloc 0 nlnno 0 checksum 0x0 assoc 0 comdat 0
Unwind info:

Function Table:
  Start Address: ravi_vm_asm_begin
  End Address: ravi_vm_asm_begin + 0x06a2
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

