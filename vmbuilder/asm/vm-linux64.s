
ravi.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <ravi_BC_MOVE>:
   0:	0f b6 cc             	movzbl %ah,%ecx
   3:	c1 e8 10             	shr    $0x10,%eax
   6:	0f b6 d4             	movzbl %ah,%edx
   9:	c1 e2 04             	shl    $0x4,%edx
   c:	4d 8d 14 10          	lea    (%r8,%rdx,1),%r10
  10:	c1 e1 04             	shl    $0x4,%ecx
  13:	4d 8d 1c 08          	lea    (%r8,%rcx,1),%r11
  17:	49 8b 02             	mov    (%r10),%rax
  1a:	45 8b 4a 08          	mov    0x8(%r10),%r9d
  1e:	49 89 03             	mov    %rax,(%r11)
  21:	45 89 4b 08          	mov    %r9d,0x8(%r11)
  25:	8b 03                	mov    (%rbx),%eax
  27:	0f b6 d0             	movzbl %al,%edx
  2a:	48 83 c3 04          	add    $0x4,%rbx
  2e:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000032 <ravi_BC_LOADK>:
  32:	0f b6 cc             	movzbl %ah,%ecx
  35:	c1 e8 10             	shr    $0x10,%eax
  38:	0f b7 d0             	movzwl %ax,%edx
  3b:	c1 e2 04             	shl    $0x4,%edx
  3e:	4d 8d 14 17          	lea    (%r15,%rdx,1),%r10
  42:	c1 e1 04             	shl    $0x4,%ecx
  45:	4d 8d 1c 08          	lea    (%r8,%rcx,1),%r11
  49:	49 8b 02             	mov    (%r10),%rax
  4c:	45 8b 4a 08          	mov    0x8(%r10),%r9d
  50:	49 89 03             	mov    %rax,(%r11)
  53:	45 89 4b 08          	mov    %r9d,0x8(%r11)
  57:	8b 03                	mov    (%rbx),%eax
  59:	0f b6 d0             	movzbl %al,%edx
  5c:	48 83 c3 04          	add    $0x4,%rbx
  60:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000064 <ravi_BC_LOADKX>:
  64:	8b 03                	mov    (%rbx),%eax
  66:	0f b6 d0             	movzbl %al,%edx
  69:	48 83 c3 04          	add    $0x4,%rbx
  6d:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000071 <ravi_BC_LOADBOOL>:
  71:	8b 03                	mov    (%rbx),%eax
  73:	0f b6 d0             	movzbl %al,%edx
  76:	48 83 c3 04          	add    $0x4,%rbx
  7a:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000007e <ravi_BC_LOADNIL>:
  7e:	8b 03                	mov    (%rbx),%eax
  80:	0f b6 d0             	movzbl %al,%edx
  83:	48 83 c3 04          	add    $0x4,%rbx
  87:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000008b <ravi_BC_GETUPVAL>:
  8b:	8b 03                	mov    (%rbx),%eax
  8d:	0f b6 d0             	movzbl %al,%edx
  90:	48 83 c3 04          	add    $0x4,%rbx
  94:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000098 <ravi_BC_GETTABUP>:
  98:	8b 03                	mov    (%rbx),%eax
  9a:	0f b6 d0             	movzbl %al,%edx
  9d:	48 83 c3 04          	add    $0x4,%rbx
  a1:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000000a5 <ravi_BC_GETTABLE>:
  a5:	8b 03                	mov    (%rbx),%eax
  a7:	0f b6 d0             	movzbl %al,%edx
  aa:	48 83 c3 04          	add    $0x4,%rbx
  ae:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000000b2 <ravi_BC_SETTABUP>:
  b2:	8b 03                	mov    (%rbx),%eax
  b4:	0f b6 d0             	movzbl %al,%edx
  b7:	48 83 c3 04          	add    $0x4,%rbx
  bb:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000000bf <ravi_BC_SETUPVAL>:
  bf:	8b 03                	mov    (%rbx),%eax
  c1:	0f b6 d0             	movzbl %al,%edx
  c4:	48 83 c3 04          	add    $0x4,%rbx
  c8:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000000cc <ravi_BC_SETTABLE>:
  cc:	8b 03                	mov    (%rbx),%eax
  ce:	0f b6 d0             	movzbl %al,%edx
  d1:	48 83 c3 04          	add    $0x4,%rbx
  d5:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000000d9 <ravi_BC_NEWTABLE>:
  d9:	8b 03                	mov    (%rbx),%eax
  db:	0f b6 d0             	movzbl %al,%edx
  de:	48 83 c3 04          	add    $0x4,%rbx
  e2:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000000e6 <ravi_BC_SELF>:
  e6:	8b 03                	mov    (%rbx),%eax
  e8:	0f b6 d0             	movzbl %al,%edx
  eb:	48 83 c3 04          	add    $0x4,%rbx
  ef:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000000f3 <ravi_BC_ADD>:
  f3:	8b 03                	mov    (%rbx),%eax
  f5:	0f b6 d0             	movzbl %al,%edx
  f8:	48 83 c3 04          	add    $0x4,%rbx
  fc:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000100 <ravi_BC_SUB>:
 100:	8b 03                	mov    (%rbx),%eax
 102:	0f b6 d0             	movzbl %al,%edx
 105:	48 83 c3 04          	add    $0x4,%rbx
 109:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000010d <ravi_BC_MUL>:
 10d:	8b 03                	mov    (%rbx),%eax
 10f:	0f b6 d0             	movzbl %al,%edx
 112:	48 83 c3 04          	add    $0x4,%rbx
 116:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000011a <ravi_BC_MOD>:
 11a:	8b 03                	mov    (%rbx),%eax
 11c:	0f b6 d0             	movzbl %al,%edx
 11f:	48 83 c3 04          	add    $0x4,%rbx
 123:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000127 <ravi_BC_POW>:
 127:	8b 03                	mov    (%rbx),%eax
 129:	0f b6 d0             	movzbl %al,%edx
 12c:	48 83 c3 04          	add    $0x4,%rbx
 130:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000134 <ravi_BC_DIV>:
 134:	8b 03                	mov    (%rbx),%eax
 136:	0f b6 d0             	movzbl %al,%edx
 139:	48 83 c3 04          	add    $0x4,%rbx
 13d:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000141 <ravi_BC_IDIV>:
 141:	8b 03                	mov    (%rbx),%eax
 143:	0f b6 d0             	movzbl %al,%edx
 146:	48 83 c3 04          	add    $0x4,%rbx
 14a:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000014e <ravi_BC_BAND>:
 14e:	8b 03                	mov    (%rbx),%eax
 150:	0f b6 d0             	movzbl %al,%edx
 153:	48 83 c3 04          	add    $0x4,%rbx
 157:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000015b <ravi_BC_BOR>:
 15b:	8b 03                	mov    (%rbx),%eax
 15d:	0f b6 d0             	movzbl %al,%edx
 160:	48 83 c3 04          	add    $0x4,%rbx
 164:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000168 <ravi_BC_BXOR>:
 168:	8b 03                	mov    (%rbx),%eax
 16a:	0f b6 d0             	movzbl %al,%edx
 16d:	48 83 c3 04          	add    $0x4,%rbx
 171:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000175 <ravi_BC_SHL>:
 175:	8b 03                	mov    (%rbx),%eax
 177:	0f b6 d0             	movzbl %al,%edx
 17a:	48 83 c3 04          	add    $0x4,%rbx
 17e:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000182 <ravi_BC_SHR>:
 182:	8b 03                	mov    (%rbx),%eax
 184:	0f b6 d0             	movzbl %al,%edx
 187:	48 83 c3 04          	add    $0x4,%rbx
 18b:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000018f <ravi_BC_UNM>:
 18f:	8b 03                	mov    (%rbx),%eax
 191:	0f b6 d0             	movzbl %al,%edx
 194:	48 83 c3 04          	add    $0x4,%rbx
 198:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000019c <ravi_BC_BNOT>:
 19c:	8b 03                	mov    (%rbx),%eax
 19e:	0f b6 d0             	movzbl %al,%edx
 1a1:	48 83 c3 04          	add    $0x4,%rbx
 1a5:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000001a9 <ravi_BC_NOT>:
 1a9:	8b 03                	mov    (%rbx),%eax
 1ab:	0f b6 d0             	movzbl %al,%edx
 1ae:	48 83 c3 04          	add    $0x4,%rbx
 1b2:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000001b6 <ravi_BC_LEN>:
 1b6:	8b 03                	mov    (%rbx),%eax
 1b8:	0f b6 d0             	movzbl %al,%edx
 1bb:	48 83 c3 04          	add    $0x4,%rbx
 1bf:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000001c3 <ravi_BC_CONCAT>:
 1c3:	8b 03                	mov    (%rbx),%eax
 1c5:	0f b6 d0             	movzbl %al,%edx
 1c8:	48 83 c3 04          	add    $0x4,%rbx
 1cc:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000001d0 <ravi_BC_JMP>:
 1d0:	8b 03                	mov    (%rbx),%eax
 1d2:	0f b6 d0             	movzbl %al,%edx
 1d5:	48 83 c3 04          	add    $0x4,%rbx
 1d9:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000001dd <ravi_BC_EQ>:
 1dd:	8b 03                	mov    (%rbx),%eax
 1df:	0f b6 d0             	movzbl %al,%edx
 1e2:	48 83 c3 04          	add    $0x4,%rbx
 1e6:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000001ea <ravi_BC_LT>:
 1ea:	8b 03                	mov    (%rbx),%eax
 1ec:	0f b6 d0             	movzbl %al,%edx
 1ef:	48 83 c3 04          	add    $0x4,%rbx
 1f3:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000001f7 <ravi_BC_LE>:
 1f7:	8b 03                	mov    (%rbx),%eax
 1f9:	0f b6 d0             	movzbl %al,%edx
 1fc:	48 83 c3 04          	add    $0x4,%rbx
 200:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000204 <ravi_BC_TEST>:
 204:	8b 03                	mov    (%rbx),%eax
 206:	0f b6 d0             	movzbl %al,%edx
 209:	48 83 c3 04          	add    $0x4,%rbx
 20d:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000211 <ravi_BC_TESTSET>:
 211:	8b 03                	mov    (%rbx),%eax
 213:	0f b6 d0             	movzbl %al,%edx
 216:	48 83 c3 04          	add    $0x4,%rbx
 21a:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000021e <ravi_BC_CALL>:
 21e:	8b 03                	mov    (%rbx),%eax
 220:	0f b6 d0             	movzbl %al,%edx
 223:	48 83 c3 04          	add    $0x4,%rbx
 227:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000022b <ravi_BC_TAILCALL>:
 22b:	8b 03                	mov    (%rbx),%eax
 22d:	0f b6 d0             	movzbl %al,%edx
 230:	48 83 c3 04          	add    $0x4,%rbx
 234:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000238 <ravi_BC_RETURN>:
 238:	4d 8b 55 18          	mov    0x18(%r13),%r10
 23c:	41 83 7a 20 00       	cmpl   $0x0,0x20(%r10)
 241:	74 17                	je     25a <ravi_BC_RETURN+0x22>
 243:	49 89 c7             	mov    %rax,%r15
 246:	4d 89 c5             	mov    %r8,%r13
 249:	48 89 ef             	mov    %rbp,%rdi
 24c:	4c 89 c6             	mov    %r8,%rsi
 24f:	e8 00 00 00 00       	callq  254 <ravi_BC_RETURN+0x1c>
 254:	4d 89 e8             	mov    %r13,%r8
 257:	4c 89 f8             	mov    %r15,%rax
 25a:	49 89 5c 24 28       	mov    %rbx,0x28(%r12)
 25f:	0f b6 cc             	movzbl %ah,%ecx
 262:	c1 e8 10             	shr    $0x10,%eax
 265:	0f b6 d4             	movzbl %ah,%edx
 268:	41 89 ca             	mov    %ecx,%r10d
 26b:	85 d2                	test   %edx,%edx
 26d:	74 0e                	je     27d <ravi_BC_RETURN+0x45>
 26f:	ff ca                	dec    %edx
 271:	89 d1                	mov    %edx,%ecx
 273:	41 c1 e2 04          	shl    $0x4,%r10d
 277:	4b 8d 14 10          	lea    (%r8,%r10,1),%rdx
 27b:	eb 13                	jmp    290 <ravi_BC_RETURN+0x58>
 27d:	41 c1 e2 04          	shl    $0x4,%r10d
 281:	4b 8d 14 10          	lea    (%r8,%r10,1),%rdx
 285:	48 8b 4d 10          	mov    0x10(%rbp),%rcx
 289:	48 29 d1             	sub    %rdx,%rcx
 28c:	48 c1 e9 04          	shr    $0x4,%rcx
 290:	48 89 ef             	mov    %rbp,%rdi
 293:	4c 89 e6             	mov    %r12,%rsi
 296:	e8 00 00 00 00       	callq  29b <ravi_BC_RETURN+0x63>
 29b:	66 41 f7 44 24 42 08 	testw  $0x8,0x42(%r12)
 2a2:	00 
 2a3:	0f 85 fa 04 00 00    	jne    7a3 <ravi_vm_return>
 2a9:	4c 8b 65 20          	mov    0x20(%rbp),%r12
 2ad:	85 c0                	test   %eax,%eax
 2af:	74 09                	je     2ba <ravi_BC_RETURN+0x82>
 2b1:	4d 8b 6c 24 08       	mov    0x8(%r12),%r13
 2b6:	4c 89 6d 10          	mov    %r13,0x10(%rbp)
 2ba:	e9 be 04 00 00       	jmpq   77d <ravi_new_frame>

