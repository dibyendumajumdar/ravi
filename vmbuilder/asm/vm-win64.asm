
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
     21d:	c1 e1 04 	shll	$4, %ecx
     220:	4d 8d 04 08 	leaq	(%r8,%rcx), %r8
     224:	85 d2 	testl	%edx, %edx
     226:	74 08 	je	8 <ravi_BC_RETURN+0x42>
     228:	ff ca 	decl	%edx
     22a:	4c 0f b7 ca 	movzwq	%dx, %r9
     22e:	eb 0b 	jmp	11 <ravi_BC_RETURN+0x4D>
     230:	4c 8b 4d 10 	movq	16(%rbp), %r9
     234:	4d 29 c1 	subq	%r8, %r9
     237:	49 c1 e9 04 	shrq	$4, %r9
     23b:	48 89 e9 	movq	%rbp, %rcx
     23e:	4c 89 e2 	movq	%r12, %rdx
     241:	e8 00 00 00 00 	callq	0 <ravi_BC_RETURN+0x58>
     246:	41 f7 44 24 42 08 00 00 00 	testl	$8, 66(%r12)
     24f:	0f 85 3f 04 00 00 	jne	1087 <ravi_vm_return>
     255:	4c 8b 65 20 	movq	32(%rbp), %r12
     259:	85 c0 	testl	%eax, %eax
     25b:	74 09 	je	9 <ravi_BC_RETURN+0x78>
     25d:	4d 8b 6c 24 08 	movq	8(%r12), %r13
     262:	4c 89 6d 10 	movq	%r13, 16(%rbp)
     266:	e9 0b 04 00 00 	jmp	1035 <ravi_new_frame>