00000000000002bf <ravi_BC_FORLOOP>:
 2bf:	8b 03                	mov    (%rbx),%eax
 2c1:	0f b6 d0             	movzbl %al,%edx
 2c4:	48 83 c3 04          	add    $0x4,%rbx
 2c8:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000002cc <ravi_BC_FORPREP>:
 2cc:	8b 03                	mov    (%rbx),%eax
 2ce:	0f b6 d0             	movzbl %al,%edx
 2d1:	48 83 c3 04          	add    $0x4,%rbx
 2d5:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000002d9 <ravi_BC_TFORCALL>:
 2d9:	8b 03                	mov    (%rbx),%eax
 2db:	0f b6 d0             	movzbl %al,%edx
 2de:	48 83 c3 04          	add    $0x4,%rbx
 2e2:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000002e6 <ravi_BC_TFORLOOP>:
 2e6:	8b 03                	mov    (%rbx),%eax
 2e8:	0f b6 d0             	movzbl %al,%edx
 2eb:	48 83 c3 04          	add    $0x4,%rbx
 2ef:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000002f3 <ravi_BC_SETLIST>:
 2f3:	8b 03                	mov    (%rbx),%eax
 2f5:	0f b6 d0             	movzbl %al,%edx
 2f8:	48 83 c3 04          	add    $0x4,%rbx
 2fc:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000300 <ravi_BC_CLOSURE>:
 300:	8b 03                	mov    (%rbx),%eax
 302:	0f b6 d0             	movzbl %al,%edx
 305:	48 83 c3 04          	add    $0x4,%rbx
 309:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000030d <ravi_BC_VARARG>:
 30d:	8b 03                	mov    (%rbx),%eax
 30f:	0f b6 d0             	movzbl %al,%edx
 312:	48 83 c3 04          	add    $0x4,%rbx
 316:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000031a <ravi_BC_EXTRAARG>:
 31a:	8b 03                	mov    (%rbx),%eax
 31c:	0f b6 d0             	movzbl %al,%edx
 31f:	48 83 c3 04          	add    $0x4,%rbx
 323:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000327 <ravi_BC_NEWARRAYI>:
 327:	8b 03                	mov    (%rbx),%eax
 329:	0f b6 d0             	movzbl %al,%edx
 32c:	48 83 c3 04          	add    $0x4,%rbx
 330:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000334 <ravi_BC_NEWARRAYF>:
 334:	8b 03                	mov    (%rbx),%eax
 336:	0f b6 d0             	movzbl %al,%edx
 339:	48 83 c3 04          	add    $0x4,%rbx
 33d:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000341 <ravi_BC_LOADIZ>:
 341:	8b 03                	mov    (%rbx),%eax
 343:	0f b6 d0             	movzbl %al,%edx
 346:	48 83 c3 04          	add    $0x4,%rbx
 34a:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000034e <ravi_BC_LOADFZ>:
 34e:	8b 03                	mov    (%rbx),%eax
 350:	0f b6 d0             	movzbl %al,%edx
 353:	48 83 c3 04          	add    $0x4,%rbx
 357:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000035b <ravi_BC_UNMF>:
 35b:	8b 03                	mov    (%rbx),%eax
 35d:	0f b6 d0             	movzbl %al,%edx
 360:	48 83 c3 04          	add    $0x4,%rbx
 364:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000368 <ravi_BC_UNMI>:
 368:	8b 03                	mov    (%rbx),%eax
 36a:	0f b6 d0             	movzbl %al,%edx
 36d:	48 83 c3 04          	add    $0x4,%rbx
 371:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000375 <ravi_BC_ADDFF>:
 375:	8b 03                	mov    (%rbx),%eax
 377:	0f b6 d0             	movzbl %al,%edx
 37a:	48 83 c3 04          	add    $0x4,%rbx
 37e:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000382 <ravi_BC_ADDFI>:
 382:	8b 03                	mov    (%rbx),%eax
 384:	0f b6 d0             	movzbl %al,%edx
 387:	48 83 c3 04          	add    $0x4,%rbx
 38b:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000038f <ravi_BC_ADDII>:
 38f:	8b 03                	mov    (%rbx),%eax
 391:	0f b6 d0             	movzbl %al,%edx
 394:	48 83 c3 04          	add    $0x4,%rbx
 398:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000039c <ravi_BC_SUBFF>:
 39c:	8b 03                	mov    (%rbx),%eax
 39e:	0f b6 d0             	movzbl %al,%edx
 3a1:	48 83 c3 04          	add    $0x4,%rbx
 3a5:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000003a9 <ravi_BC_SUBFI>:
 3a9:	8b 03                	mov    (%rbx),%eax
 3ab:	0f b6 d0             	movzbl %al,%edx
 3ae:	48 83 c3 04          	add    $0x4,%rbx
 3b2:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000003b6 <ravi_BC_SUBIF>:
 3b6:	8b 03                	mov    (%rbx),%eax
 3b8:	0f b6 d0             	movzbl %al,%edx
 3bb:	48 83 c3 04          	add    $0x4,%rbx
 3bf:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000003c3 <ravi_BC_SUBII>:
 3c3:	8b 03                	mov    (%rbx),%eax
 3c5:	0f b6 d0             	movzbl %al,%edx
 3c8:	48 83 c3 04          	add    $0x4,%rbx
 3cc:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000003d0 <ravi_BC_MULFF>:
 3d0:	8b 03                	mov    (%rbx),%eax
 3d2:	0f b6 d0             	movzbl %al,%edx
 3d5:	48 83 c3 04          	add    $0x4,%rbx
 3d9:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000003dd <ravi_BC_MULFI>:
 3dd:	8b 03                	mov    (%rbx),%eax
 3df:	0f b6 d0             	movzbl %al,%edx
 3e2:	48 83 c3 04          	add    $0x4,%rbx
 3e6:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000003ea <ravi_BC_MULII>:
 3ea:	8b 03                	mov    (%rbx),%eax
 3ec:	0f b6 d0             	movzbl %al,%edx
 3ef:	48 83 c3 04          	add    $0x4,%rbx
 3f3:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000003f7 <ravi_BC_DIVFF>:
 3f7:	8b 03                	mov    (%rbx),%eax
 3f9:	0f b6 d0             	movzbl %al,%edx
 3fc:	48 83 c3 04          	add    $0x4,%rbx
 400:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000404 <ravi_BC_DIVFI>:
 404:	8b 03                	mov    (%rbx),%eax
 406:	0f b6 d0             	movzbl %al,%edx
 409:	48 83 c3 04          	add    $0x4,%rbx
 40d:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000411 <ravi_BC_DIVIF>:
 411:	8b 03                	mov    (%rbx),%eax
 413:	0f b6 d0             	movzbl %al,%edx
 416:	48 83 c3 04          	add    $0x4,%rbx
 41a:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000041e <ravi_BC_DIVII>:
 41e:	8b 03                	mov    (%rbx),%eax
 420:	0f b6 d0             	movzbl %al,%edx
 423:	48 83 c3 04          	add    $0x4,%rbx
 427:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000042b <ravi_BC_TOINT>:
 42b:	8b 03                	mov    (%rbx),%eax
 42d:	0f b6 d0             	movzbl %al,%edx
 430:	48 83 c3 04          	add    $0x4,%rbx
 434:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000438 <ravi_BC_TOFLT>:
 438:	8b 03                	mov    (%rbx),%eax
 43a:	0f b6 d0             	movzbl %al,%edx
 43d:	48 83 c3 04          	add    $0x4,%rbx
 441:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000445 <ravi_BC_TOARRAYI>:
 445:	8b 03                	mov    (%rbx),%eax
 447:	0f b6 d0             	movzbl %al,%edx
 44a:	48 83 c3 04          	add    $0x4,%rbx
 44e:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000452 <ravi_BC_TOARRAYF>:
 452:	8b 03                	mov    (%rbx),%eax
 454:	0f b6 d0             	movzbl %al,%edx
 457:	48 83 c3 04          	add    $0x4,%rbx
 45b:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000045f <ravi_BC_TOTAB>:
 45f:	8b 03                	mov    (%rbx),%eax
 461:	0f b6 d0             	movzbl %al,%edx
 464:	48 83 c3 04          	add    $0x4,%rbx
 468:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000046c <ravi_BC_TOSTRING>:
 46c:	8b 03                	mov    (%rbx),%eax
 46e:	0f b6 d0             	movzbl %al,%edx
 471:	48 83 c3 04          	add    $0x4,%rbx
 475:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000479 <ravi_BC_TOCLOSURE>:
 479:	8b 03                	mov    (%rbx),%eax
 47b:	0f b6 d0             	movzbl %al,%edx
 47e:	48 83 c3 04          	add    $0x4,%rbx
 482:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000486 <ravi_BC_TOTYPE>:
 486:	8b 03                	mov    (%rbx),%eax
 488:	0f b6 d0             	movzbl %al,%edx
 48b:	48 83 c3 04          	add    $0x4,%rbx
 48f:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000493 <ravi_BC_MOVEI>:
 493:	8b 03                	mov    (%rbx),%eax
 495:	0f b6 d0             	movzbl %al,%edx
 498:	48 83 c3 04          	add    $0x4,%rbx
 49c:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000004a0 <ravi_BC_MOVEF>:
 4a0:	8b 03                	mov    (%rbx),%eax
 4a2:	0f b6 d0             	movzbl %al,%edx
 4a5:	48 83 c3 04          	add    $0x4,%rbx
 4a9:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000004ad <ravi_BC_MOVEAI>:
 4ad:	8b 03                	mov    (%rbx),%eax
 4af:	0f b6 d0             	movzbl %al,%edx
 4b2:	48 83 c3 04          	add    $0x4,%rbx
 4b6:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000004ba <ravi_BC_MOVEAF>:
 4ba:	8b 03                	mov    (%rbx),%eax
 4bc:	0f b6 d0             	movzbl %al,%edx
 4bf:	48 83 c3 04          	add    $0x4,%rbx
 4c3:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000004c7 <ravi_BC_MOVETAB>:
 4c7:	8b 03                	mov    (%rbx),%eax
 4c9:	0f b6 d0             	movzbl %al,%edx
 4cc:	48 83 c3 04          	add    $0x4,%rbx
 4d0:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000004d4 <ravi_BC_GETTABLE_AI>:
 4d4:	8b 03                	mov    (%rbx),%eax
 4d6:	0f b6 d0             	movzbl %al,%edx
 4d9:	48 83 c3 04          	add    $0x4,%rbx
 4dd:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000004e1 <ravi_BC_GETTABLE_AF>:
 4e1:	8b 03                	mov    (%rbx),%eax
 4e3:	0f b6 d0             	movzbl %al,%edx
 4e6:	48 83 c3 04          	add    $0x4,%rbx
 4ea:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000004ee <ravi_BC_SETTABLE_AI>:
 4ee:	8b 03                	mov    (%rbx),%eax
 4f0:	0f b6 d0             	movzbl %al,%edx
 4f3:	48 83 c3 04          	add    $0x4,%rbx
 4f7:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000004fb <ravi_BC_SETTABLE_AF>:
 4fb:	8b 03                	mov    (%rbx),%eax
 4fd:	0f b6 d0             	movzbl %al,%edx
 500:	48 83 c3 04          	add    $0x4,%rbx
 504:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000508 <ravi_BC_SETTABLE_AII>:
 508:	8b 03                	mov    (%rbx),%eax
 50a:	0f b6 d0             	movzbl %al,%edx
 50d:	48 83 c3 04          	add    $0x4,%rbx
 511:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000515 <ravi_BC_SETTABLE_AFF>:
 515:	8b 03                	mov    (%rbx),%eax
 517:	0f b6 d0             	movzbl %al,%edx
 51a:	48 83 c3 04          	add    $0x4,%rbx
 51e:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000522 <ravi_BC_FORLOOP_IP>:
 522:	0f b6 cc             	movzbl %ah,%ecx
 525:	c1 e1 04             	shl    $0x4,%ecx
 528:	4d 8d 0c 08          	lea    (%r8,%rcx,1),%r9
 52c:	4d 8b 11             	mov    (%r9),%r10
 52f:	4d 03 51 20          	add    0x20(%r9),%r10
 533:	4d 3b 51 10          	cmp    0x10(%r9),%r10
 537:	7f 1f                	jg     558 <ravi_BC_FORLOOP_IP+0x36>
 539:	4d 89 11             	mov    %r10,(%r9)
 53c:	4d 8d 59 30          	lea    0x30(%r9),%r11
 540:	4d 89 13             	mov    %r10,(%r11)
 543:	66 41 c7 43 08 13 00 	movw   $0x13,0x8(%r11)
 54a:	c1 e8 10             	shr    $0x10,%eax
 54d:	0f b7 d0             	movzwl %ax,%edx
 550:	48 8d 9c 93 00 00 fe 	lea    -0x20000(%rbx,%rdx,4),%rbx
 557:	ff 
 558:	8b 03                	mov    (%rbx),%eax
 55a:	0f b6 d0             	movzbl %al,%edx
 55d:	48 83 c3 04          	add    $0x4,%rbx
 561:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000565 <ravi_BC_FORLOOP_I1>:
 565:	0f b6 cc             	movzbl %ah,%ecx
 568:	c1 e1 04             	shl    $0x4,%ecx
 56b:	4d 8d 0c 08          	lea    (%r8,%rcx,1),%r9
 56f:	4d 8b 11             	mov    (%r9),%r10
 572:	49 ff c2             	inc    %r10
 575:	4d 3b 51 10          	cmp    0x10(%r9),%r10
 579:	7f 1f                	jg     59a <ravi_BC_FORLOOP_I1+0x35>
 57b:	4d 89 11             	mov    %r10,(%r9)
 57e:	4d 8d 59 30          	lea    0x30(%r9),%r11
 582:	4d 89 13             	mov    %r10,(%r11)
 585:	66 41 c7 43 08 13 00 	movw   $0x13,0x8(%r11)
 58c:	c1 e8 10             	shr    $0x10,%eax
 58f:	0f b7 d0             	movzwl %ax,%edx
 592:	48 8d 9c 93 00 00 fe 	lea    -0x20000(%rbx,%rdx,4),%rbx
 599:	ff 
 59a:	8b 03                	mov    (%rbx),%eax
 59c:	0f b6 d0             	movzbl %al,%edx
 59f:	48 83 c3 04          	add    $0x4,%rbx
 5a3:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000005a7 <ravi_BC_FORPREP_IP>:
 5a7:	0f b6 cc             	movzbl %ah,%ecx
 5aa:	c1 e8 10             	shr    $0x10,%eax
 5ad:	0f b7 d0             	movzwl %ax,%edx
 5b0:	c1 e1 04             	shl    $0x4,%ecx
 5b3:	49 8d 04 08          	lea    (%r8,%rcx,1),%rax
 5b7:	4c 8b 10             	mov    (%rax),%r10
 5ba:	4c 2b 50 20          	sub    0x20(%rax),%r10
 5be:	4c 89 10             	mov    %r10,(%rax)
 5c1:	48 8d 9c 93 00 00 fe 	lea    -0x20000(%rbx,%rdx,4),%rbx
 5c8:	ff 
 5c9:	8b 03                	mov    (%rbx),%eax
 5cb:	0f b6 d0             	movzbl %al,%edx
 5ce:	48 83 c3 04          	add    $0x4,%rbx
 5d2:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000005d6 <ravi_BC_FORPREP_I1>:
 5d6:	0f b6 cc             	movzbl %ah,%ecx
 5d9:	c1 e8 10             	shr    $0x10,%eax
 5dc:	0f b7 d0             	movzwl %ax,%edx
 5df:	c1 e1 04             	shl    $0x4,%ecx
 5e2:	49 8d 04 08          	lea    (%r8,%rcx,1),%rax
 5e6:	4c 8b 10             	mov    (%rax),%r10
 5e9:	49 ff ca             	dec    %r10
 5ec:	4c 89 10             	mov    %r10,(%rax)
 5ef:	48 8d 9c 93 00 00 fe 	lea    -0x20000(%rbx,%rdx,4),%rbx
 5f6:	ff 
 5f7:	8b 03                	mov    (%rbx),%eax
 5f9:	0f b6 d0             	movzbl %al,%edx
 5fc:	48 83 c3 04          	add    $0x4,%rbx
 600:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000604 <ravi_BC_SETUPVALI>:
 604:	8b 03                	mov    (%rbx),%eax
 606:	0f b6 d0             	movzbl %al,%edx
 609:	48 83 c3 04          	add    $0x4,%rbx
 60d:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000611 <ravi_BC_SETUPVALF>:
 611:	8b 03                	mov    (%rbx),%eax
 613:	0f b6 d0             	movzbl %al,%edx
 616:	48 83 c3 04          	add    $0x4,%rbx
 61a:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000061e <ravi_BC_SETUPVALAI>:
 61e:	8b 03                	mov    (%rbx),%eax
 620:	0f b6 d0             	movzbl %al,%edx
 623:	48 83 c3 04          	add    $0x4,%rbx
 627:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000062b <ravi_BC_SETUPVALAF>:
 62b:	8b 03                	mov    (%rbx),%eax
 62d:	0f b6 d0             	movzbl %al,%edx
 630:	48 83 c3 04          	add    $0x4,%rbx
 634:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000638 <ravi_BC_SETUPVALT>:
 638:	8b 03                	mov    (%rbx),%eax
 63a:	0f b6 d0             	movzbl %al,%edx
 63d:	48 83 c3 04          	add    $0x4,%rbx
 641:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000645 <ravi_BC_BAND_II>:
 645:	8b 03                	mov    (%rbx),%eax
 647:	0f b6 d0             	movzbl %al,%edx
 64a:	48 83 c3 04          	add    $0x4,%rbx
 64e:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000652 <ravi_BC_BOR_II>:
 652:	8b 03                	mov    (%rbx),%eax
 654:	0f b6 d0             	movzbl %al,%edx
 657:	48 83 c3 04          	add    $0x4,%rbx
 65b:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000065f <ravi_BC_BXOR_II>:
 65f:	8b 03                	mov    (%rbx),%eax
 661:	0f b6 d0             	movzbl %al,%edx
 664:	48 83 c3 04          	add    $0x4,%rbx
 668:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000066c <ravi_BC_SHL_II>:
 66c:	8b 03                	mov    (%rbx),%eax
 66e:	0f b6 d0             	movzbl %al,%edx
 671:	48 83 c3 04          	add    $0x4,%rbx
 675:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000679 <ravi_BC_SHR_II>:
 679:	8b 03                	mov    (%rbx),%eax
 67b:	0f b6 d0             	movzbl %al,%edx
 67e:	48 83 c3 04          	add    $0x4,%rbx
 682:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000686 <ravi_BC_BNOT_I>:
 686:	8b 03                	mov    (%rbx),%eax
 688:	0f b6 d0             	movzbl %al,%edx
 68b:	48 83 c3 04          	add    $0x4,%rbx
 68f:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000693 <ravi_BC_EQ_II>:
 693:	8b 03                	mov    (%rbx),%eax
 695:	0f b6 d0             	movzbl %al,%edx
 698:	48 83 c3 04          	add    $0x4,%rbx
 69c:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000006a0 <ravi_BC_EQ_FF>:
 6a0:	8b 03                	mov    (%rbx),%eax
 6a2:	0f b6 d0             	movzbl %al,%edx
 6a5:	48 83 c3 04          	add    $0x4,%rbx
 6a9:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000006ad <ravi_BC_LT_II>:
 6ad:	8b 03                	mov    (%rbx),%eax
 6af:	0f b6 d0             	movzbl %al,%edx
 6b2:	48 83 c3 04          	add    $0x4,%rbx
 6b6:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000006ba <ravi_BC_LT_FF>:
 6ba:	8b 03                	mov    (%rbx),%eax
 6bc:	0f b6 d0             	movzbl %al,%edx
 6bf:	48 83 c3 04          	add    $0x4,%rbx
 6c3:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000006c7 <ravi_BC_LE_II>:
 6c7:	8b 03                	mov    (%rbx),%eax
 6c9:	0f b6 d0             	movzbl %al,%edx
 6cc:	48 83 c3 04          	add    $0x4,%rbx
 6d0:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000006d4 <ravi_BC_LE_FF>:
 6d4:	8b 03                	mov    (%rbx),%eax
 6d6:	0f b6 d0             	movzbl %al,%edx
 6d9:	48 83 c3 04          	add    $0x4,%rbx
 6dd:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000006e1 <ravi_BC_GETTABLE_S>:
 6e1:	8b 03                	mov    (%rbx),%eax
 6e3:	0f b6 d0             	movzbl %al,%edx
 6e6:	48 83 c3 04          	add    $0x4,%rbx
 6ea:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000006ee <ravi_BC_SETTABLE_S>:
 6ee:	8b 03                	mov    (%rbx),%eax
 6f0:	0f b6 d0             	movzbl %al,%edx
 6f3:	48 83 c3 04          	add    $0x4,%rbx
 6f7:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000006fb <ravi_BC_SELF_S>:
 6fb:	8b 03                	mov    (%rbx),%eax
 6fd:	0f b6 d0             	movzbl %al,%edx
 700:	48 83 c3 04          	add    $0x4,%rbx
 704:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000708 <ravi_BC_GETTABLE_I>:
 708:	8b 03                	mov    (%rbx),%eax
 70a:	0f b6 d0             	movzbl %al,%edx
 70d:	48 83 c3 04          	add    $0x4,%rbx
 711:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000715 <ravi_BC_SETTABLE_I>:
 715:	8b 03                	mov    (%rbx),%eax
 717:	0f b6 d0             	movzbl %al,%edx
 71a:	48 83 c3 04          	add    $0x4,%rbx
 71e:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000722 <ravi_BC_GETTABLE_SK>:
 722:	8b 03                	mov    (%rbx),%eax
 724:	0f b6 d0             	movzbl %al,%edx
 727:	48 83 c3 04          	add    $0x4,%rbx
 72b:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000072f <ravi_BC_SELF_SK>:
 72f:	8b 03                	mov    (%rbx),%eax
 731:	0f b6 d0             	movzbl %al,%edx
 734:	48 83 c3 04          	add    $0x4,%rbx
 738:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