ravi_BC_FORLOOP:
     26b:	8b 03 	movl	(%rbx), %eax
     26d:	0f b6 d0 	movzbl	%al, %edx
     270:	48 83 c3 08 	addq	$8, %rbx
     274:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP:
     278:	8b 03 	movl	(%rbx), %eax
     27a:	0f b6 d0 	movzbl	%al, %edx
     27d:	48 83 c3 08 	addq	$8, %rbx
     281:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORCALL:
     285:	8b 03 	movl	(%rbx), %eax
     287:	0f b6 d0 	movzbl	%al, %edx
     28a:	48 83 c3 08 	addq	$8, %rbx
     28e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TFORLOOP:
     292:	8b 03 	movl	(%rbx), %eax
     294:	0f b6 d0 	movzbl	%al, %edx
     297:	48 83 c3 08 	addq	$8, %rbx
     29b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETLIST:
     29f:	8b 03 	movl	(%rbx), %eax
     2a1:	0f b6 d0 	movzbl	%al, %edx
     2a4:	48 83 c3 08 	addq	$8, %rbx
     2a8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_CLOSURE:
     2ac:	8b 03 	movl	(%rbx), %eax
     2ae:	0f b6 d0 	movzbl	%al, %edx
     2b1:	48 83 c3 08 	addq	$8, %rbx
     2b5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_VARARG:
     2b9:	8b 03 	movl	(%rbx), %eax
     2bb:	0f b6 d0 	movzbl	%al, %edx
     2be:	48 83 c3 08 	addq	$8, %rbx
     2c2:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EXTRAARG:
     2c6:	8b 03 	movl	(%rbx), %eax
     2c8:	0f b6 d0 	movzbl	%al, %edx
     2cb:	48 83 c3 08 	addq	$8, %rbx
     2cf:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYI:
     2d3:	8b 03 	movl	(%rbx), %eax
     2d5:	0f b6 d0 	movzbl	%al, %edx
     2d8:	48 83 c3 08 	addq	$8, %rbx
     2dc:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_NEWARRAYF:
     2e0:	8b 03 	movl	(%rbx), %eax
     2e2:	0f b6 d0 	movzbl	%al, %edx
     2e5:	48 83 c3 08 	addq	$8, %rbx
     2e9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADIZ:
     2ed:	8b 03 	movl	(%rbx), %eax
     2ef:	0f b6 d0 	movzbl	%al, %edx
     2f2:	48 83 c3 08 	addq	$8, %rbx
     2f6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LOADFZ:
     2fa:	8b 03 	movl	(%rbx), %eax
     2fc:	0f b6 d0 	movzbl	%al, %edx
     2ff:	48 83 c3 08 	addq	$8, %rbx
     303:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMF:
     307:	8b 03 	movl	(%rbx), %eax
     309:	0f b6 d0 	movzbl	%al, %edx
     30c:	48 83 c3 08 	addq	$8, %rbx
     310:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_UNMI:
     314:	8b 03 	movl	(%rbx), %eax
     316:	0f b6 d0 	movzbl	%al, %edx
     319:	48 83 c3 08 	addq	$8, %rbx
     31d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFF:
     321:	8b 03 	movl	(%rbx), %eax
     323:	0f b6 d0 	movzbl	%al, %edx
     326:	48 83 c3 08 	addq	$8, %rbx
     32a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDFI:
     32e:	8b 03 	movl	(%rbx), %eax
     330:	0f b6 d0 	movzbl	%al, %edx
     333:	48 83 c3 08 	addq	$8, %rbx
     337:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_ADDII:
     33b:	8b 03 	movl	(%rbx), %eax
     33d:	0f b6 d0 	movzbl	%al, %edx
     340:	48 83 c3 08 	addq	$8, %rbx
     344:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFF:
     348:	8b 03 	movl	(%rbx), %eax
     34a:	0f b6 d0 	movzbl	%al, %edx
     34d:	48 83 c3 08 	addq	$8, %rbx
     351:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBFI:
     355:	8b 03 	movl	(%rbx), %eax
     357:	0f b6 d0 	movzbl	%al, %edx
     35a:	48 83 c3 08 	addq	$8, %rbx
     35e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBIF:
     362:	8b 03 	movl	(%rbx), %eax
     364:	0f b6 d0 	movzbl	%al, %edx
     367:	48 83 c3 08 	addq	$8, %rbx
     36b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SUBII:
     36f:	8b 03 	movl	(%rbx), %eax
     371:	0f b6 d0 	movzbl	%al, %edx
     374:	48 83 c3 08 	addq	$8, %rbx
     378:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFF:
     37c:	8b 03 	movl	(%rbx), %eax
     37e:	0f b6 d0 	movzbl	%al, %edx
     381:	48 83 c3 08 	addq	$8, %rbx
     385:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULFI:
     389:	8b 03 	movl	(%rbx), %eax
     38b:	0f b6 d0 	movzbl	%al, %edx
     38e:	48 83 c3 08 	addq	$8, %rbx
     392:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MULII:
     396:	8b 03 	movl	(%rbx), %eax
     398:	0f b6 d0 	movzbl	%al, %edx
     39b:	48 83 c3 08 	addq	$8, %rbx
     39f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFF:
     3a3:	8b 03 	movl	(%rbx), %eax
     3a5:	0f b6 d0 	movzbl	%al, %edx
     3a8:	48 83 c3 08 	addq	$8, %rbx
     3ac:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVFI:
     3b0:	8b 03 	movl	(%rbx), %eax
     3b2:	0f b6 d0 	movzbl	%al, %edx
     3b5:	48 83 c3 08 	addq	$8, %rbx
     3b9:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVIF:
     3bd:	8b 03 	movl	(%rbx), %eax
     3bf:	0f b6 d0 	movzbl	%al, %edx
     3c2:	48 83 c3 08 	addq	$8, %rbx
     3c6:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_DIVII:
     3ca:	8b 03 	movl	(%rbx), %eax
     3cc:	0f b6 d0 	movzbl	%al, %edx
     3cf:	48 83 c3 08 	addq	$8, %rbx
     3d3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOINT:
     3d7:	8b 03 	movl	(%rbx), %eax
     3d9:	0f b6 d0 	movzbl	%al, %edx
     3dc:	48 83 c3 08 	addq	$8, %rbx
     3e0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOFLT:
     3e4:	8b 03 	movl	(%rbx), %eax
     3e6:	0f b6 d0 	movzbl	%al, %edx
     3e9:	48 83 c3 08 	addq	$8, %rbx
     3ed:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYI:
     3f1:	8b 03 	movl	(%rbx), %eax
     3f3:	0f b6 d0 	movzbl	%al, %edx
     3f6:	48 83 c3 08 	addq	$8, %rbx
     3fa:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOARRAYF:
     3fe:	8b 03 	movl	(%rbx), %eax
     400:	0f b6 d0 	movzbl	%al, %edx
     403:	48 83 c3 08 	addq	$8, %rbx
     407:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTAB:
     40b:	8b 03 	movl	(%rbx), %eax
     40d:	0f b6 d0 	movzbl	%al, %edx
     410:	48 83 c3 08 	addq	$8, %rbx
     414:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOSTRING:
     418:	8b 03 	movl	(%rbx), %eax
     41a:	0f b6 d0 	movzbl	%al, %edx
     41d:	48 83 c3 08 	addq	$8, %rbx
     421:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOCLOSURE:
     425:	8b 03 	movl	(%rbx), %eax
     427:	0f b6 d0 	movzbl	%al, %edx
     42a:	48 83 c3 08 	addq	$8, %rbx
     42e:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_TOTYPE:
     432:	8b 03 	movl	(%rbx), %eax
     434:	0f b6 d0 	movzbl	%al, %edx
     437:	48 83 c3 08 	addq	$8, %rbx
     43b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEI:
     43f:	8b 03 	movl	(%rbx), %eax
     441:	0f b6 d0 	movzbl	%al, %edx
     444:	48 83 c3 08 	addq	$8, %rbx
     448:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEF:
     44c:	8b 03 	movl	(%rbx), %eax
     44e:	0f b6 d0 	movzbl	%al, %edx
     451:	48 83 c3 08 	addq	$8, %rbx
     455:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAI:
     459:	8b 03 	movl	(%rbx), %eax
     45b:	0f b6 d0 	movzbl	%al, %edx
     45e:	48 83 c3 08 	addq	$8, %rbx
     462:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVEAF:
     466:	8b 03 	movl	(%rbx), %eax
     468:	0f b6 d0 	movzbl	%al, %edx
     46b:	48 83 c3 08 	addq	$8, %rbx
     46f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_MOVETAB:
     473:	8b 03 	movl	(%rbx), %eax
     475:	0f b6 d0 	movzbl	%al, %edx
     478:	48 83 c3 08 	addq	$8, %rbx
     47c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AI:
     480:	8b 03 	movl	(%rbx), %eax
     482:	0f b6 d0 	movzbl	%al, %edx
     485:	48 83 c3 08 	addq	$8, %rbx
     489:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_AF:
     48d:	8b 03 	movl	(%rbx), %eax
     48f:	0f b6 d0 	movzbl	%al, %edx
     492:	48 83 c3 08 	addq	$8, %rbx
     496:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AI:
     49a:	8b 03 	movl	(%rbx), %eax
     49c:	0f b6 d0 	movzbl	%al, %edx
     49f:	48 83 c3 08 	addq	$8, %rbx
     4a3:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AF:
     4a7:	8b 03 	movl	(%rbx), %eax
     4a9:	0f b6 d0 	movzbl	%al, %edx
     4ac:	48 83 c3 08 	addq	$8, %rbx
     4b0:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AII:
     4b4:	8b 03 	movl	(%rbx), %eax
     4b6:	0f b6 d0 	movzbl	%al, %edx
     4b9:	48 83 c3 08 	addq	$8, %rbx
     4bd:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_AFF:
     4c1:	8b 03 	movl	(%rbx), %eax
     4c3:	0f b6 d0 	movzbl	%al, %edx
     4c6:	48 83 c3 08 	addq	$8, %rbx
     4ca:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_IP:
     4ce:	8b 03 	movl	(%rbx), %eax
     4d0:	0f b6 d0 	movzbl	%al, %edx
     4d3:	48 83 c3 08 	addq	$8, %rbx
     4d7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORLOOP_I1:
     4db:	8b 03 	movl	(%rbx), %eax
     4dd:	0f b6 d0 	movzbl	%al, %edx
     4e0:	48 83 c3 08 	addq	$8, %rbx
     4e4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_IP:
     4e8:	8b 03 	movl	(%rbx), %eax
     4ea:	0f b6 d0 	movzbl	%al, %edx
     4ed:	48 83 c3 08 	addq	$8, %rbx
     4f1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_FORPREP_I1:
     4f5:	8b 03 	movl	(%rbx), %eax
     4f7:	0f b6 d0 	movzbl	%al, %edx
     4fa:	48 83 c3 08 	addq	$8, %rbx
     4fe:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALI:
     502:	8b 03 	movl	(%rbx), %eax
     504:	0f b6 d0 	movzbl	%al, %edx
     507:	48 83 c3 08 	addq	$8, %rbx
     50b:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALF:
     50f:	8b 03 	movl	(%rbx), %eax
     511:	0f b6 d0 	movzbl	%al, %edx
     514:	48 83 c3 08 	addq	$8, %rbx
     518:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAI:
     51c:	8b 03 	movl	(%rbx), %eax
     51e:	0f b6 d0 	movzbl	%al, %edx
     521:	48 83 c3 08 	addq	$8, %rbx
     525:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALAF:
     529:	8b 03 	movl	(%rbx), %eax
     52b:	0f b6 d0 	movzbl	%al, %edx
     52e:	48 83 c3 08 	addq	$8, %rbx
     532:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETUPVALT:
     536:	8b 03 	movl	(%rbx), %eax
     538:	0f b6 d0 	movzbl	%al, %edx
     53b:	48 83 c3 08 	addq	$8, %rbx
     53f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BAND_II:
     543:	8b 03 	movl	(%rbx), %eax
     545:	0f b6 d0 	movzbl	%al, %edx
     548:	48 83 c3 08 	addq	$8, %rbx
     54c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BOR_II:
     550:	8b 03 	movl	(%rbx), %eax
     552:	0f b6 d0 	movzbl	%al, %edx
     555:	48 83 c3 08 	addq	$8, %rbx
     559:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BXOR_II:
     55d:	8b 03 	movl	(%rbx), %eax
     55f:	0f b6 d0 	movzbl	%al, %edx
     562:	48 83 c3 08 	addq	$8, %rbx
     566:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHL_II:
     56a:	8b 03 	movl	(%rbx), %eax
     56c:	0f b6 d0 	movzbl	%al, %edx
     56f:	48 83 c3 08 	addq	$8, %rbx
     573:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SHR_II:
     577:	8b 03 	movl	(%rbx), %eax
     579:	0f b6 d0 	movzbl	%al, %edx
     57c:	48 83 c3 08 	addq	$8, %rbx
     580:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_BNOT_I:
     584:	8b 03 	movl	(%rbx), %eax
     586:	0f b6 d0 	movzbl	%al, %edx
     589:	48 83 c3 08 	addq	$8, %rbx
     58d:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_II:
     591:	8b 03 	movl	(%rbx), %eax
     593:	0f b6 d0 	movzbl	%al, %edx
     596:	48 83 c3 08 	addq	$8, %rbx
     59a:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_EQ_FF:
     59e:	8b 03 	movl	(%rbx), %eax
     5a0:	0f b6 d0 	movzbl	%al, %edx
     5a3:	48 83 c3 08 	addq	$8, %rbx
     5a7:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_II:
     5ab:	8b 03 	movl	(%rbx), %eax
     5ad:	0f b6 d0 	movzbl	%al, %edx
     5b0:	48 83 c3 08 	addq	$8, %rbx
     5b4:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LT_FF:
     5b8:	8b 03 	movl	(%rbx), %eax
     5ba:	0f b6 d0 	movzbl	%al, %edx
     5bd:	48 83 c3 08 	addq	$8, %rbx
     5c1:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_II:
     5c5:	8b 03 	movl	(%rbx), %eax
     5c7:	0f b6 d0 	movzbl	%al, %edx
     5ca:	48 83 c3 08 	addq	$8, %rbx
     5ce:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_LE_FF:
     5d2:	8b 03 	movl	(%rbx), %eax
     5d4:	0f b6 d0 	movzbl	%al, %edx
     5d7:	48 83 c3 08 	addq	$8, %rbx
     5db:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_S:
     5df:	8b 03 	movl	(%rbx), %eax
     5e1:	0f b6 d0 	movzbl	%al, %edx
     5e4:	48 83 c3 08 	addq	$8, %rbx
     5e8:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_S:
     5ec:	8b 03 	movl	(%rbx), %eax
     5ee:	0f b6 d0 	movzbl	%al, %edx
     5f1:	48 83 c3 08 	addq	$8, %rbx
     5f5:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_S:
     5f9:	8b 03 	movl	(%rbx), %eax
     5fb:	0f b6 d0 	movzbl	%al, %edx
     5fe:	48 83 c3 08 	addq	$8, %rbx
     602:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_I:
     606:	8b 03 	movl	(%rbx), %eax
     608:	0f b6 d0 	movzbl	%al, %edx
     60b:	48 83 c3 08 	addq	$8, %rbx
     60f:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_I:
     613:	8b 03 	movl	(%rbx), %eax
     615:	0f b6 d0 	movzbl	%al, %edx
     618:	48 83 c3 08 	addq	$8, %rbx
     61c:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABLE_SK:
     620:	8b 03 	movl	(%rbx), %eax
     622:	0f b6 d0 	movzbl	%al, %edx
     625:	48 83 c3 08 	addq	$8, %rbx
     629:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SELF_SK:
     62d:	8b 03 	movl	(%rbx), %eax
     62f:	0f b6 d0 	movzbl	%al, %edx
     632:	48 83 c3 08 	addq	$8, %rbx
     636:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_SETTABLE_SK:
     63a:	8b 03 	movl	(%rbx), %eax
     63c:	0f b6 d0 	movzbl	%al, %edx
     63f:	48 83 c3 08 	addq	$8, %rbx
     643:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_BC_GETTABUP_SK:
     647:	8b 03 	movl	(%rbx), %eax
     649:	0f b6 d0 	movzbl	%al, %edx
     64c:	48 83 c3 08 	addq	$8, %rbx
     650:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_luaV_interp:
     654:	55 	pushq	%rbp
     655:	57 	pushq	%rdi
     656:	56 	pushq	%rsi
     657:	53 	pushq	%rbx
     658:	41 54 	pushq	%r12
     65a:	41 55 	pushq	%r13
     65c:	41 56 	pushq	%r14
     65e:	41 57 	pushq	%r15
     660:	48 83 ec 28 	subq	$40, %rsp
     664:	48 89 cd 	movq	%rcx, %rbp
     667:	4c 8b 75 18 	movq	24(%rbp), %r14
     66b:	49 81 c6 40 05 00 00 	addq	$1344, %r14
     672:	4c 8b 65 20 	movq	32(%rbp), %r12

ravi_new_frame:
     676:	49 8b 34 24 	movq	(%r12), %rsi
     67a:	4c 8b 2e 	movq	(%rsi), %r13
     67d:	4d 8b 44 24 20 	movq	32(%r12), %r8
     682:	49 8b 5c 24 28 	movq	40(%r12), %rbx
     687:	8b 03 	movl	(%rbx), %eax
     689:	0f b6 d0 	movzbl	%al, %edx
     68c:	48 83 c3 08 	addq	$8, %rbx
     690:	41 ff 24 d6 	jmpq	*(%r14,%rdx,8)

ravi_vm_return:
     694:	48 83 c4 28 	addq	$40, %rsp
     698:	41 5f 	popq	%r15
     69a:	41 5e 	popq	%r14
     69c:	41 5d 	popq	%r13
     69e:	41 5c 	popq	%r12
     6a0:	5b 	popq	%rbx
     6a1:	5e 	popq	%rsi
     6a2:	5f 	popq	%rdi
     6a3:	5d 	popq	%rbp
     6a4:	c3 	retq
SYMBOL TABLE:
[ 0](sec -1)(fl 0x00)(ty   0)(scl   3) (nx 0) 0x00000001 @feat.00
[ 1](sec  1)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .text
AUX scnlen 0x6a5 nreloc 2 nlnno 0 checksum 0x0 assoc 0 comdat 0
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
[49](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000026b ravi_BC_FORLOOP
[50](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000278 ravi_BC_FORPREP
[51](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000285 ravi_BC_TFORCALL
[52](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000292 ravi_BC_TFORLOOP
[53](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000029f ravi_BC_SETLIST
[54](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002ac ravi_BC_CLOSURE
[55](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002b9 ravi_BC_VARARG
[56](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002c6 ravi_BC_EXTRAARG
[57](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002d3 ravi_BC_NEWARRAYI
[58](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002e0 ravi_BC_NEWARRAYF
[59](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002ed ravi_BC_LOADIZ
[60](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000002fa ravi_BC_LOADFZ
[61](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000307 ravi_BC_UNMF
[62](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000314 ravi_BC_UNMI
[63](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000321 ravi_BC_ADDFF
[64](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000032e ravi_BC_ADDFI
[65](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000033b ravi_BC_ADDII
[66](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000348 ravi_BC_SUBFF
[67](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000355 ravi_BC_SUBFI
[68](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000362 ravi_BC_SUBIF
[69](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000036f ravi_BC_SUBII
[70](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000037c ravi_BC_MULFF
[71](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000389 ravi_BC_MULFI
[72](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000396 ravi_BC_MULII
[73](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003a3 ravi_BC_DIVFF
[74](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003b0 ravi_BC_DIVFI
[75](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003bd ravi_BC_DIVIF
[76](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003ca ravi_BC_DIVII
[77](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003d7 ravi_BC_TOINT
[78](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003e4 ravi_BC_TOFLT
[79](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003f1 ravi_BC_TOARRAYI
[80](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000003fe ravi_BC_TOARRAYF
[81](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000040b ravi_BC_TOTAB
[82](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000418 ravi_BC_TOSTRING
[83](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000425 ravi_BC_TOCLOSURE
[84](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000432 ravi_BC_TOTYPE
[85](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000043f ravi_BC_MOVEI
[86](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000044c ravi_BC_MOVEF
[87](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000459 ravi_BC_MOVEAI
[88](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000466 ravi_BC_MOVEAF
[89](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000473 ravi_BC_MOVETAB
[90](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000480 ravi_BC_GETTABLE_AI
[91](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000048d ravi_BC_GETTABLE_AF
[92](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000049a ravi_BC_SETTABLE_AI
[93](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004a7 ravi_BC_SETTABLE_AF
[94](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004b4 ravi_BC_SETTABLE_AII
[95](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004c1 ravi_BC_SETTABLE_AFF
[96](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004ce ravi_BC_FORLOOP_IP
[97](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004db ravi_BC_FORLOOP_I1
[98](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004e8 ravi_BC_FORPREP_IP
[99](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000004f5 ravi_BC_FORPREP_I1
[100](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000502 ravi_BC_SETUPVALI
[101](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000050f ravi_BC_SETUPVALF
[102](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000051c ravi_BC_SETUPVALAI
[103](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000529 ravi_BC_SETUPVALAF
[104](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000536 ravi_BC_SETUPVALT
[105](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000543 ravi_BC_BAND_II
[106](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000550 ravi_BC_BOR_II
[107](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000055d ravi_BC_BXOR_II
[108](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000056a ravi_BC_SHL_II
[109](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000577 ravi_BC_SHR_II
[110](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000584 ravi_BC_BNOT_I
[111](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000591 ravi_BC_EQ_II
[112](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000059e ravi_BC_EQ_FF
[113](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005ab ravi_BC_LT_II
[114](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005b8 ravi_BC_LT_FF
[115](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005c5 ravi_BC_LE_II
[116](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005d2 ravi_BC_LE_FF
[117](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005df ravi_BC_GETTABLE_S
[118](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005ec ravi_BC_SETTABLE_S
[119](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x000005f9 ravi_BC_SELF_S
[120](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000606 ravi_BC_GETTABLE_I
[121](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000613 ravi_BC_SETTABLE_I
[122](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000620 ravi_BC_GETTABLE_SK
[123](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000062d ravi_BC_SELF_SK
[124](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x0000063a ravi_BC_SETTABLE_SK
[125](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000647 ravi_BC_GETTABUP_SK
[126](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000654 ravi_luaV_interp
[127](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000676 ravi_new_frame
[128](sec  1)(fl 0x00)(ty  20)(scl   2) (nx 0) 0x00000694 ravi_vm_return
[129](sec  4)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .rdata$Z
AUX scnlen 0xd nreloc 0 nlnno 0 checksum 0x0 assoc 0 comdat 0
Unwind info:

Function Table:
  Start Address: ravi_vm_asm_begin
  End Address: ravi_vm_asm_begin + 0x06a5
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