000000000000073c <ravi_BC_SETTABLE_SK>:
 73c:	8b 03                	mov    (%rbx),%eax
 73e:	0f b6 d0             	movzbl %al,%edx
 741:	48 83 c3 04          	add    $0x4,%rbx
 745:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000749 <ravi_BC_GETTABUP_SK>:
 749:	8b 03                	mov    (%rbx),%eax
 74b:	0f b6 d0             	movzbl %al,%edx
 74e:	48 83 c3 04          	add    $0x4,%rbx
 752:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

0000000000000756 <ravi_luaV_interp>:
 756:	55                   	push   %rbp
 757:	53                   	push   %rbx
 758:	41 57                	push   %r15
 75a:	41 56                	push   %r14
 75c:	41 55                	push   %r13
 75e:	41 54                	push   %r12
 760:	48 83 ec 28          	sub    $0x28,%rsp
 764:	48 89 fd             	mov    %rdi,%rbp
 767:	4c 8b 75 18          	mov    0x18(%rbp),%r14
 76b:	49 81 c6 40 05 00 00 	add    $0x540,%r14
 772:	4c 8b 65 20          	mov    0x20(%rbp),%r12
 776:	66 41 83 4c 24 42 08 	orw    $0x8,0x42(%r12)

000000000000077d <ravi_new_frame>:
 77d:	4d 8b 14 24          	mov    (%r12),%r10
 781:	4d 8b 2a             	mov    (%r10),%r13
 784:	4d 8b 44 24 20       	mov    0x20(%r12),%r8
 789:	49 8b 5d 18          	mov    0x18(%r13),%rbx
 78d:	4c 8b 7b 30          	mov    0x30(%rbx),%r15
 791:	49 8b 5c 24 28       	mov    0x28(%r12),%rbx
 796:	8b 03                	mov    (%rbx),%eax
 798:	0f b6 d0             	movzbl %al,%edx
 79b:	48 83 c3 04          	add    $0x4,%rbx
 79f:	41 ff 24 d6          	jmpq   *(%r14,%rdx,8)

00000000000007a3 <ravi_vm_return>:
 7a3:	48 83 c4 28          	add    $0x28,%rsp
 7a7:	41 5c                	pop    %r12
 7a9:	41 5d                	pop    %r13
 7ab:	41 5e                	pop    %r14
 7ad:	41 5f                	pop    %r15
 7af:	5b                   	pop    %rbx
 7b0:	5d                   	pop    %rbp
 7b1:	c3                   	retq   
