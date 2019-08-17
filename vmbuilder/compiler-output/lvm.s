
CMakeFiles/libravinojit_static.dir/src/lvm.c.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <getcached>:
** check whether cached closure in prototype 'p' may be reused, that is,
** whether there is a cached closure with the same upvalues needed by
** new closure to be created.
*/
static LClosure *getcached (Proto *p, UpVal **encup, StkId base) {
  LClosure *c = p->cache;
       0:	48 8b 47 60          	mov    rax,QWORD PTR [rdi+0x60]
  if (c != NULL) {  /* is there a cached closure? */
       4:	48 85 c0             	test   rax,rax
       7:	74 5d                	je     66 <getcached+0x66>
    int nup = p->sizeupvalues;
       9:	8b 4f 10             	mov    ecx,DWORD PTR [rdi+0x10]
    Upvaldesc *uv = p->upvalues;
       c:	48 8b 7f 58          	mov    rdi,QWORD PTR [rdi+0x58]
    int i;
    for (i = 0; i < nup; i++) {  /* check whether it has right upvalues */
      10:	85 c9                	test   ecx,ecx
      12:	7e 52                	jle    66 <getcached+0x66>
      14:	83 e9 01             	sub    ecx,0x1
      17:	48 83 c7 11          	add    rdi,0x11
      1b:	4c 8d 40 20          	lea    r8,[rax+0x20]
      1f:	4c 8d 54 c8 28       	lea    r10,[rax+rcx*8+0x28]
      24:	eb 26                	jmp    4c <getcached+0x4c>
      26:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
      2d:	00 00 00 
      TValue *v = uv[i].instack ? base + uv[i].idx : encup[uv[i].idx]->v;
      if (c->upvals[i]->v != v)
      30:	4d 8b 08             	mov    r9,QWORD PTR [r8]
      TValue *v = uv[i].instack ? base + uv[i].idx : encup[uv[i].idx]->v;
      33:	48 c1 e1 04          	shl    rcx,0x4
      37:	48 01 d1             	add    rcx,rdx
      if (c->upvals[i]->v != v)
      3a:	49 39 09             	cmp    QWORD PTR [r9],rcx
      3d:	75 25                	jne    64 <getcached+0x64>
      3f:	49 83 c0 08          	add    r8,0x8
      43:	48 83 c7 18          	add    rdi,0x18
    for (i = 0; i < nup; i++) {  /* check whether it has right upvalues */
      47:	4d 39 d0             	cmp    r8,r10
      4a:	74 24                	je     70 <getcached+0x70>
      TValue *v = uv[i].instack ? base + uv[i].idx : encup[uv[i].idx]->v;
      4c:	80 3f 00             	cmp    BYTE PTR [rdi],0x0
      4f:	0f b6 4f 01          	movzx  ecx,BYTE PTR [rdi+0x1]
      53:	75 db                	jne    30 <getcached+0x30>
      55:	48 8b 0c ce          	mov    rcx,QWORD PTR [rsi+rcx*8]
      if (c->upvals[i]->v != v)
      59:	4d 8b 08             	mov    r9,QWORD PTR [r8]
      TValue *v = uv[i].instack ? base + uv[i].idx : encup[uv[i].idx]->v;
      5c:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
      if (c->upvals[i]->v != v)
      5f:	49 39 09             	cmp    QWORD PTR [r9],rcx
      62:	74 db                	je     3f <getcached+0x3f>
      64:	31 c0                	xor    eax,eax
        return NULL;  /* wrong upvalue; cannot reuse closure */
    }
  }
  return c;  /* return cached closure (or NULL if no cached closure) */
}
      66:	c3                   	ret    
      67:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
      6e:	00 00 
      70:	c3                   	ret    
      71:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
      78:	00 00 00 00 
      7c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000000080 <l_strcmp>:
static int l_strcmp (const TString *ls, const TString *rs) {
      80:	41 55                	push   r13
      82:	41 54                	push   r12
      84:	55                   	push   rbp
      85:	53                   	push   rbx
  const char *l = getstr(ls);
      86:	48 8d 5f 18          	lea    rbx,[rdi+0x18]
static int l_strcmp (const TString *ls, const TString *rs) {
      8a:	48 83 ec 08          	sub    rsp,0x8
  size_t ll = tsslen(ls);
      8e:	80 7f 08 04          	cmp    BYTE PTR [rdi+0x8],0x4
      92:	74 5c                	je     f0 <l_strcmp+0x70>
  size_t lr = tsslen(rs);
      94:	80 7e 08 04          	cmp    BYTE PTR [rsi+0x8],0x4
  size_t ll = tsslen(ls);
      98:	4c 8b 6f 10          	mov    r13,QWORD PTR [rdi+0x10]
  const char *r = getstr(rs);
      9c:	48 8d 6e 18          	lea    rbp,[rsi+0x18]
  size_t lr = tsslen(rs);
      a0:	74 5d                	je     ff <l_strcmp+0x7f>
      a2:	4c 8b 66 10          	mov    r12,QWORD PTR [rsi+0x10]
      a6:	eb 2a                	jmp    d2 <l_strcmp+0x52>
      a8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
      af:	00 
      size_t len = strlen(l);  /* index of first '\0' in both strings */
      b0:	48 89 df             	mov    rdi,rbx
      b3:	e8 00 00 00 00       	call   b8 <l_strcmp+0x38>
      if (len == lr)  /* 'rs' is finished? */
      b8:	49 39 c4             	cmp    r12,rax
      bb:	74 53                	je     110 <l_strcmp+0x90>
      else if (len == ll)  /* 'ls' is finished? */
      bd:	49 39 c5             	cmp    r13,rax
      c0:	74 66                	je     128 <l_strcmp+0xa8>
      len++;
      c2:	48 83 c0 01          	add    rax,0x1
      l += len; ll -= len; r += len; lr -= len;
      c6:	48 01 c3             	add    rbx,rax
      c9:	49 29 c5             	sub    r13,rax
      cc:	48 01 c5             	add    rbp,rax
      cf:	49 29 c4             	sub    r12,rax
    int temp = strcoll(l, r);
      d2:	48 89 ee             	mov    rsi,rbp
      d5:	48 89 df             	mov    rdi,rbx
      d8:	e8 00 00 00 00       	call   dd <l_strcmp+0x5d>
    if (temp != 0)  /* not equal? */
      dd:	85 c0                	test   eax,eax
      df:	74 cf                	je     b0 <l_strcmp+0x30>
}
      e1:	48 83 c4 08          	add    rsp,0x8
      e5:	5b                   	pop    rbx
      e6:	5d                   	pop    rbp
      e7:	41 5c                	pop    r12
      e9:	41 5d                	pop    r13
      eb:	c3                   	ret    
      ec:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  size_t lr = tsslen(rs);
      f0:	80 7e 08 04          	cmp    BYTE PTR [rsi+0x8],0x4
  size_t ll = tsslen(ls);
      f4:	44 0f b6 6f 0b       	movzx  r13d,BYTE PTR [rdi+0xb]
  const char *r = getstr(rs);
      f9:	48 8d 6e 18          	lea    rbp,[rsi+0x18]
  size_t lr = tsslen(rs);
      fd:	75 a3                	jne    a2 <l_strcmp+0x22>
      ff:	44 0f b6 66 0b       	movzx  r12d,BYTE PTR [rsi+0xb]
     104:	eb cc                	jmp    d2 <l_strcmp+0x52>
     106:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
     10d:	00 00 00 
        return (len == ll) ? 0 : 1;  /* check 'ls' */
     110:	31 c0                	xor    eax,eax
     112:	4d 39 e5             	cmp    r13,r12
     115:	0f 95 c0             	setne  al
}
     118:	48 83 c4 08          	add    rsp,0x8
     11c:	5b                   	pop    rbx
     11d:	5d                   	pop    rbp
     11e:	41 5c                	pop    r12
     120:	41 5d                	pop    r13
     122:	c3                   	ret    
     123:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
     128:	48 83 c4 08          	add    rsp,0x8
        return -1;  /* 'ls' is smaller than 'rs' ('rs' is not finished) */
     12c:	b8 ff ff ff ff       	mov    eax,0xffffffff
}
     131:	5b                   	pop    rbx
     132:	5d                   	pop    rbp
     133:	41 5c                	pop    r12
     135:	41 5d                	pop    r13
     137:	c3                   	ret    
     138:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
     13f:	00 

0000000000000140 <copy2buff>:
static void copy2buff (StkId top, int n, char *buff) {
     140:	41 56                	push   r14
     142:	41 55                	push   r13
     144:	49 89 d5             	mov    r13,rdx
     147:	48 63 d6             	movsxd rdx,esi
     14a:	48 89 d0             	mov    rax,rdx
     14d:	41 54                	push   r12
     14f:	48 c1 e0 04          	shl    rax,0x4
     153:	55                   	push   rbp
  size_t tl = 0;  /* size already copied */
     154:	31 ed                	xor    ebp,ebp
static void copy2buff (StkId top, int n, char *buff) {
     156:	53                   	push   rbx
     157:	48 89 fb             	mov    rbx,rdi
     15a:	48 29 c3             	sub    rbx,rax
     15d:	8d 46 ff             	lea    eax,[rsi-0x1]
     160:	48 29 d0             	sub    rax,rdx
     163:	48 c1 e0 04          	shl    rax,0x4
     167:	4c 8d 64 07 10       	lea    r12,[rdi+rax*1+0x10]
     16c:	eb 23                	jmp    191 <copy2buff+0x51>
     16e:	66 90                	xchg   ax,ax
    size_t l = vslen(top - n);  /* length of string being copied */
     170:	4c 8b 76 10          	mov    r14,QWORD PTR [rsi+0x10]
    memcpy(buff + tl, svalue(top - n), l * sizeof(char));
     174:	49 8d 7c 2d 00       	lea    rdi,[r13+rbp*1+0x0]

__fortify_function void *
__NTH (memcpy (void *__restrict __dest, const void *__restrict __src,
	       size_t __len))
{
  return __builtin___memcpy_chk (__dest, __src, __len, __bos0 (__dest));
     179:	48 83 c6 18          	add    rsi,0x18
     17d:	4c 89 f2             	mov    rdx,r14
     180:	48 83 c3 10          	add    rbx,0x10
     184:	e8 00 00 00 00       	call   189 <copy2buff+0x49>
    tl += l;
     189:	4c 01 f5             	add    rbp,r14
  } while (--n > 0);
     18c:	4c 39 e3             	cmp    rbx,r12
     18f:	74 17                	je     1a8 <copy2buff+0x68>
    size_t l = vslen(top - n);  /* length of string being copied */
     191:	48 8b 33             	mov    rsi,QWORD PTR [rbx]
     194:	80 7e 08 04          	cmp    BYTE PTR [rsi+0x8],0x4
     198:	75 d6                	jne    170 <copy2buff+0x30>
     19a:	44 0f b6 76 0b       	movzx  r14d,BYTE PTR [rsi+0xb]
     19f:	eb d3                	jmp    174 <copy2buff+0x34>
     1a1:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
}
     1a8:	5b                   	pop    rbx
     1a9:	5d                   	pop    rbp
     1aa:	41 5c                	pop    r12
     1ac:	41 5d                	pop    r13
     1ae:	41 5e                	pop    r14
     1b0:	c3                   	ret    
     1b1:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
     1b8:	00 00 00 00 
     1bc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

00000000000001c0 <pushclosure.isra.6>:
** create a new Lua closure, push it in the stack, and initialize
** its upvalues. Note that the closure is not cached if prototype is
** already black (which means that 'cache' was already cleared by the
** GC).
*/
static void pushclosure (lua_State *L, Proto *p, UpVal **encup, StkId base,
     1c0:	41 57                	push   r15
     1c2:	41 56                	push   r14
     1c4:	4d 89 ce             	mov    r14,r9
     1c7:	41 55                	push   r13
     1c9:	41 54                	push   r12
     1cb:	49 89 f4             	mov    r12,rsi
     1ce:	55                   	push   rbp
     1cf:	48 89 d5             	mov    rbp,rdx
     1d2:	53                   	push   rbx
     1d3:	48 83 ec 28          	sub    rsp,0x28
                         StkId ra) {
  int nup = p->sizeupvalues;
     1d7:	44 8b 7e 10          	mov    r15d,DWORD PTR [rsi+0x10]
  Upvaldesc *uv = p->upvalues;
     1db:	48 8b 5e 58          	mov    rbx,QWORD PTR [rsi+0x58]
static void pushclosure (lua_State *L, Proto *p, UpVal **encup, StkId base,
     1df:	4c 89 44 24 18       	mov    QWORD PTR [rsp+0x18],r8
  int i;
  LClosure *ncl = luaF_newLclosure(L, nup);
     1e4:	44 89 fe             	mov    esi,r15d
static void pushclosure (lua_State *L, Proto *p, UpVal **encup, StkId base,
     1e7:	48 89 7c 24 08       	mov    QWORD PTR [rsp+0x8],rdi
     1ec:	48 89 4c 24 10       	mov    QWORD PTR [rsp+0x10],rcx
  LClosure *ncl = luaF_newLclosure(L, nup);
     1f1:	e8 00 00 00 00       	call   1f6 <pushclosure.isra.6+0x36>
  ncl->p = p;
  setclLvalue(L, ra, ncl);  /* anchor new closure in stack */
     1f6:	4c 8b 44 24 18       	mov    r8,QWORD PTR [rsp+0x18]
  ncl->p = p;
     1fb:	4c 89 60 18          	mov    QWORD PTR [rax+0x18],r12
  LClosure *ncl = luaF_newLclosure(L, nup);
     1ff:	49 89 c5             	mov    r13,rax
  setclLvalue(L, ra, ncl);  /* anchor new closure in stack */
     202:	49 89 00             	mov    QWORD PTR [r8],rax
     205:	b8 06 80 ff ff       	mov    eax,0xffff8006
     20a:	66 41 89 06          	mov    WORD PTR [r14],ax
  for (i = 0; i < nup; i++) {  /* fill in its upvalues */
     20e:	45 85 ff             	test   r15d,r15d
     211:	7e 5d                	jle    270 <pushclosure.isra.6+0xb0>
     213:	41 8d 47 ff          	lea    eax,[r15-0x1]
     217:	4d 8d 75 20          	lea    r14,[r13+0x20]
     21b:	48 83 c3 12          	add    rbx,0x12
     21f:	4d 8d 7c c5 28       	lea    r15,[r13+rax*8+0x28]
     224:	eb 24                	jmp    24a <pushclosure.isra.6+0x8a>
     226:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
     22d:	00 00 00 
    if (uv[i].instack)  /* upvalue refers to local variable? */
      ncl->upvals[i] = luaF_findupval(L, base + uv[i].idx);
    else  /* get upvalue from enclosing function */
      ncl->upvals[i] = encup[uv[i].idx];
     230:	48 8b 44 f5 00       	mov    rax,QWORD PTR [rbp+rsi*8+0x0]
     235:	49 89 06             	mov    QWORD PTR [r14],rax
    ncl->upvals[i]->refcount++;
     238:	49 83 c6 08          	add    r14,0x8
     23c:	48 83 40 08 01       	add    QWORD PTR [rax+0x8],0x1
     241:	48 83 c3 18          	add    rbx,0x18
  for (i = 0; i < nup; i++) {  /* fill in its upvalues */
     245:	4d 39 fe             	cmp    r14,r15
     248:	74 26                	je     270 <pushclosure.isra.6+0xb0>
    if (uv[i].instack)  /* upvalue refers to local variable? */
     24a:	80 7b ff 00          	cmp    BYTE PTR [rbx-0x1],0x0
     24e:	0f b6 33             	movzx  esi,BYTE PTR [rbx]
     251:	74 dd                	je     230 <pushclosure.isra.6+0x70>
      ncl->upvals[i] = luaF_findupval(L, base + uv[i].idx);
     253:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
     258:	48 c1 e6 04          	shl    rsi,0x4
     25c:	48 03 74 24 10       	add    rsi,QWORD PTR [rsp+0x10]
     261:	e8 00 00 00 00       	call   266 <pushclosure.isra.6+0xa6>
     266:	49 89 06             	mov    QWORD PTR [r14],rax
     269:	eb cd                	jmp    238 <pushclosure.isra.6+0x78>
     26b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    /* new closure is white, so we do not need a barrier here */
  }
  if (!isblack(p))  /* cache will not break GC invariant? */
     270:	41 f6 44 24 09 04    	test   BYTE PTR [r12+0x9],0x4
     276:	75 05                	jne    27d <pushclosure.isra.6+0xbd>
    p->cache = ncl;  /* save it on cache for reuse */
     278:	4d 89 6c 24 60       	mov    QWORD PTR [r12+0x60],r13
}
     27d:	48 83 c4 28          	add    rsp,0x28
     281:	5b                   	pop    rbx
     282:	5d                   	pop    rbp
     283:	41 5c                	pop    r12
     285:	41 5d                	pop    r13
     287:	41 5e                	pop    r14
     289:	41 5f                	pop    r15
     28b:	c3                   	ret    
     28c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000000290 <luaV_flttointeger.part.7>:
    lua_Number n = fltvalue(obj);
     290:	f2 0f 10 0f          	movsd  xmm1,QWORD PTR [rdi]
    lua_Number f = l_floor(n);
     294:	f2 0f 10 1d 00 00 00 	movsd  xmm3,QWORD PTR [rip+0x0]        # 29c <luaV_flttointeger.part.7+0xc>
     29b:	00 
     29c:	f2 0f 10 25 00 00 00 	movsd  xmm4,QWORD PTR [rip+0x0]        # 2a4 <luaV_flttointeger.part.7+0x14>
     2a3:	00 
     2a4:	66 0f 28 d1          	movapd xmm2,xmm1
     2a8:	66 0f 28 c1          	movapd xmm0,xmm1
     2ac:	66 0f 54 d3          	andpd  xmm2,xmm3
     2b0:	66 0f 2e e2          	ucomisd xmm4,xmm2
     2b4:	76 33                	jbe    2e9 <luaV_flttointeger.part.7+0x59>
     2b6:	f2 48 0f 2c c1       	cvttsd2si rax,xmm1
     2bb:	66 0f ef d2          	pxor   xmm2,xmm2
     2bf:	f2 0f 10 25 00 00 00 	movsd  xmm4,QWORD PTR [rip+0x0]        # 2c7 <luaV_flttointeger.part.7+0x37>
     2c6:	00 
     2c7:	66 0f 55 d9          	andnpd xmm3,xmm1
     2cb:	f2 48 0f 2a d0       	cvtsi2sd xmm2,rax
     2d0:	66 0f 28 c2          	movapd xmm0,xmm2
     2d4:	f2 0f c2 c1 06       	cmpnlesd xmm0,xmm1
     2d9:	66 0f 54 c4          	andpd  xmm0,xmm4
     2dd:	f2 0f 5c d0          	subsd  xmm2,xmm0
     2e1:	66 0f 28 c2          	movapd xmm0,xmm2
     2e5:	66 0f 56 c3          	orpd   xmm0,xmm3
    if (n != f) {  /* not an integral value? */
     2e9:	66 0f 2e c8          	ucomisd xmm1,xmm0
     2ed:	7a 02                	jp     2f1 <luaV_flttointeger.part.7+0x61>
     2ef:	74 1c                	je     30d <luaV_flttointeger.part.7+0x7d>
      if (mode == 0) return 0;  /* fails if mode demands integral value */
     2f1:	31 c0                	xor    eax,eax
     2f3:	85 d2                	test   edx,edx
     2f5:	75 09                	jne    300 <luaV_flttointeger.part.7+0x70>
}
     2f7:	c3                   	ret    
     2f8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
     2ff:	00 
      else if (mode > 1)  /* needs ceil? */
     300:	83 fa 01             	cmp    edx,0x1
     303:	7e 08                	jle    30d <luaV_flttointeger.part.7+0x7d>
        f += 1;  /* convert floor to ceil (remember: n != f) */
     305:	f2 0f 58 05 00 00 00 	addsd  xmm0,QWORD PTR [rip+0x0]        # 30d <luaV_flttointeger.part.7+0x7d>
     30c:	00 
    return lua_numbertointeger(f, p);
     30d:	66 0f 2f 05 00 00 00 	comisd xmm0,QWORD PTR [rip+0x0]        # 315 <luaV_flttointeger.part.7+0x85>
     314:	00 
     315:	72 21                	jb     338 <luaV_flttointeger.part.7+0xa8>
     317:	f2 0f 10 0d 00 00 00 	movsd  xmm1,QWORD PTR [rip+0x0]        # 31f <luaV_flttointeger.part.7+0x8f>
     31e:	00 
     31f:	66 0f 2f c8          	comisd xmm1,xmm0
     323:	76 1b                	jbe    340 <luaV_flttointeger.part.7+0xb0>
     325:	f2 48 0f 2c c0       	cvttsd2si rax,xmm0
     32a:	48 89 06             	mov    QWORD PTR [rsi],rax
     32d:	b8 01 00 00 00       	mov    eax,0x1
     332:	c3                   	ret    
     333:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
      if (mode == 0) return 0;  /* fails if mode demands integral value */
     338:	31 c0                	xor    eax,eax
     33a:	c3                   	ret    
     33b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
     340:	31 c0                	xor    eax,eax
}
     342:	c3                   	ret    
     343:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
     34a:	00 00 00 00 
     34e:	66 90                	xchg   ax,ax

0000000000000350 <LEnum.isra.4>:
  if (ttisinteger(l)) {
     350:	66 83 fe 13          	cmp    si,0x13
     354:	74 72                	je     3c8 <LEnum.isra.4+0x78>
    lua_Number lf = fltvalue(l);  /* 'l' must be float */
     356:	48 89 7c 24 f8       	mov    QWORD PTR [rsp-0x8],rdi
     35b:	f2 0f 10 44 24 f8    	movsd  xmm0,QWORD PTR [rsp-0x8]
    if (ttisfloat(r))
     361:	66 83 f9 03          	cmp    cx,0x3
     365:	0f 84 ed 00 00 00    	je     458 <LEnum.isra.4+0x108>
    else if (luai_numisnan(lf))  /* 'r' is int and 'l' is float */
     36b:	66 0f 2e c0          	ucomisd xmm0,xmm0
     36f:	7a 4f                	jp     3c0 <LEnum.isra.4+0x70>
     371:	75 4d                	jne    3c0 <LEnum.isra.4+0x70>
  if (!l_intfitsf(i)) {
     373:	48 b8 00 00 00 00 00 	movabs rax,0x20000000000000
     37a:	00 20 00 
     37d:	48 b9 00 00 00 00 00 	movabs rcx,0x40000000000000
     384:	00 40 00 
     387:	48 01 d0             	add    rax,rdx
     38a:	48 39 c8             	cmp    rax,rcx
     38d:	0f 86 95 00 00 00    	jbe    428 <LEnum.isra.4+0xd8>
    if (f >= -cast_num(LUA_MININTEGER))  /* -minint == maxint + 1 */
     393:	31 c0                	xor    eax,eax
     395:	66 0f 2f 05 00 00 00 	comisd xmm0,QWORD PTR [rip+0x0]        # 39d <LEnum.isra.4+0x4d>
     39c:	00 
     39d:	73 23                	jae    3c2 <LEnum.isra.4+0x72>
    else if (f > cast_num(LUA_MININTEGER))  /* minint < f <= maxint ? */
     39f:	66 0f 2f 05 00 00 00 	comisd xmm0,QWORD PTR [rip+0x0]        # 3a7 <LEnum.isra.4+0x57>
     3a6:	00 
     3a7:	0f 86 d3 00 00 00    	jbe    480 <LEnum.isra.4+0x130>
      return (i < cast(lua_Integer, f));  /* compare them as integers */
     3ad:	f2 48 0f 2c c0       	cvttsd2si rax,xmm0
     3b2:	48 39 c2             	cmp    rdx,rax
     3b5:	0f 9d c0             	setge  al
     3b8:	0f b6 c0             	movzx  eax,al
     3bb:	c3                   	ret    
     3bc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
      return 0;
     3c0:	31 c0                	xor    eax,eax
}
     3c2:	c3                   	ret    
     3c3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    if (ttisinteger(r))
     3c8:	66 83 f9 13          	cmp    cx,0x13
     3cc:	0f 84 9e 00 00 00    	je     470 <LEnum.isra.4+0x120>
  if (!l_intfitsf(i)) {
     3d2:	48 b8 00 00 00 00 00 	movabs rax,0x20000000000000
     3d9:	00 20 00 
      return LEintfloat(li, fltvalue(r));  /* l <= r ? */
     3dc:	48 89 54 24 f8       	mov    QWORD PTR [rsp-0x8],rdx
  if (!l_intfitsf(i)) {
     3e1:	48 ba 00 00 00 00 00 	movabs rdx,0x40000000000000
     3e8:	00 40 00 
      return LEintfloat(li, fltvalue(r));  /* l <= r ? */
     3eb:	f2 0f 10 4c 24 f8    	movsd  xmm1,QWORD PTR [rsp-0x8]
  if (!l_intfitsf(i)) {
     3f1:	48 01 f8             	add    rax,rdi
     3f4:	48 39 d0             	cmp    rax,rdx
     3f7:	76 47                	jbe    440 <LEnum.isra.4+0xf0>
    if (f >= -cast_num(LUA_MININTEGER))  /* -minint == maxint + 1 */
     3f9:	66 0f 2f 0d 00 00 00 	comisd xmm1,QWORD PTR [rip+0x0]        # 401 <LEnum.isra.4+0xb1>
     400:	00 
      return 1;  /* f >= maxint + 1 > i */
     401:	b8 01 00 00 00       	mov    eax,0x1
    if (f >= -cast_num(LUA_MININTEGER))  /* -minint == maxint + 1 */
     406:	73 ba                	jae    3c2 <LEnum.isra.4+0x72>
    else if (f >= cast_num(LUA_MININTEGER))  /* minint <= f <= maxint ? */
     408:	66 0f 2f 0d 00 00 00 	comisd xmm1,QWORD PTR [rip+0x0]        # 410 <LEnum.isra.4+0xc0>
     40f:	00 
     410:	72 7e                	jb     490 <LEnum.isra.4+0x140>
      return (i <= cast(lua_Integer, f));  /* compare them as integers */
     412:	f2 48 0f 2c c1       	cvttsd2si rax,xmm1
     417:	48 39 c7             	cmp    rdi,rax
     41a:	0f 9e c0             	setle  al
     41d:	0f b6 c0             	movzx  eax,al
     420:	c3                   	ret    
     421:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  return luai_numlt(cast_num(i), f);  /* compare them as floats */
     428:	66 0f ef c9          	pxor   xmm1,xmm1
     42c:	31 c0                	xor    eax,eax
     42e:	f2 48 0f 2a ca       	cvtsi2sd xmm1,rdx
     433:	66 0f 2f c1          	comisd xmm0,xmm1
     437:	0f 96 c0             	setbe  al
     43a:	c3                   	ret    
     43b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  return luai_numle(cast_num(i), f);  /* compare them as floats */
     440:	66 0f ef c0          	pxor   xmm0,xmm0
     444:	31 c0                	xor    eax,eax
     446:	f2 48 0f 2a c7       	cvtsi2sd xmm0,rdi
     44b:	66 0f 2f c8          	comisd xmm1,xmm0
     44f:	0f 93 c0             	setae  al
     452:	c3                   	ret    
     453:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
      return luai_numle(lf, fltvalue(r));  /* both are float */
     458:	48 89 54 24 f8       	mov    QWORD PTR [rsp-0x8],rdx
     45d:	f2 0f 10 4c 24 f8    	movsd  xmm1,QWORD PTR [rsp-0x8]
     463:	31 c0                	xor    eax,eax
     465:	66 0f 2f c8          	comisd xmm1,xmm0
     469:	0f 93 c0             	setae  al
     46c:	c3                   	ret    
     46d:	0f 1f 00             	nop    DWORD PTR [rax]
      return li <= ivalue(r);  /* both are integers */
     470:	31 c0                	xor    eax,eax
     472:	48 39 d7             	cmp    rdi,rdx
     475:	0f 9e c0             	setle  al
     478:	c3                   	ret    
     479:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    else if (f > cast_num(LUA_MININTEGER))  /* minint < f <= maxint ? */
     480:	b8 01 00 00 00       	mov    eax,0x1
     485:	c3                   	ret    
     486:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
     48d:	00 00 00 
      return 0;
     490:	31 c0                	xor    eax,eax
     492:	c3                   	ret    
     493:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
     49a:	00 00 00 00 
     49e:	66 90                	xchg   ax,ax

00000000000004a0 <LTnum.isra.2>:
  if (ttisinteger(l)) {
     4a0:	66 83 fe 13          	cmp    si,0x13
     4a4:	74 72                	je     518 <LTnum.isra.2+0x78>
    lua_Number lf = fltvalue(l);  /* 'l' must be float */
     4a6:	48 89 7c 24 f8       	mov    QWORD PTR [rsp-0x8],rdi
     4ab:	f2 0f 10 44 24 f8    	movsd  xmm0,QWORD PTR [rsp-0x8]
    if (ttisfloat(r))
     4b1:	66 83 f9 03          	cmp    cx,0x3
     4b5:	0f 84 ed 00 00 00    	je     5a8 <LTnum.isra.2+0x108>
    else if (luai_numisnan(lf))  /* 'r' is int and 'l' is float */
     4bb:	66 0f 2e c0          	ucomisd xmm0,xmm0
     4bf:	7a 4f                	jp     510 <LTnum.isra.2+0x70>
     4c1:	75 4d                	jne    510 <LTnum.isra.2+0x70>
  if (!l_intfitsf(i)) {
     4c3:	48 b8 00 00 00 00 00 	movabs rax,0x20000000000000
     4ca:	00 20 00 
     4cd:	48 b9 00 00 00 00 00 	movabs rcx,0x40000000000000
     4d4:	00 40 00 
     4d7:	48 01 d0             	add    rax,rdx
     4da:	48 39 c8             	cmp    rax,rcx
     4dd:	0f 86 95 00 00 00    	jbe    578 <LTnum.isra.2+0xd8>
    if (f >= -cast_num(LUA_MININTEGER))  /* -minint == maxint + 1 */
     4e3:	31 c0                	xor    eax,eax
     4e5:	66 0f 2f 05 00 00 00 	comisd xmm0,QWORD PTR [rip+0x0]        # 4ed <LTnum.isra.2+0x4d>
     4ec:	00 
     4ed:	73 23                	jae    512 <LTnum.isra.2+0x72>
    else if (f >= cast_num(LUA_MININTEGER))  /* minint <= f <= maxint ? */
     4ef:	66 0f 2f 05 00 00 00 	comisd xmm0,QWORD PTR [rip+0x0]        # 4f7 <LTnum.isra.2+0x57>
     4f6:	00 
     4f7:	0f 82 d3 00 00 00    	jb     5d0 <LTnum.isra.2+0x130>
      return (i <= cast(lua_Integer, f));  /* compare them as integers */
     4fd:	f2 48 0f 2c c0       	cvttsd2si rax,xmm0
     502:	48 39 c2             	cmp    rdx,rax
     505:	0f 9f c0             	setg   al
     508:	0f b6 c0             	movzx  eax,al
     50b:	c3                   	ret    
     50c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
      return 0;
     510:	31 c0                	xor    eax,eax
}
     512:	c3                   	ret    
     513:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    if (ttisinteger(r))
     518:	66 83 f9 13          	cmp    cx,0x13
     51c:	0f 84 9e 00 00 00    	je     5c0 <LTnum.isra.2+0x120>
  if (!l_intfitsf(i)) {
     522:	48 b8 00 00 00 00 00 	movabs rax,0x20000000000000
     529:	00 20 00 
      return LTintfloat(li, fltvalue(r));  /* l < r ? */
     52c:	48 89 54 24 f8       	mov    QWORD PTR [rsp-0x8],rdx
  if (!l_intfitsf(i)) {
     531:	48 ba 00 00 00 00 00 	movabs rdx,0x40000000000000
     538:	00 40 00 
      return LTintfloat(li, fltvalue(r));  /* l < r ? */
     53b:	f2 0f 10 4c 24 f8    	movsd  xmm1,QWORD PTR [rsp-0x8]
  if (!l_intfitsf(i)) {
     541:	48 01 f8             	add    rax,rdi
     544:	48 39 d0             	cmp    rax,rdx
     547:	76 47                	jbe    590 <LTnum.isra.2+0xf0>
    if (f >= -cast_num(LUA_MININTEGER))  /* -minint == maxint + 1 */
     549:	66 0f 2f 0d 00 00 00 	comisd xmm1,QWORD PTR [rip+0x0]        # 551 <LTnum.isra.2+0xb1>
     550:	00 
      return 1;  /* f >= maxint + 1 > i */
     551:	b8 01 00 00 00       	mov    eax,0x1
    if (f >= -cast_num(LUA_MININTEGER))  /* -minint == maxint + 1 */
     556:	73 ba                	jae    512 <LTnum.isra.2+0x72>
    else if (f > cast_num(LUA_MININTEGER))  /* minint < f <= maxint ? */
     558:	66 0f 2f 0d 00 00 00 	comisd xmm1,QWORD PTR [rip+0x0]        # 560 <LTnum.isra.2+0xc0>
     55f:	00 
     560:	76 7e                	jbe    5e0 <LTnum.isra.2+0x140>
      return (i < cast(lua_Integer, f));  /* compare them as integers */
     562:	f2 48 0f 2c c1       	cvttsd2si rax,xmm1
     567:	48 39 c7             	cmp    rdi,rax
     56a:	0f 9c c0             	setl   al
     56d:	0f b6 c0             	movzx  eax,al
     570:	c3                   	ret    
     571:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  return luai_numle(cast_num(i), f);  /* compare them as floats */
     578:	66 0f ef c9          	pxor   xmm1,xmm1
     57c:	31 c0                	xor    eax,eax
     57e:	f2 48 0f 2a ca       	cvtsi2sd xmm1,rdx
     583:	66 0f 2f c1          	comisd xmm0,xmm1
     587:	0f 92 c0             	setb   al
     58a:	c3                   	ret    
     58b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  return luai_numlt(cast_num(i), f);  /* compare them as floats */
     590:	66 0f ef c0          	pxor   xmm0,xmm0
     594:	31 c0                	xor    eax,eax
     596:	f2 48 0f 2a c7       	cvtsi2sd xmm0,rdi
     59b:	66 0f 2f c8          	comisd xmm1,xmm0
     59f:	0f 97 c0             	seta   al
     5a2:	c3                   	ret    
     5a3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
      return luai_numlt(lf, fltvalue(r));  /* both are float */
     5a8:	48 89 54 24 f8       	mov    QWORD PTR [rsp-0x8],rdx
     5ad:	f2 0f 10 4c 24 f8    	movsd  xmm1,QWORD PTR [rsp-0x8]
     5b3:	31 c0                	xor    eax,eax
     5b5:	66 0f 2f c8          	comisd xmm1,xmm0
     5b9:	0f 97 c0             	seta   al
     5bc:	c3                   	ret    
     5bd:	0f 1f 00             	nop    DWORD PTR [rax]
      return li < ivalue(r);  /* both are integers */
     5c0:	31 c0                	xor    eax,eax
     5c2:	48 39 d7             	cmp    rdi,rdx
     5c5:	0f 9c c0             	setl   al
     5c8:	c3                   	ret    
     5c9:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    else if (f >= cast_num(LUA_MININTEGER))  /* minint <= f <= maxint ? */
     5d0:	b8 01 00 00 00       	mov    eax,0x1
     5d5:	c3                   	ret    
     5d6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
     5dd:	00 00 00 
      return 0;
     5e0:	31 c0                	xor    eax,eax
     5e2:	c3                   	ret    
     5e3:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
     5ea:	00 00 00 00 
     5ee:	66 90                	xchg   ax,ax

00000000000005f0 <luaV_tonumber_>:
int luaV_tonumber_ (const TValue *obj, lua_Number *n) {
     5f0:	55                   	push   rbp
     5f1:	48 89 f5             	mov    rbp,rsi
     5f4:	53                   	push   rbx
     5f5:	48 89 fb             	mov    rbx,rdi
     5f8:	48 83 ec 38          	sub    rsp,0x38
     5fc:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
     603:	00 00 
     605:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
     60a:	31 c0                	xor    eax,eax
  if (ttisinteger(obj)) {
     60c:	0f b7 47 08          	movzx  eax,WORD PTR [rdi+0x8]
     610:	66 83 f8 13          	cmp    ax,0x13
     614:	0f 84 7e 00 00 00    	je     698 <luaV_tonumber_+0xa8>
  else if (cvt2num(obj) &&  /* string coercible to number? */
     61a:	83 e0 0f             	and    eax,0xf
     61d:	66 83 f8 04          	cmp    ax,0x4
     621:	74 1d                	je     640 <luaV_tonumber_+0x50>
    return 0;  /* conversion failed */
     623:	31 c0                	xor    eax,eax
}
     625:	48 8b 4c 24 28       	mov    rcx,QWORD PTR [rsp+0x28]
     62a:	64 48 33 0c 25 28 00 	xor    rcx,QWORD PTR fs:0x28
     631:	00 00 
     633:	0f 85 81 00 00 00    	jne    6ba <luaV_tonumber_+0xca>
     639:	48 83 c4 38          	add    rsp,0x38
     63d:	5b                   	pop    rbx
     63e:	5d                   	pop    rbp
     63f:	c3                   	ret    
            luaO_str2num(svalue(obj), &v) == vslen(obj) + 1) {
     640:	48 8b 07             	mov    rax,QWORD PTR [rdi]
     643:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
     648:	48 8d 78 18          	lea    rdi,[rax+0x18]
     64c:	e8 00 00 00 00       	call   651 <luaV_tonumber_+0x61>
     651:	48 8b 13             	mov    rdx,QWORD PTR [rbx]
     654:	80 7a 08 04          	cmp    BYTE PTR [rdx+0x8],0x4
     658:	74 56                	je     6b0 <luaV_tonumber_+0xc0>
     65a:	48 8b 52 10          	mov    rdx,QWORD PTR [rdx+0x10]
     65e:	48 83 c2 01          	add    rdx,0x1
  else if (cvt2num(obj) &&  /* string coercible to number? */
     662:	48 39 d0             	cmp    rax,rdx
     665:	75 bc                	jne    623 <luaV_tonumber_+0x33>
    *n = nvalue(&v);  /* convert result of 'luaO_str2num' to a float */
     667:	66 83 7c 24 18 13    	cmp    WORD PTR [rsp+0x18],0x13
     66d:	f2 0f 10 44 24 10    	movsd  xmm0,QWORD PTR [rsp+0x10]
     673:	75 11                	jne    686 <luaV_tonumber_+0x96>
     675:	66 0f d6 44 24 08    	movq   QWORD PTR [rsp+0x8],xmm0
     67b:	66 0f ef c0          	pxor   xmm0,xmm0
     67f:	f2 48 0f 2a 44 24 08 	cvtsi2sd xmm0,QWORD PTR [rsp+0x8]
     686:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    return 1;
     68b:	b8 01 00 00 00       	mov    eax,0x1
     690:	eb 93                	jmp    625 <luaV_tonumber_+0x35>
     692:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    *n = cast_num(ivalue(obj));
     698:	66 0f ef c0          	pxor   xmm0,xmm0
    return 1;
     69c:	b8 01 00 00 00       	mov    eax,0x1
    *n = cast_num(ivalue(obj));
     6a1:	f2 48 0f 2a 07       	cvtsi2sd xmm0,QWORD PTR [rdi]
     6a6:	f2 0f 11 06          	movsd  QWORD PTR [rsi],xmm0
    return 1;
     6aa:	e9 76 ff ff ff       	jmp    625 <luaV_tonumber_+0x35>
     6af:	90                   	nop
            luaO_str2num(svalue(obj), &v) == vslen(obj) + 1) {
     6b0:	0f b6 52 0b          	movzx  edx,BYTE PTR [rdx+0xb]
     6b4:	48 83 c2 01          	add    rdx,0x1
     6b8:	eb a8                	jmp    662 <luaV_tonumber_+0x72>
}
     6ba:	e8 00 00 00 00       	call   6bf <luaV_tonumber_+0xcf>
     6bf:	90                   	nop

00000000000006c0 <luaV_flttointeger>:
  if (!ttisfloat(obj))
     6c0:	66 83 7f 08 03       	cmp    WORD PTR [rdi+0x8],0x3
     6c5:	75 09                	jne    6d0 <luaV_flttointeger+0x10>
     6c7:	e9 c4 fb ff ff       	jmp    290 <luaV_flttointeger.part.7>
     6cc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
}
     6d0:	31 c0                	xor    eax,eax
     6d2:	c3                   	ret    
     6d3:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
     6da:	00 00 00 00 
     6de:	66 90                	xchg   ax,ax

00000000000006e0 <luaV_tointeger>:
int luaV_tointeger (const TValue *obj, lua_Integer *p, int mode) {
     6e0:	55                   	push   rbp
     6e1:	48 89 f5             	mov    rbp,rsi
     6e4:	53                   	push   rbx
     6e5:	48 89 fb             	mov    rbx,rdi
     6e8:	48 83 ec 38          	sub    rsp,0x38
  if (cvt2num(obj) && luaO_str2num(svalue(obj), &v) == vslen(obj) + 1)
     6ec:	0f b7 4f 08          	movzx  ecx,WORD PTR [rdi+0x8]
int luaV_tointeger (const TValue *obj, lua_Integer *p, int mode) {
     6f0:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
     6f7:	00 00 
     6f9:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
     6fe:	31 c0                	xor    eax,eax
  if (cvt2num(obj) && luaO_str2num(svalue(obj), &v) == vslen(obj) + 1)
     700:	89 c8                	mov    eax,ecx
     702:	83 e0 0f             	and    eax,0xf
     705:	66 83 f8 04          	cmp    ax,0x4
     709:	74 45                	je     750 <luaV_tointeger+0x70>
  if (ttisinteger(obj)) {
     70b:	66 83 f9 13          	cmp    cx,0x13
     70f:	74 2f                	je     740 <luaV_tointeger+0x60>
    return 0;
     711:	31 c0                	xor    eax,eax
  if (!ttisfloat(obj))
     713:	66 83 f9 03          	cmp    cx,0x3
     717:	74 17                	je     730 <luaV_tointeger+0x50>
}
     719:	48 8b 74 24 28       	mov    rsi,QWORD PTR [rsp+0x28]
     71e:	64 48 33 34 25 28 00 	xor    rsi,QWORD PTR fs:0x28
     725:	00 00 
     727:	75 7e                	jne    7a7 <luaV_tointeger+0xc7>
     729:	48 83 c4 38          	add    rsp,0x38
     72d:	5b                   	pop    rbx
     72e:	5d                   	pop    rbp
     72f:	c3                   	ret    
     730:	48 89 ee             	mov    rsi,rbp
     733:	48 89 df             	mov    rdi,rbx
     736:	e8 55 fb ff ff       	call   290 <luaV_flttointeger.part.7>
     73b:	eb dc                	jmp    719 <luaV_tointeger+0x39>
     73d:	0f 1f 00             	nop    DWORD PTR [rax]
    *p = ivalue(obj);
     740:	48 8b 03             	mov    rax,QWORD PTR [rbx]
     743:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    return 1;
     747:	b8 01 00 00 00       	mov    eax,0x1
     74c:	eb cb                	jmp    719 <luaV_tointeger+0x39>
     74e:	66 90                	xchg   ax,ax
  if (cvt2num(obj) && luaO_str2num(svalue(obj), &v) == vslen(obj) + 1)
     750:	48 8b 07             	mov    rax,QWORD PTR [rdi]
     753:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
     758:	89 54 24 0c          	mov    DWORD PTR [rsp+0xc],edx
     75c:	48 8d 78 18          	lea    rdi,[rax+0x18]
     760:	e8 00 00 00 00       	call   765 <luaV_tointeger+0x85>
     765:	48 8b 0b             	mov    rcx,QWORD PTR [rbx]
     768:	8b 54 24 0c          	mov    edx,DWORD PTR [rsp+0xc]
     76c:	80 79 08 04          	cmp    BYTE PTR [rcx+0x8],0x4
     770:	74 16                	je     788 <luaV_tointeger+0xa8>
     772:	48 8b 49 10          	mov    rcx,QWORD PTR [rcx+0x10]
     776:	48 83 c1 01          	add    rcx,0x1
     77a:	48 39 c8             	cmp    rax,rcx
     77d:	74 19                	je     798 <luaV_tointeger+0xb8>
     77f:	0f b7 4b 08          	movzx  ecx,WORD PTR [rbx+0x8]
     783:	eb 86                	jmp    70b <luaV_tointeger+0x2b>
     785:	0f 1f 00             	nop    DWORD PTR [rax]
     788:	0f b6 49 0b          	movzx  ecx,BYTE PTR [rcx+0xb]
     78c:	48 83 c1 01          	add    rcx,0x1
     790:	eb e8                	jmp    77a <luaV_tointeger+0x9a>
     792:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    obj = &v;  /* change string to its corresponding number */
     798:	0f b7 4c 24 18       	movzx  ecx,WORD PTR [rsp+0x18]
     79d:	48 8d 5c 24 10       	lea    rbx,[rsp+0x10]
     7a2:	e9 64 ff ff ff       	jmp    70b <luaV_tointeger+0x2b>
}
     7a7:	e8 00 00 00 00       	call   7ac <luaV_tointeger+0xcc>
     7ac:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

00000000000007b0 <luaV_tointeger_>:
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
     7b0:	31 d2                	xor    edx,edx
     7b2:	e9 29 ff ff ff       	jmp    6e0 <luaV_tointeger>
     7b7:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
     7be:	00 00 

00000000000007c0 <luaV_forlimit>:
                     int *stopnow) {
     7c0:	41 55                	push   r13
     7c2:	41 54                	push   r12
     7c4:	55                   	push   rbp
     7c5:	48 89 f5             	mov    rbp,rsi
     7c8:	53                   	push   rbx
     7c9:	48 89 fb             	mov    rbx,rdi
     7cc:	48 83 ec 18          	sub    rsp,0x18
     7d0:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
     7d7:	00 00 
     7d9:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
     7de:	31 c0                	xor    eax,eax
  if (ttisinteger(obj))
     7e0:	66 83 7f 08 13       	cmp    WORD PTR [rdi+0x8],0x13
  *stopnow = 0;  /* usually, let loops run */
     7e5:	c7 01 00 00 00 00    	mov    DWORD PTR [rcx],0x0
  if (ttisinteger(obj))
     7eb:	74 6b                	je     858 <luaV_forlimit+0x98>
     7ed:	49 89 d5             	mov    r13,rdx
  else if (!luaV_tointeger(obj, p, (step < 0 ? 2 : 1))) {
     7f0:	48 c1 ea 3f          	shr    rdx,0x3f
     7f4:	49 89 cc             	mov    r12,rcx
     7f7:	83 c2 01             	add    edx,0x1
     7fa:	e8 00 00 00 00       	call   7ff <luaV_forlimit+0x3f>
  return 1;
     7ff:	ba 01 00 00 00       	mov    edx,0x1
  else if (!luaV_tointeger(obj, p, (step < 0 ? 2 : 1))) {
     804:	85 c0                	test   eax,eax
     806:	75 2d                	jne    835 <luaV_forlimit+0x75>
    if (!tonumber(obj, &n)) /* cannot convert to float? */
     808:	66 83 7b 08 03       	cmp    WORD PTR [rbx+0x8],0x3
     80d:	75 79                	jne    888 <luaV_forlimit+0xc8>
     80f:	f2 0f 10 03          	movsd  xmm0,QWORD PTR [rbx]
    if (luai_numlt(0, n)) {  /* if true, float is larger than max integer */
     813:	66 0f 2f 05 00 00 00 	comisd xmm0,QWORD PTR [rip+0x0]        # 81b <luaV_forlimit+0x5b>
     81a:	00 
     81b:	76 4b                	jbe    868 <luaV_forlimit+0xa8>
      *p = LUA_MAXINTEGER;
     81d:	48 b8 ff ff ff ff ff 	movabs rax,0x7fffffffffffffff
     824:	ff ff 7f 
     827:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
      if (step < 0) *stopnow = 1;
     82b:	4d 85 ed             	test   r13,r13
     82e:	78 4b                	js     87b <luaV_forlimit+0xbb>
  return 1;
     830:	ba 01 00 00 00       	mov    edx,0x1
}
     835:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
     83a:	64 48 33 0c 25 28 00 	xor    rcx,QWORD PTR fs:0x28
     841:	00 00 
     843:	89 d0                	mov    eax,edx
     845:	75 5c                	jne    8a3 <luaV_forlimit+0xe3>
     847:	48 83 c4 18          	add    rsp,0x18
     84b:	5b                   	pop    rbx
     84c:	5d                   	pop    rbp
     84d:	41 5c                	pop    r12
     84f:	41 5d                	pop    r13
     851:	c3                   	ret    
     852:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    *p = ivalue(obj);
     858:	48 8b 07             	mov    rax,QWORD PTR [rdi]
  return 1;
     85b:	ba 01 00 00 00       	mov    edx,0x1
    *p = ivalue(obj);
     860:	48 89 06             	mov    QWORD PTR [rsi],rax
     863:	eb d0                	jmp    835 <luaV_forlimit+0x75>
     865:	0f 1f 00             	nop    DWORD PTR [rax]
      *p = LUA_MININTEGER;
     868:	48 b8 00 00 00 00 00 	movabs rax,0x8000000000000000
     86f:	00 00 80 
     872:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
      if (step >= 0) *stopnow = 1;
     876:	4d 85 ed             	test   r13,r13
     879:	78 b5                	js     830 <luaV_forlimit+0x70>
      if (step < 0) *stopnow = 1;
     87b:	41 c7 04 24 01 00 00 	mov    DWORD PTR [r12],0x1
     882:	00 
     883:	eb ab                	jmp    830 <luaV_forlimit+0x70>
     885:	0f 1f 00             	nop    DWORD PTR [rax]
    if (!tonumber(obj, &n)) /* cannot convert to float? */
     888:	48 89 e6             	mov    rsi,rsp
     88b:	48 89 df             	mov    rdi,rbx
     88e:	e8 00 00 00 00       	call   893 <luaV_forlimit+0xd3>
     893:	89 c2                	mov    edx,eax
     895:	85 c0                	test   eax,eax
     897:	74 9c                	je     835 <luaV_forlimit+0x75>
     899:	f2 0f 10 04 24       	movsd  xmm0,QWORD PTR [rsp]
     89e:	e9 70 ff ff ff       	jmp    813 <luaV_forlimit+0x53>
}
     8a3:	e8 00 00 00 00       	call   8a8 <luaV_forlimit+0xe8>
     8a8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
     8af:	00 

00000000000008b0 <luaV_finishget>:
                      const TValue *slot) {
     8b0:	41 57                	push   r15
     8b2:	4c 89 c0             	mov    rax,r8
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
     8b5:	45 31 ff             	xor    r15d,r15d
                      const TValue *slot) {
     8b8:	41 56                	push   r14
     8ba:	49 89 f6             	mov    r14,rsi
     8bd:	41 55                	push   r13
     8bf:	49 89 cd             	mov    r13,rcx
     8c2:	41 54                	push   r12
     8c4:	49 89 d4             	mov    r12,rdx
     8c7:	55                   	push   rbp
     8c8:	48 89 fd             	mov    rbp,rdi
     8cb:	53                   	push   rbx
     8cc:	48 83 ec 08          	sub    rsp,0x8
    if (slot == NULL) {  /* 't' is not a table? */
     8d0:	48 85 c0             	test   rax,rax
     8d3:	0f 84 07 01 00 00    	je     9e0 <luaV_finishget+0x130>
      tm = fasttm(L, hvalue(t)->metatable, TM_INDEX);  /* table's metamethod */
     8d9:	49 8b 06             	mov    rax,QWORD PTR [r14]
     8dc:	48 8b 78 28          	mov    rdi,QWORD PTR [rax+0x28]
     8e0:	48 85 ff             	test   rdi,rdi
     8e3:	0f 84 df 00 00 00    	je     9c8 <luaV_finishget+0x118>
     8e9:	f6 47 0a 01          	test   BYTE PTR [rdi+0xa],0x1
     8ed:	0f 85 d5 00 00 00    	jne    9c8 <luaV_finishget+0x118>
     8f3:	48 8b 45 18          	mov    rax,QWORD PTR [rbp+0x18]
     8f7:	31 f6                	xor    esi,esi
     8f9:	48 8b 90 e0 00 00 00 	mov    rdx,QWORD PTR [rax+0xe0]
     900:	e8 00 00 00 00       	call   905 <luaV_finishget+0x55>
     905:	48 89 c3             	mov    rbx,rax
      if (tm == NULL) {  /* no metamethod? */
     908:	48 85 c0             	test   rax,rax
     90b:	0f 84 b7 00 00 00    	je     9c8 <luaV_finishget+0x118>
     911:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
    if (ttisfunction(tm)) {  /* is metamethod a function? */
     915:	83 e0 0f             	and    eax,0xf
     918:	66 83 f8 06          	cmp    ax,0x6
     91c:	74 3f                	je     95d <luaV_finishget+0xad>
    if (luaV_fastget(L,t,key,slot,luaH_get)) {  /* fast track? */
     91e:	66 83 f8 05          	cmp    ax,0x5
     922:	74 64                	je     988 <luaV_finishget+0xd8>
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
     924:	41 83 c7 01          	add    r15d,0x1
     928:	41 81 ff d0 07 00 00 	cmp    r15d,0x7d0
     92f:	74 7f                	je     9b0 <luaV_finishget+0x100>
      tm = luaT_gettmbyobj(L, t, TM_INDEX);
     931:	31 d2                	xor    edx,edx
     933:	48 89 de             	mov    rsi,rbx
     936:	48 89 ef             	mov    rdi,rbp
     939:	e8 00 00 00 00       	call   93e <luaV_finishget+0x8e>
     93e:	48 89 c2             	mov    rdx,rax
      if (ttisnil(tm))
     941:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
     945:	66 85 c0             	test   ax,ax
     948:	0f 84 a8 00 00 00    	je     9f6 <luaV_finishget+0x146>
     94e:	83 e0 0f             	and    eax,0xf
     951:	49 89 de             	mov    r14,rbx
     954:	48 89 d3             	mov    rbx,rdx
    if (ttisfunction(tm)) {  /* is metamethod a function? */
     957:	66 83 f8 06          	cmp    ax,0x6
     95b:	75 c1                	jne    91e <luaV_finishget+0x6e>
}
     95d:	48 83 c4 08          	add    rsp,0x8
      luaT_callTM(L, tm, t, key, val, 1);  /* call it */
     961:	4d 89 e8             	mov    r8,r13
     964:	4c 89 e1             	mov    rcx,r12
     967:	4c 89 f2             	mov    rdx,r14
     96a:	48 89 de             	mov    rsi,rbx
     96d:	48 89 ef             	mov    rdi,rbp
}
     970:	5b                   	pop    rbx
      luaT_callTM(L, tm, t, key, val, 1);  /* call it */
     971:	41 b9 01 00 00 00    	mov    r9d,0x1
}
     977:	5d                   	pop    rbp
     978:	41 5c                	pop    r12
     97a:	41 5d                	pop    r13
     97c:	41 5e                	pop    r14
     97e:	41 5f                	pop    r15
      luaT_callTM(L, tm, t, key, val, 1);  /* call it */
     980:	e9 00 00 00 00       	jmp    985 <luaV_finishget+0xd5>
     985:	0f 1f 00             	nop    DWORD PTR [rax]
    if (luaV_fastget(L,t,key,slot,luaH_get)) {  /* fast track? */
     988:	48 8b 3b             	mov    rdi,QWORD PTR [rbx]
     98b:	4c 89 e6             	mov    rsi,r12
     98e:	e8 00 00 00 00       	call   993 <luaV_finishget+0xe3>
     993:	0f b7 50 08          	movzx  edx,WORD PTR [rax+0x8]
     997:	66 85 d2             	test   dx,dx
     99a:	75 4c                	jne    9e8 <luaV_finishget+0x138>
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
     99c:	41 83 c7 01          	add    r15d,0x1
     9a0:	49 89 de             	mov    r14,rbx
     9a3:	41 81 ff d0 07 00 00 	cmp    r15d,0x7d0
     9aa:	0f 85 20 ff ff ff    	jne    8d0 <luaV_finishget+0x20>
  luaG_runerror(L, "'__index' chain too long; possible loop");
     9b0:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9b7 <luaV_finishget+0x107>
     9b7:	48 89 ef             	mov    rdi,rbp
     9ba:	31 c0                	xor    eax,eax
     9bc:	e8 00 00 00 00       	call   9c1 <luaV_finishget+0x111>
     9c1:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
        setnilvalue(val);  /* result is nil */
     9c8:	31 c0                	xor    eax,eax
     9ca:	66 41 89 45 08       	mov    WORD PTR [r13+0x8],ax
}
     9cf:	48 83 c4 08          	add    rsp,0x8
     9d3:	5b                   	pop    rbx
     9d4:	5d                   	pop    rbp
     9d5:	41 5c                	pop    r12
     9d7:	41 5d                	pop    r13
     9d9:	41 5e                	pop    r14
     9db:	41 5f                	pop    r15
     9dd:	c3                   	ret    
     9de:	66 90                	xchg   ax,ax
     9e0:	4c 89 f3             	mov    rbx,r14
     9e3:	e9 49 ff ff ff       	jmp    931 <luaV_finishget+0x81>
      setobj2s(L, val, slot);  /* done */
     9e8:	48 8b 00             	mov    rax,QWORD PTR [rax]
     9eb:	66 41 89 55 08       	mov    WORD PTR [r13+0x8],dx
     9f0:	49 89 45 00          	mov    QWORD PTR [r13+0x0],rax
      return;
     9f4:	eb d9                	jmp    9cf <luaV_finishget+0x11f>
        luaG_typeerror(L, t, "index");  /* no metamethod */
     9f6:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9fd <luaV_finishget+0x14d>
     9fd:	48 89 de             	mov    rsi,rbx
     a00:	48 89 ef             	mov    rdi,rbp
     a03:	e8 00 00 00 00       	call   a08 <luaV_finishget+0x158>
     a08:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
     a0f:	00 

0000000000000a10 <luaV_finishset>:
                     StkId val, const TValue *slot) {
     a10:	41 57                	push   r15
     a12:	4d 89 c7             	mov    r15,r8
     a15:	41 56                	push   r14
     a17:	41 55                	push   r13
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
     a19:	45 31 ed             	xor    r13d,r13d
                     StkId val, const TValue *slot) {
     a1c:	41 54                	push   r12
     a1e:	55                   	push   rbp
     a1f:	48 89 fd             	mov    rbp,rdi
     a22:	53                   	push   rbx
     a23:	48 89 f3             	mov    rbx,rsi
     a26:	48 83 ec 18          	sub    rsp,0x18
     a2a:	48 89 14 24          	mov    QWORD PTR [rsp],rdx
     a2e:	48 89 4c 24 08       	mov    QWORD PTR [rsp+0x8],rcx
    if (slot != NULL) {  /* is 't' a table? */
     a33:	4d 85 ff             	test   r15,r15
     a36:	0f 84 84 01 00 00    	je     bc0 <luaV_finishset+0x1b0>
      Table *h = hvalue(t);  /* save 't' table */
     a3c:	4c 8b 23             	mov    r12,QWORD PTR [rbx]
      tm = fasttm(L, h->metatable, TM_NEWINDEX);  /* get metamethod */
     a3f:	49 8b 7c 24 28       	mov    rdi,QWORD PTR [r12+0x28]
     a44:	48 85 ff             	test   rdi,rdi
     a47:	0f 84 f3 00 00 00    	je     b40 <luaV_finishset+0x130>
     a4d:	f6 47 0a 02          	test   BYTE PTR [rdi+0xa],0x2
     a51:	0f 85 e9 00 00 00    	jne    b40 <luaV_finishset+0x130>
     a57:	48 8b 45 18          	mov    rax,QWORD PTR [rbp+0x18]
     a5b:	be 01 00 00 00       	mov    esi,0x1
     a60:	48 8b 90 e8 00 00 00 	mov    rdx,QWORD PTR [rax+0xe8]
     a67:	e8 00 00 00 00       	call   a6c <luaV_finishset+0x5c>
     a6c:	49 89 c6             	mov    r14,rax
      if (tm == NULL) {  /* no metamethod? */
     a6f:	48 85 c0             	test   rax,rax
     a72:	0f 84 c8 00 00 00    	je     b40 <luaV_finishset+0x130>
     a78:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
    if (ttisfunction(tm)) {
     a7c:	83 e0 0f             	and    eax,0xf
     a7f:	66 83 f8 06          	cmp    ax,0x6
     a83:	74 46                	je     acb <luaV_finishset+0xbb>
    if (luaV_fastget(L, t, key, slot, luaH_get)) {
     a85:	66 83 f8 05          	cmp    ax,0x5
     a89:	74 6d                	je     af8 <luaV_finishset+0xe8>
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
     a8b:	41 83 c5 01          	add    r13d,0x1
     a8f:	41 81 fd d0 07 00 00 	cmp    r13d,0x7d0
     a96:	0f 84 8a 00 00 00    	je     b26 <luaV_finishset+0x116>
      if (ttisnil(tm = luaT_gettmbyobj(L, t, TM_NEWINDEX)))
     a9c:	ba 01 00 00 00       	mov    edx,0x1
     aa1:	4c 89 f6             	mov    rsi,r14
     aa4:	48 89 ef             	mov    rdi,rbp
     aa7:	e8 00 00 00 00       	call   aac <luaV_finishset+0x9c>
     aac:	48 89 c2             	mov    rdx,rax
     aaf:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
     ab3:	66 85 c0             	test   ax,ax
     ab6:	0f 84 53 01 00 00    	je     c0f <luaV_finishset+0x1ff>
     abc:	83 e0 0f             	and    eax,0xf
     abf:	4c 89 f3             	mov    rbx,r14
     ac2:	49 89 d6             	mov    r14,rdx
    if (ttisfunction(tm)) {
     ac5:	66 83 f8 06          	cmp    ax,0x6
     ac9:	75 ba                	jne    a85 <luaV_finishset+0x75>
      luaT_callTM(L, tm, t, key, val, 0);
     acb:	4c 8b 44 24 08       	mov    r8,QWORD PTR [rsp+0x8]
     ad0:	48 8b 0c 24          	mov    rcx,QWORD PTR [rsp]
}
     ad4:	48 83 c4 18          	add    rsp,0x18
      luaT_callTM(L, tm, t, key, val, 0);
     ad8:	48 89 da             	mov    rdx,rbx
     adb:	4c 89 f6             	mov    rsi,r14
}
     ade:	5b                   	pop    rbx
      luaT_callTM(L, tm, t, key, val, 0);
     adf:	48 89 ef             	mov    rdi,rbp
     ae2:	45 31 c9             	xor    r9d,r9d
}
     ae5:	5d                   	pop    rbp
     ae6:	41 5c                	pop    r12
     ae8:	41 5d                	pop    r13
     aea:	41 5e                	pop    r14
     aec:	41 5f                	pop    r15
      luaT_callTM(L, tm, t, key, val, 0);
     aee:	e9 00 00 00 00       	jmp    af3 <luaV_finishset+0xe3>
     af3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    if (luaV_fastget(L, t, key, slot, luaH_get)) {
     af8:	48 8b 34 24          	mov    rsi,QWORD PTR [rsp]
     afc:	49 8b 3e             	mov    rdi,QWORD PTR [r14]
     aff:	e8 00 00 00 00       	call   b04 <luaV_finishset+0xf4>
     b04:	66 83 78 08 00       	cmp    WORD PTR [rax+0x8],0x0
     b09:	49 89 c7             	mov    r15,rax
     b0c:	0f 85 b6 00 00 00    	jne    bc8 <luaV_finishset+0x1b8>
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
     b12:	41 83 c5 01          	add    r13d,0x1
     b16:	4c 89 f3             	mov    rbx,r14
     b19:	41 81 fd d0 07 00 00 	cmp    r13d,0x7d0
     b20:	0f 85 0d ff ff ff    	jne    a33 <luaV_finishset+0x23>
  luaG_runerror(L, "'__newindex' chain too long; possible loop");
     b26:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # b2d <luaV_finishset+0x11d>
     b2d:	48 89 ef             	mov    rdi,rbp
     b30:	31 c0                	xor    eax,eax
     b32:	e8 00 00 00 00       	call   b37 <luaV_finishset+0x127>
     b37:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
     b3e:	00 00 
        if (slot == luaO_nilobject)  /* no previous entry? */
     b40:	48 8d 05 00 00 00 00 	lea    rax,[rip+0x0]        # b47 <luaV_finishset+0x137>
     b47:	49 39 c7             	cmp    r15,rax
     b4a:	74 5e                	je     baa <luaV_finishset+0x19a>
        setobj2t(L, cast(TValue *, slot), val);  /* set its new value */
     b4c:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
     b51:	48 8b 01             	mov    rax,QWORD PTR [rcx]
     b54:	49 89 07             	mov    QWORD PTR [r15],rax
     b57:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
     b5b:	66 41 89 47 08       	mov    WORD PTR [r15+0x8],ax
        invalidateTMcache(h);
     b60:	41 c6 44 24 0a 00    	mov    BYTE PTR [r12+0xa],0x0
        luaC_barrierback(L, h, val);
     b66:	66 83 79 08 00       	cmp    WORD PTR [rcx+0x8],0x0
     b6b:	78 13                	js     b80 <luaV_finishset+0x170>
}
     b6d:	48 83 c4 18          	add    rsp,0x18
     b71:	5b                   	pop    rbx
     b72:	5d                   	pop    rbp
     b73:	41 5c                	pop    r12
     b75:	41 5d                	pop    r13
     b77:	41 5e                	pop    r14
     b79:	41 5f                	pop    r15
     b7b:	c3                   	ret    
     b7c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
        luaC_barrierback(L, h, val);
     b80:	41 f6 44 24 09 04    	test   BYTE PTR [r12+0x9],0x4
     b86:	74 e5                	je     b6d <luaV_finishset+0x15d>
     b88:	48 8b 01             	mov    rax,QWORD PTR [rcx]
     b8b:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
     b8f:	74 dc                	je     b6d <luaV_finishset+0x15d>
}
     b91:	48 83 c4 18          	add    rsp,0x18
        luaC_barrierback(L, h, val);
     b95:	4c 89 e6             	mov    rsi,r12
     b98:	48 89 ef             	mov    rdi,rbp
}
     b9b:	5b                   	pop    rbx
     b9c:	5d                   	pop    rbp
     b9d:	41 5c                	pop    r12
     b9f:	41 5d                	pop    r13
     ba1:	41 5e                	pop    r14
     ba3:	41 5f                	pop    r15
        luaC_barrierback(L, h, val);
     ba5:	e9 00 00 00 00       	jmp    baa <luaV_finishset+0x19a>
          slot = luaH_newkey(L, h, key);  /* create one */
     baa:	48 8b 14 24          	mov    rdx,QWORD PTR [rsp]
     bae:	4c 89 e6             	mov    rsi,r12
     bb1:	48 89 ef             	mov    rdi,rbp
     bb4:	e8 00 00 00 00       	call   bb9 <luaV_finishset+0x1a9>
     bb9:	49 89 c7             	mov    r15,rax
     bbc:	eb 8e                	jmp    b4c <luaV_finishset+0x13c>
     bbe:	66 90                	xchg   ax,ax
     bc0:	49 89 de             	mov    r14,rbx
     bc3:	e9 d4 fe ff ff       	jmp    a9c <luaV_finishset+0x8c>
      luaV_finishfastset(L, t, slot, val);
     bc8:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
     bcd:	48 8b 01             	mov    rax,QWORD PTR [rcx]
     bd0:	49 89 07             	mov    QWORD PTR [r15],rax
     bd3:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
     bd7:	66 41 89 47 08       	mov    WORD PTR [r15+0x8],ax
     bdc:	66 83 79 08 00       	cmp    WORD PTR [rcx+0x8],0x0
     be1:	79 8a                	jns    b6d <luaV_finishset+0x15d>
     be3:	49 8b 36             	mov    rsi,QWORD PTR [r14]
     be6:	f6 46 09 04          	test   BYTE PTR [rsi+0x9],0x4
     bea:	74 81                	je     b6d <luaV_finishset+0x15d>
     bec:	48 8b 01             	mov    rax,QWORD PTR [rcx]
     bef:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
     bf3:	0f 84 74 ff ff ff    	je     b6d <luaV_finishset+0x15d>
}
     bf9:	48 83 c4 18          	add    rsp,0x18
      luaV_finishfastset(L, t, slot, val);
     bfd:	48 89 ef             	mov    rdi,rbp
}
     c00:	5b                   	pop    rbx
     c01:	5d                   	pop    rbp
     c02:	41 5c                	pop    r12
     c04:	41 5d                	pop    r13
     c06:	41 5e                	pop    r14
     c08:	41 5f                	pop    r15
      luaV_finishfastset(L, t, slot, val);
     c0a:	e9 00 00 00 00       	jmp    c0f <luaV_finishset+0x1ff>
        luaG_typeerror(L, t, "index");
     c0f:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # c16 <luaV_finishset+0x206>
     c16:	4c 89 f6             	mov    rsi,r14
     c19:	48 89 ef             	mov    rdi,rbp
     c1c:	e8 00 00 00 00       	call   c21 <luaV_finishset+0x211>
     c21:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
     c28:	00 00 00 00 
     c2c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000000c30 <luaV_gettable>:
void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val) {
     c30:	41 55                	push   r13
     c32:	49 89 fd             	mov    r13,rdi
     c35:	41 54                	push   r12
     c37:	49 89 cc             	mov    r12,rcx
     c3a:	55                   	push   rbp
     c3b:	48 89 d5             	mov    rbp,rdx
     c3e:	53                   	push   rbx
     c3f:	48 89 f3             	mov    rbx,rsi
     c42:	48 83 ec 08          	sub    rsp,0x8
  GETTABLE_INLINE(L, t, key, val);
     c46:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
     c4a:	66 3d 05 80          	cmp    ax,0x8005
     c4e:	75 60                	jne    cb0 <luaV_gettable+0x80>
     c50:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
     c55:	48 8b 3e             	mov    rdi,QWORD PTR [rsi]
     c58:	75 46                	jne    ca0 <luaV_gettable+0x70>
     c5a:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
     c5d:	8b 47 0c             	mov    eax,DWORD PTR [rdi+0xc]
     c60:	48 8d 56 ff          	lea    rdx,[rsi-0x1]
     c64:	48 39 c2             	cmp    rdx,rax
     c67:	0f 83 cb 00 00 00    	jae    d38 <luaV_gettable+0x108>
     c6d:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
     c71:	48 c1 e6 04          	shl    rsi,0x4
     c75:	48 8d 44 30 f0       	lea    rax,[rax+rsi*1-0x10]
     c7a:	0f b7 50 08          	movzx  edx,WORD PTR [rax+0x8]
     c7e:	66 85 d2             	test   dx,dx
     c81:	74 7d                	je     d00 <luaV_gettable+0xd0>
     c83:	48 8b 00             	mov    rax,QWORD PTR [rax]
     c86:	66 41 89 54 24 08    	mov    WORD PTR [r12+0x8],dx
     c8c:	49 89 04 24          	mov    QWORD PTR [r12],rax
}
     c90:	48 83 c4 08          	add    rsp,0x8
     c94:	5b                   	pop    rbx
     c95:	5d                   	pop    rbp
     c96:	41 5c                	pop    r12
     c98:	41 5d                	pop    r13
     c9a:	c3                   	ret    
     c9b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  GETTABLE_INLINE(L, t, key, val);
     ca0:	48 89 d6             	mov    rsi,rdx
     ca3:	e8 00 00 00 00       	call   ca8 <luaV_gettable+0x78>
     ca8:	eb d0                	jmp    c7a <luaV_gettable+0x4a>
     caa:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
     cb0:	66 3d 25 80          	cmp    ax,0x8025
     cb4:	0f 84 8e 00 00 00    	je     d48 <luaV_gettable+0x118>
     cba:	66 3d 15 80          	cmp    ax,0x8015
     cbe:	75 60                	jne    d20 <luaV_gettable+0xf0>
     cc0:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
     cc5:	0f 85 c2 00 00 00    	jne    d8d <luaV_gettable+0x15d>
     ccb:	48 8b 16             	mov    rdx,QWORD PTR [rsi]
     cce:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
     cd2:	48 8b 4a 38          	mov    rcx,QWORD PTR [rdx+0x38]
     cd6:	39 42 40             	cmp    DWORD PTR [rdx+0x40],eax
     cd9:	0f 86 9d 00 00 00    	jbe    d7c <luaV_gettable+0x14c>
     cdf:	89 c0                	mov    eax,eax
     ce1:	48 8b 04 c1          	mov    rax,QWORD PTR [rcx+rax*8]
     ce5:	49 89 04 24          	mov    QWORD PTR [r12],rax
     ce9:	b8 13 00 00 00       	mov    eax,0x13
     cee:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
     cf4:	eb 9a                	jmp    c90 <luaV_gettable+0x60>
     cf6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
     cfd:	00 00 00 
}
     d00:	48 83 c4 08          	add    rsp,0x8
  GETTABLE_INLINE(L, t, key, val);
     d04:	4c 89 e1             	mov    rcx,r12
     d07:	48 89 ea             	mov    rdx,rbp
     d0a:	48 89 de             	mov    rsi,rbx
     d0d:	4c 89 ef             	mov    rdi,r13
}
     d10:	5b                   	pop    rbx
  GETTABLE_INLINE(L, t, key, val);
     d11:	49 89 c0             	mov    r8,rax
}
     d14:	5d                   	pop    rbp
     d15:	41 5c                	pop    r12
     d17:	41 5d                	pop    r13
  GETTABLE_INLINE(L, t, key, val);
     d19:	e9 92 fb ff ff       	jmp    8b0 <luaV_finishget>
     d1e:	66 90                	xchg   ax,ax
}
     d20:	48 83 c4 08          	add    rsp,0x8
  GETTABLE_INLINE(L, t, key, val);
     d24:	45 31 c0             	xor    r8d,r8d
}
     d27:	5b                   	pop    rbx
     d28:	5d                   	pop    rbp
     d29:	41 5c                	pop    r12
     d2b:	41 5d                	pop    r13
  GETTABLE_INLINE(L, t, key, val);
     d2d:	e9 7e fb ff ff       	jmp    8b0 <luaV_finishget>
     d32:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
     d38:	e8 00 00 00 00       	call   d3d <luaV_gettable+0x10d>
     d3d:	e9 38 ff ff ff       	jmp    c7a <luaV_gettable+0x4a>
     d42:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
     d48:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
     d4d:	75 3e                	jne    d8d <luaV_gettable+0x15d>
     d4f:	48 8b 16             	mov    rdx,QWORD PTR [rsi]
     d52:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
     d56:	48 8b 4a 38          	mov    rcx,QWORD PTR [rdx+0x38]
     d5a:	39 42 40             	cmp    DWORD PTR [rdx+0x40],eax
     d5d:	76 1d                	jbe    d7c <luaV_gettable+0x14c>
     d5f:	89 c0                	mov    eax,eax
     d61:	ba 03 00 00 00       	mov    edx,0x3
     d66:	f2 0f 10 04 c1       	movsd  xmm0,QWORD PTR [rcx+rax*8]
     d6b:	66 41 89 54 24 08    	mov    WORD PTR [r12+0x8],dx
     d71:	f2 41 0f 11 04 24    	movsd  QWORD PTR [r12],xmm0
     d77:	e9 14 ff ff ff       	jmp    c90 <luaV_gettable+0x60>
     d7c:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # d83 <luaV_gettable+0x153>
     d83:	4c 89 ef             	mov    rdi,r13
     d86:	31 c0                	xor    eax,eax
     d88:	e8 00 00 00 00       	call   d8d <luaV_gettable+0x15d>
     d8d:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # d94 <luaV_gettable+0x164>
     d94:	48 89 ee             	mov    rsi,rbp
     d97:	4c 89 ef             	mov    rdi,r13
     d9a:	e8 00 00 00 00       	call   d9f <luaV_gettable+0x16f>
     d9f:	90                   	nop

0000000000000da0 <luaV_settable>:
void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val) {
     da0:	41 55                	push   r13
     da2:	49 89 fd             	mov    r13,rdi
     da5:	41 54                	push   r12
     da7:	49 89 d4             	mov    r12,rdx
     daa:	55                   	push   rbp
     dab:	48 89 cd             	mov    rbp,rcx
     dae:	53                   	push   rbx
     daf:	48 89 f3             	mov    rbx,rsi
     db2:	48 83 ec 18          	sub    rsp,0x18
     db6:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
     dbd:	00 00 
     dbf:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
     dc4:	31 c0                	xor    eax,eax
  SETTABLE_INLINE(L, t, key, val);
     dc6:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
     dca:	66 3d 05 80          	cmp    ax,0x8005
     dce:	0f 85 b4 00 00 00    	jne    e88 <luaV_settable+0xe8>
     dd4:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
     dd9:	48 8b 3e             	mov    rdi,QWORD PTR [rsi]
     ddc:	75 62                	jne    e40 <luaV_settable+0xa0>
     dde:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
     de1:	8b 47 0c             	mov    eax,DWORD PTR [rdi+0xc]
     de4:	48 8d 56 ff          	lea    rdx,[rsi-0x1]
     de8:	48 39 c2             	cmp    rdx,rax
     deb:	0f 83 ef 00 00 00    	jae    ee0 <luaV_settable+0x140>
     df1:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
     df5:	48 c1 e6 04          	shl    rsi,0x4
     df9:	48 8d 44 30 f0       	lea    rax,[rax+rsi*1-0x10]
     dfe:	66 83 78 08 00       	cmp    WORD PTR [rax+0x8],0x0
     e03:	74 4a                	je     e4f <luaV_settable+0xaf>
     e05:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
     e09:	48 89 10             	mov    QWORD PTR [rax],rdx
     e0c:	0f b7 55 08          	movzx  edx,WORD PTR [rbp+0x8]
     e10:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
     e14:	66 83 7d 08 00       	cmp    WORD PTR [rbp+0x8],0x0
     e19:	78 4d                	js     e68 <luaV_settable+0xc8>
}
     e1b:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
     e20:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
     e27:	00 00 
     e29:	0f 85 f1 01 00 00    	jne    1020 <luaV_settable+0x280>
     e2f:	48 83 c4 18          	add    rsp,0x18
     e33:	5b                   	pop    rbx
     e34:	5d                   	pop    rbp
     e35:	41 5c                	pop    r12
     e37:	41 5d                	pop    r13
     e39:	c3                   	ret    
     e3a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  SETTABLE_INLINE(L, t, key, val);
     e40:	48 89 d6             	mov    rsi,rdx
     e43:	e8 00 00 00 00       	call   e48 <luaV_settable+0xa8>
     e48:	66 83 78 08 00       	cmp    WORD PTR [rax+0x8],0x0
     e4d:	75 b6                	jne    e05 <luaV_settable+0x65>
     e4f:	49 89 c0             	mov    r8,rax
     e52:	48 89 e9             	mov    rcx,rbp
     e55:	4c 89 e2             	mov    rdx,r12
     e58:	48 89 de             	mov    rsi,rbx
     e5b:	4c 89 ef             	mov    rdi,r13
     e5e:	e8 00 00 00 00       	call   e63 <luaV_settable+0xc3>
     e63:	eb b6                	jmp    e1b <luaV_settable+0x7b>
     e65:	0f 1f 00             	nop    DWORD PTR [rax]
     e68:	48 8b 33             	mov    rsi,QWORD PTR [rbx]
     e6b:	f6 46 09 04          	test   BYTE PTR [rsi+0x9],0x4
     e6f:	74 aa                	je     e1b <luaV_settable+0x7b>
     e71:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
     e75:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
     e79:	74 a0                	je     e1b <luaV_settable+0x7b>
     e7b:	4c 89 ef             	mov    rdi,r13
     e7e:	e8 00 00 00 00       	call   e83 <luaV_settable+0xe3>
     e83:	eb 96                	jmp    e1b <luaV_settable+0x7b>
     e85:	0f 1f 00             	nop    DWORD PTR [rax]
     e88:	66 3d 25 80          	cmp    ax,0x8025
     e8c:	74 62                	je     ef0 <luaV_settable+0x150>
     e8e:	66 3d 15 80          	cmp    ax,0x8015
     e92:	75 3c                	jne    ed0 <luaV_settable+0x130>
     e94:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
     e99:	48 8b 1e             	mov    rbx,QWORD PTR [rsi]
     e9c:	0f 85 a5 01 00 00    	jne    1047 <luaV_settable+0x2a7>
     ea2:	66 83 79 08 13       	cmp    WORD PTR [rcx+0x8],0x13
     ea7:	0f 85 93 00 00 00    	jne    f40 <luaV_settable+0x1a0>
     ead:	48 8b 02             	mov    rax,QWORD PTR [rdx]
     eb0:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
     eb3:	89 c2                	mov    edx,eax
     eb5:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
     eb8:	0f 86 17 01 00 00    	jbe    fd5 <luaV_settable+0x235>
     ebe:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
     ec2:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
     ec6:	e9 50 ff ff ff       	jmp    e1b <luaV_settable+0x7b>
     ecb:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
     ed0:	45 31 c0             	xor    r8d,r8d
     ed3:	e8 00 00 00 00       	call   ed8 <luaV_settable+0x138>
}
     ed8:	e9 3e ff ff ff       	jmp    e1b <luaV_settable+0x7b>
     edd:	0f 1f 00             	nop    DWORD PTR [rax]
  SETTABLE_INLINE(L, t, key, val);
     ee0:	e8 00 00 00 00       	call   ee5 <luaV_settable+0x145>
     ee5:	e9 14 ff ff ff       	jmp    dfe <luaV_settable+0x5e>
     eea:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
     ef0:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
     ef5:	48 8b 1e             	mov    rbx,QWORD PTR [rsi]
     ef8:	0f 85 49 01 00 00    	jne    1047 <luaV_settable+0x2a7>
     efe:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
     f02:	66 83 f8 03          	cmp    ax,0x3
     f06:	74 78                	je     f80 <luaV_settable+0x1e0>
     f08:	66 83 f8 13          	cmp    ax,0x13
     f0c:	0f 85 8e 00 00 00    	jne    fa0 <luaV_settable+0x200>
     f12:	48 8b 02             	mov    rax,QWORD PTR [rdx]
     f15:	66 0f ef c0          	pxor   xmm0,xmm0
     f19:	f2 48 0f 2a 01       	cvtsi2sd xmm0,QWORD PTR [rcx]
     f1e:	89 c2                	mov    edx,eax
     f20:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
     f23:	0f 86 c9 00 00 00    	jbe    ff2 <luaV_settable+0x252>
     f29:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
     f2d:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
     f32:	e9 e4 fe ff ff       	jmp    e1b <luaV_settable+0x7b>
     f37:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
     f3e:	00 00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
     f40:	31 d2                	xor    edx,edx
     f42:	48 89 e6             	mov    rsi,rsp
     f45:	48 89 cf             	mov    rdi,rcx
  SETTABLE_INLINE(L, t, key, val);
     f48:	48 c7 04 24 00 00 00 	mov    QWORD PTR [rsp],0x0
     f4f:	00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
     f50:	e8 00 00 00 00       	call   f55 <luaV_settable+0x1b5>
  SETTABLE_INLINE(L, t, key, val);
     f55:	85 c0                	test   eax,eax
     f57:	0f 84 d9 00 00 00    	je     1036 <luaV_settable+0x296>
     f5d:	49 8b 04 24          	mov    rax,QWORD PTR [r12]
     f61:	48 8b 0c 24          	mov    rcx,QWORD PTR [rsp]
     f65:	89 c2                	mov    edx,eax
     f67:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
     f6a:	76 76                	jbe    fe2 <luaV_settable+0x242>
     f6c:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
     f70:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
     f74:	e9 a2 fe ff ff       	jmp    e1b <luaV_settable+0x7b>
     f79:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
     f80:	48 8b 02             	mov    rax,QWORD PTR [rdx]
     f83:	89 c2                	mov    edx,eax
     f85:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
     f88:	76 75                	jbe    fff <luaV_settable+0x25f>
     f8a:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
     f8e:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
     f92:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
     f97:	e9 7f fe ff ff       	jmp    e1b <luaV_settable+0x7b>
     f9c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
     fa0:	48 89 e6             	mov    rsi,rsp
     fa3:	48 89 cf             	mov    rdi,rcx
     fa6:	48 c7 04 24 00 00 00 	mov    QWORD PTR [rsp],0x0
     fad:	00 
     fae:	e8 00 00 00 00       	call   fb3 <luaV_settable+0x213>
     fb3:	85 c0                	test   eax,eax
     fb5:	74 6e                	je     1025 <luaV_settable+0x285>
     fb7:	49 8b 04 24          	mov    rax,QWORD PTR [r12]
     fbb:	f2 0f 10 04 24       	movsd  xmm0,QWORD PTR [rsp]
     fc0:	89 c2                	mov    edx,eax
     fc2:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
     fc5:	76 49                	jbe    1010 <luaV_settable+0x270>
     fc7:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
     fcb:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
     fd0:	e9 46 fe ff ff       	jmp    e1b <luaV_settable+0x7b>
     fd5:	48 89 de             	mov    rsi,rbx
     fd8:	e8 00 00 00 00       	call   fdd <luaV_settable+0x23d>
     fdd:	e9 39 fe ff ff       	jmp    e1b <luaV_settable+0x7b>
     fe2:	48 89 de             	mov    rsi,rbx
     fe5:	4c 89 ef             	mov    rdi,r13
     fe8:	e8 00 00 00 00       	call   fed <luaV_settable+0x24d>
     fed:	e9 29 fe ff ff       	jmp    e1b <luaV_settable+0x7b>
     ff2:	48 89 de             	mov    rsi,rbx
     ff5:	e8 00 00 00 00       	call   ffa <luaV_settable+0x25a>
     ffa:	e9 1c fe ff ff       	jmp    e1b <luaV_settable+0x7b>
     fff:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    1003:	48 89 de             	mov    rsi,rbx
    1006:	e8 00 00 00 00       	call   100b <luaV_settable+0x26b>
    100b:	e9 0b fe ff ff       	jmp    e1b <luaV_settable+0x7b>
    1010:	48 89 de             	mov    rsi,rbx
    1013:	4c 89 ef             	mov    rdi,r13
    1016:	e8 00 00 00 00       	call   101b <luaV_settable+0x27b>
    101b:	e9 fb fd ff ff       	jmp    e1b <luaV_settable+0x7b>
}
    1020:	e8 00 00 00 00       	call   1025 <luaV_settable+0x285>
  SETTABLE_INLINE(L, t, key, val);
    1025:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 102c <luaV_settable+0x28c>
    102c:	4c 89 ef             	mov    rdi,r13
    102f:	31 c0                	xor    eax,eax
    1031:	e8 00 00 00 00       	call   1036 <luaV_settable+0x296>
    1036:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 103d <luaV_settable+0x29d>
    103d:	4c 89 ef             	mov    rdi,r13
    1040:	31 c0                	xor    eax,eax
    1042:	e8 00 00 00 00       	call   1047 <luaV_settable+0x2a7>
    1047:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 104e <luaV_settable+0x2ae>
    104e:	4c 89 e6             	mov    rsi,r12
    1051:	4c 89 ef             	mov    rdi,r13
    1054:	e8 00 00 00 00       	call   1059 <luaV_settable+0x2b9>
    1059:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

0000000000001060 <luaV_lessthan>:
int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r) {
    1060:	41 54                	push   r12
    1062:	49 89 fc             	mov    r12,rdi
    1065:	55                   	push   rbp
    1066:	48 89 d5             	mov    rbp,rdx
    1069:	53                   	push   rbx
    106a:	48 89 f3             	mov    rbx,rsi
  if (ttisnumber(l) && ttisnumber(r))  /* both operands are numbers? */
    106d:	0f b7 76 08          	movzx  esi,WORD PTR [rsi+0x8]
    1071:	89 f0                	mov    eax,esi
    1073:	83 e0 0f             	and    eax,0xf
    1076:	66 83 f8 03          	cmp    ax,0x3
    107a:	74 34                	je     10b0 <luaV_lessthan+0x50>
  else if (ttisstring(l) && ttisstring(r))  /* both are strings? */
    107c:	66 83 f8 04          	cmp    ax,0x4
    1080:	75 0d                	jne    108f <luaV_lessthan+0x2f>
    1082:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    1086:	83 e0 0f             	and    eax,0xf
    1089:	66 83 f8 04          	cmp    ax,0x4
    108d:	74 41                	je     10d0 <luaV_lessthan+0x70>
  else if ((res = luaT_callorderTM(L, l, r, TM_LT)) < 0)  /* no metamethod? */
    108f:	b9 14 00 00 00       	mov    ecx,0x14
    1094:	48 89 ea             	mov    rdx,rbp
    1097:	48 89 de             	mov    rsi,rbx
    109a:	4c 89 e7             	mov    rdi,r12
    109d:	e8 00 00 00 00       	call   10a2 <luaV_lessthan+0x42>
    10a2:	85 c0                	test   eax,eax
    10a4:	78 3a                	js     10e0 <luaV_lessthan+0x80>
}
    10a6:	5b                   	pop    rbx
    10a7:	5d                   	pop    rbp
    10a8:	41 5c                	pop    r12
    10aa:	c3                   	ret    
    10ab:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  if (ttisnumber(l) && ttisnumber(r))  /* both operands are numbers? */
    10b0:	0f b7 4a 08          	movzx  ecx,WORD PTR [rdx+0x8]
    10b4:	89 c8                	mov    eax,ecx
    10b6:	83 e0 0f             	and    eax,0xf
    10b9:	66 83 f8 03          	cmp    ax,0x3
    10bd:	75 d0                	jne    108f <luaV_lessthan+0x2f>
    return LTnum(l, r);
    10bf:	48 8b 3b             	mov    rdi,QWORD PTR [rbx]
    10c2:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
}
    10c5:	5b                   	pop    rbx
    10c6:	5d                   	pop    rbp
    10c7:	41 5c                	pop    r12
    return LTnum(l, r);
    10c9:	e9 d2 f3 ff ff       	jmp    4a0 <LTnum.isra.2>
    10ce:	66 90                	xchg   ax,ax
    return l_strcmp(tsvalue(l), tsvalue(r)) < 0;
    10d0:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    10d3:	48 8b 3b             	mov    rdi,QWORD PTR [rbx]
    10d6:	e8 a5 ef ff ff       	call   80 <l_strcmp>
    10db:	c1 e8 1f             	shr    eax,0x1f
    10de:	eb c6                	jmp    10a6 <luaV_lessthan+0x46>
    luaG_ordererror(L, l, r);  /* error */
    10e0:	48 89 ea             	mov    rdx,rbp
    10e3:	48 89 de             	mov    rsi,rbx
    10e6:	4c 89 e7             	mov    rdi,r12
    10e9:	e8 00 00 00 00       	call   10ee <luaV_lessthan+0x8e>
    10ee:	66 90                	xchg   ax,ax

00000000000010f0 <luaV_lessequal>:
int luaV_lessequal (lua_State *L, const TValue *l, const TValue *r) {
    10f0:	41 54                	push   r12
    10f2:	49 89 fc             	mov    r12,rdi
    10f5:	55                   	push   rbp
    10f6:	48 89 d5             	mov    rbp,rdx
    10f9:	53                   	push   rbx
    10fa:	48 89 f3             	mov    rbx,rsi
  if (ttisnumber(l) && ttisnumber(r))  /* both operands are numbers? */
    10fd:	0f b7 76 08          	movzx  esi,WORD PTR [rsi+0x8]
    1101:	89 f0                	mov    eax,esi
    1103:	83 e0 0f             	and    eax,0xf
    1106:	66 83 f8 03          	cmp    ax,0x3
    110a:	74 74                	je     1180 <luaV_lessequal+0x90>
  else if (ttisstring(l) && ttisstring(r))  /* both are strings? */
    110c:	66 83 f8 04          	cmp    ax,0x4
    1110:	75 11                	jne    1123 <luaV_lessequal+0x33>
    1112:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    1116:	83 e0 0f             	and    eax,0xf
    1119:	66 83 f8 04          	cmp    ax,0x4
    111d:	0f 84 7d 00 00 00    	je     11a0 <luaV_lessequal+0xb0>
  else if ((res = luaT_callorderTM(L, l, r, TM_LE)) >= 0)  /* try 'le' */
    1123:	b9 15 00 00 00       	mov    ecx,0x15
    1128:	48 89 ea             	mov    rdx,rbp
    112b:	48 89 de             	mov    rsi,rbx
    112e:	4c 89 e7             	mov    rdi,r12
    1131:	e8 00 00 00 00       	call   1136 <luaV_lessequal+0x46>
    1136:	85 c0                	test   eax,eax
    1138:	78 06                	js     1140 <luaV_lessequal+0x50>
}
    113a:	5b                   	pop    rbx
    113b:	5d                   	pop    rbp
    113c:	41 5c                	pop    r12
    113e:	c3                   	ret    
    113f:	90                   	nop
    L->ci->callstatus |= CIST_LEQ;  /* mark it is doing 'lt' for 'le' */
    1140:	49 8b 44 24 20       	mov    rax,QWORD PTR [r12+0x20]
    res = luaT_callorderTM(L, r, l, TM_LT);
    1145:	48 89 da             	mov    rdx,rbx
    1148:	b9 14 00 00 00       	mov    ecx,0x14
    114d:	48 89 ee             	mov    rsi,rbp
    1150:	4c 89 e7             	mov    rdi,r12
    L->ci->callstatus |= CIST_LEQ;  /* mark it is doing 'lt' for 'le' */
    1153:	66 81 48 42 80 00    	or     WORD PTR [rax+0x42],0x80
    res = luaT_callorderTM(L, r, l, TM_LT);
    1159:	e8 00 00 00 00       	call   115e <luaV_lessequal+0x6e>
    L->ci->callstatus ^= CIST_LEQ;  /* clear mark */
    115e:	49 8b 54 24 20       	mov    rdx,QWORD PTR [r12+0x20]
    1163:	66 81 72 42 80 00    	xor    WORD PTR [rdx+0x42],0x80
    if (res < 0)
    1169:	85 c0                	test   eax,eax
    116b:	78 48                	js     11b5 <luaV_lessequal+0xc5>
    return !res;  /* result is negated */
    116d:	0f 94 c0             	sete   al
}
    1170:	5b                   	pop    rbx
    1171:	5d                   	pop    rbp
    return !res;  /* result is negated */
    1172:	0f b6 c0             	movzx  eax,al
}
    1175:	41 5c                	pop    r12
    1177:	c3                   	ret    
    1178:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    117f:	00 
  if (ttisnumber(l) && ttisnumber(r))  /* both operands are numbers? */
    1180:	0f b7 4a 08          	movzx  ecx,WORD PTR [rdx+0x8]
    1184:	89 c8                	mov    eax,ecx
    1186:	83 e0 0f             	and    eax,0xf
    1189:	66 83 f8 03          	cmp    ax,0x3
    118d:	75 94                	jne    1123 <luaV_lessequal+0x33>
    return LEnum(l, r);
    118f:	48 8b 3b             	mov    rdi,QWORD PTR [rbx]
    1192:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
}
    1195:	5b                   	pop    rbx
    1196:	5d                   	pop    rbp
    1197:	41 5c                	pop    r12
    return LEnum(l, r);
    1199:	e9 b2 f1 ff ff       	jmp    350 <LEnum.isra.4>
    119e:	66 90                	xchg   ax,ax
    return l_strcmp(tsvalue(l), tsvalue(r)) <= 0;
    11a0:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    11a3:	48 8b 3b             	mov    rdi,QWORD PTR [rbx]
    11a6:	e8 d5 ee ff ff       	call   80 <l_strcmp>
    11ab:	85 c0                	test   eax,eax
    11ad:	0f 9e c0             	setle  al
    11b0:	0f b6 c0             	movzx  eax,al
    11b3:	eb 85                	jmp    113a <luaV_lessequal+0x4a>
      luaG_ordererror(L, l, r);
    11b5:	48 89 ea             	mov    rdx,rbp
    11b8:	48 89 de             	mov    rsi,rbx
    11bb:	4c 89 e7             	mov    rdi,r12
    11be:	e8 00 00 00 00       	call   11c3 <luaV_lessequal+0xd3>
    11c3:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    11ca:	00 00 00 00 
    11ce:	66 90                	xchg   ax,ax

00000000000011d0 <luaV_equalobj>:
int luaV_equalobj (lua_State *L, const TValue *t1, const TValue *t2) {
    11d0:	41 54                	push   r12
    11d2:	55                   	push   rbp
    11d3:	48 89 f5             	mov    rbp,rsi
    11d6:	53                   	push   rbx
    11d7:	48 89 d3             	mov    rbx,rdx
    11da:	48 83 ec 20          	sub    rsp,0x20
  if (ttype(t1) != ttype(t2)) {  /* not the same variant? */
    11de:	0f b7 4a 08          	movzx  ecx,WORD PTR [rdx+0x8]
int luaV_equalobj (lua_State *L, const TValue *t1, const TValue *t2) {
    11e2:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    11e9:	00 00 
    11eb:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    11f0:	31 c0                	xor    eax,eax
  if (ttype(t1) != ttype(t2)) {  /* not the same variant? */
    11f2:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    11f6:	89 c2                	mov    edx,eax
    11f8:	31 ca                	xor    edx,ecx
    11fa:	f6 c2 7f             	test   dl,0x7f
    11fd:	74 31                	je     1230 <luaV_equalobj+0x60>
    if (ttnov(t1) != ttnov(t2) || ttnov(t1) != LUA_TNUMBER)
    11ff:	83 e2 0f             	and    edx,0xf
    1202:	75 0b                	jne    120f <luaV_equalobj+0x3f>
    1204:	89 c2                	mov    edx,eax
    1206:	83 e2 0f             	and    edx,0xf
    1209:	66 83 fa 03          	cmp    dx,0x3
    120d:	74 49                	je     1258 <luaV_equalobj+0x88>
  return !l_isfalse(L->top);
    120f:	31 c0                	xor    eax,eax
}
    1211:	48 8b 74 24 18       	mov    rsi,QWORD PTR [rsp+0x18]
    1216:	64 48 33 34 25 28 00 	xor    rsi,QWORD PTR fs:0x28
    121d:	00 00 
    121f:	0f 85 f8 02 00 00    	jne    151d <luaV_equalobj+0x34d>
    1225:	48 83 c4 20          	add    rsp,0x20
    1229:	5b                   	pop    rbx
    122a:	5d                   	pop    rbp
    122b:	41 5c                	pop    r12
    122d:	c3                   	ret    
    122e:	66 90                	xchg   ax,ax
  switch (ttype(t1)) {
    1230:	83 e0 7f             	and    eax,0x7f
    1233:	66 83 f8 46          	cmp    ax,0x46
    1237:	0f 87 9b 00 00 00    	ja     12d8 <luaV_equalobj+0x108>
    123d:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 1244 <luaV_equalobj+0x74>
    1244:	0f b7 c0             	movzx  eax,ax
    1247:	49 89 fc             	mov    r12,rdi
    124a:	48 63 04 82          	movsxd rax,DWORD PTR [rdx+rax*4]
    124e:	48 01 d0             	add    rax,rdx
    1251:	ff e0                	jmp    rax
    1253:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
      return (tointegerns(t1, &i1) && tointegerns(t2, &i2) && i1 == i2);
    1258:	66 83 f8 13          	cmp    ax,0x13
    125c:	0f 84 46 02 00 00    	je     14a8 <luaV_equalobj+0x2d8>
  if (!ttisfloat(obj))
    1262:	66 83 f8 03          	cmp    ax,0x3
    1266:	0f 85 84 00 00 00    	jne    12f0 <luaV_equalobj+0x120>
    126c:	31 d2                	xor    edx,edx
    126e:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    1273:	48 89 ef             	mov    rdi,rbp
    1276:	e8 15 f0 ff ff       	call   290 <luaV_flttointeger.part.7>
      return (tointegerns(t1, &i1) && tointegerns(t2, &i2) && i1 == i2);
    127b:	85 c0                	test   eax,eax
    127d:	74 71                	je     12f0 <luaV_equalobj+0x120>
    127f:	0f b7 4b 08          	movzx  ecx,WORD PTR [rbx+0x8]
    1283:	66 83 f9 13          	cmp    cx,0x13
    1287:	0f 84 2b 02 00 00    	je     14b8 <luaV_equalobj+0x2e8>
  if (!ttisfloat(obj))
    128d:	66 83 f9 03          	cmp    cx,0x3
    1291:	75 5d                	jne    12f0 <luaV_equalobj+0x120>
    1293:	31 d2                	xor    edx,edx
    1295:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    129a:	48 89 df             	mov    rdi,rbx
    129d:	e8 ee ef ff ff       	call   290 <luaV_flttointeger.part.7>
      return (tointegerns(t1, &i1) && tointegerns(t2, &i2) && i1 == i2);
    12a2:	85 c0                	test   eax,eax
    12a4:	74 4a                	je     12f0 <luaV_equalobj+0x120>
    12a6:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    12ab:	48 39 44 24 08       	cmp    QWORD PTR [rsp+0x8],rax
    12b0:	0f 94 c0             	sete   al
    12b3:	0f b6 c0             	movzx  eax,al
    12b6:	e9 56 ff ff ff       	jmp    1211 <luaV_equalobj+0x41>
    12bb:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    case LUA_TLIGHTUSERDATA: return pvalue(t1) == pvalue(t2);
    12c0:	48 8b 03             	mov    rax,QWORD PTR [rbx]
    12c3:	48 39 06             	cmp    QWORD PTR [rsi],rax
    12c6:	0f 94 c0             	sete   al
    12c9:	0f b6 c0             	movzx  eax,al
    12cc:	e9 40 ff ff ff       	jmp    1211 <luaV_equalobj+0x41>
    12d1:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    case LUA_TSHRSTR: return eqshrstr(tsvalue(t1), tsvalue(t2));
    12d8:	48 8b 03             	mov    rax,QWORD PTR [rbx]
    12db:	48 39 45 00          	cmp    QWORD PTR [rbp+0x0],rax
    12df:	0f 94 c0             	sete   al
    12e2:	0f b6 c0             	movzx  eax,al
    12e5:	e9 27 ff ff ff       	jmp    1211 <luaV_equalobj+0x41>
    12ea:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
      return (tointegerns(t1, &i1) && tointegerns(t2, &i2) && i1 == i2);
    12f0:	31 c0                	xor    eax,eax
    12f2:	e9 1a ff ff ff       	jmp    1211 <luaV_equalobj+0x41>
    12f7:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
    12fe:	00 00 
    case LUA_TNIL: return 1;
    1300:	b8 01 00 00 00       	mov    eax,0x1
    1305:	e9 07 ff ff ff       	jmp    1211 <luaV_equalobj+0x41>
    130a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    case LUA_TBOOLEAN: return bvalue(t1) == bvalue(t2);  /* true must be 1 !! */
    1310:	8b 03                	mov    eax,DWORD PTR [rbx]
    1312:	39 06                	cmp    DWORD PTR [rsi],eax
    1314:	0f 94 c0             	sete   al
    1317:	0f b6 c0             	movzx  eax,al
    131a:	e9 f2 fe ff ff       	jmp    1211 <luaV_equalobj+0x41>
    131f:	90                   	nop
    case LUA_TNUMFLT: return luai_numeq(fltvalue(t1), fltvalue(t2));
    1320:	31 c0                	xor    eax,eax
    1322:	f2 0f 10 06          	movsd  xmm0,QWORD PTR [rsi]
    1326:	66 0f 2e 03          	ucomisd xmm0,QWORD PTR [rbx]
    132a:	ba 00 00 00 00       	mov    edx,0x0
    132f:	0f 9b c0             	setnp  al
    1332:	0f 45 c2             	cmovne eax,edx
    1335:	e9 d7 fe ff ff       	jmp    1211 <luaV_equalobj+0x41>
    133a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
      if (hvalue(t1) == hvalue(t2)) return 1;
    1340:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    1343:	48 8b 13             	mov    rdx,QWORD PTR [rbx]
    1346:	48 39 d0             	cmp    rax,rdx
    1349:	0f 84 8a 00 00 00    	je     13d9 <luaV_equalobj+0x209>
      else if (L == NULL) return 0;
    134f:	48 85 ff             	test   rdi,rdi
    1352:	0f 84 b7 fe ff ff    	je     120f <luaV_equalobj+0x3f>
      tm = fasttm(L, hvalue(t1)->metatable, TM_EQ);
    1358:	48 8b 78 28          	mov    rdi,QWORD PTR [rax+0x28]
    135c:	48 85 ff             	test   rdi,rdi
    135f:	74 0a                	je     136b <luaV_equalobj+0x19b>
    1361:	f6 47 0a 20          	test   BYTE PTR [rdi+0xa],0x20
    1365:	0f 84 7c 01 00 00    	je     14e7 <luaV_equalobj+0x317>
        tm = fasttm(L, hvalue(t2)->metatable, TM_EQ);
    136b:	48 8b 7a 28          	mov    rdi,QWORD PTR [rdx+0x28]
    136f:	48 85 ff             	test   rdi,rdi
    1372:	0f 84 97 fe ff ff    	je     120f <luaV_equalobj+0x3f>
    1378:	f6 47 0a 20          	test   BYTE PTR [rdi+0xa],0x20
    137c:	0f 85 8d fe ff ff    	jne    120f <luaV_equalobj+0x3f>
    1382:	49 8b 44 24 18       	mov    rax,QWORD PTR [r12+0x18]
    1387:	be 05 00 00 00       	mov    esi,0x5
    138c:	48 8b 90 08 01 00 00 	mov    rdx,QWORD PTR [rax+0x108]
    1393:	e8 00 00 00 00       	call   1398 <luaV_equalobj+0x1c8>
  if (tm == NULL)  /* no TM? */
    1398:	48 85 c0             	test   rax,rax
    139b:	0f 84 6e fe ff ff    	je     120f <luaV_equalobj+0x3f>
  luaT_callTM(L, tm, t1, t2, L->top, 1);  /* call TM */
    13a1:	4d 8b 44 24 10       	mov    r8,QWORD PTR [r12+0x10]
    13a6:	48 89 ea             	mov    rdx,rbp
    13a9:	48 89 c6             	mov    rsi,rax
    13ac:	48 89 d9             	mov    rcx,rbx
    13af:	41 b9 01 00 00 00    	mov    r9d,0x1
    13b5:	4c 89 e7             	mov    rdi,r12
    13b8:	e8 00 00 00 00       	call   13bd <luaV_equalobj+0x1ed>
  return !l_isfalse(L->top);
    13bd:	49 8b 54 24 10       	mov    rdx,QWORD PTR [r12+0x10]
    13c2:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    13c6:	66 85 c0             	test   ax,ax
    13c9:	0f 84 40 fe ff ff    	je     120f <luaV_equalobj+0x3f>
    13cf:	66 83 f8 01          	cmp    ax,0x1
    13d3:	0f 84 35 01 00 00    	je     150e <luaV_equalobj+0x33e>
      if (uvalue(t1) == uvalue(t2)) return 1;
    13d9:	b8 01 00 00 00       	mov    eax,0x1
    13de:	e9 2e fe ff ff       	jmp    1211 <luaV_equalobj+0x41>
    13e3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    13e8:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    13eb:	48 8b 13             	mov    rdx,QWORD PTR [rbx]
    13ee:	48 39 d0             	cmp    rax,rdx
    13f1:	74 e6                	je     13d9 <luaV_equalobj+0x209>
      else if (L == NULL) return 0;
    13f3:	48 85 ff             	test   rdi,rdi
    13f6:	0f 84 13 fe ff ff    	je     120f <luaV_equalobj+0x3f>
      tm = fasttm(L, uvalue(t1)->metatable, TM_EQ);
    13fc:	48 8b 78 10          	mov    rdi,QWORD PTR [rax+0x10]
    1400:	48 85 ff             	test   rdi,rdi
    1403:	74 0a                	je     140f <luaV_equalobj+0x23f>
    1405:	f6 47 0a 20          	test   BYTE PTR [rdi+0xa],0x20
    1409:	0f 84 b1 00 00 00    	je     14c0 <luaV_equalobj+0x2f0>
        tm = fasttm(L, uvalue(t2)->metatable, TM_EQ);
    140f:	48 8b 7a 10          	mov    rdi,QWORD PTR [rdx+0x10]
    1413:	48 85 ff             	test   rdi,rdi
    1416:	0f 84 f3 fd ff ff    	je     120f <luaV_equalobj+0x3f>
    141c:	f6 47 0a 20          	test   BYTE PTR [rdi+0xa],0x20
    1420:	0f 85 e9 fd ff ff    	jne    120f <luaV_equalobj+0x3f>
    1426:	49 8b 44 24 18       	mov    rax,QWORD PTR [r12+0x18]
    142b:	be 05 00 00 00       	mov    esi,0x5
    1430:	48 8b 90 08 01 00 00 	mov    rdx,QWORD PTR [rax+0x108]
    1437:	e8 00 00 00 00       	call   143c <luaV_equalobj+0x26c>
    143c:	e9 57 ff ff ff       	jmp    1398 <luaV_equalobj+0x1c8>
    1441:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    case LUA_TNUMINT: return (ivalue(t1) == ivalue(t2));
    1448:	48 8b 03             	mov    rax,QWORD PTR [rbx]
    144b:	48 39 06             	cmp    QWORD PTR [rsi],rax
    144e:	0f 94 c0             	sete   al
    1451:	0f b6 c0             	movzx  eax,al
    1454:	e9 b8 fd ff ff       	jmp    1211 <luaV_equalobj+0x41>
    1459:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    case LUA_TLNGSTR: return luaS_eqlngstr(tsvalue(t1), tsvalue(t2));
    1460:	48 8b 33             	mov    rsi,QWORD PTR [rbx]
    1463:	48 8b 7d 00          	mov    rdi,QWORD PTR [rbp+0x0]
    1467:	e8 00 00 00 00       	call   146c <luaV_equalobj+0x29c>
    146c:	e9 a0 fd ff ff       	jmp    1211 <luaV_equalobj+0x41>
    1471:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
      if (hvalue(t1) == hvalue(t2)) return 1;
    1478:	48 8b 03             	mov    rax,QWORD PTR [rbx]
    147b:	48 39 06             	cmp    QWORD PTR [rsi],rax
    147e:	0f 94 c0             	sete   al
    1481:	0f b6 c0             	movzx  eax,al
    1484:	e9 88 fd ff ff       	jmp    1211 <luaV_equalobj+0x41>
    1489:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    case LUA_TLCF: return fvalue(t1) == fvalue(t2);
    1490:	48 8b 03             	mov    rax,QWORD PTR [rbx]
    1493:	48 39 06             	cmp    QWORD PTR [rsi],rax
    1496:	0f 94 c0             	sete   al
    1499:	0f b6 c0             	movzx  eax,al
    149c:	e9 70 fd ff ff       	jmp    1211 <luaV_equalobj+0x41>
    14a1:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
      return (tointegerns(t1, &i1) && tointegerns(t2, &i2) && i1 == i2);
    14a8:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    14ab:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
    14b0:	e9 ce fd ff ff       	jmp    1283 <luaV_equalobj+0xb3>
    14b5:	0f 1f 00             	nop    DWORD PTR [rax]
    14b8:	48 8b 03             	mov    rax,QWORD PTR [rbx]
    14bb:	e9 eb fd ff ff       	jmp    12ab <luaV_equalobj+0xdb>
      tm = fasttm(L, uvalue(t1)->metatable, TM_EQ);
    14c0:	49 8b 44 24 18       	mov    rax,QWORD PTR [r12+0x18]
    14c5:	be 05 00 00 00       	mov    esi,0x5
    14ca:	48 8b 90 08 01 00 00 	mov    rdx,QWORD PTR [rax+0x108]
    14d1:	e8 00 00 00 00       	call   14d6 <luaV_equalobj+0x306>
      if (tm == NULL)
    14d6:	48 85 c0             	test   rax,rax
    14d9:	0f 85 c2 fe ff ff    	jne    13a1 <luaV_equalobj+0x1d1>
    14df:	48 8b 13             	mov    rdx,QWORD PTR [rbx]
    14e2:	e9 28 ff ff ff       	jmp    140f <luaV_equalobj+0x23f>
      tm = fasttm(L, hvalue(t1)->metatable, TM_EQ);
    14e7:	49 8b 44 24 18       	mov    rax,QWORD PTR [r12+0x18]
    14ec:	be 05 00 00 00       	mov    esi,0x5
    14f1:	48 8b 90 08 01 00 00 	mov    rdx,QWORD PTR [rax+0x108]
    14f8:	e8 00 00 00 00       	call   14fd <luaV_equalobj+0x32d>
      if (tm == NULL)
    14fd:	48 85 c0             	test   rax,rax
    1500:	0f 85 9b fe ff ff    	jne    13a1 <luaV_equalobj+0x1d1>
    1506:	48 8b 13             	mov    rdx,QWORD PTR [rbx]
    1509:	e9 5d fe ff ff       	jmp    136b <luaV_equalobj+0x19b>
  return !l_isfalse(L->top);
    150e:	8b 02                	mov    eax,DWORD PTR [rdx]
    1510:	85 c0                	test   eax,eax
    1512:	0f 85 c1 fe ff ff    	jne    13d9 <luaV_equalobj+0x209>
    1518:	e9 f2 fc ff ff       	jmp    120f <luaV_equalobj+0x3f>
}
    151d:	e8 00 00 00 00       	call   1522 <luaV_equalobj+0x352>
    1522:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    1529:	00 00 00 00 
    152d:	0f 1f 00             	nop    DWORD PTR [rax]

0000000000001530 <luaV_concat>:
void luaV_concat (lua_State *L, int total) {
    1530:	41 57                	push   r15
    1532:	41 56                	push   r14
    1534:	41 55                	push   r13
    1536:	41 54                	push   r12
    1538:	55                   	push   rbp
    1539:	89 f5                	mov    ebp,esi
    153b:	53                   	push   rbx
    153c:	48 89 fb             	mov    rbx,rdi
    153f:	48 83 ec 68          	sub    rsp,0x68
    1543:	4c 8b 7f 10          	mov    r15,QWORD PTR [rdi+0x10]
    1547:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    154e:	00 00 
    1550:	48 89 44 24 58       	mov    QWORD PTR [rsp+0x58],rax
    1555:	31 c0                	xor    eax,eax
        copy2buff(top, n, buff);  /* copy strings to buffer */
    1557:	48 8d 44 24 30       	lea    rax,[rsp+0x30]
    155c:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
    1561:	eb 4e                	jmp    15b1 <luaV_concat+0x81>
    1563:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    if (!(ttisstring(top-2) || cvt2str(top-2)) || !tostring(L, top-1))
    1568:	41 0f b7 47 f8       	movzx  eax,WORD PTR [r15-0x8]
    156d:	89 c6                	mov    esi,eax
    156f:	83 e6 0f             	and    esi,0xf
    1572:	66 83 fe 04          	cmp    si,0x4
    1576:	74 6d                	je     15e5 <luaV_concat+0xb5>
    1578:	49 8d 57 f0          	lea    rdx,[r15-0x10]
    157c:	66 83 fe 03          	cmp    si,0x3
    1580:	74 4e                	je     15d0 <luaV_concat+0xa0>
      luaT_trybinTM(L, top-2, top-1, top-2, TM_CONCAT);
    1582:	49 8d 77 e0          	lea    rsi,[r15-0x20]
    1586:	41 b8 16 00 00 00    	mov    r8d,0x16
    158c:	48 89 df             	mov    rdi,rbx
    158f:	83 ed 01             	sub    ebp,0x1
    1592:	48 89 f1             	mov    rcx,rsi
    1595:	e8 00 00 00 00       	call   159a <luaV_concat+0x6a>
    159a:	49 c7 c0 f0 ff ff ff 	mov    r8,0xfffffffffffffff0
    L->top -= n-1;  /* popped 'n' strings and pushed one */
    15a1:	4c 8b 7b 10          	mov    r15,QWORD PTR [rbx+0x10]
    15a5:	4d 01 c7             	add    r15,r8
    15a8:	4c 89 7b 10          	mov    QWORD PTR [rbx+0x10],r15
  } while (total > 1);  /* repeat until only 1 result left */
    15ac:	83 fd 01             	cmp    ebp,0x1
    15af:	7e 7f                	jle    1630 <luaV_concat+0x100>
    if (!(ttisstring(top-2) || cvt2str(top-2)) || !tostring(L, top-1))
    15b1:	41 0f b7 57 e8       	movzx  edx,WORD PTR [r15-0x18]
    15b6:	89 d0                	mov    eax,edx
    15b8:	83 e0 0f             	and    eax,0xf
    15bb:	83 e8 03             	sub    eax,0x3
    15be:	83 f8 01             	cmp    eax,0x1
    15c1:	76 a5                	jbe    1568 <luaV_concat+0x38>
    15c3:	49 8d 57 f0          	lea    rdx,[r15-0x10]
    15c7:	eb b9                	jmp    1582 <luaV_concat+0x52>
    15c9:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    15d0:	48 89 d6             	mov    rsi,rdx
    15d3:	48 89 df             	mov    rdi,rbx
    15d6:	e8 00 00 00 00       	call   15db <luaV_concat+0xab>
    15db:	41 0f b7 47 f8       	movzx  eax,WORD PTR [r15-0x8]
    15e0:	41 0f b7 57 e8       	movzx  edx,WORD PTR [r15-0x18]
    else if (isemptystr(top - 1))  /* second operand is empty? */
    15e5:	49 8b 77 f0          	mov    rsi,QWORD PTR [r15-0x10]
    15e9:	66 3d 04 80          	cmp    ax,0x8004
    15ed:	0f 84 35 01 00 00    	je     1728 <luaV_concat+0x1f8>
    else if (isemptystr(top - 2)) {  /* first operand is an empty string? */
    15f3:	66 81 fa 04 80       	cmp    dx,0x8004
    15f8:	75 5e                	jne    1658 <luaV_concat+0x128>
    15fa:	49 8b 57 e0          	mov    rdx,QWORD PTR [r15-0x20]
    15fe:	80 7a 0b 00          	cmp    BYTE PTR [rdx+0xb],0x0
    1602:	75 54                	jne    1658 <luaV_concat+0x128>
      setobjs2s(L, top - 2, top - 1);  /* result is second op. */
    1604:	49 8b 57 f0          	mov    rdx,QWORD PTR [r15-0x10]
    1608:	66 41 89 47 e8       	mov    WORD PTR [r15-0x18],ax
    160d:	49 c7 c0 f0 ff ff ff 	mov    r8,0xfffffffffffffff0
    1614:	83 ed 01             	sub    ebp,0x1
    1617:	49 89 57 e0          	mov    QWORD PTR [r15-0x20],rdx
    L->top -= n-1;  /* popped 'n' strings and pushed one */
    161b:	4c 8b 7b 10          	mov    r15,QWORD PTR [rbx+0x10]
    161f:	4d 01 c7             	add    r15,r8
    1622:	4c 89 7b 10          	mov    QWORD PTR [rbx+0x10],r15
  } while (total > 1);  /* repeat until only 1 result left */
    1626:	83 fd 01             	cmp    ebp,0x1
    1629:	7f 86                	jg     15b1 <luaV_concat+0x81>
    162b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
}
    1630:	48 8b 44 24 58       	mov    rax,QWORD PTR [rsp+0x58]
    1635:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    163c:	00 00 
    163e:	0f 85 2e 02 00 00    	jne    1872 <luaV_concat+0x342>
    1644:	48 83 c4 68          	add    rsp,0x68
    1648:	5b                   	pop    rbx
    1649:	5d                   	pop    rbp
    164a:	41 5c                	pop    r12
    164c:	41 5d                	pop    r13
    164e:	41 5e                	pop    r14
    1650:	41 5f                	pop    r15
    1652:	c3                   	ret    
    1653:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
      size_t tl = vslen(top - 1);
    1658:	80 7e 08 04          	cmp    BYTE PTR [rsi+0x8],0x4
    165c:	0f 84 de 01 00 00    	je     1840 <luaV_concat+0x310>
    1662:	4c 8b 76 10          	mov    r14,QWORD PTR [rsi+0x10]
      for (n = 1; n < total && tostring(L, top - n - 1); n++) {
    1666:	83 fd 01             	cmp    ebp,0x1
    1669:	0f 8e db 01 00 00    	jle    184a <luaV_concat+0x31a>
    166f:	44 8d 65 fe          	lea    r12d,[rbp-0x2]
    1673:	4d 8d 47 e0          	lea    r8,[r15-0x20]
    1677:	49 8d 44 24 02       	lea    rax,[r12+0x2]
    167c:	41 bc 01 00 00 00    	mov    r12d,0x1
    1682:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    1687:	eb 3b                	jmp    16c4 <luaV_concat+0x194>
    1689:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
        size_t l = vslen(top - n - 1);
    1690:	48 8b 52 10          	mov    rdx,QWORD PTR [rdx+0x10]
        if (l >= (MAX_SIZE/sizeof(char)) - tl)
    1694:	48 be ff ff ff ff ff 	movabs rsi,0x7fffffffffffffff
    169b:	ff ff 7f 
    169e:	4c 29 f6             	sub    rsi,r14
    16a1:	48 39 d6             	cmp    rsi,rdx
    16a4:	0f 86 b7 01 00 00    	jbe    1861 <luaV_concat+0x331>
        tl += l;
    16aa:	49 01 d6             	add    r14,rdx
    16ad:	45 8d 4d 01          	lea    r9d,[r13+0x1]
    16b1:	49 83 c4 01          	add    r12,0x1
    16b5:	49 83 e8 10          	sub    r8,0x10
      for (n = 1; n < total && tostring(L, top - n - 1); n++) {
    16b9:	4c 39 64 24 18       	cmp    QWORD PTR [rsp+0x18],r12
    16be:	0f 84 a4 00 00 00    	je     1768 <luaV_concat+0x238>
    16c4:	41 0f b7 50 08       	movzx  edx,WORD PTR [r8+0x8]
    16c9:	4c 89 e6             	mov    rsi,r12
    16cc:	45 89 e5             	mov    r13d,r12d
    16cf:	44 89 64 24 24       	mov    DWORD PTR [rsp+0x24],r12d
    16d4:	48 c1 e6 04          	shl    rsi,0x4
    16d8:	4d 89 c3             	mov    r11,r8
    16db:	83 e2 0f             	and    edx,0xf
    16de:	66 83 fa 04          	cmp    dx,0x4
    16e2:	74 29                	je     170d <luaV_concat+0x1dd>
    16e4:	66 83 fa 03          	cmp    dx,0x3
    16e8:	0f 85 2a 01 00 00    	jne    1818 <luaV_concat+0x2e8>
    16ee:	4c 89 c6             	mov    rsi,r8
    16f1:	48 89 df             	mov    rdi,rbx
    16f4:	4c 89 44 24 10       	mov    QWORD PTR [rsp+0x10],r8
    16f9:	4c 89 44 24 08       	mov    QWORD PTR [rsp+0x8],r8
    16fe:	e8 00 00 00 00       	call   1703 <luaV_concat+0x1d3>
    1703:	4c 8b 44 24 08       	mov    r8,QWORD PTR [rsp+0x8]
    1708:	4c 8b 5c 24 10       	mov    r11,QWORD PTR [rsp+0x10]
        size_t l = vslen(top - n - 1);
    170d:	49 8b 13             	mov    rdx,QWORD PTR [r11]
    1710:	80 7a 08 04          	cmp    BYTE PTR [rdx+0x8],0x4
    1714:	0f 85 76 ff ff ff    	jne    1690 <luaV_concat+0x160>
    171a:	0f b6 52 0b          	movzx  edx,BYTE PTR [rdx+0xb]
    171e:	e9 71 ff ff ff       	jmp    1694 <luaV_concat+0x164>
    1723:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    else if (isemptystr(top - 1))  /* second operand is empty? */
    1728:	80 7e 0b 00          	cmp    BYTE PTR [rsi+0xb],0x0
    172c:	0f 85 c1 fe ff ff    	jne    15f3 <luaV_concat+0xc3>
      cast_void(tostring(L, top - 2));  /* result is first operand */
    1732:	83 e2 0f             	and    edx,0xf
    1735:	83 ed 01             	sub    ebp,0x1
    1738:	49 c7 c0 f0 ff ff ff 	mov    r8,0xfffffffffffffff0
    173f:	66 83 fa 03          	cmp    dx,0x3
    1743:	0f 85 58 fe ff ff    	jne    15a1 <luaV_concat+0x71>
    1749:	49 8d 77 e0          	lea    rsi,[r15-0x20]
    174d:	48 89 df             	mov    rdi,rbx
    1750:	e8 00 00 00 00       	call   1755 <luaV_concat+0x225>
    1755:	49 c7 c0 f0 ff ff ff 	mov    r8,0xfffffffffffffff0
    175c:	e9 40 fe ff ff       	jmp    15a1 <luaV_concat+0x71>
    1761:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    1768:	4d 63 c1             	movsxd r8,r9d
    176b:	2b 6c 24 24          	sub    ebp,DWORD PTR [rsp+0x24]
    176f:	4d 89 c4             	mov    r12,r8
    1772:	49 f7 d8             	neg    r8
    1775:	49 c1 e4 04          	shl    r12,0x4
    1779:	49 c1 e0 04          	shl    r8,0x4
    177d:	49 f7 dc             	neg    r12
    1780:	49 83 c0 10          	add    r8,0x10
    1784:	4c 89 64 24 08       	mov    QWORD PTR [rsp+0x8],r12
      if (tl <= LUAI_MAXSHORTLEN) {  /* is result a short string? */
    1789:	49 83 fe 28          	cmp    r14,0x28
    178d:	77 51                	ja     17e0 <luaV_concat+0x2b0>
        copy2buff(top, n, buff);  /* copy strings to buffer */
    178f:	4c 8b 6c 24 28       	mov    r13,QWORD PTR [rsp+0x28]
    1794:	44 89 ce             	mov    esi,r9d
    1797:	4c 89 ff             	mov    rdi,r15
    179a:	4c 89 44 24 10       	mov    QWORD PTR [rsp+0x10],r8
    179f:	4c 89 ea             	mov    rdx,r13
    17a2:	e8 99 e9 ff ff       	call   140 <copy2buff>
        ts = luaS_newlstr(L, buff, tl);
    17a7:	4c 89 ee             	mov    rsi,r13
    17aa:	4c 89 f2             	mov    rdx,r14
    17ad:	48 89 df             	mov    rdi,rbx
    17b0:	e8 00 00 00 00       	call   17b5 <luaV_concat+0x285>
    17b5:	4c 8b 44 24 10       	mov    r8,QWORD PTR [rsp+0x10]
    17ba:	49 89 c5             	mov    r13,rax
      setsvalue2s(L, top - n, ts);  /* create result */
    17bd:	4c 8b 64 24 08       	mov    r12,QWORD PTR [rsp+0x8]
    17c2:	4d 01 fc             	add    r12,r15
    17c5:	4d 89 2c 24          	mov    QWORD PTR [r12],r13
    17c9:	41 0f b6 45 08       	movzx  eax,BYTE PTR [r13+0x8]
    17ce:	66 0d 00 80          	or     ax,0x8000
    17d2:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
    17d8:	e9 c4 fd ff ff       	jmp    15a1 <luaV_concat+0x71>
    17dd:	0f 1f 00             	nop    DWORD PTR [rax]
        ts = luaS_createlngstrobj(L, tl);
    17e0:	4c 89 f6             	mov    rsi,r14
    17e3:	48 89 df             	mov    rdi,rbx
    17e6:	4c 89 44 24 18       	mov    QWORD PTR [rsp+0x18],r8
    17eb:	44 89 4c 24 10       	mov    DWORD PTR [rsp+0x10],r9d
    17f0:	e8 00 00 00 00       	call   17f5 <luaV_concat+0x2c5>
        copy2buff(top, n, getstr(ts));
    17f5:	44 8b 4c 24 10       	mov    r9d,DWORD PTR [rsp+0x10]
    17fa:	4c 89 ff             	mov    rdi,r15
    17fd:	48 8d 50 18          	lea    rdx,[rax+0x18]
        ts = luaS_createlngstrobj(L, tl);
    1801:	49 89 c5             	mov    r13,rax
        copy2buff(top, n, getstr(ts));
    1804:	44 89 ce             	mov    esi,r9d
    1807:	e8 34 e9 ff ff       	call   140 <copy2buff>
    180c:	4c 8b 44 24 18       	mov    r8,QWORD PTR [rsp+0x18]
    1811:	eb aa                	jmp    17bd <luaV_concat+0x28d>
    1813:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    1818:	4d 89 e0             	mov    r8,r12
    181b:	45 8d 4c 24 ff       	lea    r9d,[r12-0x1]
    1820:	48 f7 de             	neg    rsi
    1823:	49 f7 d8             	neg    r8
    1826:	44 29 cd             	sub    ebp,r9d
    1829:	48 89 74 24 08       	mov    QWORD PTR [rsp+0x8],rsi
      for (n = 1; n < total && tostring(L, top - n - 1); n++) {
    182e:	45 89 e1             	mov    r9d,r12d
    1831:	49 c1 e0 04          	shl    r8,0x4
    1835:	49 83 c0 10          	add    r8,0x10
    1839:	e9 4b ff ff ff       	jmp    1789 <luaV_concat+0x259>
    183e:	66 90                	xchg   ax,ax
      size_t tl = vslen(top - 1);
    1840:	44 0f b6 76 0b       	movzx  r14d,BYTE PTR [rsi+0xb]
    1845:	e9 1c fe ff ff       	jmp    1666 <luaV_concat+0x136>
      for (n = 1; n < total && tostring(L, top - n - 1); n++) {
    184a:	45 31 c0             	xor    r8d,r8d
    184d:	41 b9 01 00 00 00    	mov    r9d,0x1
    1853:	48 c7 44 24 08 f0 ff 	mov    QWORD PTR [rsp+0x8],0xfffffffffffffff0
    185a:	ff ff 
    185c:	e9 28 ff ff ff       	jmp    1789 <luaV_concat+0x259>
          luaG_runerror(L, "string length overflow");
    1861:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 1868 <luaV_concat+0x338>
    1868:	48 89 df             	mov    rdi,rbx
    186b:	31 c0                	xor    eax,eax
    186d:	e8 00 00 00 00       	call   1872 <luaV_concat+0x342>
}
    1872:	e8 00 00 00 00       	call   1877 <luaV_concat+0x347>
    1877:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
    187e:	00 00 

0000000000001880 <luaV_objlen>:
void luaV_objlen (lua_State *L, StkId ra, const TValue *rb) {
    1880:	41 55                	push   r13
    1882:	41 54                	push   r12
    1884:	49 89 fc             	mov    r12,rdi
    1887:	55                   	push   rbp
    1888:	48 89 f5             	mov    rbp,rsi
    188b:	53                   	push   rbx
    188c:	48 89 d3             	mov    rbx,rdx
    188f:	48 83 ec 08          	sub    rsp,0x8
  switch (ttype(rb)) {
    1893:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    1897:	83 e0 7f             	and    eax,0x7f
    189a:	83 e8 04             	sub    eax,0x4
    189d:	66 83 f8 21          	cmp    ax,0x21
    18a1:	0f 87 c9 00 00 00    	ja     1970 <luaV_objlen+0xf0>
    18a7:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 18ae <luaV_objlen+0x2e>
    18ae:	0f b7 c0             	movzx  eax,ax
    18b1:	48 63 04 82          	movsxd rax,DWORD PTR [rdx+rax*4]
    18b5:	48 01 d0             	add    rax,rdx
    18b8:	ff e0                	jmp    rax
    18ba:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
      setivalue(ra, raviH_getn(h));
    18c0:	48 8b 3b             	mov    rdi,QWORD PTR [rbx]
    18c3:	e8 00 00 00 00       	call   18c8 <luaV_objlen+0x48>
    18c8:	be 13 00 00 00       	mov    esi,0x13
    18cd:	48 98                	cdqe   
    18cf:	66 89 75 08          	mov    WORD PTR [rbp+0x8],si
    18d3:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
}
    18d7:	48 83 c4 08          	add    rsp,0x8
    18db:	5b                   	pop    rbx
    18dc:	5d                   	pop    rbp
    18dd:	41 5c                	pop    r12
    18df:	41 5d                	pop    r13
    18e1:	c3                   	ret    
    18e2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
      setivalue(ra, tsvalue(rb)->u.lnglen);
    18e8:	48 8b 03             	mov    rax,QWORD PTR [rbx]
    18eb:	48 8b 40 10          	mov    rax,QWORD PTR [rax+0x10]
    18ef:	48 89 06             	mov    QWORD PTR [rsi],rax
    18f2:	b8 13 00 00 00       	mov    eax,0x13
    18f7:	66 89 46 08          	mov    WORD PTR [rsi+0x8],ax
}
    18fb:	48 83 c4 08          	add    rsp,0x8
    18ff:	5b                   	pop    rbx
    1900:	5d                   	pop    rbp
    1901:	41 5c                	pop    r12
    1903:	41 5d                	pop    r13
    1905:	c3                   	ret    
    1906:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
    190d:	00 00 00 
      Table *h = hvalue(rb);
    1910:	4c 8b 2b             	mov    r13,QWORD PTR [rbx]
      tm = fasttm(L, h->metatable, TM_LEN);
    1913:	49 8b 7d 28          	mov    rdi,QWORD PTR [r13+0x28]
    1917:	48 85 ff             	test   rdi,rdi
    191a:	74 0a                	je     1926 <luaV_objlen+0xa6>
    191c:	f6 47 0a 10          	test   BYTE PTR [rdi+0xa],0x10
    1920:	0f 84 8a 00 00 00    	je     19b0 <luaV_objlen+0x130>
      setivalue(ra, luaH_getn(h));  /* else primitive len */
    1926:	4c 89 ef             	mov    rdi,r13
    1929:	e8 00 00 00 00       	call   192e <luaV_objlen+0xae>
    192e:	b9 13 00 00 00       	mov    ecx,0x13
    1933:	48 98                	cdqe   
    1935:	66 89 4d 08          	mov    WORD PTR [rbp+0x8],cx
    1939:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
}
    193d:	48 83 c4 08          	add    rsp,0x8
    1941:	5b                   	pop    rbx
    1942:	5d                   	pop    rbp
    1943:	41 5c                	pop    r12
    1945:	41 5d                	pop    r13
    1947:	c3                   	ret    
    1948:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    194f:	00 
      setivalue(ra, tsvalue(rb)->shrlen);
    1950:	48 8b 03             	mov    rax,QWORD PTR [rbx]
    1953:	ba 13 00 00 00       	mov    edx,0x13
    1958:	0f b6 40 0b          	movzx  eax,BYTE PTR [rax+0xb]
    195c:	66 89 56 08          	mov    WORD PTR [rsi+0x8],dx
    1960:	48 89 06             	mov    QWORD PTR [rsi],rax
}
    1963:	48 83 c4 08          	add    rsp,0x8
    1967:	5b                   	pop    rbx
    1968:	5d                   	pop    rbp
    1969:	41 5c                	pop    r12
    196b:	41 5d                	pop    r13
    196d:	c3                   	ret    
    196e:	66 90                	xchg   ax,ax
      tm = luaT_gettmbyobj(L, rb, TM_LEN);
    1970:	ba 04 00 00 00       	mov    edx,0x4
    1975:	48 89 de             	mov    rsi,rbx
    1978:	4c 89 e7             	mov    rdi,r12
    197b:	e8 00 00 00 00       	call   1980 <luaV_objlen+0x100>
      if (ttisnil(tm))  /* no metamethod? */
    1980:	66 83 78 08 00       	cmp    WORD PTR [rax+0x8],0x0
    1985:	74 49                	je     19d0 <luaV_objlen+0x150>
}
    1987:	48 83 c4 08          	add    rsp,0x8
  luaT_callTM(L, tm, rb, rb, ra, 1);
    198b:	49 89 e8             	mov    r8,rbp
    198e:	48 89 d9             	mov    rcx,rbx
    1991:	48 89 da             	mov    rdx,rbx
    1994:	4c 89 e7             	mov    rdi,r12
}
    1997:	5b                   	pop    rbx
  luaT_callTM(L, tm, rb, rb, ra, 1);
    1998:	41 b9 01 00 00 00    	mov    r9d,0x1
}
    199e:	5d                   	pop    rbp
  luaT_callTM(L, tm, rb, rb, ra, 1);
    199f:	48 89 c6             	mov    rsi,rax
}
    19a2:	41 5c                	pop    r12
    19a4:	41 5d                	pop    r13
  luaT_callTM(L, tm, rb, rb, ra, 1);
    19a6:	e9 00 00 00 00       	jmp    19ab <luaV_objlen+0x12b>
    19ab:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
      tm = fasttm(L, h->metatable, TM_LEN);
    19b0:	49 8b 44 24 18       	mov    rax,QWORD PTR [r12+0x18]
    19b5:	be 04 00 00 00       	mov    esi,0x4
    19ba:	48 8b 90 00 01 00 00 	mov    rdx,QWORD PTR [rax+0x100]
    19c1:	e8 00 00 00 00       	call   19c6 <luaV_objlen+0x146>
      if (tm) break;  /* metamethod? break switch to call it */
    19c6:	48 85 c0             	test   rax,rax
    19c9:	75 bc                	jne    1987 <luaV_objlen+0x107>
    19cb:	e9 56 ff ff ff       	jmp    1926 <luaV_objlen+0xa6>
        luaG_typeerror(L, rb, "get length of");
    19d0:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 19d7 <luaV_objlen+0x157>
    19d7:	48 89 de             	mov    rsi,rbx
    19da:	4c 89 e7             	mov    rdi,r12
    19dd:	e8 00 00 00 00       	call   19e2 <luaV_objlen+0x162>
    19e2:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    19e9:	00 00 00 00 
    19ed:	0f 1f 00             	nop    DWORD PTR [rax]

00000000000019f0 <luaV_div>:
  if (l_castS2U(n) + 1u <= 1u) {  /* special cases: -1 or 0 */
    19f0:	48 8d 42 01          	lea    rax,[rdx+0x1]
lua_Integer luaV_div (lua_State *L, lua_Integer m, lua_Integer n) {
    19f4:	48 89 d1             	mov    rcx,rdx
  if (l_castS2U(n) + 1u <= 1u) {  /* special cases: -1 or 0 */
    19f7:	48 83 f8 01          	cmp    rax,0x1
    19fb:	77 13                	ja     1a10 <luaV_div+0x20>
    if (n == 0)
    19fd:	48 85 d2             	test   rdx,rdx
    1a00:	74 2a                	je     1a2c <luaV_div+0x3c>
    return intop(-, 0, m);   /* n==-1; avoid overflow with 0x80000...//-1 */
    1a02:	48 89 f0             	mov    rax,rsi
    1a05:	48 f7 d8             	neg    rax
    1a08:	c3                   	ret    
    1a09:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    lua_Integer q = m / n;  /* perform C division */
    1a10:	48 89 f0             	mov    rax,rsi
    1a13:	48 99                	cqo    
    1a15:	48 f7 f9             	idiv   rcx
    if ((m ^ n) < 0 && m % n != 0)  /* 'm/n' would be negative non-integer? */
    1a18:	48 31 f1             	xor    rcx,rsi
    1a1b:	78 03                	js     1a20 <luaV_div+0x30>
}
    1a1d:	c3                   	ret    
    1a1e:	66 90                	xchg   ax,ax
      q -= 1;  /* correct result for different rounding */
    1a20:	48 8d 48 ff          	lea    rcx,[rax-0x1]
    1a24:	48 85 d2             	test   rdx,rdx
    1a27:	48 0f 45 c1          	cmovne rax,rcx
}
    1a2b:	c3                   	ret    
lua_Integer luaV_div (lua_State *L, lua_Integer m, lua_Integer n) {
    1a2c:	50                   	push   rax
      luaG_runerror(L, "attempt to divide by zero");
    1a2d:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 1a34 <luaV_div+0x44>
    1a34:	31 c0                	xor    eax,eax
    1a36:	e8 00 00 00 00       	call   1a3b <luaV_div+0x4b>
    1a3b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]

0000000000001a40 <luaV_mod>:
  if (l_castS2U(n) + 1u <= 1u) {  /* special cases: -1 or 0 */
    1a40:	48 8d 42 01          	lea    rax,[rdx+0x1]
lua_Integer luaV_mod (lua_State *L, lua_Integer m, lua_Integer n) {
    1a44:	48 89 d1             	mov    rcx,rdx
  if (l_castS2U(n) + 1u <= 1u) {  /* special cases: -1 or 0 */
    1a47:	48 83 f8 01          	cmp    rax,0x1
    1a4b:	77 13                	ja     1a60 <luaV_mod+0x20>
    if (n == 0)
    1a4d:	48 85 d2             	test   rdx,rdx
    1a50:	74 2a                	je     1a7c <luaV_mod+0x3c>
    return 0;   /* m % -1 == 0; avoid overflow with 0x80000...%-1 */
    1a52:	31 d2                	xor    edx,edx
}
    1a54:	48 89 d0             	mov    rax,rdx
    1a57:	c3                   	ret    
    1a58:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    1a5f:	00 
    lua_Integer r = m % n;
    1a60:	48 89 f0             	mov    rax,rsi
    1a63:	48 99                	cqo    
    1a65:	48 f7 f9             	idiv   rcx
    if (r != 0 && (m ^ n) < 0)  /* 'm/n' would be non-integer negative? */
    1a68:	48 85 d2             	test   rdx,rdx
    1a6b:	74 e7                	je     1a54 <luaV_mod+0x14>
      r += n;  /* correct result for different rounding */
    1a6d:	48 8d 04 0a          	lea    rax,[rdx+rcx*1]
    1a71:	48 31 f1             	xor    rcx,rsi
    1a74:	48 0f 48 d0          	cmovs  rdx,rax
}
    1a78:	48 89 d0             	mov    rax,rdx
    1a7b:	c3                   	ret    
lua_Integer luaV_mod (lua_State *L, lua_Integer m, lua_Integer n) {
    1a7c:	50                   	push   rax
      luaG_runerror(L, "attempt to perform 'n%%0'");
    1a7d:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 1a84 <luaV_mod+0x44>
    1a84:	31 c0                	xor    eax,eax
    1a86:	e8 00 00 00 00       	call   1a8b <luaV_mod+0x4b>
    1a8b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]

0000000000001a90 <luaV_shiftl>:
  if (y < 0) {  /* shift right? */
    1a90:	48 85 f6             	test   rsi,rsi
    1a93:	78 1b                	js     1ab0 <luaV_shiftl+0x20>
    else return intop(<<, x, y);
    1a95:	89 f1                	mov    ecx,esi
    1a97:	b8 00 00 00 00       	mov    eax,0x0
    1a9c:	48 d3 e7             	shl    rdi,cl
    1a9f:	48 83 fe 40          	cmp    rsi,0x40
    1aa3:	48 0f 4c c7          	cmovl  rax,rdi
}
    1aa7:	c3                   	ret    
    1aa8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    1aaf:	00 
    if (y <= -NBITS) return 0;
    1ab0:	31 c0                	xor    eax,eax
    1ab2:	48 83 fe c1          	cmp    rsi,0xffffffffffffffc1
    1ab6:	7c ef                	jl     1aa7 <luaV_shiftl+0x17>
    else return intop(>>, x, -y);
    1ab8:	89 f1                	mov    ecx,esi
    1aba:	48 89 f8             	mov    rax,rdi
    1abd:	f7 d9                	neg    ecx
    1abf:	48 d3 e8             	shr    rax,cl
    1ac2:	c3                   	ret    
    1ac3:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    1aca:	00 00 00 00 
    1ace:	66 90                	xchg   ax,ax

0000000000001ad0 <luaV_finishOp>:


/*
** finish execution of an opcode interrupted by an yield
*/
void luaV_finishOp (lua_State *L) {
    1ad0:	41 54                	push   r12
    1ad2:	55                   	push   rbp
    1ad3:	53                   	push   rbx
  CallInfo *ci = L->ci;
    1ad4:	48 8b 6f 20          	mov    rbp,QWORD PTR [rdi+0x20]
  StkId base = ci->u.l.base;
  Instruction inst = *(ci->u.l.savedpc - 1);  /* interrupted instruction */
    1ad8:	48 8b 4d 28          	mov    rcx,QWORD PTR [rbp+0x28]
  StkId base = ci->u.l.base;
    1adc:	4c 8b 45 20          	mov    r8,QWORD PTR [rbp+0x20]
  Instruction inst = *(ci->u.l.savedpc - 1);  /* interrupted instruction */
    1ae0:	8b 59 fc             	mov    ebx,DWORD PTR [rcx-0x4]
  OpCode op = GET_OPCODE(inst);
    1ae3:	0f b6 c3             	movzx  eax,bl
  switch (op) {  /* finish its execution */
    1ae6:	83 e8 06             	sub    eax,0x6
    1ae9:	83 f8 6d             	cmp    eax,0x6d
    1aec:	77 65                	ja     1b53 <luaV_finishOp+0x83>
    1aee:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 1af5 <luaV_finishOp+0x25>
    1af5:	48 63 04 82          	movsxd rax,DWORD PTR [rdx+rax*4]
    1af9:	48 01 d0             	add    rax,rdx
    1afc:	ff e0                	jmp    rax
    1afe:	66 90                	xchg   ax,ax
    case OP_GETTABUP: case OP_GETTABLE: case OP_SELF: {
      setobjs2s(L, base + GETARG_A(inst), --L->top);
      break;
    }
    case OP_LE: case OP_LT: case OP_EQ: {
      int res = !l_isfalse(L->top - 1);
    1b00:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
    1b04:	31 d2                	xor    edx,edx
    1b06:	0f b7 70 f8          	movzx  esi,WORD PTR [rax-0x8]
    1b0a:	66 85 f6             	test   si,si
    1b0d:	74 19                	je     1b28 <luaV_finishOp+0x58>
    1b0f:	ba 01 00 00 00       	mov    edx,0x1
    1b14:	66 83 fe 01          	cmp    si,0x1
    1b18:	75 0e                	jne    1b28 <luaV_finishOp+0x58>
    1b1a:	8b 70 f0             	mov    esi,DWORD PTR [rax-0x10]
    1b1d:	31 d2                	xor    edx,edx
    1b1f:	85 f6                	test   esi,esi
    1b21:	0f 95 c2             	setne  dl
    1b24:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
      L->top--;
    1b28:	48 83 e8 10          	sub    rax,0x10
    1b2c:	48 89 47 10          	mov    QWORD PTR [rdi+0x10],rax
      if (ci->callstatus & CIST_LEQ) {  /* "<=" using "<" instead? */
    1b30:	0f b7 45 42          	movzx  eax,WORD PTR [rbp+0x42]
    1b34:	a8 80                	test   al,0x80
    1b36:	74 09                	je     1b41 <luaV_finishOp+0x71>
        lua_assert(op == OP_LE);
        ci->callstatus ^= CIST_LEQ;  /* clear mark */
    1b38:	34 80                	xor    al,0x80
    1b3a:	83 f2 01             	xor    edx,0x1
    1b3d:	66 89 45 42          	mov    WORD PTR [rbp+0x42],ax
        res = !res;  /* negate result */
      }
      lua_assert(GET_OPCODE(*ci->u.l.savedpc) == OP_JMP);
      if (res != GETARG_A(inst))  /* condition failed? */
    1b41:	c1 eb 08             	shr    ebx,0x8
    1b44:	83 e3 7f             	and    ebx,0x7f
    1b47:	39 d3                	cmp    ebx,edx
    1b49:	74 08                	je     1b53 <luaV_finishOp+0x83>
        ci->u.l.savedpc++;  /* skip jump instruction */
    1b4b:	48 83 c1 04          	add    rcx,0x4
    1b4f:	48 89 4d 28          	mov    QWORD PTR [rbp+0x28],rcx
    case OP_RAVI_SETI: case OP_RAVI_TABLE_SETFIELD: case OP_RAVI_SETFIELD:
    case OP_TAILCALL: case OP_SETTABUP: case OP_SETTABLE:
      break;
    default: lua_assert(0);
  }
}
    1b53:	5b                   	pop    rbx
    1b54:	5d                   	pop    rbp
    1b55:	41 5c                	pop    r12
    1b57:	c3                   	ret    
    1b58:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    1b5f:	00 
      if (GETARG_C(inst) - 1 >= 0)  /* nresults >= 0? */
    1b60:	81 e3 00 00 ff 00    	and    ebx,0xff0000
    1b66:	74 eb                	je     1b53 <luaV_finishOp+0x83>
        L->top = ci->top;  /* adjust results */
    1b68:	48 8b 45 08          	mov    rax,QWORD PTR [rbp+0x8]
    1b6c:	48 89 47 10          	mov    QWORD PTR [rdi+0x10],rax
}
    1b70:	eb e1                	jmp    1b53 <luaV_finishOp+0x83>
    1b72:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
      setobjs2s(L, base + GETARG_A(inst), --L->top);
    1b78:	48 8b 57 10          	mov    rdx,QWORD PTR [rdi+0x10]
    1b7c:	c1 eb 08             	shr    ebx,0x8
    1b7f:	83 e3 7f             	and    ebx,0x7f
    1b82:	48 8d 4a f0          	lea    rcx,[rdx-0x10]
    1b86:	48 c1 e3 04          	shl    rbx,0x4
    1b8a:	48 89 4f 10          	mov    QWORD PTR [rdi+0x10],rcx
    1b8e:	48 8b 4a f0          	mov    rcx,QWORD PTR [rdx-0x10]
    1b92:	49 8d 04 18          	lea    rax,[r8+rbx*1]
    1b96:	0f b7 52 f8          	movzx  edx,WORD PTR [rdx-0x8]
    1b9a:	48 89 08             	mov    QWORD PTR [rax],rcx
    1b9d:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
}
    1ba1:	5b                   	pop    rbx
    1ba2:	5d                   	pop    rbp
    1ba3:	41 5c                	pop    r12
    1ba5:	c3                   	ret    
    1ba6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
    1bad:	00 00 00 
      L->top = ci->top;  /* correct top */
    1bb0:	48 8b 45 08          	mov    rax,QWORD PTR [rbp+0x8]
    1bb4:	48 89 47 10          	mov    QWORD PTR [rdi+0x10],rax
}
    1bb8:	5b                   	pop    rbx
    1bb9:	5d                   	pop    rbp
    1bba:	41 5c                	pop    r12
    1bbc:	c3                   	ret    
    1bbd:	0f 1f 00             	nop    DWORD PTR [rax]
      StkId top = L->top - 1;  /* top when 'luaT_trybinTM' was called */
    1bc0:	48 8b 57 10          	mov    rdx,QWORD PTR [rdi+0x10]
      int b = GETARG_B(inst);      /* first element to concatenate */
    1bc4:	89 d8                	mov    eax,ebx
    1bc6:	49 89 fc             	mov    r12,rdi
    1bc9:	c1 e8 18             	shr    eax,0x18
      int total = cast_int(top - 1 - (base + b));  /* yet to concatenate */
    1bcc:	48 8d 7a e0          	lea    rdi,[rdx-0x20]
    1bd0:	48 c1 e0 04          	shl    rax,0x4
      setobj2s(L, top - 2, top);  /* put TM result in proper position */
    1bd4:	0f b7 4a f8          	movzx  ecx,WORD PTR [rdx-0x8]
      int total = cast_int(top - 1 - (base + b));  /* yet to concatenate */
    1bd8:	4c 01 c0             	add    rax,r8
    1bdb:	48 89 fe             	mov    rsi,rdi
    1bde:	48 29 c6             	sub    rsi,rax
      setobj2s(L, top - 2, top);  /* put TM result in proper position */
    1be1:	48 8b 42 f0          	mov    rax,QWORD PTR [rdx-0x10]
    1be5:	66 89 4a d8          	mov    WORD PTR [rdx-0x28],cx
      int total = cast_int(top - 1 - (base + b));  /* yet to concatenate */
    1be9:	48 c1 fe 04          	sar    rsi,0x4
      setobj2s(L, top - 2, top);  /* put TM result in proper position */
    1bed:	48 89 42 d0          	mov    QWORD PTR [rdx-0x30],rax
      if (total > 1) {  /* are there elements to concat? */
    1bf1:	83 fe 01             	cmp    esi,0x1
    1bf4:	7e 1a                	jle    1c10 <luaV_finishOp+0x140>
        L->top = top - 1;  /* top is one after last element (at top-2) */
    1bf6:	49 89 7c 24 10       	mov    QWORD PTR [r12+0x10],rdi
        luaV_concat(L, total);  /* concat them (may yield again) */
    1bfb:	4c 89 e7             	mov    rdi,r12
    1bfe:	e8 00 00 00 00       	call   1c03 <luaV_finishOp+0x133>
    1c03:	49 8b 54 24 10       	mov    rdx,QWORD PTR [r12+0x10]
    1c08:	4c 8b 45 20          	mov    r8,QWORD PTR [rbp+0x20]
    1c0c:	0f b7 4a f8          	movzx  ecx,WORD PTR [rdx-0x8]
      setobj2s(L, ci->u.l.base + GETARG_A(inst), L->top - 1);
    1c10:	89 d8                	mov    eax,ebx
    1c12:	48 8b 52 f0          	mov    rdx,QWORD PTR [rdx-0x10]
    1c16:	c1 e8 08             	shr    eax,0x8
    1c19:	83 e0 7f             	and    eax,0x7f
    1c1c:	48 c1 e0 04          	shl    rax,0x4
    1c20:	4c 01 c0             	add    rax,r8
    1c23:	48 89 10             	mov    QWORD PTR [rax],rdx
    1c26:	66 89 48 08          	mov    WORD PTR [rax+0x8],cx
      L->top = ci->top;  /* restore top */
    1c2a:	48 8b 45 08          	mov    rax,QWORD PTR [rbp+0x8]
    1c2e:	49 89 44 24 10       	mov    QWORD PTR [r12+0x10],rax
}
    1c33:	5b                   	pop    rbx
    1c34:	5d                   	pop    rbp
    1c35:	41 5c                	pop    r12
    1c37:	c3                   	ret    
    1c38:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    1c3f:	00 

0000000000001c40 <raviV_check_usertype>:


int raviV_check_usertype(lua_State *L, TString *name, const TValue *o)
{
  Table *mt;
  switch (ttnov(o)) {
    1c40:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    1c44:	83 e0 0f             	and    eax,0xf
    1c47:	66 83 f8 05          	cmp    ax,0x5
    1c4b:	0f 84 7f 00 00 00    	je     1cd0 <raviV_check_usertype+0x90>
    1c51:	66 83 f8 07          	cmp    ax,0x7
    1c55:	75 59                	jne    1cb0 <raviV_check_usertype+0x70>
  case LUA_TTABLE:
    mt = hvalue(o)->metatable;
    break;
  case LUA_TUSERDATA:
    mt = uvalue(o)->metatable;
    1c57:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    1c5a:	48 8b 48 10          	mov    rcx,QWORD PTR [rax+0x10]
    break;
  default:
    mt = NULL;
  }
  if (mt == NULL)
    1c5e:	48 85 c9             	test   rcx,rcx
    1c61:	74 79                	je     1cdc <raviV_check_usertype+0x9c>
    return 0;
  /* get global table from registry */
  Table *reg = hvalue(&G(L)->l_registry);
    1c63:	48 8b 47 18          	mov    rax,QWORD PTR [rdi+0x18]
#if defined(RAVI_ENABLED)
/*
** search function for short strings
*/
static RAVI_ALWAYS_INLINE const TValue *luaH_getshortstr(Table *t, TString *key) {
  Node *n = hashstr(t, key);
    1c67:	8b 56 0c             	mov    edx,DWORD PTR [rsi+0xc]
    1c6a:	48 8b 40 40          	mov    rax,QWORD PTR [rax+0x40]
    1c6e:	23 50 50             	and    edx,DWORD PTR [rax+0x50]
    1c71:	48 c1 e2 05          	shl    rdx,0x5
    1c75:	48 03 50 18          	add    rdx,QWORD PTR [rax+0x18]
    1c79:	eb 14                	jmp    1c8f <raviV_check_usertype+0x4f>
    1c7b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  for (;;) {  /* check whether 'key' is somewhere in the chain */
    const TValue *k = gkey(n);
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
      return gval(n);  /* that's it */
    else {
      int nx = gnext(n);
    1c80:	48 63 42 1c          	movsxd rax,DWORD PTR [rdx+0x1c]
      if (nx == 0)
    1c84:	85 c0                	test   eax,eax
    1c86:	74 30                	je     1cb8 <raviV_check_usertype+0x78>
        return luaO_nilobject;  /* not found */
      n += nx;
    1c88:	48 c1 e0 05          	shl    rax,0x5
    1c8c:	48 01 c2             	add    rdx,rax
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
    1c8f:	66 81 7a 18 04 80    	cmp    WORD PTR [rdx+0x18],0x8004
    1c95:	75 e9                	jne    1c80 <raviV_check_usertype+0x40>
    1c97:	48 3b 72 10          	cmp    rsi,QWORD PTR [rdx+0x10]
    1c9b:	75 e3                	jne    1c80 <raviV_check_usertype+0x40>
  const TValue *metatab = luaH_getshortstr(reg, name);
  return (!ttisnil(metatab) && ttisLtable(metatab) && hvalue(metatab) == mt) || 0;
    1c9d:	31 c0                	xor    eax,eax
    1c9f:	66 81 7a 08 05 80    	cmp    WORD PTR [rdx+0x8],0x8005
    1ca5:	74 39                	je     1ce0 <raviV_check_usertype+0xa0>
}
    1ca7:	c3                   	ret    
    1ca8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    1caf:	00 
    return 0;
    1cb0:	31 c0                	xor    eax,eax
    1cb2:	c3                   	ret    
    1cb3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
        return luaO_nilobject;  /* not found */
    1cb8:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 1cbf <raviV_check_usertype+0x7f>
  return (!ttisnil(metatab) && ttisLtable(metatab) && hvalue(metatab) == mt) || 0;
    1cbf:	31 c0                	xor    eax,eax
    1cc1:	66 81 7a 08 05 80    	cmp    WORD PTR [rdx+0x8],0x8005
    1cc7:	75 de                	jne    1ca7 <raviV_check_usertype+0x67>
    1cc9:	eb 15                	jmp    1ce0 <raviV_check_usertype+0xa0>
    1ccb:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    mt = hvalue(o)->metatable;
    1cd0:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    1cd3:	48 8b 48 28          	mov    rcx,QWORD PTR [rax+0x28]
  if (mt == NULL)
    1cd7:	48 85 c9             	test   rcx,rcx
    1cda:	75 87                	jne    1c63 <raviV_check_usertype+0x23>
  return (!ttisnil(metatab) && ttisLtable(metatab) && hvalue(metatab) == mt) || 0;
    1cdc:	31 c0                	xor    eax,eax
}
    1cde:	c3                   	ret    
    1cdf:	90                   	nop
  return (!ttisnil(metatab) && ttisLtable(metatab) && hvalue(metatab) == mt) || 0;
    1ce0:	31 c0                	xor    eax,eax
    1ce2:	48 39 0a             	cmp    QWORD PTR [rdx],rcx
    1ce5:	0f 94 c0             	sete   al
    1ce8:	c3                   	ret    
    1ce9:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

0000000000001cf0 <luaV_execute>:

int luaV_execute (lua_State *L) {
    1cf0:	41 57                	push   r15
    1cf2:	41 56                	push   r14
  for (;;) {
    Instruction i;
    OpCode op;
    StkId ra;

    vmfetch;
    1cf4:	4c 8d 35 00 00 00 00 	lea    r14,[rip+0x0]        # 1cfb <luaV_execute+0xb>
int luaV_execute (lua_State *L) {
    1cfb:	41 55                	push   r13
    1cfd:	41 54                	push   r12
    1cff:	55                   	push   rbp
    1d00:	53                   	push   rbx
    1d01:	48 81 ec 18 01 00 00 	sub    rsp,0x118
    1d08:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    1d0f:	00 00 
    1d11:	48 89 84 24 08 01 00 	mov    QWORD PTR [rsp+0x108],rax
    1d18:	00 
    1d19:	31 c0                	xor    eax,eax
  CallInfo *ci = L->ci;
    1d1b:	48 8b 47 20          	mov    rax,QWORD PTR [rdi+0x20]
    1d1f:	48 89 7c 24 08       	mov    QWORD PTR [rsp+0x8],rdi
  ci->callstatus |= CIST_FRESH;  /* fresh invocation of 'luaV_execute" */
    1d24:	66 83 48 42 08       	or     WORD PTR [rax+0x42],0x8
    1d29:	0f b6 97 c8 00 00 00 	movzx  edx,BYTE PTR [rdi+0xc8]
  CallInfo *ci = L->ci;
    1d30:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
  ci->callstatus |= CIST_FRESH;  /* fresh invocation of 'luaV_execute" */
    1d35:	48 8b 40 28          	mov    rax,QWORD PTR [rax+0x28]
  cl = clLvalue(ci->func);  /* local reference to function's closure */
    1d39:	48 8b 7c 24 18       	mov    rdi,QWORD PTR [rsp+0x18]
    1d3e:	45 31 ed             	xor    r13d,r13d
    vmfetch;
    1d41:	83 e2 0c             	and    edx,0xc
    1d44:	48 8d 58 04          	lea    rbx,[rax+0x4]
    1d48:	44 8b 00             	mov    r8d,DWORD PTR [rax]
  cl = clLvalue(ci->func);  /* local reference to function's closure */
    1d4b:	48 8b 0f             	mov    rcx,QWORD PTR [rdi]
  base = ci->u.l.base;  /* local copy of function's base */
    1d4e:	4c 8b 67 20          	mov    r12,QWORD PTR [rdi+0x20]
  cl = clLvalue(ci->func);  /* local reference to function's closure */
    1d52:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
    1d55:	48 89 4c 24 20       	mov    QWORD PTR [rsp+0x20],rcx
  k = cl->p->k;  /* local reference to function's constant table */
    1d5a:	48 8b 49 18          	mov    rcx,QWORD PTR [rcx+0x18]
    1d5e:	48 8b 71 30          	mov    rsi,QWORD PTR [rcx+0x30]
    1d62:	48 89 74 24 10       	mov    QWORD PTR [rsp+0x10],rsi
    vmfetch;
    1d67:	0f 85 5c 57 00 00    	jne    74c9 <luaV_execute+0x57d9>
    1d6d:	44 89 c5             	mov    ebp,r8d
    1d70:	41 0f b6 c0          	movzx  eax,r8b
    1d74:	c1 ed 08             	shr    ebp,0x8
    1d77:	48 89 c2             	mov    rdx,rax
    1d7a:	83 e5 7f             	and    ebp,0x7f
    1d7d:	48 c1 e5 04          	shl    rbp,0x4
    1d81:	4c 01 e5             	add    rbp,r12
    1d84:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
      vmcase(OP_RAVI_TABLE_GETFIELD) {
        /* This opcode is used when the key is known to be
           short string and the variable is known to be
           a table
        */
        StkId rb = RB(i);
    1d88:	44 89 c6             	mov    esi,r8d
    1d8b:	c1 ee 18             	shr    esi,0x18
    1d8e:	48 c1 e6 04          	shl    rsi,0x4
    1d92:	4c 01 e6             	add    rsi,r12
        if (op == OP_RAVI_TABLE_SELF_SK) { setobjs2s(L, ra + 1, rb); }
    1d95:	83 fa 6d             	cmp    edx,0x6d
    1d98:	75 0f                	jne    1da9 <luaV_execute+0xb9>
    1d9a:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    1d9d:	48 89 45 10          	mov    QWORD PTR [rbp+0x10],rax
    1da1:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    1da5:	66 89 45 18          	mov    WORD PTR [rbp+0x18],ax
        {
          lua_assert(ISK(GETARG_C(i)));
          TValue *rc = k + INDEXK(GETARG_C(i));
    1da9:	44 89 c2             	mov    edx,r8d
          TString *key = tsvalue(rc);
          lua_assert(key->tt == LUA_TSHRSTR);
          Table *h = hvalue(rb);
    1dac:	48 8b 06             	mov    rax,QWORD PTR [rsi]
          TValue *rc = k + INDEXK(GETARG_C(i));
    1daf:	c1 ea 10             	shr    edx,0x10
    1db2:	83 e2 7f             	and    edx,0x7f
    1db5:	48 c1 e2 04          	shl    rdx,0x4
    1db9:	48 03 54 24 10       	add    rdx,QWORD PTR [rsp+0x10]
          TString *key = tsvalue(rc);
    1dbe:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
  Node *n = hashstr(t, key);
    1dc1:	44 8b 41 0c          	mov    r8d,DWORD PTR [rcx+0xc]
    1dc5:	44 23 40 50          	and    r8d,DWORD PTR [rax+0x50]
    1dc9:	49 c1 e0 05          	shl    r8,0x5
    1dcd:	4c 03 40 18          	add    r8,QWORD PTR [rax+0x18]
    1dd1:	eb 18                	jmp    1deb <luaV_execute+0xfb>
    1dd3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
      int nx = gnext(n);
    1dd8:	49 63 40 1c          	movsxd rax,DWORD PTR [r8+0x1c]
      if (nx == 0)
    1ddc:	85 c0                	test   eax,eax
    1dde:	0f 84 fc 37 00 00    	je     55e0 <luaV_execute+0x38f0>
      n += nx;
    1de4:	48 c1 e0 05          	shl    rax,0x5
    1de8:	49 01 c0             	add    r8,rax
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
    1deb:	66 41 81 78 18 04 80 	cmp    WORD PTR [r8+0x18],0x8004
    1df2:	75 e4                	jne    1dd8 <luaV_execute+0xe8>
    1df4:	49 3b 48 10          	cmp    rcx,QWORD PTR [r8+0x10]
    1df8:	75 de                	jne    1dd8 <luaV_execute+0xe8>
          const TValue *v = luaH_getshortstr(h, key);
          if (RAVI_LIKELY(!ttisnil(v))) {
    1dfa:	41 0f b7 40 08       	movzx  eax,WORD PTR [r8+0x8]
    1dff:	66 85 c0             	test   ax,ax
    1e02:	0f 84 ed 37 00 00    	je     55f5 <luaV_execute+0x3905>
            setobj2s(L, ra, v);
    1e08:	49 8b 10             	mov    rdx,QWORD PTR [r8]
    1e0b:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
          }
          else {
            Protect(luaV_finishget(L, rb, rc, ra, v));
          }
        }
        vmbreak;
    1e0f:	48 89 d8             	mov    rax,rbx
    1e12:	48 83 c3 04          	add    rbx,0x4
            setobj2s(L, ra, v);
    1e16:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    1e1a:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    1e1d:	45 85 ed             	test   r13d,r13d
    1e20:	0f 85 0b 38 00 00    	jne    5631 <luaV_execute+0x3941>
    1e26:	44 89 c5             	mov    ebp,r8d
    1e29:	41 0f b6 c0          	movzx  eax,r8b
    1e2d:	c1 ed 08             	shr    ebp,0x8
    1e30:	48 89 c2             	mov    rdx,rax
    1e33:	83 e5 7f             	and    ebp,0x7f
    1e36:	48 c1 e5 04          	shl    rbp,0x4
    1e3a:	4c 01 e5             	add    rbp,r12
    1e3d:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *pstep = RAVI_LIKELY((op == OP_RAVI_FORPREP_I1)) ? NULL : ra + 2;
    1e41:	83 fa 59             	cmp    edx,0x59
    1e44:	0f 85 9e 57 00 00    	jne    75e8 <luaV_execute+0x58f8>
        lua_Integer initv = ivalue(pinit);
    1e4a:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
        lua_Integer istep = RAVI_LIKELY((op == OP_RAVI_FORPREP_I1)) ? 1 : ivalue(pstep);
    1e4e:	ba 01 00 00 00       	mov    edx,0x1
        setivalue(pinit, initv - istep);
    1e53:	48 29 d0             	sub    rax,rdx
        pc += GETARG_sBx(i);
    1e56:	41 c1 e8 10          	shr    r8d,0x10
        setivalue(pinit, initv - istep);
    1e5a:	41 bf 13 00 00 00    	mov    r15d,0x13
    1e60:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        pc += GETARG_sBx(i);
    1e64:	4a 8d 84 83 00 00 fe 	lea    rax,[rbx+r8*4-0x20000]
    1e6b:	ff 
        setivalue(pinit, initv - istep);
    1e6c:	66 44 89 7d 08       	mov    WORD PTR [rbp+0x8],r15w
        vmbreak;
    1e71:	48 8d 58 04          	lea    rbx,[rax+0x4]
    1e75:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    1e78:	45 85 ed             	test   r13d,r13d
    1e7b:	0f 85 74 57 00 00    	jne    75f5 <luaV_execute+0x5905>
    1e81:	44 89 c5             	mov    ebp,r8d
    1e84:	41 0f b6 c0          	movzx  eax,r8b
    1e88:	c1 ed 08             	shr    ebp,0x8
    1e8b:	48 89 c2             	mov    rdx,rax
    1e8e:	83 e5 7f             	and    ebp,0x7f
    1e91:	48 c1 e5 04          	shl    rbp,0x4
    1e95:	4c 01 e5             	add    rbp,r12
    1e98:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        lua_Integer step = op == OP_RAVI_FORLOOP_I1 ? 1 : ivalue(ra + 2);
    1e9c:	b8 01 00 00 00       	mov    eax,0x1
    1ea1:	83 fa 57             	cmp    edx,0x57
    1ea4:	74 04                	je     1eaa <luaV_execute+0x1ba>
    1ea6:	48 8b 45 20          	mov    rax,QWORD PTR [rbp+0x20]
        lua_Integer idx = ivalue(ra) + step; /* increment index */
    1eaa:	48 03 45 00          	add    rax,QWORD PTR [rbp+0x0]
        if (idx <= limit) {
    1eae:	48 89 da             	mov    rdx,rbx
    1eb1:	48 3b 45 10          	cmp    rax,QWORD PTR [rbp+0x10]
    1eb5:	7f 20                	jg     1ed7 <luaV_execute+0x1e7>
          chgivalue(ra, idx);               /* update internal index... */
    1eb7:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
          pc += GETARG_sBx(i); /* jump back */
    1ebb:	41 c1 e8 10          	shr    r8d,0x10
          setivalue(ra + 3, idx);           /* ...and external index */
    1ebf:	48 89 45 30          	mov    QWORD PTR [rbp+0x30],rax
    1ec3:	b8 13 00 00 00       	mov    eax,0x13
          pc += GETARG_sBx(i); /* jump back */
    1ec8:	44 89 c1             	mov    ecx,r8d
          setivalue(ra + 3, idx);           /* ...and external index */
    1ecb:	66 89 45 38          	mov    WORD PTR [rbp+0x38],ax
          pc += GETARG_sBx(i); /* jump back */
    1ecf:	48 8d 94 8b 00 00 fe 	lea    rdx,[rbx+rcx*4-0x20000]
    1ed6:	ff 
        vmbreak;
    1ed7:	48 8d 5a 04          	lea    rbx,[rdx+0x4]
    1edb:	44 8b 02             	mov    r8d,DWORD PTR [rdx]
    1ede:	45 85 ed             	test   r13d,r13d
    1ee1:	0f 85 85 57 00 00    	jne    766c <luaV_execute+0x597c>
    1ee7:	44 89 c5             	mov    ebp,r8d
    1eea:	41 0f b6 c0          	movzx  eax,r8b
    1eee:	c1 ed 08             	shr    ebp,0x8
    1ef1:	48 89 c2             	mov    rdx,rax
    1ef4:	83 e5 7f             	and    ebp,0x7f
    1ef7:	48 c1 e5 04          	shl    rbp,0x4
    1efb:	4c 01 e5             	add    rbp,r12
    1efe:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        setobjs2s(L, cb+2, ra+2);
    1f02:	48 8b 45 20          	mov    rax,QWORD PTR [rbp+0x20]
        StkId cb = ra + 3;  /* call base */
    1f06:	48 8d 75 30          	lea    rsi,[rbp+0x30]
        L->top = cb + 3;  /* func. + 2 args (state and index) */
    1f0a:	48 83 c5 60          	add    rbp,0x60
        Protect(luaD_call(L, cb, GETARG_C(i)));
    1f0e:	41 c1 e8 10          	shr    r8d,0x10
        L->top = cb + 3;  /* func. + 2 args (state and index) */
    1f12:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
        Protect(luaD_call(L, cb, GETARG_C(i)));
    1f17:	41 0f b6 d0          	movzx  edx,r8b
        setobjs2s(L, cb+2, ra+2);
    1f1b:	48 89 46 20          	mov    QWORD PTR [rsi+0x20],rax
    1f1f:	0f b7 45 c8          	movzx  eax,WORD PTR [rbp-0x38]
        L->top = cb + 3;  /* func. + 2 args (state and index) */
    1f23:	4c 89 ff             	mov    rdi,r15
        setobjs2s(L, cb+2, ra+2);
    1f26:	66 89 46 28          	mov    WORD PTR [rsi+0x28],ax
        setobjs2s(L, cb+1, ra+1);
    1f2a:	48 8b 45 b0          	mov    rax,QWORD PTR [rbp-0x50]
    1f2e:	48 89 46 10          	mov    QWORD PTR [rsi+0x10],rax
    1f32:	0f b7 45 b8          	movzx  eax,WORD PTR [rbp-0x48]
    1f36:	66 89 46 18          	mov    WORD PTR [rsi+0x18],ax
        setobjs2s(L, cb, ra);
    1f3a:	48 8b 45 a0          	mov    rax,QWORD PTR [rbp-0x60]
    1f3e:	48 89 45 d0          	mov    QWORD PTR [rbp-0x30],rax
    1f42:	0f b7 45 a8          	movzx  eax,WORD PTR [rbp-0x58]
    1f46:	66 89 46 08          	mov    WORD PTR [rsi+0x8],ax
        L->top = cb + 3;  /* func. + 2 args (state and index) */
    1f4a:	49 89 6f 10          	mov    QWORD PTR [r15+0x10],rbp
        Protect(luaD_call(L, cb, GETARG_C(i)));
    1f4e:	48 8b 6c 24 18       	mov    rbp,QWORD PTR [rsp+0x18]
    1f53:	48 89 5d 28          	mov    QWORD PTR [rbp+0x28],rbx
        i = *(pc++);  /* go to next instruction */
    1f57:	48 83 c3 04          	add    rbx,0x4
        Protect(luaD_call(L, cb, GETARG_C(i)));
    1f5b:	e8 00 00 00 00       	call   1f60 <luaV_execute+0x270>
        i = *(pc++);  /* go to next instruction */
    1f60:	44 8b 43 fc          	mov    r8d,DWORD PTR [rbx-0x4]
        Protect(luaD_call(L, cb, GETARG_C(i)));
    1f64:	4c 8b 65 20          	mov    r12,QWORD PTR [rbp+0x20]
        L->top = ci->top;
    1f68:	48 8b 45 08          	mov    rax,QWORD PTR [rbp+0x8]
        Protect(luaD_call(L, cb, GETARG_C(i)));
    1f6c:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    1f73:	00 
        ra = RA(i);
    1f74:	44 89 c5             	mov    ebp,r8d
    1f77:	c1 ed 08             	shr    ebp,0x8
        L->top = ci->top;
    1f7a:	49 89 47 10          	mov    QWORD PTR [r15+0x10],rax
        Protect(luaD_call(L, cb, GETARG_C(i)));
    1f7e:	41 83 e5 0c          	and    r13d,0xc
        ra = RA(i);
    1f82:	83 e5 7f             	and    ebp,0x7f
    1f85:	48 c1 e5 04          	shl    rbp,0x4
    1f89:	4c 01 e5             	add    rbp,r12
        if (!ttisnil(ra + 1)) {  /* continue loop? */
    1f8c:	0f b7 55 18          	movzx  edx,WORD PTR [rbp+0x18]
    1f90:	48 89 d8             	mov    rax,rbx
    1f93:	66 85 d2             	test   dx,dx
    1f96:	74 1b                	je     1fb3 <luaV_execute+0x2c3>
          setobjs2s(L, ra, ra + 1);  /* save control variable */
    1f98:	48 8b 4d 10          	mov    rcx,QWORD PTR [rbp+0x10]
          pc += GETARG_sBx(i);  /* jump back */
    1f9c:	41 c1 e8 10          	shr    r8d,0x10
          setobjs2s(L, ra, ra + 1);  /* save control variable */
    1fa0:	66 89 55 08          	mov    WORD PTR [rbp+0x8],dx
          pc += GETARG_sBx(i);  /* jump back */
    1fa4:	44 89 c2             	mov    edx,r8d
          setobjs2s(L, ra, ra + 1);  /* save control variable */
    1fa7:	48 89 4d 00          	mov    QWORD PTR [rbp+0x0],rcx
          pc += GETARG_sBx(i);  /* jump back */
    1fab:	48 8d 84 93 00 00 fe 	lea    rax,[rbx+rdx*4-0x20000]
    1fb2:	ff 
        vmbreak;
    1fb3:	48 8d 58 04          	lea    rbx,[rax+0x4]
    1fb7:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    1fba:	45 85 ed             	test   r13d,r13d
    1fbd:	0f 85 de 56 00 00    	jne    76a1 <luaV_execute+0x59b1>
    1fc3:	44 89 c5             	mov    ebp,r8d
    1fc6:	41 0f b6 c0          	movzx  eax,r8b
    1fca:	c1 ed 08             	shr    ebp,0x8
    1fcd:	48 89 c2             	mov    rdx,rax
    1fd0:	83 e5 7f             	and    ebp,0x7f
    1fd3:	48 c1 e5 04          	shl    rbp,0x4
    1fd7:	4c 01 e5             	add    rbp,r12
    1fda:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    1fde:	44 89 c2             	mov    edx,r8d
    1fe1:	c1 ea 18             	shr    edx,0x18
    1fe4:	f6 c2 80             	test   dl,0x80
    1fe7:	0f 84 5f 37 00 00    	je     574c <luaV_execute+0x3a5c>
    1fed:	83 e2 7f             	and    edx,0x7f
    1ff0:	48 c1 e2 04          	shl    rdx,0x4
    1ff4:	48 03 54 24 10       	add    rdx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    1ff9:	41 c1 e8 10          	shr    r8d,0x10
    1ffd:	41 f6 c0 80          	test   r8b,0x80
    2001:	0f 84 ed 36 00 00    	je     56f4 <luaV_execute+0x3a04>
    2007:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    200c:	41 83 e0 7f          	and    r8d,0x7f
    2010:	49 c1 e0 04          	shl    r8,0x4
    2014:	4a 8d 0c 00          	lea    rcx,[rax+r8*1]
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
    2018:	0f b7 45 08          	movzx  eax,WORD PTR [rbp+0x8]
    201c:	66 3d 05 80          	cmp    ax,0x8005
    2020:	0f 85 e8 36 00 00    	jne    570e <luaV_execute+0x3a1e>
    2026:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    202a:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
  Node *n = hashstr(t, key);
    202d:	44 8b 40 50          	mov    r8d,DWORD PTR [rax+0x50]
    2031:	44 23 46 0c          	and    r8d,DWORD PTR [rsi+0xc]
    2035:	49 c1 e0 05          	shl    r8,0x5
    2039:	4c 03 40 18          	add    r8,QWORD PTR [rax+0x18]
    203d:	eb 14                	jmp    2053 <luaV_execute+0x363>
    203f:	90                   	nop
      int nx = gnext(n);
    2040:	49 63 40 1c          	movsxd rax,DWORD PTR [r8+0x1c]
      if (nx == 0)
    2044:	85 c0                	test   eax,eax
    2046:	0f 84 24 36 00 00    	je     5670 <luaV_execute+0x3980>
      n += nx;
    204c:	48 c1 e0 05          	shl    rax,0x5
    2050:	49 01 c0             	add    r8,rax
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
    2053:	66 41 81 78 18 04 80 	cmp    WORD PTR [r8+0x18],0x8004
    205a:	75 e4                	jne    2040 <luaV_execute+0x350>
    205c:	49 3b 70 10          	cmp    rsi,QWORD PTR [r8+0x10]
    2060:	75 de                	jne    2040 <luaV_execute+0x350>
    2062:	66 41 83 78 08 00    	cmp    WORD PTR [r8+0x8],0x0
    2068:	0f 84 15 36 00 00    	je     5683 <luaV_execute+0x3993>
    206e:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    2071:	49 89 00             	mov    QWORD PTR [r8],rax
    2074:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    2078:	66 41 89 40 08       	mov    WORD PTR [r8+0x8],ax
    207d:	66 83 79 08 00       	cmp    WORD PTR [rcx+0x8],0x0
    2082:	0f 88 12 4d 00 00    	js     6d9a <luaV_execute+0x50aa>
        vmbreak;
    2088:	48 89 d8             	mov    rax,rbx
    208b:	48 83 c3 04          	add    rbx,0x4
    208f:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2092:	45 85 ed             	test   r13d,r13d
    2095:	0f 85 24 36 00 00    	jne    56bf <luaV_execute+0x39cf>
    209b:	44 89 c5             	mov    ebp,r8d
    209e:	41 0f b6 c0          	movzx  eax,r8b
    20a2:	c1 ed 08             	shr    ebp,0x8
    20a5:	48 89 c2             	mov    rdx,rax
    20a8:	83 e5 7f             	and    ebp,0x7f
    20ab:	48 c1 e5 04          	shl    rbp,0x4
    20af:	4c 01 e5             	add    rbp,r12
    20b2:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        savepc(L);  /* in case of allocation errors */
    20b6:	48 8b 7c 24 18       	mov    rdi,QWORD PTR [rsp+0x18]
        t = raviH_new(L, RAVI_TARRAYINT, 0);
    20bb:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    20c0:	31 d2                	xor    edx,edx
    20c2:	be 03 00 00 00       	mov    esi,0x3
        savepc(L);  /* in case of allocation errors */
    20c7:	48 89 5f 28          	mov    QWORD PTR [rdi+0x28],rbx
        t = raviH_new(L, RAVI_TARRAYINT, 0);
    20cb:	4c 89 ff             	mov    rdi,r15
    20ce:	e8 00 00 00 00       	call   20d3 <luaV_execute+0x3e3>
        setiarrayvalue(L, ra, t);
    20d3:	41 bb 15 80 ff ff    	mov    r11d,0xffff8015
    20d9:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        checkGC(L, ra + 1);
    20dd:	49 8b 47 18          	mov    rax,QWORD PTR [r15+0x18]
        setiarrayvalue(L, ra, t);
    20e1:	66 44 89 5d 08       	mov    WORD PTR [rbp+0x8],r11w
        checkGC(L, ra + 1);
    20e6:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    20eb:	0f 8f 88 47 00 00    	jg     6879 <luaV_execute+0x4b89>
        vmbreak;
    20f1:	48 89 d8             	mov    rax,rbx
    20f4:	48 83 c3 04          	add    rbx,0x4
    20f8:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    20fb:	45 85 ed             	test   r13d,r13d
    20fe:	0f 85 67 67 00 00    	jne    886b <luaV_execute+0x6b7b>
    2104:	44 89 c5             	mov    ebp,r8d
    2107:	41 0f b6 c0          	movzx  eax,r8b
    210b:	c1 ed 08             	shr    ebp,0x8
    210e:	48 89 c2             	mov    rdx,rax
    2111:	83 e5 7f             	and    ebp,0x7f
    2114:	48 c1 e5 04          	shl    rbp,0x4
    2118:	4c 01 e5             	add    rbp,r12
    211b:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    211f:	44 89 c0             	mov    eax,r8d
    2122:	c1 e8 18             	shr    eax,0x18
    2125:	a8 80                	test   al,0x80
    2127:	0f 85 6b 3e 00 00    	jne    5f98 <luaV_execute+0x42a8>
        TValue *rc = RKC(i);
    212d:	44 89 c2             	mov    edx,r8d
        TValue *rb = RKB(i);
    2130:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    2134:	c1 ea 10             	shr    edx,0x10
        TValue *rb = RKB(i);
    2137:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    213a:	f6 c2 80             	test   dl,0x80
    213d:	0f 84 70 3e 00 00    	je     5fb3 <luaV_execute+0x42c3>
    2143:	83 e2 7f             	and    edx,0x7f
    2146:	48 c1 e2 04          	shl    rdx,0x4
    214a:	48 03 54 24 10       	add    rdx,QWORD PTR [rsp+0x10]
        if (lessequals != GETARG_A(i))
    214f:	41 c1 e8 08          	shr    r8d,0x8
        int lessequals = (fltvalue(rb) <= fltvalue(rc));
    2153:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
        if (lessequals != GETARG_A(i))
    2157:	41 83 e0 7f          	and    r8d,0x7f
        int lessequals = (fltvalue(rb) <= fltvalue(rc));
    215b:	66 0f 2f 00          	comisd xmm0,QWORD PTR [rax]
    215f:	0f 93 c0             	setae  al
    2162:	0f b6 c0             	movzx  eax,al
        if (lessequals != GETARG_A(i))
    2165:	41 39 c0             	cmp    r8d,eax
    2168:	0f 84 68 4e 00 00    	je     6fd6 <luaV_execute+0x52e6>
          pc++;
    216e:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    2172:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2176:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2179:	45 85 ed             	test   r13d,r13d
    217c:	0f 85 53 58 00 00    	jne    79d5 <luaV_execute+0x5ce5>
    2182:	44 89 c5             	mov    ebp,r8d
    2185:	41 0f b6 c0          	movzx  eax,r8b
    2189:	c1 ed 08             	shr    ebp,0x8
    218c:	48 89 c2             	mov    rdx,rax
    218f:	83 e5 7f             	and    ebp,0x7f
    2192:	48 c1 e5 04          	shl    rbp,0x4
    2196:	4c 01 e5             	add    rbp,r12
    2199:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        StkId rb = RB(i); /* variable - may not be a table */
    219d:	44 89 c6             	mov    esi,r8d
        TValue *rc = k + INDEXK(GETARG_C(i));
    21a0:	44 89 c2             	mov    edx,r8d
        StkId rb = RB(i); /* variable - may not be a table */
    21a3:	c1 ee 18             	shr    esi,0x18
        TValue *rc = k + INDEXK(GETARG_C(i));
    21a6:	c1 ea 10             	shr    edx,0x10
        StkId rb = RB(i); /* variable - may not be a table */
    21a9:	48 c1 e6 04          	shl    rsi,0x4
        TValue *rc = k + INDEXK(GETARG_C(i));
    21ad:	83 e2 7f             	and    edx,0x7f
        StkId rb = RB(i); /* variable - may not be a table */
    21b0:	4c 01 e6             	add    rsi,r12
        TValue *rc = k + INDEXK(GETARG_C(i));
    21b3:	48 c1 e2 04          	shl    rdx,0x4
    21b7:	48 03 54 24 10       	add    rdx,QWORD PTR [rsp+0x10]
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    21bc:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    21c0:	66 3d 05 80          	cmp    ax,0x8005
    21c4:	0f 85 0b 67 00 00    	jne    88d5 <luaV_execute+0x6be5>
    21ca:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    21cd:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
  Node *n = hashstr(t, key);
    21d0:	44 8b 41 0c          	mov    r8d,DWORD PTR [rcx+0xc]
    21d4:	44 23 40 50          	and    r8d,DWORD PTR [rax+0x50]
    21d8:	49 c1 e0 05          	shl    r8,0x5
    21dc:	4c 03 40 18          	add    r8,QWORD PTR [rax+0x18]
    21e0:	eb 19                	jmp    21fb <luaV_execute+0x50b>
    21e2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
      int nx = gnext(n);
    21e8:	49 63 40 1c          	movsxd rax,DWORD PTR [r8+0x1c]
      if (nx == 0)
    21ec:	85 c0                	test   eax,eax
    21ee:	0f 84 94 4a 00 00    	je     6c88 <luaV_execute+0x4f98>
      n += nx;
    21f4:	48 c1 e0 05          	shl    rax,0x5
    21f8:	49 01 c0             	add    r8,rax
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
    21fb:	66 41 81 78 18 04 80 	cmp    WORD PTR [r8+0x18],0x8004
    2202:	75 e4                	jne    21e8 <luaV_execute+0x4f8>
    2204:	49 3b 48 10          	cmp    rcx,QWORD PTR [r8+0x10]
    2208:	75 de                	jne    21e8 <luaV_execute+0x4f8>
    220a:	41 0f b7 40 08       	movzx  eax,WORD PTR [r8+0x8]
    220f:	66 85 c0             	test   ax,ax
    2212:	0f 84 14 6d 00 00    	je     8f2c <luaV_execute+0x723c>
    2218:	49 8b 10             	mov    rdx,QWORD PTR [r8]
    221b:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    221f:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    2223:	48 89 d8             	mov    rax,rbx
    2226:	48 83 c3 04          	add    rbx,0x4
    222a:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    222d:	45 85 ed             	test   r13d,r13d
    2230:	0f 85 75 60 00 00    	jne    82ab <luaV_execute+0x65bb>
    2236:	44 89 c5             	mov    ebp,r8d
    2239:	41 0f b6 c0          	movzx  eax,r8b
    223d:	c1 ed 08             	shr    ebp,0x8
    2240:	48 89 c2             	mov    rdx,rax
    2243:	83 e5 7f             	and    ebp,0x7f
    2246:	48 c1 e5 04          	shl    rbp,0x4
    224a:	4c 01 e5             	add    rbp,r12
    224d:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        StkId rb = cl->upvals[GETARG_B(i)]->v; /* variable - may not be a table */
    2251:	48 8b 74 24 20       	mov    rsi,QWORD PTR [rsp+0x20]
    2256:	44 89 c0             	mov    eax,r8d
        TValue *rc = k + INDEXK(GETARG_C(i));
    2259:	44 89 c2             	mov    edx,r8d
        StkId rb = cl->upvals[GETARG_B(i)]->v; /* variable - may not be a table */
    225c:	c1 e8 18             	shr    eax,0x18
        TValue *rc = k + INDEXK(GETARG_C(i));
    225f:	c1 ea 10             	shr    edx,0x10
        StkId rb = cl->upvals[GETARG_B(i)]->v; /* variable - may not be a table */
    2262:	48 8b 44 c6 20       	mov    rax,QWORD PTR [rsi+rax*8+0x20]
        TValue *rc = k + INDEXK(GETARG_C(i));
    2267:	83 e2 7f             	and    edx,0x7f
    226a:	48 c1 e2 04          	shl    rdx,0x4
    226e:	48 03 54 24 10       	add    rdx,QWORD PTR [rsp+0x10]
        StkId rb = cl->upvals[GETARG_B(i)]->v; /* variable - may not be a table */
    2273:	48 8b 30             	mov    rsi,QWORD PTR [rax]
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    2276:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    227a:	66 3d 05 80          	cmp    ax,0x8005
    227e:	0f 85 5c 60 00 00    	jne    82e0 <luaV_execute+0x65f0>
    2284:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    2287:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
  Node *n = hashstr(t, key);
    228a:	44 8b 41 0c          	mov    r8d,DWORD PTR [rcx+0xc]
    228e:	44 23 40 50          	and    r8d,DWORD PTR [rax+0x50]
    2292:	49 c1 e0 05          	shl    r8,0x5
    2296:	4c 03 40 18          	add    r8,QWORD PTR [rax+0x18]
    229a:	eb 17                	jmp    22b3 <luaV_execute+0x5c3>
    229c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
      int nx = gnext(n);
    22a0:	49 63 40 1c          	movsxd rax,DWORD PTR [r8+0x1c]
      if (nx == 0)
    22a4:	85 c0                	test   eax,eax
    22a6:	0f 84 e8 49 00 00    	je     6c94 <luaV_execute+0x4fa4>
      n += nx;
    22ac:	48 c1 e0 05          	shl    rax,0x5
    22b0:	49 01 c0             	add    r8,rax
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
    22b3:	66 41 81 78 18 04 80 	cmp    WORD PTR [r8+0x18],0x8004
    22ba:	75 e4                	jne    22a0 <luaV_execute+0x5b0>
    22bc:	49 3b 48 10          	cmp    rcx,QWORD PTR [r8+0x10]
    22c0:	75 de                	jne    22a0 <luaV_execute+0x5b0>
    22c2:	41 0f b7 40 08       	movzx  eax,WORD PTR [r8+0x8]
    22c7:	66 85 c0             	test   ax,ax
    22ca:	0f 84 29 6d 00 00    	je     8ff9 <luaV_execute+0x7309>
    22d0:	49 8b 10             	mov    rdx,QWORD PTR [r8]
    22d3:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    22d7:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    22db:	48 89 d8             	mov    rax,rbx
    22de:	48 83 c3 04          	add    rbx,0x4
    22e2:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    22e5:	45 85 ed             	test   r13d,r13d
    22e8:	0f 85 30 60 00 00    	jne    831e <luaV_execute+0x662e>
    22ee:	44 89 c5             	mov    ebp,r8d
    22f1:	41 0f b6 c0          	movzx  eax,r8b
    22f5:	c1 ed 08             	shr    ebp,0x8
    22f8:	48 89 c2             	mov    rdx,rax
    22fb:	83 e5 7f             	and    ebp,0x7f
    22fe:	48 c1 e5 04          	shl    rbp,0x4
    2302:	4c 01 e5             	add    rbp,r12
    2305:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2309:	44 89 c0             	mov    eax,r8d
    230c:	c1 e8 18             	shr    eax,0x18
    230f:	a8 80                	test   al,0x80
    2311:	0f 85 ab 3c 00 00    	jne    5fc2 <luaV_execute+0x42d2>
    2317:	48 c1 e0 04          	shl    rax,0x4
    231b:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    231f:	44 89 c0             	mov    eax,r8d
    2322:	c1 e8 10             	shr    eax,0x10
    2325:	a8 80                	test   al,0x80
    2327:	0f 84 b3 3c 00 00    	je     5fe0 <luaV_execute+0x42f0>
    232d:	83 e0 7f             	and    eax,0x7f
    2330:	48 c1 e0 04          	shl    rax,0x4
    2334:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        if (lessequals != GETARG_A(i))
    2339:	41 c1 e8 08          	shr    r8d,0x8
        int lessequals = (ivalue(rb) <= ivalue(rc));
    233d:	48 8b 00             	mov    rax,QWORD PTR [rax]
        if (lessequals != GETARG_A(i))
    2340:	41 83 e0 7f          	and    r8d,0x7f
        int lessequals = (ivalue(rb) <= ivalue(rc));
    2344:	48 39 02             	cmp    QWORD PTR [rdx],rax
    2347:	0f 9e c0             	setle  al
    234a:	0f b6 c0             	movzx  eax,al
        if (lessequals != GETARG_A(i))
    234d:	41 39 c0             	cmp    r8d,eax
    2350:	0f 84 ec 4b 00 00    	je     6f42 <luaV_execute+0x5252>
          pc++;
    2356:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    235a:	48 8d 58 04          	lea    rbx,[rax+0x4]
    235e:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2361:	45 85 ed             	test   r13d,r13d
    2364:	0f 85 36 56 00 00    	jne    79a0 <luaV_execute+0x5cb0>
    236a:	44 89 c5             	mov    ebp,r8d
    236d:	41 0f b6 c0          	movzx  eax,r8b
    2371:	c1 ed 08             	shr    ebp,0x8
    2374:	48 89 c2             	mov    rdx,rax
    2377:	83 e5 7f             	and    ebp,0x7f
    237a:	48 c1 e5 04          	shl    rbp,0x4
    237e:	4c 01 e5             	add    rbp,r12
    2381:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2385:	44 89 c0             	mov    eax,r8d
    2388:	c1 e8 18             	shr    eax,0x18
    238b:	a8 80                	test   al,0x80
    238d:	0f 85 5c 3c 00 00    	jne    5fef <luaV_execute+0x42ff>
        TValue *rc = RKC(i);
    2393:	44 89 c2             	mov    edx,r8d
        TValue *rb = RKB(i);
    2396:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    239a:	c1 ea 10             	shr    edx,0x10
        TValue *rb = RKB(i);
    239d:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    23a0:	f6 c2 80             	test   dl,0x80
    23a3:	0f 84 61 3c 00 00    	je     600a <luaV_execute+0x431a>
    23a9:	83 e2 7f             	and    edx,0x7f
    23ac:	48 c1 e2 04          	shl    rdx,0x4
    23b0:	48 03 54 24 10       	add    rdx,QWORD PTR [rsp+0x10]
        if (lessthan != GETARG_A(i))
    23b5:	41 c1 e8 08          	shr    r8d,0x8
        int lessthan = (fltvalue(rb) < fltvalue(rc));
    23b9:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
        if (lessthan != GETARG_A(i))
    23bd:	41 83 e0 7f          	and    r8d,0x7f
        int lessthan = (fltvalue(rb) < fltvalue(rc));
    23c1:	66 0f 2f 00          	comisd xmm0,QWORD PTR [rax]
    23c5:	0f 97 c0             	seta   al
    23c8:	0f b6 c0             	movzx  eax,al
        if (lessthan != GETARG_A(i))
    23cb:	41 39 c0             	cmp    r8d,eax
    23ce:	0f 84 37 4b 00 00    	je     6f0b <luaV_execute+0x521b>
          pc++;
    23d4:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    23d8:	48 8d 58 04          	lea    rbx,[rax+0x4]
    23dc:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    23df:	45 85 ed             	test   r13d,r13d
    23e2:	0f 85 83 55 00 00    	jne    796b <luaV_execute+0x5c7b>
    23e8:	44 89 c5             	mov    ebp,r8d
    23eb:	41 0f b6 c0          	movzx  eax,r8b
    23ef:	c1 ed 08             	shr    ebp,0x8
    23f2:	48 89 c2             	mov    rdx,rax
    23f5:	83 e5 7f             	and    ebp,0x7f
    23f8:	48 c1 e5 04          	shl    rbp,0x4
    23fc:	4c 01 e5             	add    rbp,r12
    23ff:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2403:	44 89 c0             	mov    eax,r8d
    2406:	c1 e8 18             	shr    eax,0x18
    2409:	a8 80                	test   al,0x80
    240b:	0f 85 28 3a 00 00    	jne    5e39 <luaV_execute+0x4149>
    2411:	48 c1 e0 04          	shl    rax,0x4
    2415:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    2419:	44 89 c0             	mov    eax,r8d
    241c:	c1 e8 10             	shr    eax,0x10
    241f:	a8 80                	test   al,0x80
    2421:	0f 84 30 3a 00 00    	je     5e57 <luaV_execute+0x4167>
    2427:	83 e0 7f             	and    eax,0x7f
    242a:	48 c1 e0 04          	shl    rax,0x4
    242e:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        if (lessthan != GETARG_A(i))
    2433:	41 c1 e8 08          	shr    r8d,0x8
        int lessthan = (ivalue(rb) < ivalue(rc));
    2437:	48 8b 00             	mov    rax,QWORD PTR [rax]
        if (lessthan != GETARG_A(i))
    243a:	41 83 e0 7f          	and    r8d,0x7f
        int lessthan = (ivalue(rb) < ivalue(rc));
    243e:	48 39 02             	cmp    QWORD PTR [rdx],rax
    2441:	0f 9c c0             	setl   al
    2444:	0f b6 c0             	movzx  eax,al
        if (lessthan != GETARG_A(i))
    2447:	41 39 c0             	cmp    r8d,eax
    244a:	0f 84 29 4b 00 00    	je     6f79 <luaV_execute+0x5289>
          pc++;
    2450:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    2454:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2458:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    245b:	45 85 ed             	test   r13d,r13d
    245e:	0f 85 25 57 00 00    	jne    7b89 <luaV_execute+0x5e99>
    2464:	44 89 c5             	mov    ebp,r8d
    2467:	41 0f b6 c0          	movzx  eax,r8b
    246b:	c1 ed 08             	shr    ebp,0x8
    246e:	48 89 c2             	mov    rdx,rax
    2471:	83 e5 7f             	and    ebp,0x7f
    2474:	48 c1 e5 04          	shl    rbp,0x4
    2478:	4c 01 e5             	add    rbp,r12
    247b:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    247f:	44 89 c0             	mov    eax,r8d
    2482:	c1 e8 18             	shr    eax,0x18
    2485:	a8 80                	test   al,0x80
    2487:	0f 85 de 3a 00 00    	jne    5f6b <luaV_execute+0x427b>
    248d:	48 c1 e0 04          	shl    rax,0x4
    2491:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    2495:	44 89 c0             	mov    eax,r8d
    2498:	c1 e8 10             	shr    eax,0x10
    249b:	a8 80                	test   al,0x80
    249d:	0f 84 e6 3a 00 00    	je     5f89 <luaV_execute+0x4299>
    24a3:	83 e0 7f             	and    eax,0x7f
    24a6:	48 c1 e0 04          	shl    rax,0x4
    24aa:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        if (equals != GETARG_A(i))
    24af:	41 c1 e8 08          	shr    r8d,0x8
        int equals = (fltvalue(rb) == fltvalue(rc));
    24b3:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    24b7:	31 d2                	xor    edx,edx
        if (equals != GETARG_A(i))
    24b9:	41 83 e0 7f          	and    r8d,0x7f
        int equals = (fltvalue(rb) == fltvalue(rc));
    24bd:	66 0f 2e 00          	ucomisd xmm0,QWORD PTR [rax]
    24c1:	b8 00 00 00 00       	mov    eax,0x0
    24c6:	0f 9b c2             	setnp  dl
    24c9:	0f 44 c2             	cmove  eax,edx
        if (equals != GETARG_A(i))
    24cc:	41 39 c0             	cmp    r8d,eax
    24cf:	0f 84 38 4b 00 00    	je     700d <luaV_execute+0x531d>
          pc++;
    24d5:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    24d9:	48 8d 58 04          	lea    rbx,[rax+0x4]
    24dd:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    24e0:	45 85 ed             	test   r13d,r13d
    24e3:	0f 85 21 55 00 00    	jne    7a0a <luaV_execute+0x5d1a>
    24e9:	44 89 c5             	mov    ebp,r8d
    24ec:	41 0f b6 c0          	movzx  eax,r8b
    24f0:	c1 ed 08             	shr    ebp,0x8
    24f3:	48 89 c2             	mov    rdx,rax
    24f6:	83 e5 7f             	and    ebp,0x7f
    24f9:	48 c1 e5 04          	shl    rbp,0x4
    24fd:	4c 01 e5             	add    rbp,r12
    2500:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2504:	44 89 c0             	mov    eax,r8d
    2507:	c1 e8 18             	shr    eax,0x18
    250a:	a8 80                	test   al,0x80
    250c:	0f 85 cd 39 00 00    	jne    5edf <luaV_execute+0x41ef>
    2512:	48 c1 e0 04          	shl    rax,0x4
    2516:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    251a:	44 89 c0             	mov    eax,r8d
    251d:	c1 e8 10             	shr    eax,0x10
    2520:	a8 80                	test   al,0x80
    2522:	0f 84 d5 39 00 00    	je     5efd <luaV_execute+0x420d>
    2528:	83 e0 7f             	and    eax,0x7f
    252b:	48 c1 e0 04          	shl    rax,0x4
    252f:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        if (equals != GETARG_A(i))
    2534:	41 c1 e8 08          	shr    r8d,0x8
        int equals = (ivalue(rb) == ivalue(rc));
    2538:	48 8b 00             	mov    rax,QWORD PTR [rax]
        if (equals != GETARG_A(i))
    253b:	41 83 e0 7f          	and    r8d,0x7f
        int equals = (ivalue(rb) == ivalue(rc));
    253f:	48 39 02             	cmp    QWORD PTR [rdx],rax
    2542:	0f 94 c0             	sete   al
    2545:	0f b6 c0             	movzx  eax,al
        if (equals != GETARG_A(i))
    2548:	41 39 c0             	cmp    r8d,eax
    254b:	0f 84 f3 4a 00 00    	je     7044 <luaV_execute+0x5354>
          pc++;
    2551:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    2555:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2559:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    255c:	45 85 ed             	test   r13d,r13d
    255f:	0f 85 50 55 00 00    	jne    7ab5 <luaV_execute+0x5dc5>
    2565:	44 89 c5             	mov    ebp,r8d
    2568:	41 0f b6 c0          	movzx  eax,r8b
    256c:	c1 ed 08             	shr    ebp,0x8
    256f:	48 89 c2             	mov    rdx,rax
    2572:	83 e5 7f             	and    ebp,0x7f
    2575:	48 c1 e5 04          	shl    rbp,0x4
    2579:	4c 01 e5             	add    rbp,r12
    257c:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    2580:	41 c1 e8 18          	shr    r8d,0x18
    2584:	44 89 c0             	mov    eax,r8d
        lua_Integer ib = ivalue(rb);
    2587:	48 c1 e0 04          	shl    rax,0x4
        setivalue(ra, intop(^, ~l_castS2U(0), ib));
    258b:	49 8b 04 04          	mov    rax,QWORD PTR [r12+rax*1]
    258f:	48 f7 d0             	not    rax
    2592:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    2596:	b8 13 00 00 00       	mov    eax,0x13
    259b:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    259f:	48 89 d8             	mov    rax,rbx
    25a2:	48 83 c3 04          	add    rbx,0x4
    25a6:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    25a9:	45 85 ed             	test   r13d,r13d
    25ac:	0f 85 ee 62 00 00    	jne    88a0 <luaV_execute+0x6bb0>
    25b2:	44 89 c5             	mov    ebp,r8d
    25b5:	41 0f b6 c0          	movzx  eax,r8b
    25b9:	c1 ed 08             	shr    ebp,0x8
    25bc:	48 89 c2             	mov    rdx,rax
    25bf:	83 e5 7f             	and    ebp,0x7f
    25c2:	48 c1 e5 04          	shl    rbp,0x4
    25c6:	4c 01 e5             	add    rbp,r12
    25c9:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    25cd:	44 89 c0             	mov    eax,r8d
    25d0:	c1 e8 18             	shr    eax,0x18
    25d3:	a8 80                	test   al,0x80
    25d5:	0f 85 ba 38 00 00    	jne    5e95 <luaV_execute+0x41a5>
    25db:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    25df:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    25e3:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    25e6:	41 f6 c0 80          	test   r8b,0x80
    25ea:	0f 84 bf 38 00 00    	je     5eaf <luaV_execute+0x41bf>
    25f0:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    25f5:	41 83 e0 7f          	and    r8d,0x7f
    25f9:	49 c1 e0 04          	shl    r8,0x4
    25fd:	4a 8d 14 01          	lea    rdx,[rcx+r8*1]
        lua_Integer ic = ivalue(rc);
    2601:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
        setivalue(ra, luaV_shiftl(ivalue(rb), -ic));
    2604:	48 8b 10             	mov    rdx,QWORD PTR [rax]
  if (y < 0) {  /* shift right? */
    2607:	48 85 c9             	test   rcx,rcx
    260a:	0f 8f ba 38 00 00    	jg     5eca <luaV_execute+0x41da>
    if (y <= -NBITS) return 0;
    2610:	31 c0                	xor    eax,eax
    if (y >= NBITS) return 0;
    2612:	48 83 f9 c1          	cmp    rcx,0xffffffffffffffc1
    2616:	7c 09                	jl     2621 <luaV_execute+0x931>
        setivalue(ra, luaV_shiftl(ivalue(rb), -ic));
    2618:	48 f7 d9             	neg    rcx
    else return intop(<<, x, y);
    261b:	48 89 d0             	mov    rax,rdx
    261e:	48 d3 e0             	shl    rax,cl
        setivalue(ra, luaV_shiftl(ivalue(rb), -ic));
    2621:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    2625:	b8 13 00 00 00       	mov    eax,0x13
    262a:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    262e:	48 89 d8             	mov    rax,rbx
    2631:	48 83 c3 04          	add    rbx,0x4
    2635:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2638:	45 85 ed             	test   r13d,r13d
    263b:	0f 85 a9 54 00 00    	jne    7aea <luaV_execute+0x5dfa>
    2641:	44 89 c5             	mov    ebp,r8d
    2644:	41 0f b6 c0          	movzx  eax,r8b
    2648:	c1 ed 08             	shr    ebp,0x8
    264b:	48 89 c2             	mov    rdx,rax
    264e:	83 e5 7f             	and    ebp,0x7f
    2651:	48 c1 e5 04          	shl    rbp,0x4
    2655:	4c 01 e5             	add    rbp,r12
    2658:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    265c:	44 89 c0             	mov    eax,r8d
    265f:	c1 e8 18             	shr    eax,0x18
    2662:	a8 80                	test   al,0x80
    2664:	0f 84 b8 38 00 00    	je     5f22 <luaV_execute+0x4232>
    266a:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    266d:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    2671:	48 c1 e0 04          	shl    rax,0x4
    2675:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    267a:	41 f6 c0 80          	test   r8b,0x80
    267e:	0f 84 b3 38 00 00    	je     5f37 <luaV_execute+0x4247>
    2684:	41 83 e0 7f          	and    r8d,0x7f
    2688:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
    268d:	49 c1 e0 04          	shl    r8,0x4
    2691:	4a 8d 14 07          	lea    rdx,[rdi+r8*1]
        setivalue(ra, luaV_shiftl(ivalue(rb), ivalue(rc)));
    2695:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
    2698:	48 8b 10             	mov    rdx,QWORD PTR [rax]
  if (y < 0) {  /* shift right? */
    269b:	48 85 c9             	test   rcx,rcx
    269e:	0f 88 ae 38 00 00    	js     5f52 <luaV_execute+0x4262>
    else return intop(<<, x, y);
    26a4:	48 d3 e2             	shl    rdx,cl
    26a7:	b8 00 00 00 00       	mov    eax,0x0
    26ac:	48 83 f9 40          	cmp    rcx,0x40
    26b0:	48 0f 4c c2          	cmovl  rax,rdx
        setivalue(ra, luaV_shiftl(ivalue(rb), ivalue(rc)));
    26b4:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    26b8:	b8 13 00 00 00       	mov    eax,0x13
    26bd:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    26c1:	48 89 d8             	mov    rax,rbx
    26c4:	48 83 c3 04          	add    rbx,0x4
    26c8:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    26cb:	45 85 ed             	test   r13d,r13d
    26ce:	0f 85 4b 54 00 00    	jne    7b1f <luaV_execute+0x5e2f>
    26d4:	44 89 c5             	mov    ebp,r8d
    26d7:	41 0f b6 c0          	movzx  eax,r8b
    26db:	c1 ed 08             	shr    ebp,0x8
    26de:	48 89 c2             	mov    rdx,rax
    26e1:	83 e5 7f             	and    ebp,0x7f
    26e4:	48 c1 e5 04          	shl    rbp,0x4
    26e8:	4c 01 e5             	add    rbp,r12
    26eb:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    26ef:	45 89 c7             	mov    r15d,r8d
        TValue *rc = RKC(i);
    26f2:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RB(i);
    26f6:	41 c1 ef 18          	shr    r15d,0x18
    26fa:	49 c1 e7 04          	shl    r15,0x4
    26fe:	4d 01 e7             	add    r15,r12
        TValue *rc = RKC(i);
    2701:	41 f6 c0 80          	test   r8b,0x80
    2705:	0f 85 01 38 00 00    	jne    5f0c <luaV_execute+0x421c>
    270b:	45 0f b6 c0          	movzx  r8d,r8b
    270f:	49 c1 e0 04          	shl    r8,0x4
    2713:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        GETTABLE_INLINE_PROTECTED_I(L, rb, rc, ra);
    2717:	41 0f b7 47 08       	movzx  eax,WORD PTR [r15+0x8]
    271c:	66 3d 05 80          	cmp    ax,0x8005
    2720:	0f 85 4e 53 00 00    	jne    7a74 <luaV_execute+0x5d84>
    2726:	49 8b 3f             	mov    rdi,QWORD PTR [r15]
    2729:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    272c:	8b 47 0c             	mov    eax,DWORD PTR [rdi+0xc]
    272f:	48 8d 4e ff          	lea    rcx,[rsi-0x1]
    2733:	48 39 c1             	cmp    rcx,rax
    2736:	0f 83 a2 6b 00 00    	jae    92de <luaV_execute+0x75ee>
    273c:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
    2740:	48 c1 e6 04          	shl    rsi,0x4
    2744:	4c 8d 44 30 f0       	lea    r8,[rax+rsi*1-0x10]
    2749:	41 0f b7 40 08       	movzx  eax,WORD PTR [r8+0x8]
    274e:	66 85 c0             	test   ax,ax
    2751:	0f 84 54 6b 00 00    	je     92ab <luaV_execute+0x75bb>
    2757:	49 8b 10             	mov    rdx,QWORD PTR [r8]
    275a:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    275e:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    2762:	48 89 d8             	mov    rax,rbx
    2765:	48 83 c3 04          	add    rbx,0x4
    2769:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    276c:	45 85 ed             	test   r13d,r13d
    276f:	0f 85 ca 52 00 00    	jne    7a3f <luaV_execute+0x5d4f>
    2775:	44 89 c5             	mov    ebp,r8d
    2778:	41 0f b6 c0          	movzx  eax,r8b
    277c:	c1 ed 08             	shr    ebp,0x8
    277f:	48 89 c2             	mov    rdx,rax
    2782:	83 e5 7f             	and    ebp,0x7f
    2785:	48 c1 e5 04          	shl    rbp,0x4
    2789:	4c 01 e5             	add    rbp,r12
    278c:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        savepc(L);  /* in case of allocation errors */
    2790:	48 8b 7c 24 18       	mov    rdi,QWORD PTR [rsp+0x18]
        t = raviH_new(L, RAVI_TARRAYFLT, 0);
    2795:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    279a:	31 d2                	xor    edx,edx
    279c:	be 04 00 00 00       	mov    esi,0x4
        savepc(L);  /* in case of allocation errors */
    27a1:	48 89 5f 28          	mov    QWORD PTR [rdi+0x28],rbx
        t = raviH_new(L, RAVI_TARRAYFLT, 0);
    27a5:	4c 89 ff             	mov    rdi,r15
    27a8:	e8 00 00 00 00       	call   27ad <luaV_execute+0xabd>
        setfarrayvalue(L, ra, t);
    27ad:	41 ba 25 80 ff ff    	mov    r10d,0xffff8025
    27b3:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        checkGC(L, ra + 1);
    27b7:	49 8b 47 18          	mov    rax,QWORD PTR [r15+0x18]
        setfarrayvalue(L, ra, t);
    27bb:	66 44 89 55 08       	mov    WORD PTR [rbp+0x8],r10w
        checkGC(L, ra + 1);
    27c0:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    27c5:	0f 8f 78 40 00 00    	jg     6843 <luaV_execute+0x4b53>
        vmbreak;
    27cb:	48 89 d8             	mov    rax,rbx
    27ce:	48 83 c3 04          	add    rbx,0x4
    27d2:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    27d5:	45 85 ed             	test   r13d,r13d
    27d8:	0f 85 6a 64 00 00    	jne    8c48 <luaV_execute+0x6f58>
    27de:	44 89 c5             	mov    ebp,r8d
    27e1:	41 0f b6 c0          	movzx  eax,r8b
    27e5:	c1 ed 08             	shr    ebp,0x8
    27e8:	48 89 c2             	mov    rdx,rax
    27eb:	83 e5 7f             	and    ebp,0x7f
    27ee:	48 c1 e5 04          	shl    rbp,0x4
    27f2:	4c 01 e5             	add    rbp,r12
    27f5:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        int n = cast_int(base - ci->func) - cl->p->numparams - 1;
    27f9:	48 8b 7c 24 20       	mov    rdi,QWORD PTR [rsp+0x20]
    27fe:	48 8b 4c 24 18       	mov    rcx,QWORD PTR [rsp+0x18]
    2803:	4c 89 e0             	mov    rax,r12
        int b = GETARG_B(i) - 1;  /* required results */
    2806:	44 89 c6             	mov    esi,r8d
    2809:	c1 ee 18             	shr    esi,0x18
    280c:	41 bf 00 00 00 00    	mov    r15d,0x0
        int n = cast_int(base - ci->func) - cl->p->numparams - 1;
    2812:	48 8b 57 18          	mov    rdx,QWORD PTR [rdi+0x18]
    2816:	48 2b 01             	sub    rax,QWORD PTR [rcx]
        int b = GETARG_B(i) - 1;  /* required results */
    2819:	83 ee 01             	sub    esi,0x1
        int n = cast_int(base - ci->func) - cl->p->numparams - 1;
    281c:	48 c1 f8 04          	sar    rax,0x4
    2820:	0f b6 52 0a          	movzx  edx,BYTE PTR [rdx+0xa]
    2824:	29 d0                	sub    eax,edx
        if (n < 0)  /* less arguments than parameters? */
    2826:	83 e8 01             	sub    eax,0x1
    2829:	44 0f 49 f8          	cmovns r15d,eax
        if (b < 0) {  /* B == 0? */
    282d:	83 fe ff             	cmp    esi,0xffffffff
    2830:	0f 84 17 41 00 00    	je     694d <luaV_execute+0x4c5d>
    2836:	41 39 f7             	cmp    r15d,esi
    2839:	89 f7                	mov    edi,esi
    283b:	41 0f 4e ff          	cmovle edi,r15d
        for (j = 0; j < b && j < n; j++)
    283f:	85 ff                	test   edi,edi
    2841:	74 39                	je     287c <luaV_execute+0xb8c>
    2843:	44 8d 47 ff          	lea    r8d,[rdi-0x1]
    2847:	4d 63 ff             	movsxd r15,r15d
    284a:	4c 89 e2             	mov    rdx,r12
    284d:	31 c0                	xor    eax,eax
    284f:	49 83 c0 01          	add    r8,0x1
    2853:	49 c1 e7 04          	shl    r15,0x4
    2857:	49 c1 e0 04          	shl    r8,0x4
    285b:	4c 29 fa             	sub    rdx,r15
    285e:	66 90                	xchg   ax,ax
          setobjs2s(L, ra + j, base - n + j);
    2860:	48 8b 0c 02          	mov    rcx,QWORD PTR [rdx+rax*1]
    2864:	48 89 4c 05 00       	mov    QWORD PTR [rbp+rax*1+0x0],rcx
    2869:	0f b7 4c 02 08       	movzx  ecx,WORD PTR [rdx+rax*1+0x8]
    286e:	66 89 4c 05 08       	mov    WORD PTR [rbp+rax*1+0x8],cx
    2873:	48 83 c0 10          	add    rax,0x10
        for (j = 0; j < b && j < n; j++)
    2877:	49 39 c0             	cmp    r8,rax
    287a:	75 e4                	jne    2860 <luaV_execute+0xb70>
        for (; j < b; j++)  /* complete required results with nil */
    287c:	39 f7                	cmp    edi,esi
    287e:	7d 2f                	jge    28af <luaV_execute+0xbbf>
    2880:	8d 56 ff             	lea    edx,[rsi-0x1]
    2883:	48 63 cf             	movsxd rcx,edi
    2886:	29 fa                	sub    edx,edi
    2888:	48 89 c8             	mov    rax,rcx
    288b:	48 01 ca             	add    rdx,rcx
    288e:	48 c1 e0 04          	shl    rax,0x4
    2892:	48 c1 e2 04          	shl    rdx,0x4
    2896:	48 01 e8             	add    rax,rbp
    2899:	48 8d 54 15 10       	lea    rdx,[rbp+rdx*1+0x10]
    289e:	66 90                	xchg   ax,ax
          setnilvalue(ra + j);
    28a0:	31 c9                	xor    ecx,ecx
    28a2:	48 83 c0 10          	add    rax,0x10
    28a6:	66 89 48 f8          	mov    WORD PTR [rax-0x8],cx
        for (; j < b; j++)  /* complete required results with nil */
    28aa:	48 39 c2             	cmp    rdx,rax
    28ad:	75 f1                	jne    28a0 <luaV_execute+0xbb0>
        vmbreak;
    28af:	48 89 d8             	mov    rax,rbx
    28b2:	48 83 c3 04          	add    rbx,0x4
    28b6:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    28b9:	45 85 ed             	test   r13d,r13d
    28bc:	0f 85 c8 5d 00 00    	jne    868a <luaV_execute+0x699a>
    28c2:	44 89 c5             	mov    ebp,r8d
    28c5:	41 0f b6 c0          	movzx  eax,r8b
    28c9:	c1 ed 08             	shr    ebp,0x8
    28cc:	48 89 c2             	mov    rdx,rax
    28cf:	83 e5 7f             	and    ebp,0x7f
    28d2:	48 c1 e5 04          	shl    rbp,0x4
    28d6:	4c 01 e5             	add    rbp,r12
    28d9:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        Proto *p = cl->p->p[GETARG_Bx(i)];
    28dd:	48 8b 7c 24 20       	mov    rdi,QWORD PTR [rsp+0x20]
    28e2:	41 c1 e8 10          	shr    r8d,0x10
    28e6:	44 89 c0             	mov    eax,r8d
    28e9:	48 8b 57 18          	mov    rdx,QWORD PTR [rdi+0x18]
        LClosure *ncl = getcached(p, cl->upvals, base);  /* cached closure */
    28ed:	4c 8d 7f 20          	lea    r15,[rdi+0x20]
    28f1:	4c 89 fe             	mov    rsi,r15
        Proto *p = cl->p->p[GETARG_Bx(i)];
    28f4:	48 8b 52 40          	mov    rdx,QWORD PTR [rdx+0x40]
    28f8:	4c 8b 1c c2          	mov    r11,QWORD PTR [rdx+rax*8]
        LClosure *ncl = getcached(p, cl->upvals, base);  /* cached closure */
    28fc:	4c 89 e2             	mov    rdx,r12
    28ff:	4c 89 df             	mov    rdi,r11
    2902:	e8 f9 d6 ff ff       	call   0 <getcached>
        if (ncl == NULL) { /* no match? */
    2907:	48 85 c0             	test   rax,rax
    290a:	0f 84 d3 4d 00 00    	je     76e3 <luaV_execute+0x59f3>
          setclLvalue(L, ra, ncl);  /* push cashed closure */
    2910:	be 06 80 ff ff       	mov    esi,0xffff8006
    2915:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    2919:	66 89 75 08          	mov    WORD PTR [rbp+0x8],si
        checkGC(L, ra + 1);
    291d:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    2922:	49 8b 47 18          	mov    rax,QWORD PTR [r15+0x18]
    2926:	4c 89 ff             	mov    rdi,r15
    2929:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    292e:	0f 8f ae 3f 00 00    	jg     68e2 <luaV_execute+0x4bf2>
        vmbreak;
    2934:	48 89 d8             	mov    rax,rbx
    2937:	48 83 c3 04          	add    rbx,0x4
    293b:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    293e:	45 85 ed             	test   r13d,r13d
    2941:	0f 85 a0 63 00 00    	jne    8ce7 <luaV_execute+0x6ff7>
    2947:	44 89 c5             	mov    ebp,r8d
    294a:	41 0f b6 c0          	movzx  eax,r8b
    294e:	c1 ed 08             	shr    ebp,0x8
    2951:	48 89 c2             	mov    rdx,rax
    2954:	83 e5 7f             	and    ebp,0x7f
    2957:	48 c1 e5 04          	shl    rbp,0x4
    295b:	4c 01 e5             	add    rbp,r12
    295e:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2962:	44 89 c0             	mov    eax,r8d
    2965:	c1 e8 18             	shr    eax,0x18
    2968:	a8 80                	test   al,0x80
    296a:	0f 85 e0 36 00 00    	jne    6050 <luaV_execute+0x4360>
    2970:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    2974:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    2978:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    297c:	41 f6 c0 80          	test   r8b,0x80
    2980:	0f 84 e8 36 00 00    	je     606e <luaV_execute+0x437e>
    2986:	41 83 e0 7f          	and    r8d,0x7f
    298a:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    298f:	49 c1 e0 04          	shl    r8,0x4
    2993:	4c 01 c0             	add    rax,r8
        setivalue(ra, intop(^, ivalue(rb), ivalue(rc)));
    2996:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    2999:	48 33 10             	xor    rdx,QWORD PTR [rax]
    299c:	b8 13 00 00 00       	mov    eax,0x13
    29a1:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    29a5:	48 89 d8             	mov    rax,rbx
    29a8:	48 83 c3 04          	add    rbx,0x4
        setivalue(ra, intop(^, ivalue(rb), ivalue(rc)));
    29ac:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    29b0:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    29b3:	45 85 ed             	test   r13d,r13d
    29b6:	0f 85 7a 4f 00 00    	jne    7936 <luaV_execute+0x5c46>
    29bc:	44 89 c5             	mov    ebp,r8d
    29bf:	41 0f b6 c0          	movzx  eax,r8b
    29c3:	c1 ed 08             	shr    ebp,0x8
    29c6:	48 89 c2             	mov    rdx,rax
    29c9:	83 e5 7f             	and    ebp,0x7f
    29cc:	48 c1 e5 04          	shl    rbp,0x4
    29d0:	4c 01 e5             	add    rbp,r12
    29d3:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    29d7:	44 89 c0             	mov    eax,r8d
    29da:	c1 e8 18             	shr    eax,0x18
    29dd:	a8 80                	test   al,0x80
    29df:	0f 85 9a 36 00 00    	jne    607f <luaV_execute+0x438f>
    29e5:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    29e9:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    29ed:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    29f1:	41 f6 c0 80          	test   r8b,0x80
    29f5:	0f 84 a2 36 00 00    	je     609d <luaV_execute+0x43ad>
    29fb:	41 83 e0 7f          	and    r8d,0x7f
    29ff:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    2a04:	49 c1 e0 04          	shl    r8,0x4
    2a08:	4c 01 c0             	add    rax,r8
        setivalue(ra, intop(| , ivalue(rb), ivalue(rc)));
    2a0b:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    2a0e:	48 0b 10             	or     rdx,QWORD PTR [rax]
    2a11:	b8 13 00 00 00       	mov    eax,0x13
    2a16:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    2a1a:	48 89 d8             	mov    rax,rbx
    2a1d:	48 83 c3 04          	add    rbx,0x4
        setivalue(ra, intop(| , ivalue(rb), ivalue(rc)));
    2a21:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    2a25:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2a28:	45 85 ed             	test   r13d,r13d
    2a2b:	0f 85 d0 4e 00 00    	jne    7901 <luaV_execute+0x5c11>
    2a31:	44 89 c5             	mov    ebp,r8d
    2a34:	41 0f b6 c0          	movzx  eax,r8b
    2a38:	c1 ed 08             	shr    ebp,0x8
    2a3b:	48 89 c2             	mov    rdx,rax
    2a3e:	83 e5 7f             	and    ebp,0x7f
    2a41:	48 c1 e5 04          	shl    rbp,0x4
    2a45:	4c 01 e5             	add    rbp,r12
    2a48:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2a4c:	44 89 c0             	mov    eax,r8d
    2a4f:	c1 e8 18             	shr    eax,0x18
    2a52:	a8 80                	test   al,0x80
    2a54:	0f 85 0c 34 00 00    	jne    5e66 <luaV_execute+0x4176>
    2a5a:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    2a5e:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    2a62:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    2a66:	41 f6 c0 80          	test   r8b,0x80
    2a6a:	0f 84 14 34 00 00    	je     5e84 <luaV_execute+0x4194>
    2a70:	41 83 e0 7f          	and    r8d,0x7f
    2a74:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    2a79:	49 c1 e0 04          	shl    r8,0x4
    2a7d:	4c 01 c0             	add    rax,r8
        setivalue(ra, intop(&, ivalue(rb), ivalue(rc)));
    2a80:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    2a83:	48 23 10             	and    rdx,QWORD PTR [rax]
        vmbreak;
    2a86:	48 89 d8             	mov    rax,rbx
    2a89:	48 83 c3 04          	add    rbx,0x4
        setivalue(ra, intop(&, ivalue(rb), ivalue(rc)));
    2a8d:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
    2a91:	ba 13 00 00 00       	mov    edx,0x13
        vmbreak;
    2a96:	44 8b 00             	mov    r8d,DWORD PTR [rax]
        setivalue(ra, intop(&, ivalue(rb), ivalue(rc)));
    2a99:	66 89 55 08          	mov    WORD PTR [rbp+0x8],dx
        vmbreak;
    2a9d:	45 85 ed             	test   r13d,r13d
    2aa0:	0f 85 ae 50 00 00    	jne    7b54 <luaV_execute+0x5e64>
    2aa6:	44 89 c5             	mov    ebp,r8d
    2aa9:	41 0f b6 c0          	movzx  eax,r8b
    2aad:	c1 ed 08             	shr    ebp,0x8
    2ab0:	48 89 c2             	mov    rdx,rax
    2ab3:	83 e5 7f             	and    ebp,0x7f
    2ab6:	48 c1 e5 04          	shl    rbp,0x4
    2aba:	4c 01 e5             	add    rbp,r12
    2abd:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        vmbreak;
    2ac1:	48 89 d8             	mov    rax,rbx
    2ac4:	48 83 c3 04          	add    rbx,0x4
    2ac8:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2acb:	45 85 ed             	test   r13d,r13d
    2ace:	0f 85 ea 50 00 00    	jne    7bbe <luaV_execute+0x5ece>
    2ad4:	44 89 c5             	mov    ebp,r8d
    2ad7:	41 0f b6 c0          	movzx  eax,r8b
    2adb:	c1 ed 08             	shr    ebp,0x8
    2ade:	48 89 c2             	mov    rdx,rax
    2ae1:	83 e5 7f             	and    ebp,0x7f
    2ae4:	48 c1 e5 04          	shl    rbp,0x4
    2ae8:	4c 01 e5             	add    rbp,r12
    2aeb:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (ttisinteger(init) && ttisinteger(pstep) &&
    2aef:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
        TValue *plimit = ra + 1;
    2af4:	4c 8d 4d 10          	lea    r9,[rbp+0x10]
        TValue *pstep = ra + 2;
    2af8:	4c 8d 7d 20          	lea    r15,[rbp+0x20]
        if (ttisinteger(init) && ttisinteger(pstep) &&
    2afc:	0f 84 ed 3e 00 00    	je     69ef <luaV_execute+0x4cff>
          savepc(L);  /* in case of errors */
    2b02:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
          if (!tonumber(plimit, &nlimit))
    2b07:	66 41 83 79 08 03    	cmp    WORD PTR [r9+0x8],0x3
          savepc(L);  /* in case of errors */
    2b0d:	48 89 58 28          	mov    QWORD PTR [rax+0x28],rbx
          if (!tonumber(plimit, &nlimit))
    2b11:	0f 85 ab 63 00 00    	jne    8ec2 <luaV_execute+0x71d2>
    2b17:	f2 41 0f 10 01       	movsd  xmm0,QWORD PTR [r9]
    2b1c:	f2 0f 11 84 24 c8 00 	movsd  QWORD PTR [rsp+0xc8],xmm0
    2b23:	00 00 
          setfltvalue(plimit, nlimit);
    2b25:	41 ba 03 00 00 00    	mov    r10d,0x3
    2b2b:	f2 41 0f 11 01       	movsd  QWORD PTR [r9],xmm0
    2b30:	66 45 89 51 08       	mov    WORD PTR [r9+0x8],r10w
          if (!tonumber(pstep, &nstep))
    2b35:	66 41 83 7f 08 03    	cmp    WORD PTR [r15+0x8],0x3
    2b3b:	0f 85 bb 63 00 00    	jne    8efc <luaV_execute+0x720c>
    2b41:	f2 41 0f 10 0f       	movsd  xmm1,QWORD PTR [r15]
    2b46:	f2 0f 11 8c 24 d0 00 	movsd  QWORD PTR [rsp+0xd0],xmm1
    2b4d:	00 00 
          setfltvalue(pstep, nstep);
    2b4f:	41 b9 03 00 00 00    	mov    r9d,0x3
    2b55:	f2 41 0f 11 0f       	movsd  QWORD PTR [r15],xmm1
    2b5a:	66 45 89 4f 08       	mov    WORD PTR [r15+0x8],r9w
          if (!tonumber(init, &ninit))
    2b5f:	66 83 7d 08 03       	cmp    WORD PTR [rbp+0x8],0x3
    2b64:	0f 85 65 66 00 00    	jne    91cf <luaV_execute+0x74df>
    2b6a:	f2 0f 10 45 00       	movsd  xmm0,QWORD PTR [rbp+0x0]
          setfltvalue(init, luai_numsub(L, ninit, nstep));
    2b6f:	f2 0f 5c c1          	subsd  xmm0,xmm1
    2b73:	bf 03 00 00 00       	mov    edi,0x3
    2b78:	66 89 7d 08          	mov    WORD PTR [rbp+0x8],di
    2b7c:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        pc += GETARG_sBx(i);
    2b81:	41 c1 e8 10          	shr    r8d,0x10
    2b85:	44 89 c0             	mov    eax,r8d
    2b88:	48 8d 84 83 00 00 fe 	lea    rax,[rbx+rax*4-0x20000]
    2b8f:	ff 
        vmbreak;
    2b90:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2b94:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2b97:	45 85 ed             	test   r13d,r13d
    2b9a:	0f 85 b0 5f 00 00    	jne    8b50 <luaV_execute+0x6e60>
    2ba0:	44 89 c5             	mov    ebp,r8d
    2ba3:	41 0f b6 c0          	movzx  eax,r8b
    2ba7:	c1 ed 08             	shr    ebp,0x8
    2baa:	48 89 c2             	mov    rdx,rax
    2bad:	83 e5 7f             	and    ebp,0x7f
    2bb0:	48 c1 e5 04          	shl    rbp,0x4
    2bb4:	4c 01 e5             	add    rbp,r12
    2bb7:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (RAVI_LIKELY(ttisinteger(ra))) {  /* integer loop? */
    2bbb:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    2bc0:	0f 85 bf 5f 00 00    	jne    8b85 <luaV_execute+0x6e95>
          lua_Integer step = ivalue(ra + 2);
    2bc6:	48 8b 45 20          	mov    rax,QWORD PTR [rbp+0x20]
          lua_Integer idx = intop(+, ivalue(ra), step); /* increment index */
    2bca:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
          lua_Integer limit = ivalue(ra + 1);
    2bce:	48 8b 4d 10          	mov    rcx,QWORD PTR [rbp+0x10]
          lua_Integer idx = intop(+, ivalue(ra), step); /* increment index */
    2bd2:	48 01 c2             	add    rdx,rax
          if (RAVI_LIKELY((0 < step)) ? (idx <= limit) : (limit <= idx)) {
    2bd5:	48 85 c0             	test   rax,rax
    2bd8:	0f 8e 2a 66 00 00    	jle    9208 <luaV_execute+0x7518>
    2bde:	48 39 ca             	cmp    rdx,rcx
    2be1:	0f 9e c1             	setle  cl
    2be4:	48 89 d8             	mov    rax,rbx
    2be7:	84 c9                	test   cl,cl
    2be9:	0f 85 1d 3c 00 00    	jne    680c <luaV_execute+0x4b1c>
        vmbreak;
    2bef:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2bf3:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2bf6:	45 85 ed             	test   r13d,r13d
    2bf9:	0f 85 37 5c 00 00    	jne    8836 <luaV_execute+0x6b46>
    2bff:	44 89 c5             	mov    ebp,r8d
    2c02:	41 0f b6 c0          	movzx  eax,r8b
    2c06:	c1 ed 08             	shr    ebp,0x8
    2c09:	48 89 c2             	mov    rdx,rax
    2c0c:	83 e5 7f             	and    ebp,0x7f
    2c0f:	48 c1 e5 04          	shl    rbp,0x4
    2c13:	4c 01 e5             	add    rbp,r12
    2c16:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (cl->p->sizep > 0) luaF_close(L, base);
    2c1a:	48 8b 44 24 20       	mov    rax,QWORD PTR [rsp+0x20]
        int b = GETARG_B(i);
    2c1f:	41 c1 e8 18          	shr    r8d,0x18
    2c23:	45 89 c5             	mov    r13d,r8d
        if (cl->p->sizep > 0) luaF_close(L, base);
    2c26:	48 8b 40 18          	mov    rax,QWORD PTR [rax+0x18]
    2c2a:	8b 78 20             	mov    edi,DWORD PTR [rax+0x20]
    2c2d:	85 ff                	test   edi,edi
    2c2f:	0f 8f fc 3b 00 00    	jg     6831 <luaV_execute+0x4b41>
        savepc(L);
    2c35:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
        int nres = (b != 0 ? b - 1 : cast_int(L->top - ra));
    2c3a:	41 8d 4d ff          	lea    ecx,[r13-0x1]
        savepc(L);
    2c3e:	48 89 58 28          	mov    QWORD PTR [rax+0x28],rbx
        int nres = (b != 0 ? b - 1 : cast_int(L->top - ra));
    2c42:	45 85 ed             	test   r13d,r13d
    2c45:	0f 84 e8 33 00 00    	je     6033 <luaV_execute+0x4343>
        b = luaD_poscall(L, ci, ra, nres);
    2c4b:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    2c50:	48 8b 5c 24 08       	mov    rbx,QWORD PTR [rsp+0x8]
    2c55:	48 89 ea             	mov    rdx,rbp
    2c58:	4c 89 fe             	mov    rsi,r15
    2c5b:	48 89 df             	mov    rdi,rbx
    2c5e:	e8 00 00 00 00       	call   2c63 <luaV_execute+0xf73>
        if (ci->callstatus & CIST_FRESH) /* 'ci' still the called one */ {
    2c63:	41 f6 47 42 08       	test   BYTE PTR [r15+0x42],0x8
    2c68:	0f 85 3b 6b 00 00    	jne    97a9 <luaV_execute+0x7ab9>
          ci = L->ci;
    2c6e:	48 8b 4b 20          	mov    rcx,QWORD PTR [rbx+0x20]
    2c72:	0f b6 93 c8 00 00 00 	movzx  edx,BYTE PTR [rbx+0xc8]
    2c79:	48 89 4c 24 18       	mov    QWORD PTR [rsp+0x18],rcx
          if (b) L->top = ci->top;
    2c7e:	85 c0                	test   eax,eax
    2c80:	0f 84 78 3b 00 00    	je     67fe <luaV_execute+0x4b0e>
    2c86:	48 8b 41 08          	mov    rax,QWORD PTR [rcx+0x8]
    2c8a:	48 89 43 10          	mov    QWORD PTR [rbx+0x10],rax
    2c8e:	48 8b 41 28          	mov    rax,QWORD PTR [rcx+0x28]
    2c92:	e9 a2 f0 ff ff       	jmp    1d39 <luaV_execute+0x49>
        if (b != 0) L->top = ra+b;  /* else previous instruction set top */
    2c97:	41 c1 e8 18          	shr    r8d,0x18
    2c9b:	0f 85 78 33 00 00    	jne    6019 <luaV_execute+0x4329>
        savepc(L);
    2ca1:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
        if (luaD_precall(L, ra, LUA_MULTRET,
    2ca6:	48 89 ee             	mov    rsi,rbp
    2ca9:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    2cae:	b9 01 00 00 00       	mov    ecx,0x1
    2cb3:	ba ff ff ff ff       	mov    edx,0xffffffff
        savepc(L);
    2cb8:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
        if (luaD_precall(L, ra, LUA_MULTRET,
    2cbc:	48 89 ef             	mov    rdi,rbp
    2cbf:	e8 00 00 00 00       	call   2cc4 <luaV_execute+0xfd4>
    2cc4:	85 c0                	test   eax,eax
    2cc6:	0f 84 19 6a 00 00    	je     96e5 <luaV_execute+0x79f5>
        vmbreak;
    2ccc:	48 89 d8             	mov    rax,rbx
          Protect((void)0);  /* update 'base' */
    2ccf:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    2cd3:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
        vmbreak;
    2cd7:	48 83 c3 04          	add    rbx,0x4
    2cdb:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2cde:	f6 85 c8 00 00 00 0c 	test   BYTE PTR [rbp+0xc8],0xc
    2ce5:	0f 85 45 64 00 00    	jne    9130 <luaV_execute+0x7440>
    2ceb:	45 31 ed             	xor    r13d,r13d
    2cee:	44 89 c5             	mov    ebp,r8d
    2cf1:	41 0f b6 c0          	movzx  eax,r8b
    2cf5:	c1 ed 08             	shr    ebp,0x8
    2cf8:	48 89 c2             	mov    rdx,rax
    2cfb:	83 e5 7f             	and    ebp,0x7f
    2cfe:	48 c1 e5 04          	shl    rbp,0x4
    2d02:	4c 01 e5             	add    rbp,r12
    2d05:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        int nresults = GETARG_C(i) - 1;
    2d09:	45 89 c5             	mov    r13d,r8d
    2d0c:	41 c1 ed 10          	shr    r13d,0x10
    2d10:	45 0f b6 ed          	movzx  r13d,r13b
    2d14:	41 83 ed 01          	sub    r13d,0x1
        if (b != 0)  /* fixed number of arguments? */
    2d18:	41 c1 e8 18          	shr    r8d,0x18
    2d1c:	0f 85 fd 30 00 00    	jne    5e1f <luaV_execute+0x412f>
        Protect(c_or_compiled = luaD_precall(L, ra, nresults, 1 /* OP_CALL */));
    2d22:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    2d27:	48 89 ee             	mov    rsi,rbp
    2d2a:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    2d2f:	44 89 ea             	mov    edx,r13d
    2d32:	b9 01 00 00 00       	mov    ecx,0x1
    2d37:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    2d3b:	48 89 ef             	mov    rdi,rbp
    2d3e:	e8 00 00 00 00       	call   2d43 <luaV_execute+0x1053>
    2d43:	0f b6 95 c8 00 00 00 	movzx  edx,BYTE PTR [rbp+0xc8]
    2d4a:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    2d4e:	89 d1                	mov    ecx,edx
    2d50:	83 e1 0c             	and    ecx,0xc
        if (c_or_compiled) { /* C or Lua JITed function? */
    2d53:	85 c0                	test   eax,eax
    2d55:	0f 84 77 6a 00 00    	je     97d2 <luaV_execute+0x7ae2>
          if (c_or_compiled == 1 && nresults >= 0) {
    2d5b:	83 f8 01             	cmp    eax,0x1
    2d5e:	75 0d                	jne    2d6d <luaV_execute+0x107d>
    2d60:	45 85 ed             	test   r13d,r13d
    2d63:	78 08                	js     2d6d <luaV_execute+0x107d>
            L->top = ci->top; /* adjust results */
    2d65:	49 8b 47 08          	mov    rax,QWORD PTR [r15+0x8]
    2d69:	48 89 45 10          	mov    QWORD PTR [rbp+0x10],rax
        vmbreak;
    2d6d:	48 89 d8             	mov    rax,rbx
    2d70:	45 31 ed             	xor    r13d,r13d
    2d73:	48 83 c3 04          	add    rbx,0x4
    2d77:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2d7a:	84 c9                	test   cl,cl
    2d7c:	0f 85 18 64 00 00    	jne    919a <luaV_execute+0x74aa>
    2d82:	44 89 c5             	mov    ebp,r8d
    2d85:	41 0f b6 c0          	movzx  eax,r8b
    2d89:	c1 ed 08             	shr    ebp,0x8
    2d8c:	48 89 c2             	mov    rdx,rax
    2d8f:	83 e5 7f             	and    ebp,0x7f
    2d92:	48 c1 e5 04          	shl    rbp,0x4
    2d96:	4c 01 e5             	add    rbp,r12
    2d99:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    2d9d:	44 89 c0             	mov    eax,r8d
    2da0:	c1 e8 18             	shr    eax,0x18
    2da3:	48 c1 e0 04          	shl    rax,0x4
    2da7:	4c 01 e0             	add    rax,r12
        if (GETARG_C(i) ? l_isfalse(rb) : !l_isfalse(rb))
    2daa:	41 81 e0 00 00 ff 00 	and    r8d,0xff0000
    2db1:	0f 84 04 30 00 00    	je     5dbb <luaV_execute+0x40cb>
    2db7:	0f b7 50 08          	movzx  edx,WORD PTR [rax+0x8]
    2dbb:	66 85 d2             	test   dx,dx
    2dbe:	0f 85 65 44 00 00    	jne    7229 <luaV_execute+0x5539>
          pc++;
    2dc4:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    2dc8:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2dcc:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2dcf:	45 85 ed             	test   r13d,r13d
    2dd2:	0f 85 1b 4e 00 00    	jne    7bf3 <luaV_execute+0x5f03>
    2dd8:	44 89 c5             	mov    ebp,r8d
    2ddb:	41 0f b6 c0          	movzx  eax,r8b
    2ddf:	c1 ed 08             	shr    ebp,0x8
    2de2:	48 89 c2             	mov    rdx,rax
    2de5:	83 e5 7f             	and    ebp,0x7f
    2de8:	48 c1 e5 04          	shl    rbp,0x4
    2dec:	4c 01 e5             	add    rbp,r12
    2def:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (GETARG_C(i) ? l_isfalse(ra) : !l_isfalse(ra))
    2df3:	41 81 e0 00 00 ff 00 	and    r8d,0xff0000
    2dfa:	0f 84 f1 2e 00 00    	je     5cf1 <luaV_execute+0x4001>
    2e00:	0f b7 45 08          	movzx  eax,WORD PTR [rbp+0x8]
    2e04:	66 85 c0             	test   ax,ax
    2e07:	0f 85 37 44 00 00    	jne    7244 <luaV_execute+0x5554>
          pc++;
    2e0d:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    2e11:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2e15:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2e18:	45 85 ed             	test   r13d,r13d
    2e1b:	0f 85 07 4e 00 00    	jne    7c28 <luaV_execute+0x5f38>
    2e21:	44 89 c5             	mov    ebp,r8d
    2e24:	41 0f b6 c0          	movzx  eax,r8b
    2e28:	c1 ed 08             	shr    ebp,0x8
    2e2b:	48 89 c2             	mov    rdx,rax
    2e2e:	83 e5 7f             	and    ebp,0x7f
    2e31:	48 c1 e5 04          	shl    rbp,0x4
    2e35:	4c 01 e5             	add    rbp,r12
    2e38:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2e3c:	44 89 c0             	mov    eax,r8d
    2e3f:	c1 e8 18             	shr    eax,0x18
    2e42:	a8 80                	test   al,0x80
    2e44:	0f 85 fa 2d 00 00    	jne    5c44 <luaV_execute+0x3f54>
        TValue *rc = RKC(i);
    2e4a:	44 89 c2             	mov    edx,r8d
        TValue *rb = RKB(i);
    2e4d:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    2e51:	c1 ea 10             	shr    edx,0x10
        TValue *rb = RKB(i);
    2e54:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    2e57:	f6 c2 80             	test   dl,0x80
    2e5a:	0f 84 ff 2d 00 00    	je     5c5f <luaV_execute+0x3f6f>
        if (ttisinteger(rb) && ttisinteger(rc))
    2e60:	0f b7 70 08          	movzx  esi,WORD PTR [rax+0x8]
        TValue *rc = RKC(i);
    2e64:	83 e2 7f             	and    edx,0x7f
    2e67:	48 c1 e2 04          	shl    rdx,0x4
    2e6b:	48 03 54 24 10       	add    rdx,QWORD PTR [rsp+0x10]
        if (ttisinteger(rb) && ttisinteger(rc))
    2e70:	66 83 fe 13          	cmp    si,0x13
    2e74:	0f 84 fd 2d 00 00    	je     5c77 <luaV_execute+0x3f87>
        else if (ttisnumber(rb) && ttisnumber(rc))
    2e7a:	89 f1                	mov    ecx,esi
    2e7c:	83 e1 0f             	and    ecx,0xf
    2e7f:	66 83 f9 03          	cmp    cx,0x3
    2e83:	0f 84 ef 46 00 00    	je     7578 <luaV_execute+0x5888>
        else Protect(
    2e89:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    2e8e:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    2e93:	48 89 c6             	mov    rsi,rax
    2e96:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    2e9b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    2e9f:	48 89 ef             	mov    rdi,rbp
    2ea2:	e8 00 00 00 00       	call   2ea7 <luaV_execute+0x11b7>
    2ea7:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    2eab:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    2eb0:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    2eb7:	00 
    2eb8:	41 83 e5 0c          	and    r13d,0xc
        if (res != GETARG_A(i))
    2ebc:	41 c1 e8 08          	shr    r8d,0x8
    2ec0:	41 83 e0 7f          	and    r8d,0x7f
    2ec4:	41 39 c0             	cmp    r8d,eax
    2ec7:	0f 84 ed 2d 00 00    	je     5cba <luaV_execute+0x3fca>
          pc++;
    2ecd:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    2ed1:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2ed5:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2ed8:	45 85 ed             	test   r13d,r13d
    2edb:	0f 85 b1 4d 00 00    	jne    7c92 <luaV_execute+0x5fa2>
    2ee1:	44 89 c5             	mov    ebp,r8d
    2ee4:	41 0f b6 c0          	movzx  eax,r8b
    2ee8:	c1 ed 08             	shr    ebp,0x8
    2eeb:	48 89 c2             	mov    rdx,rax
    2eee:	83 e5 7f             	and    ebp,0x7f
    2ef1:	48 c1 e5 04          	shl    rbp,0x4
    2ef5:	4c 01 e5             	add    rbp,r12
    2ef8:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2efc:	44 89 c0             	mov    eax,r8d
    2eff:	c1 e8 18             	shr    eax,0x18
    2f02:	a8 80                	test   al,0x80
    2f04:	0f 85 a6 29 00 00    	jne    58b0 <luaV_execute+0x3bc0>
        TValue *rc = RKC(i);
    2f0a:	44 89 c2             	mov    edx,r8d
        TValue *rb = RKB(i);
    2f0d:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    2f11:	c1 ea 10             	shr    edx,0x10
        TValue *rb = RKB(i);
    2f14:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    2f17:	f6 c2 80             	test   dl,0x80
    2f1a:	0f 84 ab 29 00 00    	je     58cb <luaV_execute+0x3bdb>
        if (ttisinteger(rb) && ttisinteger(rc))
    2f20:	0f b7 70 08          	movzx  esi,WORD PTR [rax+0x8]
        TValue *rc = RKC(i);
    2f24:	83 e2 7f             	and    edx,0x7f
    2f27:	48 c1 e2 04          	shl    rdx,0x4
    2f2b:	48 03 54 24 10       	add    rdx,QWORD PTR [rsp+0x10]
        if (ttisinteger(rb) && ttisinteger(rc))
    2f30:	66 83 fe 13          	cmp    si,0x13
    2f34:	0f 84 a9 29 00 00    	je     58e3 <luaV_execute+0x3bf3>
        else if (ttisnumber(rb) && ttisnumber(rc))
    2f3a:	89 f1                	mov    ecx,esi
    2f3c:	83 e1 0f             	and    ecx,0xf
    2f3f:	66 83 f9 03          	cmp    cx,0x3
    2f43:	0f 84 47 46 00 00    	je     7590 <luaV_execute+0x58a0>
        else Protect(
    2f49:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    2f4e:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    2f53:	48 89 c6             	mov    rsi,rax
    2f56:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    2f5b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    2f5f:	48 89 ef             	mov    rdi,rbp
    2f62:	e8 00 00 00 00       	call   2f67 <luaV_execute+0x1277>
    2f67:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    2f6b:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    2f70:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    2f77:	00 
    2f78:	41 83 e5 0c          	and    r13d,0xc
        if (res != GETARG_A(i))
    2f7c:	41 c1 e8 08          	shr    r8d,0x8
    2f80:	41 83 e0 7f          	and    r8d,0x7f
    2f84:	41 39 c0             	cmp    r8d,eax
    2f87:	0f 84 99 29 00 00    	je     5926 <luaV_execute+0x3c36>
          pc++;
    2f8d:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    2f91:	48 8d 58 04          	lea    rbx,[rax+0x4]
    2f95:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    2f98:	45 85 ed             	test   r13d,r13d
    2f9b:	0f 85 28 4e 00 00    	jne    7dc9 <luaV_execute+0x60d9>
    2fa1:	44 89 c5             	mov    ebp,r8d
    2fa4:	41 0f b6 c0          	movzx  eax,r8b
    2fa8:	c1 ed 08             	shr    ebp,0x8
    2fab:	48 89 c2             	mov    rdx,rax
    2fae:	83 e5 7f             	and    ebp,0x7f
    2fb1:	48 c1 e5 04          	shl    rbp,0x4
    2fb5:	4c 01 e5             	add    rbp,r12
    2fb8:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    2fbc:	44 89 c0             	mov    eax,r8d
    2fbf:	c1 e8 18             	shr    eax,0x18
    2fc2:	a8 80                	test   al,0x80
    2fc4:	0f 85 b8 28 00 00    	jne    5882 <luaV_execute+0x3b92>
    2fca:	48 c1 e0 04          	shl    rax,0x4
    2fce:	49 8d 34 04          	lea    rsi,[r12+rax*1]
        TValue *rc = RKC(i);
    2fd2:	44 89 c0             	mov    eax,r8d
    2fd5:	c1 e8 10             	shr    eax,0x10
    2fd8:	a8 80                	test   al,0x80
    2fda:	0f 84 c0 28 00 00    	je     58a0 <luaV_execute+0x3bb0>
    2fe0:	83 e0 7f             	and    eax,0x7f
    2fe3:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
    2fe8:	48 c1 e0 04          	shl    rax,0x4
    2fec:	48 8d 14 07          	lea    rdx,[rdi+rax*1]
        Protect(
    2ff0:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    2ff5:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    2ffa:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    2fff:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    3003:	48 89 ef             	mov    rdi,rbp
    3006:	e8 00 00 00 00       	call   300b <luaV_execute+0x131b>
        if (res != GETARG_A(i))
    300b:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
        Protect(
    3010:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    3014:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    301b:	00 
        if (res != GETARG_A(i))
    301c:	41 c1 e8 08          	shr    r8d,0x8
    3020:	41 83 e0 7f          	and    r8d,0x7f
        Protect(
    3024:	41 83 e5 0c          	and    r13d,0xc
        if (res != GETARG_A(i))
    3028:	41 39 c0             	cmp    r8d,eax
    302b:	0f 84 7f 3f 00 00    	je     6fb0 <luaV_execute+0x52c0>
          pc++;
    3031:	48 8d 43 04          	lea    rax,[rbx+0x4]
        vmbreak;
    3035:	48 8d 58 04          	lea    rbx,[rax+0x4]
    3039:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    303c:	45 85 ed             	test   r13d,r13d
    303f:	0f 85 0e 53 00 00    	jne    8353 <luaV_execute+0x6663>
    3045:	44 89 c5             	mov    ebp,r8d
    3048:	41 0f b6 c0          	movzx  eax,r8b
    304c:	c1 ed 08             	shr    ebp,0x8
    304f:	48 89 c2             	mov    rdx,rax
    3052:	83 e5 7f             	and    ebp,0x7f
    3055:	48 c1 e5 04          	shl    rbp,0x4
    3059:	4c 01 e5             	add    rbp,r12
    305c:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        dojump(ci, i, 0);
    3060:	44 89 c0             	mov    eax,r8d
    3063:	c1 e8 08             	shr    eax,0x8
    3066:	83 e0 7f             	and    eax,0x7f
    3069:	0f 85 d0 3a 00 00    	jne    6b3f <luaV_execute+0x4e4f>
    306f:	41 c1 e8 10          	shr    r8d,0x10
    3073:	45 31 ed             	xor    r13d,r13d
    3076:	41 8d 80 00 80 ff ff 	lea    eax,[r8-0x8000]
    307d:	48 98                	cdqe   
    307f:	48 8d 04 83          	lea    rax,[rbx+rax*4]
        vmbreak;
    3083:	48 8d 58 04          	lea    rbx,[rax+0x4]
    3087:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    308a:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
    308f:	f6 80 c8 00 00 00 0c 	test   BYTE PTR [rax+0xc8],0xc
    3096:	0f 85 80 5c 00 00    	jne    8d1c <luaV_execute+0x702c>
    309c:	44 89 c5             	mov    ebp,r8d
    309f:	41 0f b6 c0          	movzx  eax,r8b
    30a3:	c1 ed 08             	shr    ebp,0x8
    30a6:	48 89 c2             	mov    rdx,rax
    30a9:	83 e5 7f             	and    ebp,0x7f
    30ac:	48 c1 e5 04          	shl    rbp,0x4
    30b0:	4c 01 e5             	add    rbp,r12
    30b3:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        int c = GETARG_C(i);
    30b7:	44 89 c0             	mov    eax,r8d
        L->top = base + c + 1;  /* mark the end of concat operands */
    30ba:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
        int b = GETARG_B(i);
    30bf:	44 89 c5             	mov    ebp,r8d
        int c = GETARG_C(i);
    30c2:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    30c7:	c1 e8 10             	shr    eax,0x10
        int b = GETARG_B(i);
    30ca:	c1 ed 18             	shr    ebp,0x18
        int c = GETARG_C(i);
    30cd:	0f b6 d0             	movzx  edx,al
        L->top = base + c + 1;  /* mark the end of concat operands */
    30d0:	4c 89 ff             	mov    rdi,r15
        int c = GETARG_C(i);
    30d3:	48 89 d0             	mov    rax,rdx
        L->top = base + c + 1;  /* mark the end of concat operands */
    30d6:	48 83 c2 01          	add    rdx,0x1
    30da:	48 c1 e2 04          	shl    rdx,0x4
        Protect(luaV_concat(L, c - b + 1));
    30de:	29 e8                	sub    eax,ebp
        L->top = base + c + 1;  /* mark the end of concat operands */
    30e0:	49 01 d4             	add    r12,rdx
        Protect(luaV_concat(L, c - b + 1));
    30e3:	8d 70 01             	lea    esi,[rax+0x1]
        L->top = base + c + 1;  /* mark the end of concat operands */
    30e6:	4d 89 67 10          	mov    QWORD PTR [r15+0x10],r12
        Protect(luaV_concat(L, c - b + 1));
    30ea:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    30ef:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    30f4:	e8 00 00 00 00       	call   30f9 <luaV_execute+0x1409>
        ra = RA(i);  /* 'luaV_concat' may invoke TMs and move the stack */
    30f9:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
        rb = base + b;
    30fe:	89 e8                	mov    eax,ebp
        Protect(luaV_concat(L, c - b + 1));
    3100:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
        rb = base + b;
    3105:	48 c1 e0 04          	shl    rax,0x4
        Protect(luaV_concat(L, c - b + 1));
    3109:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    3110:	00 
        ra = RA(i);  /* 'luaV_concat' may invoke TMs and move the stack */
    3111:	44 89 c2             	mov    edx,r8d
        rb = base + b;
    3114:	4c 01 e0             	add    rax,r12
        ra = RA(i);  /* 'luaV_concat' may invoke TMs and move the stack */
    3117:	c1 ea 08             	shr    edx,0x8
        setobjs2s(L, ra, rb);
    311a:	48 8b 08             	mov    rcx,QWORD PTR [rax]
        Protect(luaV_concat(L, c - b + 1));
    311d:	41 83 e5 0c          	and    r13d,0xc
        ra = RA(i);  /* 'luaV_concat' may invoke TMs and move the stack */
    3121:	83 e2 7f             	and    edx,0x7f
    3124:	48 c1 e2 04          	shl    rdx,0x4
    3128:	4c 01 e2             	add    rdx,r12
        setobjs2s(L, ra, rb);
    312b:	48 89 0a             	mov    QWORD PTR [rdx],rcx
    312e:	0f b7 48 08          	movzx  ecx,WORD PTR [rax+0x8]
    3132:	66 89 4a 08          	mov    WORD PTR [rdx+0x8],cx
        checkGC(L, (ra >= rb ? ra + 1 : rb));
    3136:	49 8b 4f 18          	mov    rcx,QWORD PTR [r15+0x18]
    313a:	48 83 79 18 00       	cmp    QWORD PTR [rcx+0x18],0x0
    313f:	7e 35                	jle    3176 <luaV_execute+0x1486>
    3141:	48 39 c2             	cmp    rdx,rax
    3144:	48 8d 4a 10          	lea    rcx,[rdx+0x10]
    3148:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    314d:	48 0f 43 c1          	cmovae rax,rcx
    3151:	4c 89 ff             	mov    rdi,r15
    3154:	49 89 47 10          	mov    QWORD PTR [r15+0x10],rax
    3158:	e8 00 00 00 00       	call   315d <luaV_execute+0x146d>
    315d:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    3162:	48 89 58 28          	mov    QWORD PTR [rax+0x28],rbx
    3166:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    316d:	00 
    316e:	4c 8b 60 20          	mov    r12,QWORD PTR [rax+0x20]
    3172:	41 83 e5 0c          	and    r13d,0xc
        L->top = ci->top;  /* restore top */
    3176:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    317b:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
    3180:	48 8b 40 08          	mov    rax,QWORD PTR [rax+0x8]
    3184:	48 89 41 10          	mov    QWORD PTR [rcx+0x10],rax
        vmbreak;
    3188:	48 89 d8             	mov    rax,rbx
    318b:	48 83 c3 04          	add    rbx,0x4
    318f:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3192:	45 85 ed             	test   r13d,r13d
    3195:	0f 85 78 57 00 00    	jne    8913 <luaV_execute+0x6c23>
    319b:	44 89 c5             	mov    ebp,r8d
    319e:	41 0f b6 c0          	movzx  eax,r8b
    31a2:	c1 ed 08             	shr    ebp,0x8
    31a5:	48 89 c2             	mov    rdx,rax
    31a8:	83 e5 7f             	and    ebp,0x7f
    31ab:	48 c1 e5 04          	shl    rbp,0x4
    31af:	4c 01 e5             	add    rbp,r12
    31b2:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        Protect(luaV_objlen(L, ra, RB(i)));
    31b6:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    31bb:	41 c1 e8 18          	shr    r8d,0x18
    31bf:	48 89 ee             	mov    rsi,rbp
    31c2:	45 31 ed             	xor    r13d,r13d
    31c5:	44 89 c2             	mov    edx,r8d
    31c8:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    31cd:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    31d1:	48 c1 e2 04          	shl    rdx,0x4
    31d5:	4c 01 e2             	add    rdx,r12
    31d8:	48 89 ef             	mov    rdi,rbp
    31db:	e8 00 00 00 00       	call   31e0 <luaV_execute+0x14f0>
        vmbreak;
    31e0:	48 89 d8             	mov    rax,rbx
        Protect(luaV_objlen(L, ra, RB(i)));
    31e3:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
        vmbreak;
    31e7:	48 83 c3 04          	add    rbx,0x4
    31eb:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    31ee:	f6 85 c8 00 00 00 0c 	test   BYTE PTR [rbp+0xc8],0xc
    31f5:	0f 85 89 5b 00 00    	jne    8d84 <luaV_execute+0x7094>
    31fb:	44 89 c5             	mov    ebp,r8d
    31fe:	41 0f b6 c0          	movzx  eax,r8b
    3202:	c1 ed 08             	shr    ebp,0x8
    3205:	48 89 c2             	mov    rdx,rax
    3208:	83 e5 7f             	and    ebp,0x7f
    320b:	48 c1 e5 04          	shl    rbp,0x4
    320f:	4c 01 e5             	add    rbp,r12
    3212:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    3216:	41 c1 e8 18          	shr    r8d,0x18
        int res = l_isfalse(rb);  /* next assignment may change this value */
    321a:	ba 01 00 00 00       	mov    edx,0x1
        TValue *rb = RB(i);
    321f:	44 89 c0             	mov    eax,r8d
    3222:	48 c1 e0 04          	shl    rax,0x4
    3226:	4c 01 e0             	add    rax,r12
        int res = l_isfalse(rb);  /* next assignment may change this value */
    3229:	0f b7 48 08          	movzx  ecx,WORD PTR [rax+0x8]
    322d:	66 85 c9             	test   cx,cx
    3230:	0f 85 f7 27 00 00    	jne    5a2d <luaV_execute+0x3d3d>
        vmbreak;
    3236:	48 89 d8             	mov    rax,rbx
        setbvalue(ra, res);
    3239:	41 bf 01 00 00 00    	mov    r15d,0x1
    323f:	89 55 00             	mov    DWORD PTR [rbp+0x0],edx
        vmbreak;
    3242:	48 83 c3 04          	add    rbx,0x4
        setbvalue(ra, res);
    3246:	66 44 89 7d 08       	mov    WORD PTR [rbp+0x8],r15w
        vmbreak;
    324b:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    324e:	45 85 ed             	test   r13d,r13d
    3251:	0f 85 f8 5a 00 00    	jne    8d4f <luaV_execute+0x705f>
    3257:	44 89 c5             	mov    ebp,r8d
    325a:	41 0f b6 c0          	movzx  eax,r8b
    325e:	c1 ed 08             	shr    ebp,0x8
    3261:	48 89 c2             	mov    rdx,rax
    3264:	83 e5 7f             	and    ebp,0x7f
    3267:	48 c1 e5 04          	shl    rbp,0x4
    326b:	4c 01 e5             	add    rbp,r12
    326e:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    3272:	41 c1 e8 18          	shr    r8d,0x18
    3276:	45 89 c1             	mov    r9d,r8d
    3279:	49 c1 e1 04          	shl    r9,0x4
    327d:	4d 01 e1             	add    r9,r12
        if (tointegerns(rb, &ib)) {
    3280:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
    3285:	66 83 f8 13          	cmp    ax,0x13
    3289:	0f 84 a0 38 00 00    	je     6b2f <luaV_execute+0x4e3f>
  if (!ttisfloat(obj))
    328f:	66 83 f8 03          	cmp    ax,0x3
    3293:	0f 85 06 3c 00 00    	jne    6e9f <luaV_execute+0x51af>
    3299:	31 d2                	xor    edx,edx
    329b:	48 8d b4 24 b0 00 00 	lea    rsi,[rsp+0xb0]
    32a2:	00 
    32a3:	4c 89 cf             	mov    rdi,r9
    32a6:	e8 e5 cf ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib)) {
    32ab:	85 c0                	test   eax,eax
    32ad:	0f 84 ec 3b 00 00    	je     6e9f <luaV_execute+0x51af>
    32b3:	48 8b 84 24 b0 00 00 	mov    rax,QWORD PTR [rsp+0xb0]
    32ba:	00 
          setivalue(ra, intop(^, ~l_castS2U(0), ib));
    32bb:	48 f7 d0             	not    rax
    32be:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    32c2:	b8 13 00 00 00       	mov    eax,0x13
    32c7:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    32cb:	48 89 d8             	mov    rax,rbx
    32ce:	48 83 c3 04          	add    rbx,0x4
    32d2:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    32d5:	45 85 ed             	test   r13d,r13d
    32d8:	0f 85 e1 53 00 00    	jne    86bf <luaV_execute+0x69cf>
    32de:	44 89 c5             	mov    ebp,r8d
    32e1:	41 0f b6 c0          	movzx  eax,r8b
    32e5:	c1 ed 08             	shr    ebp,0x8
    32e8:	48 89 c2             	mov    rdx,rax
    32eb:	83 e5 7f             	and    ebp,0x7f
    32ee:	48 c1 e5 04          	shl    rbp,0x4
    32f2:	4c 01 e5             	add    rbp,r12
    32f5:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    32f9:	41 c1 e8 18          	shr    r8d,0x18
    32fd:	44 89 c6             	mov    esi,r8d
    3300:	48 c1 e6 04          	shl    rsi,0x4
    3304:	4c 01 e6             	add    rsi,r12
        if (ttisinteger(rb)) {
    3307:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    330b:	66 83 f8 13          	cmp    ax,0x13
    330f:	0f 84 02 38 00 00    	je     6b17 <luaV_execute+0x4e27>
        else if (tonumberns(rb, nb)) {
    3315:	66 83 f8 03          	cmp    ax,0x3
    3319:	0f 85 5c 3d 00 00    	jne    707b <luaV_execute+0x538b>
          setfltvalue(ra, luai_numunm(L, nb));
    331f:	b8 03 00 00 00       	mov    eax,0x3
    3324:	f2 0f 10 06          	movsd  xmm0,QWORD PTR [rsi]
    3328:	66 0f 57 05 00 00 00 	xorpd  xmm0,XMMWORD PTR [rip+0x0]        # 3330 <luaV_execute+0x1640>
    332f:	00 
    3330:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    3334:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    3339:	48 89 d8             	mov    rax,rbx
    333c:	48 83 c3 04          	add    rbx,0x4
    3340:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3343:	45 85 ed             	test   r13d,r13d
    3346:	0f 85 a8 53 00 00    	jne    86f4 <luaV_execute+0x6a04>
    334c:	44 89 c5             	mov    ebp,r8d
    334f:	41 0f b6 c0          	movzx  eax,r8b
    3353:	c1 ed 08             	shr    ebp,0x8
    3356:	48 89 c2             	mov    rdx,rax
    3359:	83 e5 7f             	and    ebp,0x7f
    335c:	48 c1 e5 04          	shl    rbp,0x4
    3360:	4c 01 e5             	add    rbp,r12
    3363:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    3367:	44 89 c0             	mov    eax,r8d
    336a:	c1 e8 18             	shr    eax,0x18
    336d:	a8 80                	test   al,0x80
    336f:	0f 85 d2 29 00 00    	jne    5d47 <luaV_execute+0x4057>
    3375:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3379:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    337d:	49 8d 34 04          	lea    rsi,[r12+rax*1]
        TValue *rc = RKC(i);
    3381:	41 f6 c0 80          	test   r8b,0x80
    3385:	0f 84 d9 29 00 00    	je     5d64 <luaV_execute+0x4074>
    338b:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    3390:	41 83 e0 7f          	and    r8d,0x7f
    3394:	49 c1 e0 04          	shl    r8,0x4
    3398:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    339c:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    33a0:	66 83 f8 03          	cmp    ax,0x3
    33a4:	0f 84 d4 29 00 00    	je     5d7e <luaV_execute+0x408e>
    33aa:	66 83 f8 13          	cmp    ax,0x13
    33ae:	0f 84 f4 41 00 00    	je     75a8 <luaV_execute+0x58b8>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_POW)); }
    33b4:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    33b9:	48 89 e9             	mov    rcx,rbp
    33bc:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    33c1:	41 b8 0a 00 00 00    	mov    r8d,0xa
    33c7:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    33cb:	48 89 ef             	mov    rdi,rbp
    33ce:	e8 00 00 00 00       	call   33d3 <luaV_execute+0x16e3>
    33d3:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    33da:	00 
    33db:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    33df:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    33e3:	48 89 d8             	mov    rax,rbx
    33e6:	48 83 c3 04          	add    rbx,0x4
    33ea:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    33ed:	45 85 ed             	test   r13d,r13d
    33f0:	0f 85 98 43 00 00    	jne    778e <luaV_execute+0x5a9e>
    33f6:	44 89 c5             	mov    ebp,r8d
    33f9:	41 0f b6 c0          	movzx  eax,r8b
    33fd:	c1 ed 08             	shr    ebp,0x8
    3400:	48 89 c2             	mov    rdx,rax
    3403:	83 e5 7f             	and    ebp,0x7f
    3406:	48 c1 e5 04          	shl    rbp,0x4
    340a:	4c 01 e5             	add    rbp,r12
    340d:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    3411:	44 89 c0             	mov    eax,r8d
    3414:	c1 e8 18             	shr    eax,0x18
    3417:	a8 80                	test   al,0x80
    3419:	0f 85 3e 25 00 00    	jne    595d <luaV_execute+0x3c6d>
    341f:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3423:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    3427:	49 8d 34 04          	lea    rsi,[r12+rax*1]
        TValue *rc = RKC(i);
    342b:	41 f6 c0 80          	test   r8b,0x80
    342f:	0f 84 46 25 00 00    	je     597b <luaV_execute+0x3c8b>
    3435:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    343a:	41 83 e0 7f          	and    r8d,0x7f
    343e:	49 c1 e0 04          	shl    r8,0x4
    3442:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    3446:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    344a:	66 83 f8 13          	cmp    ax,0x13
    344e:	0f 84 41 25 00 00    	je     5995 <luaV_execute+0x3ca5>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    3454:	66 83 f8 03          	cmp    ax,0x3
    3458:	0f 85 cb 38 00 00    	jne    6d29 <luaV_execute+0x5039>
    345e:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    3462:	f2 0f 10 06          	movsd  xmm0,QWORD PTR [rsi]
    3466:	66 83 f8 03          	cmp    ax,0x3
    346a:	0f 84 49 25 00 00    	je     59b9 <luaV_execute+0x3cc9>
    3470:	66 83 f8 13          	cmp    ax,0x13
    3474:	0f 85 af 38 00 00    	jne    6d29 <luaV_execute+0x5039>
    347a:	66 0f ef c9          	pxor   xmm1,xmm1
    347e:	f2 48 0f 2a 0a       	cvtsi2sd xmm1,QWORD PTR [rdx]
    3483:	e9 35 25 00 00       	jmp    59bd <luaV_execute+0x3ccd>
        TValue *rb = RKB(i);
    3488:	44 89 c0             	mov    eax,r8d
    348b:	c1 e8 18             	shr    eax,0x18
    348e:	a8 80                	test   al,0x80
    3490:	0f 84 18 27 00 00    	je     5bae <luaV_execute+0x3ebe>
    3496:	83 e0 7f             	and    eax,0x7f
    3499:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    349e:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    34a2:	48 c1 e0 04          	shl    rax,0x4
    34a6:	48 8d 34 01          	lea    rsi,[rcx+rax*1]
        TValue *rc = RKC(i);
    34aa:	41 f6 c0 80          	test   r8b,0x80
    34ae:	0f 84 10 27 00 00    	je     5bc4 <luaV_execute+0x3ed4>
    34b4:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    34b9:	41 83 e0 7f          	and    r8d,0x7f
    34bd:	49 c1 e0 04          	shl    r8,0x4
    34c1:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    34c5:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    34c9:	66 83 f8 13          	cmp    ax,0x13
    34cd:	0f 84 0b 27 00 00    	je     5bde <luaV_execute+0x3eee>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    34d3:	66 83 f8 03          	cmp    ax,0x3
    34d7:	0f 85 ef 37 00 00    	jne    6ccc <luaV_execute+0x4fdc>
    34dd:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    34e1:	f2 0f 10 06          	movsd  xmm0,QWORD PTR [rsi]
    34e5:	66 83 f8 03          	cmp    ax,0x3
    34e9:	0f 84 13 27 00 00    	je     5c02 <luaV_execute+0x3f12>
    34ef:	66 83 f8 13          	cmp    ax,0x13
    34f3:	0f 85 d3 37 00 00    	jne    6ccc <luaV_execute+0x4fdc>
    34f9:	66 0f ef d2          	pxor   xmm2,xmm2
    34fd:	f2 48 0f 2a 12       	cvtsi2sd xmm2,QWORD PTR [rdx]
    3502:	e9 ff 26 00 00       	jmp    5c06 <luaV_execute+0x3f16>
        TValue *rb = RKB(i);
    3507:	44 89 c0             	mov    eax,r8d
    350a:	c1 e8 18             	shr    eax,0x18
    350d:	a8 80                	test   al,0x80
    350f:	0f 85 e6 25 00 00    	jne    5afb <luaV_execute+0x3e0b>
    3515:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3519:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    351d:	4d 8d 0c 04          	lea    r9,[r12+rax*1]
        TValue *rc = RKC(i);
    3521:	41 f6 c0 80          	test   r8b,0x80
    3525:	0f 84 ee 25 00 00    	je     5b19 <luaV_execute+0x3e29>
    352b:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    3530:	41 83 e0 7f          	and    r8d,0x7f
    3534:	49 c1 e0 04          	shl    r8,0x4
    3538:	4e 8d 14 00          	lea    r10,[rax+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    353c:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
    3541:	66 83 f8 13          	cmp    ax,0x13
    3545:	0f 84 e9 25 00 00    	je     5b34 <luaV_execute+0x3e44>
  if (!ttisfloat(obj))
    354b:	66 83 f8 03          	cmp    ax,0x3
    354f:	75 1a                	jne    356b <luaV_execute+0x187b>
    3551:	31 d2                	xor    edx,edx
    3553:	48 8d b4 24 a0 00 00 	lea    rsi,[rsp+0xa0]
    355a:	00 
    355b:	4c 89 cf             	mov    rdi,r9
    355e:	e8 2d cd ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    3563:	85 c0                	test   eax,eax
    3565:	0f 85 d4 25 00 00    	jne    5b3f <luaV_execute+0x3e4f>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_SHR)); }
    356b:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    3570:	48 89 e9             	mov    rcx,rbp
    3573:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    3578:	4c 89 d2             	mov    rdx,r10
    357b:	41 b8 11 00 00 00    	mov    r8d,0x11
    3581:	4c 89 ce             	mov    rsi,r9
    3584:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    3588:	48 89 ef             	mov    rdi,rbp
    358b:	e8 00 00 00 00       	call   3590 <luaV_execute+0x18a0>
    3590:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    3597:	00 
    3598:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    359c:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    35a0:	48 89 d8             	mov    rax,rbx
    35a3:	48 83 c3 04          	add    rbx,0x4
    35a7:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    35aa:	45 85 ed             	test   r13d,r13d
    35ad:	0f 85 e4 42 00 00    	jne    7897 <luaV_execute+0x5ba7>
    35b3:	44 89 c5             	mov    ebp,r8d
    35b6:	41 0f b6 c0          	movzx  eax,r8b
    35ba:	c1 ed 08             	shr    ebp,0x8
    35bd:	48 89 c2             	mov    rdx,rax
    35c0:	83 e5 7f             	and    ebp,0x7f
    35c3:	48 c1 e5 04          	shl    rbp,0x4
    35c7:	4c 01 e5             	add    rbp,r12
    35ca:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    35ce:	44 89 c0             	mov    eax,r8d
    35d1:	c1 e8 18             	shr    eax,0x18
    35d4:	a8 80                	test   al,0x80
    35d6:	0f 85 6b 24 00 00    	jne    5a47 <luaV_execute+0x3d57>
    35dc:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    35e0:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    35e4:	4d 8d 0c 04          	lea    r9,[r12+rax*1]
        TValue *rc = RKC(i);
    35e8:	41 f6 c0 80          	test   r8b,0x80
    35ec:	0f 84 73 24 00 00    	je     5a65 <luaV_execute+0x3d75>
    35f2:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    35f7:	41 83 e0 7f          	and    r8d,0x7f
    35fb:	49 c1 e0 04          	shl    r8,0x4
    35ff:	4e 8d 14 00          	lea    r10,[rax+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    3603:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
    3608:	66 83 f8 13          	cmp    ax,0x13
    360c:	0f 84 6e 24 00 00    	je     5a80 <luaV_execute+0x3d90>
  if (!ttisfloat(obj))
    3612:	66 83 f8 03          	cmp    ax,0x3
    3616:	75 1a                	jne    3632 <luaV_execute+0x1942>
    3618:	31 d2                	xor    edx,edx
    361a:	48 8d b4 24 90 00 00 	lea    rsi,[rsp+0x90]
    3621:	00 
    3622:	4c 89 cf             	mov    rdi,r9
    3625:	e8 66 cc ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    362a:	85 c0                	test   eax,eax
    362c:	0f 85 59 24 00 00    	jne    5a8b <luaV_execute+0x3d9b>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_SHL)); }
    3632:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    3637:	48 89 e9             	mov    rcx,rbp
    363a:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    363f:	4c 89 d2             	mov    rdx,r10
    3642:	41 b8 10 00 00 00    	mov    r8d,0x10
    3648:	4c 89 ce             	mov    rsi,r9
    364b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    364f:	48 89 ef             	mov    rdi,rbp
    3652:	e8 00 00 00 00       	call   3657 <luaV_execute+0x1967>
    3657:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    365e:	00 
    365f:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    3663:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    3667:	48 89 d8             	mov    rax,rbx
    366a:	48 83 c3 04          	add    rbx,0x4
    366e:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3671:	45 85 ed             	test   r13d,r13d
    3674:	0f 85 e8 41 00 00    	jne    7862 <luaV_execute+0x5b72>
    367a:	44 89 c5             	mov    ebp,r8d
    367d:	41 0f b6 c0          	movzx  eax,r8b
    3681:	c1 ed 08             	shr    ebp,0x8
    3684:	48 89 c2             	mov    rdx,rax
    3687:	83 e5 7f             	and    ebp,0x7f
    368a:	48 c1 e5 04          	shl    rbp,0x4
    368e:	4c 01 e5             	add    rbp,r12
    3691:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    3695:	44 89 c0             	mov    eax,r8d
    3698:	c1 e8 18             	shr    eax,0x18
    369b:	a8 80                	test   al,0x80
    369d:	0f 85 44 21 00 00    	jne    57e7 <luaV_execute+0x3af7>
    36a3:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    36a7:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    36ab:	4d 8d 0c 04          	lea    r9,[r12+rax*1]
        TValue *rc = RKC(i);
    36af:	41 f6 c0 80          	test   r8b,0x80
    36b3:	0f 84 4c 21 00 00    	je     5805 <luaV_execute+0x3b15>
    36b9:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    36be:	41 83 e0 7f          	and    r8d,0x7f
    36c2:	49 c1 e0 04          	shl    r8,0x4
    36c6:	4e 8d 14 00          	lea    r10,[rax+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    36ca:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
    36cf:	66 83 f8 13          	cmp    ax,0x13
    36d3:	0f 84 47 21 00 00    	je     5820 <luaV_execute+0x3b30>
  if (!ttisfloat(obj))
    36d9:	66 83 f8 03          	cmp    ax,0x3
    36dd:	75 1a                	jne    36f9 <luaV_execute+0x1a09>
    36df:	31 d2                	xor    edx,edx
    36e1:	48 8d b4 24 80 00 00 	lea    rsi,[rsp+0x80]
    36e8:	00 
    36e9:	4c 89 cf             	mov    rdi,r9
    36ec:	e8 9f cb ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    36f1:	85 c0                	test   eax,eax
    36f3:	0f 85 32 21 00 00    	jne    582b <luaV_execute+0x3b3b>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_BXOR)); }
    36f9:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    36fe:	48 89 e9             	mov    rcx,rbp
    3701:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    3706:	4c 89 d2             	mov    rdx,r10
    3709:	41 b8 0f 00 00 00    	mov    r8d,0xf
    370f:	4c 89 ce             	mov    rsi,r9
    3712:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    3716:	48 89 ef             	mov    rdi,rbp
    3719:	e8 00 00 00 00       	call   371e <luaV_execute+0x1a2e>
    371e:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    3725:	00 
    3726:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    372a:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    372e:	48 89 d8             	mov    rax,rbx
    3731:	48 83 c3 04          	add    rbx,0x4
    3735:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3738:	45 85 ed             	test   r13d,r13d
    373b:	0f 85 82 40 00 00    	jne    77c3 <luaV_execute+0x5ad3>
    3741:	44 89 c5             	mov    ebp,r8d
    3744:	41 0f b6 c0          	movzx  eax,r8b
    3748:	c1 ed 08             	shr    ebp,0x8
    374b:	48 89 c2             	mov    rdx,rax
    374e:	83 e5 7f             	and    ebp,0x7f
    3751:	48 c1 e5 04          	shl    rbp,0x4
    3755:	4c 01 e5             	add    rbp,r12
    3758:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    375c:	44 89 c0             	mov    eax,r8d
    375f:	c1 e8 18             	shr    eax,0x18
    3762:	a8 80                	test   al,0x80
    3764:	0f 85 ee 1f 00 00    	jne    5758 <luaV_execute+0x3a68>
    376a:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    376e:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    3772:	4d 8d 0c 04          	lea    r9,[r12+rax*1]
        TValue *rc = RKC(i);
    3776:	41 f6 c0 80          	test   r8b,0x80
    377a:	0f 84 f6 1f 00 00    	je     5776 <luaV_execute+0x3a86>
    3780:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    3785:	41 83 e0 7f          	and    r8d,0x7f
    3789:	49 c1 e0 04          	shl    r8,0x4
    378d:	4e 8d 14 00          	lea    r10,[rax+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    3791:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
    3796:	66 83 f8 13          	cmp    ax,0x13
    379a:	0f 84 f1 1f 00 00    	je     5791 <luaV_execute+0x3aa1>
  if (!ttisfloat(obj))
    37a0:	66 83 f8 03          	cmp    ax,0x3
    37a4:	75 17                	jne    37bd <luaV_execute+0x1acd>
    37a6:	31 d2                	xor    edx,edx
    37a8:	48 8d 74 24 70       	lea    rsi,[rsp+0x70]
    37ad:	4c 89 cf             	mov    rdi,r9
    37b0:	e8 db ca ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    37b5:	85 c0                	test   eax,eax
    37b7:	0f 85 dc 1f 00 00    	jne    5799 <luaV_execute+0x3aa9>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_BOR)); }
    37bd:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    37c2:	48 89 e9             	mov    rcx,rbp
    37c5:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    37ca:	4c 89 d2             	mov    rdx,r10
    37cd:	41 b8 0e 00 00 00    	mov    r8d,0xe
    37d3:	4c 89 ce             	mov    rsi,r9
    37d6:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    37da:	48 89 ef             	mov    rdi,rbp
    37dd:	e8 00 00 00 00       	call   37e2 <luaV_execute+0x1af2>
    37e2:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    37e9:	00 
    37ea:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    37ee:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    37f2:	48 89 d8             	mov    rax,rbx
    37f5:	48 83 c3 04          	add    rbx,0x4
    37f9:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    37fc:	45 85 ed             	test   r13d,r13d
    37ff:	0f 85 c7 40 00 00    	jne    78cc <luaV_execute+0x5bdc>
    3805:	44 89 c5             	mov    ebp,r8d
    3808:	41 0f b6 c0          	movzx  eax,r8b
    380c:	c1 ed 08             	shr    ebp,0x8
    380f:	48 89 c2             	mov    rdx,rax
    3812:	83 e5 7f             	and    ebp,0x7f
    3815:	48 c1 e5 04          	shl    rbp,0x4
    3819:	4c 01 e5             	add    rbp,r12
    381c:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        int c = GETARG_C(i);
    3820:	44 89 c0             	mov    eax,r8d
    3823:	c1 e8 10             	shr    eax,0x10
        if (n == 0) n = cast_int(L->top - ra) - 1;
    3826:	41 c1 e8 18          	shr    r8d,0x18
        int c = GETARG_C(i);
    382a:	0f b6 c0             	movzx  eax,al
        if (n == 0) n = cast_int(L->top - ra) - 1;
    382d:	75 1c                	jne    384b <luaV_execute+0x1b5b>
    382f:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    3834:	48 8b 77 10          	mov    rsi,QWORD PTR [rdi+0x10]
    3838:	49 89 f0             	mov    r8,rsi
    383b:	48 89 74 24 28       	mov    QWORD PTR [rsp+0x28],rsi
    3840:	49 29 e8             	sub    r8,rbp
    3843:	49 c1 f8 04          	sar    r8,0x4
    3847:	41 83 e8 01          	sub    r8d,0x1
        if (c == 0) {
    384b:	48 89 df             	mov    rdi,rbx
    384e:	48 89 5c 24 38       	mov    QWORD PTR [rsp+0x38],rbx
    3853:	85 c0                	test   eax,eax
    3855:	75 0e                	jne    3865 <luaV_execute+0x1b75>
          c = GETARG_Ax(*pc++);
    3857:	48 83 c7 04          	add    rdi,0x4
    385b:	8b 03                	mov    eax,DWORD PTR [rbx]
    385d:	48 89 7c 24 38       	mov    QWORD PTR [rsp+0x38],rdi
    3862:	c1 e8 08             	shr    eax,0x8
        last = ((c-1)*LFIELDS_PER_FLUSH) + n;
    3865:	44 8d 78 ff          	lea    r15d,[rax-0x1]
        h = hvalue(ra);
    3869:	4c 8b 4d 00          	mov    r9,QWORD PTR [rbp+0x0]
        savepc(L);  /* in case of allocation errors */
    386d:	48 8b 7c 24 38       	mov    rdi,QWORD PTR [rsp+0x38]
        last = ((c-1)*LFIELDS_PER_FLUSH) + n;
    3872:	45 6b ff 32          	imul   r15d,r15d,0x32
        h = hvalue(ra);
    3876:	4c 89 4c 24 30       	mov    QWORD PTR [rsp+0x30],r9
        last = ((c-1)*LFIELDS_PER_FLUSH) + n;
    387b:	43 8d 04 07          	lea    eax,[r15+r8*1]
    387f:	89 c6                	mov    esi,eax
        savepc(L);  /* in case of allocation errors */
    3881:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    3886:	48 89 78 28          	mov    QWORD PTR [rax+0x28],rdi
        if (h->ravi_array.array_type == RAVI_TTABLE) {
    388a:	41 0f b6 41 48       	movzx  eax,BYTE PTR [r9+0x48]
    388f:	3c 06                	cmp    al,0x6
    3891:	0f 84 d3 31 00 00    	je     6a6a <luaV_execute+0x4d7a>
          int i = last - n + 1;
    3897:	41 8d 5f 01          	lea    ebx,[r15+0x1]
          for (; i <= (int)last; i++) {
    389b:	39 de                	cmp    esi,ebx
    389d:	0f 8c cc 00 00 00    	jl     396f <luaV_execute+0x1c7f>
    38a3:	4c 63 fb             	movsxd r15,ebx
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    38a6:	48 8d bc 24 d0 00 00 	lea    rdi,[rsp+0xd0]
    38ad:	00 
    38ae:	44 89 6c 24 40       	mov    DWORD PTR [rsp+0x40],r13d
    38b3:	41 89 dd             	mov    r13d,ebx
    38b6:	49 c1 e7 04          	shl    r15,0x4
    38ba:	48 89 7c 24 28       	mov    QWORD PTR [rsp+0x28],rdi
    38bf:	89 f3                	mov    ebx,esi
    38c1:	4c 89 ff             	mov    rdi,r15
    38c4:	4c 89 64 24 48       	mov    QWORD PTR [rsp+0x48],r12
    38c9:	49 89 ef             	mov    r15,rbp
    38cc:	4d 89 cc             	mov    r12,r9
    38cf:	48 89 fd             	mov    rbp,rdi
    38d2:	eb 57                	jmp    392b <luaV_execute+0x1c3b>
    38d4:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
            switch (h->ravi_array.array_type) {
    38d8:	3c 04                	cmp    al,0x4
    38da:	75 3c                	jne    3918 <luaV_execute+0x1c28>
                if (ttisfloat(val)) {
    38dc:	0f b7 47 08          	movzx  eax,WORD PTR [rdi+0x8]
    38e0:	66 83 f8 03          	cmp    ax,0x3
    38e4:	0f 84 fe 32 00 00    	je     6be8 <luaV_execute+0x4ef8>
                else if (ttisinteger(val)) {
    38ea:	66 83 f8 13          	cmp    ax,0x13
    38ee:	0f 85 1c 33 00 00    	jne    6c10 <luaV_execute+0x4f20>
                  raviH_set_float_inline(L, h, u, (lua_Number)(ivalue(val)));
    38f4:	66 0f ef c0          	pxor   xmm0,xmm0
    38f8:	44 89 ea             	mov    edx,r13d
    38fb:	f2 48 0f 2a 07       	cvtsi2sd xmm0,QWORD PTR [rdi]
    3900:	45 39 6c 24 40       	cmp    DWORD PTR [r12+0x40],r13d
    3905:	0f 86 ad 33 00 00    	jbe    6cb8 <luaV_execute+0x4fc8>
    390b:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    3910:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    3915:	0f 1f 00             	nop    DWORD PTR [rax]
          for (; i <= (int)last; i++) {
    3918:	41 83 c5 01          	add    r13d,0x1
    391c:	48 83 c5 10          	add    rbp,0x10
    3920:	44 39 eb             	cmp    ebx,r13d
    3923:	7c 40                	jl     3965 <luaV_execute+0x1c75>
    3925:	41 0f b6 44 24 48    	movzx  eax,BYTE PTR [r12+0x48]
            TValue *val = ra + i;
    392b:	49 8d 3c 2f          	lea    rdi,[r15+rbp*1]
            switch (h->ravi_array.array_type) {
    392f:	3c 03                	cmp    al,0x3
    3931:	75 a5                	jne    38d8 <luaV_execute+0x1be8>
                if (ttisinteger(val)) {
    3933:	66 83 7f 08 13       	cmp    WORD PTR [rdi+0x8],0x13
    3938:	0f 85 62 32 00 00    	jne    6ba0 <luaV_execute+0x4eb0>
                  raviH_set_int_inline(L, h, u, ivalue(val));
    393e:	48 8b 0f             	mov    rcx,QWORD PTR [rdi]
    3941:	44 89 ea             	mov    edx,r13d
    3944:	45 39 6c 24 40       	cmp    DWORD PTR [r12+0x40],r13d
    3949:	0f 86 05 33 00 00    	jbe    6c54 <luaV_execute+0x4f64>
    394f:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
          for (; i <= (int)last; i++) {
    3954:	41 83 c5 01          	add    r13d,0x1
    3958:	48 83 c5 10          	add    rbp,0x10
                  raviH_set_int_inline(L, h, u, ivalue(val));
    395c:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
          for (; i <= (int)last; i++) {
    3960:	44 39 eb             	cmp    ebx,r13d
    3963:	7d c0                	jge    3925 <luaV_execute+0x1c35>
    3965:	44 8b 6c 24 40       	mov    r13d,DWORD PTR [rsp+0x40]
    396a:	4c 8b 64 24 48       	mov    r12,QWORD PTR [rsp+0x48]
        L->top = ci->top; /* correct top (in case of previous open call) */
    396f:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    3974:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    3979:	48 8b 40 08          	mov    rax,QWORD PTR [rax+0x8]
    397d:	48 89 46 10          	mov    QWORD PTR [rsi+0x10],rax
        vmbreak;
    3981:	48 8b 44 24 38       	mov    rax,QWORD PTR [rsp+0x38]
    3986:	48 8d 58 04          	lea    rbx,[rax+0x4]
    398a:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    398d:	45 85 ed             	test   r13d,r13d
    3990:	0f 85 7a 4b 00 00    	jne    8510 <luaV_execute+0x6820>
    3996:	44 89 c5             	mov    ebp,r8d
    3999:	41 0f b6 c0          	movzx  eax,r8b
    399d:	c1 ed 08             	shr    ebp,0x8
    39a0:	48 89 c2             	mov    rdx,rax
    39a3:	83 e5 7f             	and    ebp,0x7f
    39a6:	48 c1 e5 04          	shl    rbp,0x4
    39aa:	4c 01 e5             	add    rbp,r12
    39ad:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    39b1:	44 89 c0             	mov    eax,r8d
    39b4:	c1 e8 18             	shr    eax,0x18
    39b7:	a8 80                	test   al,0x80
    39b9:	0f 85 8b 2c 00 00    	jne    664a <luaV_execute+0x495a>
    39bf:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    39c3:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    39c7:	4d 8d 3c 04          	lea    r15,[r12+rax*1]
        TValue *rc = RKC(i);
    39cb:	41 f6 c0 80          	test   r8b,0x80
    39cf:	0f 84 93 2c 00 00    	je     6668 <luaV_execute+0x4978>
    39d5:	41 83 e0 7f          	and    r8d,0x7f
    39d9:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    39de:	49 c1 e0 04          	shl    r8,0x4
    39e2:	4a 8d 0c 00          	lea    rcx,[rax+r8*1]
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    39e6:	0f b7 45 08          	movzx  eax,WORD PTR [rbp+0x8]
    39ea:	66 3d 05 80          	cmp    ax,0x8005
    39ee:	0f 85 de 44 00 00    	jne    7ed2 <luaV_execute+0x61e2>
    39f4:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
    39f8:	49 8b 37             	mov    rsi,QWORD PTR [r15]
    39fb:	8b 42 0c             	mov    eax,DWORD PTR [rdx+0xc]
    39fe:	48 8d 7e ff          	lea    rdi,[rsi-0x1]
    3a02:	48 39 c7             	cmp    rdi,rax
    3a05:	0f 83 56 59 00 00    	jae    9361 <luaV_execute+0x7671>
    3a0b:	48 8b 42 10          	mov    rax,QWORD PTR [rdx+0x10]
    3a0f:	48 c1 e6 04          	shl    rsi,0x4
    3a13:	4c 8d 44 30 f0       	lea    r8,[rax+rsi*1-0x10]
    3a18:	66 41 83 78 08 00    	cmp    WORD PTR [r8+0x8],0x0
    3a1e:	0f 84 1e 37 00 00    	je     7142 <luaV_execute+0x5452>
    3a24:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    3a27:	49 89 00             	mov    QWORD PTR [r8],rax
    3a2a:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    3a2e:	66 41 89 40 08       	mov    WORD PTR [r8+0x8],ax
    3a33:	66 83 79 08 00       	cmp    WORD PTR [rcx+0x8],0x0
    3a38:	0f 88 e7 3a 00 00    	js     7525 <luaV_execute+0x5835>
        vmbreak;
    3a3e:	48 89 d8             	mov    rax,rbx
    3a41:	48 83 c3 04          	add    rbx,0x4
    3a45:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3a48:	45 85 ed             	test   r13d,r13d
    3a4b:	0f 85 4c 44 00 00    	jne    7e9d <luaV_execute+0x61ad>
    3a51:	44 89 c5             	mov    ebp,r8d
    3a54:	41 0f b6 c0          	movzx  eax,r8b
    3a58:	c1 ed 08             	shr    ebp,0x8
    3a5b:	48 89 c2             	mov    rdx,rax
    3a5e:	83 e5 7f             	and    ebp,0x7f
    3a61:	48 c1 e5 04          	shl    rbp,0x4
    3a65:	4c 01 e5             	add    rbp,r12
    3a68:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    3a6c:	44 89 c0             	mov    eax,r8d
    3a6f:	c1 e8 18             	shr    eax,0x18
    3a72:	a8 80                	test   al,0x80
    3a74:	0f 85 a1 2b 00 00    	jne    661b <luaV_execute+0x492b>
    3a7a:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3a7e:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    3a82:	4d 8d 3c 04          	lea    r15,[r12+rax*1]
        TValue *rc = RKC(i);
    3a86:	41 f6 c0 80          	test   r8b,0x80
    3a8a:	0f 84 a9 2b 00 00    	je     6639 <luaV_execute+0x4949>
    3a90:	41 83 e0 7f          	and    r8d,0x7f
    3a94:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    3a99:	49 c1 e0 04          	shl    r8,0x4
    3a9d:	4a 8d 0c 00          	lea    rcx,[rax+r8*1]
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    3aa1:	0f b7 45 08          	movzx  eax,WORD PTR [rbp+0x8]
    3aa5:	66 3d 05 80          	cmp    ax,0x8005
    3aa9:	0f 85 b1 44 00 00    	jne    7f60 <luaV_execute+0x6270>
    3aaf:	66 41 83 7f 08 13    	cmp    WORD PTR [r15+0x8],0x13
    3ab5:	0f 85 8d 33 00 00    	jne    6e48 <luaV_execute+0x5158>
    3abb:	48 8b 7d 00          	mov    rdi,QWORD PTR [rbp+0x0]
    3abf:	49 8b 37             	mov    rsi,QWORD PTR [r15]
    3ac2:	8b 47 0c             	mov    eax,DWORD PTR [rdi+0xc]
    3ac5:	48 8d 56 ff          	lea    rdx,[rsi-0x1]
    3ac9:	48 39 c2             	cmp    rdx,rax
    3acc:	0f 83 bb 5a 00 00    	jae    958d <luaV_execute+0x789d>
    3ad2:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
    3ad6:	48 c1 e6 04          	shl    rsi,0x4
    3ada:	48 8d 44 30 f0       	lea    rax,[rax+rsi*1-0x10]
    3adf:	66 83 78 08 00       	cmp    WORD PTR [rax+0x8],0x0
    3ae4:	0f 84 7f 33 00 00    	je     6e69 <luaV_execute+0x5179>
    3aea:	48 8b 11             	mov    rdx,QWORD PTR [rcx]
    3aed:	48 89 10             	mov    QWORD PTR [rax],rdx
    3af0:	0f b7 51 08          	movzx  edx,WORD PTR [rcx+0x8]
    3af4:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
    3af8:	66 83 79 08 00       	cmp    WORD PTR [rcx+0x8],0x0
    3afd:	0f 88 4b 3a 00 00    	js     754e <luaV_execute+0x585e>
        vmbreak;
    3b03:	48 89 d8             	mov    rax,rbx
    3b06:	48 83 c3 04          	add    rbx,0x4
    3b0a:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3b0d:	45 85 ed             	test   r13d,r13d
    3b10:	0f 85 47 41 00 00    	jne    7c5d <luaV_execute+0x5f6d>
    3b16:	44 89 c5             	mov    ebp,r8d
    3b19:	41 0f b6 c0          	movzx  eax,r8b
    3b1d:	c1 ed 08             	shr    ebp,0x8
    3b20:	48 89 c2             	mov    rdx,rax
    3b23:	83 e5 7f             	and    ebp,0x7f
    3b26:	48 c1 e5 04          	shl    rbp,0x4
    3b2a:	4c 01 e5             	add    rbp,r12
    3b2d:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *upval = cl->upvals[GETARG_A(i)]->v;
    3b31:	44 89 c0             	mov    eax,r8d
    3b34:	48 8b 7c 24 20       	mov    rdi,QWORD PTR [rsp+0x20]
    3b39:	c1 e8 08             	shr    eax,0x8
    3b3c:	83 e0 7f             	and    eax,0x7f
    3b3f:	48 8b 44 c7 20       	mov    rax,QWORD PTR [rdi+rax*8+0x20]
    3b44:	4c 8b 38             	mov    r15,QWORD PTR [rax]
        TValue *rb = RKB(i);
    3b47:	44 89 c0             	mov    eax,r8d
    3b4a:	c1 e8 18             	shr    eax,0x18
    3b4d:	a8 80                	test   al,0x80
    3b4f:	0f 85 24 2b 00 00    	jne    6679 <luaV_execute+0x4989>
    3b55:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3b59:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    3b5d:	49 8d 2c 04          	lea    rbp,[r12+rax*1]
        TValue *rc = RKC(i);
    3b61:	41 f6 c0 80          	test   r8b,0x80
    3b65:	0f 84 2c 2b 00 00    	je     6697 <luaV_execute+0x49a7>
    3b6b:	41 83 e0 7f          	and    r8d,0x7f
    3b6f:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    3b74:	49 c1 e0 04          	shl    r8,0x4
    3b78:	4a 8d 0c 00          	lea    rcx,[rax+r8*1]
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    3b7c:	41 0f b7 47 08       	movzx  eax,WORD PTR [r15+0x8]
    3b81:	66 3d 05 80          	cmp    ax,0x8005
    3b85:	0f 85 88 43 00 00    	jne    7f13 <luaV_execute+0x6223>
    3b8b:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    3b90:	49 8b 3f             	mov    rdi,QWORD PTR [r15]
    3b93:	0f 85 5c 32 00 00    	jne    6df5 <luaV_execute+0x5105>
    3b99:	48 8b 75 00          	mov    rsi,QWORD PTR [rbp+0x0]
    3b9d:	8b 47 0c             	mov    eax,DWORD PTR [rdi+0xc]
    3ba0:	48 8d 56 ff          	lea    rdx,[rsi-0x1]
    3ba4:	48 39 c2             	cmp    rdx,rax
    3ba7:	0f 83 b8 59 00 00    	jae    9565 <luaV_execute+0x7875>
    3bad:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
    3bb1:	48 c1 e6 04          	shl    rsi,0x4
    3bb5:	48 8d 44 30 f0       	lea    rax,[rax+rsi*1-0x10]
    3bba:	66 83 78 08 00       	cmp    WORD PTR [rax+0x8],0x0
    3bbf:	0f 84 4d 32 00 00    	je     6e12 <luaV_execute+0x5122>
    3bc5:	48 8b 11             	mov    rdx,QWORD PTR [rcx]
    3bc8:	48 89 10             	mov    QWORD PTR [rax],rdx
    3bcb:	0f b7 51 08          	movzx  edx,WORD PTR [rcx+0x8]
    3bcf:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
    3bd3:	66 83 79 08 00       	cmp    WORD PTR [rcx+0x8],0x0
    3bd8:	0f 88 1e 39 00 00    	js     74fc <luaV_execute+0x580c>
        vmbreak;
    3bde:	48 89 d8             	mov    rax,rbx
    3be1:	48 83 c3 04          	add    rbx,0x4
    3be5:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3be8:	45 85 ed             	test   r13d,r13d
    3beb:	0f 85 77 42 00 00    	jne    7e68 <luaV_execute+0x6178>
    3bf1:	44 89 c5             	mov    ebp,r8d
    3bf4:	41 0f b6 c0          	movzx  eax,r8b
    3bf8:	c1 ed 08             	shr    ebp,0x8
    3bfb:	48 89 c2             	mov    rdx,rax
    3bfe:	83 e5 7f             	and    ebp,0x7f
    3c01:	48 c1 e5 04          	shl    rbp,0x4
    3c05:	4c 01 e5             	add    rbp,r12
    3c08:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        StkId rb = RB(i);                              /* table */
    3c0c:	45 89 c7             	mov    r15d,r8d
        TValue *rc = RKC(i);                           /* key */
    3c0f:	41 c1 e8 10          	shr    r8d,0x10
        StkId rb = RB(i);                              /* table */
    3c13:	41 c1 ef 18          	shr    r15d,0x18
    3c17:	49 c1 e7 04          	shl    r15,0x4
    3c1b:	4d 01 e7             	add    r15,r12
        TValue *rc = RKC(i);                           /* key */
    3c1e:	41 f6 c0 80          	test   r8b,0x80
    3c22:	0f 85 27 28 00 00    	jne    644f <luaV_execute+0x475f>
    3c28:	45 0f b6 c0          	movzx  r8d,r8b
    3c2c:	49 c1 e0 04          	shl    r8,0x4
    3c30:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    3c34:	41 0f b7 47 08       	movzx  eax,WORD PTR [r15+0x8]
    3c39:	66 3d 05 80          	cmp    ax,0x8005
    3c3d:	0f 85 dd 44 00 00    	jne    8120 <luaV_execute+0x6430>
    3c43:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    3c48:	0f 85 8d 31 00 00    	jne    6ddb <luaV_execute+0x50eb>
    3c4e:	49 8b 3f             	mov    rdi,QWORD PTR [r15]
    3c51:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    3c54:	8b 47 0c             	mov    eax,DWORD PTR [rdi+0xc]
    3c57:	48 8d 4e ff          	lea    rcx,[rsi-0x1]
    3c5b:	48 39 c1             	cmp    rcx,rax
    3c5e:	0f 83 ed 58 00 00    	jae    9551 <luaV_execute+0x7861>
    3c64:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
    3c68:	48 c1 e6 04          	shl    rsi,0x4
    3c6c:	48 8d 44 30 f0       	lea    rax,[rax+rsi*1-0x10]
    3c71:	0f b7 48 08          	movzx  ecx,WORD PTR [rax+0x8]
    3c75:	66 85 c9             	test   cx,cx
    3c78:	0f 84 ad 56 00 00    	je     932b <luaV_execute+0x763b>
    3c7e:	48 8b 00             	mov    rax,QWORD PTR [rax]
    3c81:	66 89 4d 08          	mov    WORD PTR [rbp+0x8],cx
    3c85:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        vmbreak;
    3c89:	48 89 d8             	mov    rax,rbx
    3c8c:	48 83 c3 04          	add    rbx,0x4
    3c90:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3c93:	45 85 ed             	test   r13d,r13d
    3c96:	0f 85 4f 44 00 00    	jne    80eb <luaV_execute+0x63fb>
    3c9c:	44 89 c5             	mov    ebp,r8d
    3c9f:	41 0f b6 c0          	movzx  eax,r8b
    3ca3:	c1 ed 08             	shr    ebp,0x8
    3ca6:	48 89 c2             	mov    rdx,rax
    3ca9:	83 e5 7f             	and    ebp,0x7f
    3cac:	48 c1 e5 04          	shl    rbp,0x4
    3cb0:	4c 01 e5             	add    rbp,r12
    3cb3:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *upval = cl->upvals[GETARG_B(i)]->v;    /* table */
    3cb7:	44 89 c0             	mov    eax,r8d
    3cba:	48 8b 4c 24 20       	mov    rcx,QWORD PTR [rsp+0x20]
        TValue *rc = RKC(i);                           /* key */
    3cbf:	41 c1 e8 10          	shr    r8d,0x10
        TValue *upval = cl->upvals[GETARG_B(i)]->v;    /* table */
    3cc3:	c1 e8 18             	shr    eax,0x18
    3cc6:	48 8b 44 c1 20       	mov    rax,QWORD PTR [rcx+rax*8+0x20]
    3ccb:	4c 8b 38             	mov    r15,QWORD PTR [rax]
        TValue *rc = RKC(i);                           /* key */
    3cce:	41 f6 c0 80          	test   r8b,0x80
    3cd2:	0f 85 61 27 00 00    	jne    6439 <luaV_execute+0x4749>
    3cd8:	45 0f b6 c0          	movzx  r8d,r8b
    3cdc:	49 c1 e0 04          	shl    r8,0x4
    3ce0:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    3ce4:	41 0f b7 47 08       	movzx  eax,WORD PTR [r15+0x8]
    3ce9:	66 3d 05 80          	cmp    ax,0x8005
    3ced:	0f 85 ca 46 00 00    	jne    83bd <luaV_execute+0x66cd>
    3cf3:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    3cf8:	49 8b 3f             	mov    rdi,QWORD PTR [r15]
    3cfb:	0f 85 c3 30 00 00    	jne    6dc4 <luaV_execute+0x50d4>
    3d01:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    3d04:	8b 47 0c             	mov    eax,DWORD PTR [rdi+0xc]
    3d07:	48 8d 4e ff          	lea    rcx,[rsi-0x1]
    3d0b:	48 39 c1             	cmp    rcx,rax
    3d0e:	0f 83 65 58 00 00    	jae    9579 <luaV_execute+0x7889>
    3d14:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
    3d18:	48 c1 e6 04          	shl    rsi,0x4
    3d1c:	48 8d 44 30 f0       	lea    rax,[rax+rsi*1-0x10]
    3d21:	0f b7 48 08          	movzx  ecx,WORD PTR [rax+0x8]
    3d25:	66 85 c9             	test   cx,cx
    3d28:	0f 84 c7 55 00 00    	je     92f5 <luaV_execute+0x7605>
    3d2e:	48 8b 00             	mov    rax,QWORD PTR [rax]
    3d31:	66 89 4d 08          	mov    WORD PTR [rbp+0x8],cx
    3d35:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        vmbreak;
    3d39:	48 89 d8             	mov    rax,rbx
    3d3c:	48 83 c3 04          	add    rbx,0x4
    3d40:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3d43:	45 85 ed             	test   r13d,r13d
    3d46:	0f 85 1e 44 00 00    	jne    816a <luaV_execute+0x647a>
    3d4c:	44 89 c5             	mov    ebp,r8d
    3d4f:	41 0f b6 c0          	movzx  eax,r8b
    3d53:	c1 ed 08             	shr    ebp,0x8
    3d56:	48 89 c2             	mov    rdx,rax
    3d59:	83 e5 7f             	and    ebp,0x7f
    3d5c:	48 c1 e5 04          	shl    rbp,0x4
    3d60:	4c 01 e5             	add    rbp,r12
    3d63:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        UpVal *uv = cl->upvals[GETARG_B(i)];
    3d67:	41 c1 e8 18          	shr    r8d,0x18
    3d6b:	48 8b 7c 24 20       	mov    rdi,QWORD PTR [rsp+0x20]
        setobj(L, uv->v, ra);
    3d70:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
        UpVal *uv = cl->upvals[GETARG_B(i)];
    3d74:	49 63 c0             	movsxd rax,r8d
    3d77:	48 8b 74 c7 20       	mov    rsi,QWORD PTR [rdi+rax*8+0x20]
        setobj(L, uv->v, ra);
    3d7c:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    3d7f:	48 89 10             	mov    QWORD PTR [rax],rdx
    3d82:	0f b7 55 08          	movzx  edx,WORD PTR [rbp+0x8]
    3d86:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
        luaC_upvalbarrier(L, uv);
    3d8a:	66 85 d2             	test   dx,dx
    3d8d:	0f 88 82 2b 00 00    	js     6915 <luaV_execute+0x4c25>
        vmbreak;
    3d93:	48 89 d8             	mov    rax,rbx
    3d96:	48 83 c3 04          	add    rbx,0x4
    3d9a:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3d9d:	45 85 ed             	test   r13d,r13d
    3da0:	0f 85 75 4d 00 00    	jne    8b1b <luaV_execute+0x6e2b>
    3da6:	44 89 c5             	mov    ebp,r8d
    3da9:	41 0f b6 c0          	movzx  eax,r8b
    3dad:	c1 ed 08             	shr    ebp,0x8
    3db0:	48 89 c2             	mov    rdx,rax
    3db3:	83 e5 7f             	and    ebp,0x7f
    3db6:	48 c1 e5 04          	shl    rbp,0x4
    3dba:	4c 01 e5             	add    rbp,r12
    3dbd:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    3dc1:	44 89 c0             	mov    eax,r8d
    3dc4:	c1 e8 18             	shr    eax,0x18
    3dc7:	a8 80                	test   al,0x80
    3dc9:	0f 85 db 25 00 00    	jne    63aa <luaV_execute+0x46ba>
    3dcf:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3dd3:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    3dd7:	4d 8d 0c 04          	lea    r9,[r12+rax*1]
        TValue *rc = RKC(i);
    3ddb:	41 f6 c0 80          	test   r8b,0x80
    3ddf:	0f 84 e3 25 00 00    	je     63c8 <luaV_execute+0x46d8>
    3de5:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    3dea:	41 83 e0 7f          	and    r8d,0x7f
    3dee:	49 c1 e0 04          	shl    r8,0x4
    3df2:	4e 8d 14 00          	lea    r10,[rax+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    3df6:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
    3dfb:	66 83 f8 13          	cmp    ax,0x13
    3dff:	0f 84 de 25 00 00    	je     63e3 <luaV_execute+0x46f3>
  if (!ttisfloat(obj))
    3e05:	66 83 f8 03          	cmp    ax,0x3
    3e09:	75 17                	jne    3e22 <luaV_execute+0x2132>
    3e0b:	31 d2                	xor    edx,edx
    3e0d:	48 8d 74 24 60       	lea    rsi,[rsp+0x60]
    3e12:	4c 89 cf             	mov    rdi,r9
    3e15:	e8 76 c4 ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    3e1a:	85 c0                	test   eax,eax
    3e1c:	0f 85 c9 25 00 00    	jne    63eb <luaV_execute+0x46fb>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_BAND)); }
    3e22:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    3e27:	48 89 e9             	mov    rcx,rbp
    3e2a:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    3e2f:	4c 89 d2             	mov    rdx,r10
    3e32:	41 b8 0d 00 00 00    	mov    r8d,0xd
    3e38:	4c 89 ce             	mov    rsi,r9
    3e3b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    3e3f:	48 89 ef             	mov    rdi,rbp
    3e42:	e8 00 00 00 00       	call   3e47 <luaV_execute+0x2157>
    3e47:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    3e4e:	00 
    3e4f:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    3e53:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    3e57:	48 89 d8             	mov    rax,rbx
    3e5a:	48 83 c3 04          	add    rbx,0x4
    3e5e:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3e61:	45 85 ed             	test   r13d,r13d
    3e64:	0f 85 c3 39 00 00    	jne    782d <luaV_execute+0x5b3d>
    3e6a:	44 89 c5             	mov    ebp,r8d
    3e6d:	41 0f b6 c0          	movzx  eax,r8b
    3e71:	c1 ed 08             	shr    ebp,0x8
    3e74:	48 89 c2             	mov    rdx,rax
    3e77:	83 e5 7f             	and    ebp,0x7f
    3e7a:	48 c1 e5 04          	shl    rbp,0x4
    3e7e:	4c 01 e5             	add    rbp,r12
    3e81:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    3e85:	44 89 c0             	mov    eax,r8d
    3e88:	c1 e8 18             	shr    eax,0x18
    3e8b:	a8 80                	test   al,0x80
    3e8d:	0f 85 a1 24 00 00    	jne    6334 <luaV_execute+0x4644>
    3e93:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3e97:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    3e9b:	49 8d 34 04          	lea    rsi,[r12+rax*1]
        TValue *rc = RKC(i);
    3e9f:	41 f6 c0 80          	test   r8b,0x80
    3ea3:	0f 84 a9 24 00 00    	je     6352 <luaV_execute+0x4662>
    3ea9:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    3eae:	41 83 e0 7f          	and    r8d,0x7f
    3eb2:	49 c1 e0 04          	shl    r8,0x4
    3eb6:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    3eba:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    3ebe:	66 83 f8 03          	cmp    ax,0x3
    3ec2:	0f 84 a4 24 00 00    	je     636c <luaV_execute+0x467c>
    3ec8:	66 83 f8 13          	cmp    ax,0x13
    3ecc:	0f 84 f6 36 00 00    	je     75c8 <luaV_execute+0x58d8>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_DIV)); }
    3ed2:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    3ed7:	48 89 e9             	mov    rcx,rbp
    3eda:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    3edf:	41 b8 0b 00 00 00    	mov    r8d,0xb
    3ee5:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    3ee9:	48 89 ef             	mov    rdi,rbp
    3eec:	e8 00 00 00 00       	call   3ef1 <luaV_execute+0x2201>
    3ef1:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    3ef8:	00 
    3ef9:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    3efd:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    3f01:	48 89 d8             	mov    rax,rbx
    3f04:	48 83 c3 04          	add    rbx,0x4
    3f08:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3f0b:	45 85 ed             	test   r13d,r13d
    3f0e:	0f 85 80 3e 00 00    	jne    7d94 <luaV_execute+0x60a4>
    3f14:	44 89 c5             	mov    ebp,r8d
    3f17:	41 0f b6 c0          	movzx  eax,r8b
    3f1b:	c1 ed 08             	shr    ebp,0x8
    3f1e:	48 89 c2             	mov    rdx,rax
    3f21:	83 e5 7f             	and    ebp,0x7f
    3f24:	48 c1 e5 04          	shl    rbp,0x4
    3f28:	4c 01 e5             	add    rbp,r12
    3f2b:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    3f2f:	44 89 c0             	mov    eax,r8d
    3f32:	c1 e8 18             	shr    eax,0x18
    3f35:	a8 80                	test   al,0x80
    3f37:	0f 85 93 23 00 00    	jne    62d0 <luaV_execute+0x45e0>
    3f3d:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3f41:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    3f45:	49 8d 34 04          	lea    rsi,[r12+rax*1]
        TValue *rc = RKC(i);
    3f49:	41 f6 c0 80          	test   r8b,0x80
    3f4d:	0f 84 9a 23 00 00    	je     62ed <luaV_execute+0x45fd>
    3f53:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    3f58:	41 83 e0 7f          	and    r8d,0x7f
    3f5c:	49 c1 e0 04          	shl    r8,0x4
    3f60:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    3f64:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    3f68:	66 83 f8 13          	cmp    ax,0x13
    3f6c:	0f 84 95 23 00 00    	je     6307 <luaV_execute+0x4617>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    3f72:	66 83 f8 03          	cmp    ax,0x3
    3f76:	0f 84 35 32 00 00    	je     71b1 <luaV_execute+0x54c1>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_MUL)); }
    3f7c:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    3f81:	48 89 e9             	mov    rcx,rbp
    3f84:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    3f89:	41 b8 08 00 00 00    	mov    r8d,0x8
    3f8f:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    3f93:	48 89 ef             	mov    rdi,rbp
    3f96:	e8 00 00 00 00       	call   3f9b <luaV_execute+0x22ab>
    3f9b:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    3fa2:	00 
    3fa3:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    3fa7:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    3fab:	48 89 d8             	mov    rax,rbx
    3fae:	48 83 c3 04          	add    rbx,0x4
    3fb2:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    3fb5:	45 85 ed             	test   r13d,r13d
    3fb8:	0f 85 49 44 00 00    	jne    8407 <luaV_execute+0x6717>
    3fbe:	44 89 c5             	mov    ebp,r8d
    3fc1:	41 0f b6 c0          	movzx  eax,r8b
    3fc5:	c1 ed 08             	shr    ebp,0x8
    3fc8:	48 89 c2             	mov    rdx,rax
    3fcb:	83 e5 7f             	and    ebp,0x7f
    3fce:	48 c1 e5 04          	shl    rbp,0x4
    3fd2:	4c 01 e5             	add    rbp,r12
    3fd5:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    3fd9:	44 89 c0             	mov    eax,r8d
    3fdc:	c1 e8 18             	shr    eax,0x18
    3fdf:	a8 80                	test   al,0x80
    3fe1:	0f 85 84 22 00 00    	jne    626b <luaV_execute+0x457b>
    3fe7:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    3feb:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    3fef:	49 8d 34 04          	lea    rsi,[r12+rax*1]
        TValue *rc = RKC(i);
    3ff3:	41 f6 c0 80          	test   r8b,0x80
    3ff7:	0f 84 8c 22 00 00    	je     6289 <luaV_execute+0x4599>
    3ffd:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    4002:	41 83 e0 7f          	and    r8d,0x7f
    4006:	49 c1 e0 04          	shl    r8,0x4
    400a:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    400e:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    4012:	66 83 f8 13          	cmp    ax,0x13
    4016:	0f 84 87 22 00 00    	je     62a3 <luaV_execute+0x45b3>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    401c:	66 83 f8 03          	cmp    ax,0x3
    4020:	0f 84 4f 31 00 00    	je     7175 <luaV_execute+0x5485>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_SUB)); }
    4026:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    402b:	48 89 e9             	mov    rcx,rbp
    402e:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    4033:	41 b8 07 00 00 00    	mov    r8d,0x7
    4039:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    403d:	48 89 ef             	mov    rdi,rbp
    4040:	e8 00 00 00 00       	call   4045 <luaV_execute+0x2355>
    4045:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    404c:	00 
    404d:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    4051:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    4055:	48 89 d8             	mov    rax,rbx
    4058:	48 83 c3 04          	add    rbx,0x4
    405c:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    405f:	45 85 ed             	test   r13d,r13d
    4062:	0f 85 d4 43 00 00    	jne    843c <luaV_execute+0x674c>
    4068:	44 89 c5             	mov    ebp,r8d
    406b:	41 0f b6 c0          	movzx  eax,r8b
    406f:	c1 ed 08             	shr    ebp,0x8
    4072:	48 89 c2             	mov    rdx,rax
    4075:	83 e5 7f             	and    ebp,0x7f
    4078:	48 c1 e5 04          	shl    rbp,0x4
    407c:	4c 01 e5             	add    rbp,r12
    407f:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4083:	44 89 c0             	mov    eax,r8d
    4086:	c1 e8 18             	shr    eax,0x18
    4089:	a8 80                	test   al,0x80
    408b:	0f 85 75 21 00 00    	jne    6206 <luaV_execute+0x4516>
    4091:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4095:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4099:	49 8d 34 04          	lea    rsi,[r12+rax*1]
        TValue *rc = RKC(i);
    409d:	41 f6 c0 80          	test   r8b,0x80
    40a1:	0f 84 7d 21 00 00    	je     6224 <luaV_execute+0x4534>
    40a7:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    40ac:	41 83 e0 7f          	and    r8d,0x7f
    40b0:	49 c1 e0 04          	shl    r8,0x4
    40b4:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    40b8:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    40bc:	66 83 f8 13          	cmp    ax,0x13
    40c0:	0f 84 78 21 00 00    	je     623e <luaV_execute+0x454e>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    40c6:	66 83 f8 03          	cmp    ax,0x3
    40ca:	0f 84 1d 31 00 00    	je     71ed <luaV_execute+0x54fd>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_ADD)); }
    40d0:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    40d5:	48 89 e9             	mov    rcx,rbp
    40d8:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    40dd:	41 b8 06 00 00 00    	mov    r8d,0x6
    40e3:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    40e7:	48 89 ef             	mov    rdi,rbp
    40ea:	e8 00 00 00 00       	call   40ef <luaV_execute+0x23ff>
    40ef:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    40f6:	00 
    40f7:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    40fb:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    40ff:	48 89 d8             	mov    rax,rbx
    4102:	48 83 c3 04          	add    rbx,0x4
    4106:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4109:	45 85 ed             	test   r13d,r13d
    410c:	0f 85 5f 43 00 00    	jne    8471 <luaV_execute+0x6781>
    4112:	44 89 c5             	mov    ebp,r8d
    4115:	41 0f b6 c0          	movzx  eax,r8b
    4119:	c1 ed 08             	shr    ebp,0x8
    411c:	48 89 c2             	mov    rdx,rax
    411f:	83 e5 7f             	and    ebp,0x7f
    4122:	48 c1 e5 04          	shl    rbp,0x4
    4126:	4c 01 e5             	add    rbp,r12
    4129:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        StkId rb = RB(i);
    412d:	45 89 c7             	mov    r15d,r8d
        TValue *rc = RKC(i);
    4130:	41 c1 e8 10          	shr    r8d,0x10
        StkId rb = RB(i);
    4134:	41 c1 ef 18          	shr    r15d,0x18
    4138:	49 c1 e7 04          	shl    r15,0x4
    413c:	4d 01 e7             	add    r15,r12
        TValue *rc = RKC(i);
    413f:	41 f6 c0 80          	test   r8b,0x80
    4143:	0f 85 a7 20 00 00    	jne    61f0 <luaV_execute+0x4500>
    4149:	45 0f b6 c0          	movzx  r8d,r8b
    414d:	49 c1 e0 04          	shl    r8,0x4
    4151:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        setobjs2s(L, ra + 1, rb);
    4155:	49 8b 07             	mov    rax,QWORD PTR [r15]
        TString *key = tsvalue(rc);  /* key must be a string */
    4158:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
        if (luaV_fastget(L, rb, key, aux, luaH_getstr)) {
    415b:	45 31 c0             	xor    r8d,r8d
        setobjs2s(L, ra + 1, rb);
    415e:	48 89 45 10          	mov    QWORD PTR [rbp+0x10],rax
    4162:	41 0f b7 47 08       	movzx  eax,WORD PTR [r15+0x8]
    4167:	66 89 45 18          	mov    WORD PTR [rbp+0x18],ax
        if (luaV_fastget(L, rb, key, aux, luaH_getstr)) {
    416b:	41 0f b7 4f 08       	movzx  ecx,WORD PTR [r15+0x8]
    4170:	83 e1 0f             	and    ecx,0xf
    4173:	66 83 f9 05          	cmp    cx,0x5
    4177:	0f 84 5c 2d 00 00    	je     6ed9 <luaV_execute+0x51e9>
        else Protect(luaV_finishget(L, rb, rc, ra, aux));
    417d:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    4182:	4c 89 fe             	mov    rsi,r15
    4185:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    418a:	48 89 e9             	mov    rcx,rbp
    418d:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    4192:	4c 89 ff             	mov    rdi,r15
    4195:	e8 00 00 00 00       	call   419a <luaV_execute+0x24aa>
    419a:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    41a1:	00 
    41a2:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    41a7:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    41ab:	48 89 d8             	mov    rax,rbx
    41ae:	48 83 c3 04          	add    rbx,0x4
    41b2:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    41b5:	45 85 ed             	test   r13d,r13d
    41b8:	0f 85 e8 42 00 00    	jne    84a6 <luaV_execute+0x67b6>
    41be:	44 89 c5             	mov    ebp,r8d
    41c1:	41 0f b6 c0          	movzx  eax,r8b
    41c5:	c1 ed 08             	shr    ebp,0x8
    41c8:	48 89 c2             	mov    rdx,rax
    41cb:	83 e5 7f             	and    ebp,0x7f
    41ce:	48 c1 e5 04          	shl    rbp,0x4
    41d2:	4c 01 e5             	add    rbp,r12
    41d5:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        StkId rb = RB(i); /* variable - may not be a table */
    41d9:	44 89 c6             	mov    esi,r8d
        TValue *rc = RKC(i);
    41dc:	41 c1 e8 10          	shr    r8d,0x10
        StkId rb = RB(i); /* variable - may not be a table */
    41e0:	c1 ee 18             	shr    esi,0x18
    41e3:	48 c1 e6 04          	shl    rsi,0x4
    41e7:	4c 01 e6             	add    rsi,r12
        TValue *rc = RKC(i);
    41ea:	41 f6 c0 80          	test   r8b,0x80
    41ee:	0f 85 e6 1f 00 00    	jne    61da <luaV_execute+0x44ea>
    41f4:	45 0f b6 c0          	movzx  r8d,r8b
    41f8:	49 c1 e0 04          	shl    r8,0x4
    41fc:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        setobjs2s(L, ra + 1, rb);
    4200:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    4203:	48 89 45 10          	mov    QWORD PTR [rbp+0x10],rax
    4207:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    420b:	66 89 45 18          	mov    WORD PTR [rbp+0x18],ax
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    420f:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    4213:	66 3d 05 80          	cmp    ax,0x8005
    4217:	0f 85 90 43 00 00    	jne    85ad <luaV_execute+0x68bd>
    421d:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    4220:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
  Node *n = hashstr(t, key);
    4223:	44 8b 41 0c          	mov    r8d,DWORD PTR [rcx+0xc]
    4227:	44 23 40 50          	and    r8d,DWORD PTR [rax+0x50]
    422b:	49 c1 e0 05          	shl    r8,0x5
    422f:	4c 03 40 18          	add    r8,QWORD PTR [rax+0x18]
    4233:	eb 16                	jmp    424b <luaV_execute+0x255b>
    4235:	0f 1f 00             	nop    DWORD PTR [rax]
      int nx = gnext(n);
    4238:	49 63 40 1c          	movsxd rax,DWORD PTR [r8+0x1c]
      if (nx == 0)
    423c:	85 c0                	test   eax,eax
    423e:	0f 84 38 2a 00 00    	je     6c7c <luaV_execute+0x4f8c>
      n += nx;
    4244:	48 c1 e0 05          	shl    rax,0x5
    4248:	49 01 c0             	add    r8,rax
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
    424b:	66 41 81 78 18 04 80 	cmp    WORD PTR [r8+0x18],0x8004
    4252:	75 e4                	jne    4238 <luaV_execute+0x2548>
    4254:	49 3b 48 10          	cmp    rcx,QWORD PTR [r8+0x10]
    4258:	75 de                	jne    4238 <luaV_execute+0x2548>
    425a:	41 0f b7 40 08       	movzx  eax,WORD PTR [r8+0x8]
    425f:	66 85 c0             	test   ax,ax
    4262:	0f 84 15 50 00 00    	je     927d <luaV_execute+0x758d>
    4268:	49 8b 10             	mov    rdx,QWORD PTR [r8]
    426b:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    426f:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    4273:	48 89 d8             	mov    rax,rbx
    4276:	48 83 c3 04          	add    rbx,0x4
    427a:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    427d:	45 85 ed             	test   r13d,r13d
    4280:	0f 85 72 35 00 00    	jne    77f8 <luaV_execute+0x5b08>
    4286:	44 89 c5             	mov    ebp,r8d
    4289:	41 0f b6 c0          	movzx  eax,r8b
    428d:	c1 ed 08             	shr    ebp,0x8
    4290:	48 89 c2             	mov    rdx,rax
    4293:	83 e5 7f             	and    ebp,0x7f
    4296:	48 c1 e5 04          	shl    rbp,0x4
    429a:	4c 01 e5             	add    rbp,r12
    429d:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        savepc(L);  /* in case of allocation errors */
    42a1:	48 8b 74 24 18       	mov    rsi,QWORD PTR [rsp+0x18]
        int b = GETARG_B(i);
    42a6:	44 89 c0             	mov    eax,r8d
        t = luaH_new(L);
    42a9:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
        int c = GETARG_C(i);
    42ae:	41 c1 e8 10          	shr    r8d,0x10
        int b = GETARG_B(i);
    42b2:	c1 e8 18             	shr    eax,0x18
        int c = GETARG_C(i);
    42b5:	45 0f b6 f8          	movzx  r15d,r8b
        savepc(L);  /* in case of allocation errors */
    42b9:	48 89 5e 28          	mov    QWORD PTR [rsi+0x28],rbx
        int b = GETARG_B(i);
    42bd:	89 44 24 28          	mov    DWORD PTR [rsp+0x28],eax
        t = luaH_new(L);
    42c1:	e8 00 00 00 00       	call   42c6 <luaV_execute+0x25d6>
        sethvalue(L, ra, t);
    42c6:	ba 05 80 ff ff       	mov    edx,0xffff8005
        t = luaH_new(L);
    42cb:	48 89 c6             	mov    rsi,rax
        sethvalue(L, ra, t);
    42ce:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        if (b != 0 || c != 0)
    42d2:	8b 44 24 28          	mov    eax,DWORD PTR [rsp+0x28]
        sethvalue(L, ra, t);
    42d6:	66 89 55 08          	mov    WORD PTR [rbp+0x8],dx
        if (b != 0 || c != 0)
    42da:	44 09 f8             	or     eax,r15d
    42dd:	0f 85 89 28 00 00    	jne    6b6c <luaV_execute+0x4e7c>
        checkGC(L, ra + 1);
    42e3:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    42e8:	49 8b 47 18          	mov    rax,QWORD PTR [r15+0x18]
    42ec:	4c 89 ff             	mov    rdi,r15
    42ef:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    42f4:	0f 8f b5 25 00 00    	jg     68af <luaV_execute+0x4bbf>
        vmbreak;
    42fa:	48 89 d8             	mov    rax,rbx
    42fd:	48 83 c3 04          	add    rbx,0x4
    4301:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4304:	45 85 ed             	test   r13d,r13d
    4307:	0f 85 ac 4a 00 00    	jne    8db9 <luaV_execute+0x70c9>
    430d:	44 89 c5             	mov    ebp,r8d
    4310:	41 0f b6 c0          	movzx  eax,r8b
    4314:	c1 ed 08             	shr    ebp,0x8
    4317:	48 89 c2             	mov    rdx,rax
    431a:	83 e5 7f             	and    ebp,0x7f
    431d:	48 c1 e5 04          	shl    rbp,0x4
    4321:	4c 01 e5             	add    rbp,r12
    4324:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        int b = GETARG_B(i);
    4328:	41 c1 e8 18          	shr    r8d,0x18
        setobj2s(L, ra, cl->upvals[b]->v);
    432c:	48 8b 4c 24 20       	mov    rcx,QWORD PTR [rsp+0x20]
    4331:	49 63 c0             	movsxd rax,r8d
    4334:	48 8b 44 c1 20       	mov    rax,QWORD PTR [rcx+rax*8+0x20]
    4339:	48 8b 00             	mov    rax,QWORD PTR [rax]
    433c:	48 8b 10             	mov    rdx,QWORD PTR [rax]
    433f:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
    4343:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    4347:	48 89 d8             	mov    rax,rbx
    434a:	48 83 c3 04          	add    rbx,0x4
        setobj2s(L, ra, cl->upvals[b]->v);
    434e:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    4352:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4355:	45 85 ed             	test   r13d,r13d
    4358:	0f 85 90 4a 00 00    	jne    8dee <luaV_execute+0x70fe>
    435e:	44 89 c5             	mov    ebp,r8d
    4361:	41 0f b6 c0          	movzx  eax,r8b
    4365:	c1 ed 08             	shr    ebp,0x8
    4368:	48 89 c2             	mov    rdx,rax
    436b:	83 e5 7f             	and    ebp,0x7f
    436e:	48 c1 e5 04          	shl    rbp,0x4
    4372:	4c 01 e5             	add    rbp,r12
    4375:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        int b = GETARG_B(i);
    4379:	41 c1 e8 18          	shr    r8d,0x18
    437d:	0f 1f 00             	nop    DWORD PTR [rax]
          setnilvalue(ra++);
    4380:	45 31 c9             	xor    r9d,r9d
        } while (b--);
    4383:	41 83 e8 01          	sub    r8d,0x1
          setnilvalue(ra++);
    4387:	48 83 c5 10          	add    rbp,0x10
    438b:	66 44 89 4d f8       	mov    WORD PTR [rbp-0x8],r9w
        } while (b--);
    4390:	41 83 f8 ff          	cmp    r8d,0xffffffff
    4394:	75 ea                	jne    4380 <luaV_execute+0x2690>
        vmbreak;
    4396:	48 89 d8             	mov    rax,rbx
    4399:	48 83 c3 04          	add    rbx,0x4
    439d:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    43a0:	45 85 ed             	test   r13d,r13d
    43a3:	0f 85 7a 4a 00 00    	jne    8e23 <luaV_execute+0x7133>
    43a9:	44 89 c5             	mov    ebp,r8d
    43ac:	41 0f b6 c0          	movzx  eax,r8b
    43b0:	c1 ed 08             	shr    ebp,0x8
    43b3:	48 89 c2             	mov    rdx,rax
    43b6:	83 e5 7f             	and    ebp,0x7f
    43b9:	48 c1 e5 04          	shl    rbp,0x4
    43bd:	4c 01 e5             	add    rbp,r12
    43c0:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        setbvalue(ra, GETARG_B(i));
    43c4:	44 89 c0             	mov    eax,r8d
    43c7:	41 ba 01 00 00 00    	mov    r10d,0x1
    43cd:	c1 e8 18             	shr    eax,0x18
        if (GETARG_C(i)) pc++;  /* skip next instruction (if C) */
    43d0:	41 81 e0 00 00 ff 00 	and    r8d,0xff0000
        setbvalue(ra, GETARG_B(i));
    43d7:	66 44 89 55 08       	mov    WORD PTR [rbp+0x8],r10w
    43dc:	89 45 00             	mov    DWORD PTR [rbp+0x0],eax
        if (GETARG_C(i)) pc++;  /* skip next instruction (if C) */
    43df:	48 8d 43 04          	lea    rax,[rbx+0x4]
    43e3:	48 0f 44 c3          	cmove  rax,rbx
        vmbreak;
    43e7:	48 8d 58 04          	lea    rbx,[rax+0x4]
    43eb:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    43ee:	45 85 ed             	test   r13d,r13d
    43f1:	0f 85 61 4a 00 00    	jne    8e58 <luaV_execute+0x7168>
    43f7:	44 89 c5             	mov    ebp,r8d
    43fa:	41 0f b6 c0          	movzx  eax,r8b
    43fe:	c1 ed 08             	shr    ebp,0x8
    4401:	48 89 c2             	mov    rdx,rax
    4404:	83 e5 7f             	and    ebp,0x7f
    4407:	48 c1 e5 04          	shl    rbp,0x4
    440b:	4c 01 e5             	add    rbp,r12
    440e:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        rb = k + GETARG_Ax(*pc++);
    4412:	8b 03                	mov    eax,DWORD PTR [rbx]
    4414:	48 89 da             	mov    rdx,rbx
        vmbreak;
    4417:	48 83 c3 08          	add    rbx,0x8
        rb = k + GETARG_Ax(*pc++);
    441b:	c1 e8 08             	shr    eax,0x8
    441e:	48 c1 e0 04          	shl    rax,0x4
    4422:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        setobj2s(L, ra, rb);
    4427:	48 8b 08             	mov    rcx,QWORD PTR [rax]
    442a:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
    442e:	48 89 4d 00          	mov    QWORD PTR [rbp+0x0],rcx
        vmbreak;
    4432:	44 8b 42 04          	mov    r8d,DWORD PTR [rdx+0x4]
        setobj2s(L, ra, rb);
    4436:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    443a:	45 85 ed             	test   r13d,r13d
    443d:	0f 85 03 45 00 00    	jne    8946 <luaV_execute+0x6c56>
    4443:	44 89 c5             	mov    ebp,r8d
    4446:	41 0f b6 c0          	movzx  eax,r8b
    444a:	c1 ed 08             	shr    ebp,0x8
    444d:	48 89 c2             	mov    rdx,rax
    4450:	83 e5 7f             	and    ebp,0x7f
    4453:	48 c1 e5 04          	shl    rbp,0x4
    4457:	4c 01 e5             	add    rbp,r12
    445a:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = k + GETARG_Bx(i);
    445e:	41 c1 e8 10          	shr    r8d,0x10
    4462:	44 89 c0             	mov    eax,r8d
    4465:	48 c1 e0 04          	shl    rax,0x4
    4469:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        setobj2s(L, ra, rb);
    446e:	48 8b 10             	mov    rdx,QWORD PTR [rax]
    4471:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
    4475:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    4479:	48 89 d8             	mov    rax,rbx
    447c:	48 83 c3 04          	add    rbx,0x4
        setobj2s(L, ra, rb);
    4480:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    4484:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4487:	45 85 ed             	test   r13d,r13d
    448a:	0f 85 eb 44 00 00    	jne    897b <luaV_execute+0x6c8b>
    4490:	44 89 c5             	mov    ebp,r8d
    4493:	41 0f b6 c0          	movzx  eax,r8b
    4497:	c1 ed 08             	shr    ebp,0x8
    449a:	48 89 c2             	mov    rdx,rax
    449d:	83 e5 7f             	and    ebp,0x7f
    44a0:	48 c1 e5 04          	shl    rbp,0x4
    44a4:	4c 01 e5             	add    rbp,r12
    44a7:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        setobjs2s(L, ra, RB(i));
    44ab:	41 c1 e8 18          	shr    r8d,0x18
        vmbreak;
    44af:	4c 8d 7b 04          	lea    r15,[rbx+0x4]
        setobjs2s(L, ra, RB(i));
    44b3:	44 89 c0             	mov    eax,r8d
    44b6:	48 c1 e0 04          	shl    rax,0x4
    44ba:	4c 01 e0             	add    rax,r12
    44bd:	48 8b 10             	mov    rdx,QWORD PTR [rax]
    44c0:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
    44c4:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    44c8:	44 8b 03             	mov    r8d,DWORD PTR [rbx]
        setobjs2s(L, ra, RB(i));
    44cb:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    44cf:	45 85 ed             	test   r13d,r13d
    44d2:	0f 85 29 43 00 00    	jne    8801 <luaV_execute+0x6b11>
    44d8:	44 89 c5             	mov    ebp,r8d
    44db:	41 0f b6 c0          	movzx  eax,r8b
    44df:	4c 89 fb             	mov    rbx,r15
    44e2:	c1 ed 08             	shr    ebp,0x8
    44e5:	48 89 c2             	mov    rdx,rax
    44e8:	49 8b 04 c6          	mov    rax,QWORD PTR [r14+rax*8]
    44ec:	83 e5 7f             	and    ebp,0x7f
    44ef:	48 c1 e5 04          	shl    rbp,0x4
    44f3:	4c 01 e5             	add    rbp,r12
    44f6:	ff e0                	jmp    rax
        if (!ttisnil(ra) && RAVI_UNLIKELY(!ttisclosure(ra)))
          luaG_runerror(L, "closure expected");
        vmbreak;
      }
      vmcase(OP_RAVI_TOTYPE) {
        if (!ttisnil(ra)) {
    44f8:	66 83 7d 08 00       	cmp    WORD PTR [rbp+0x8],0x0
    44fd:	74 37                	je     4536 <luaV_execute+0x2846>
          TValue *rb = k + GETARG_Bx(i);
    44ff:	41 c1 e8 10          	shr    r8d,0x10
    4503:	44 89 c0             	mov    eax,r8d
    4506:	48 c1 e0 04          	shl    rax,0x4
    450a:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
          if  (!ttisshrstring(rb))
    450f:	66 81 78 08 04 80    	cmp    WORD PTR [rax+0x8],0x8004
    4515:	0f 85 e9 57 00 00    	jne    9d04 <luaV_execute+0x8014>
            luaG_runerror(L, "type name must be string");
          TString *key = tsvalue(rb);
    451b:	4c 8b 00             	mov    r8,QWORD PTR [rax]
          if (!raviV_check_usertype(L, key, ra))
    451e:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    4523:	48 89 ea             	mov    rdx,rbp
    4526:	4c 89 c6             	mov    rsi,r8
    4529:	e8 00 00 00 00       	call   452e <luaV_execute+0x283e>
    452e:	85 c0                	test   eax,eax
    4530:	0f 84 8c 57 00 00    	je     9cc2 <luaV_execute+0x7fd2>
            luaG_runerror(L, "type mismatch: expected %s", getstr(key));
        }
        vmbreak;
    4536:	48 89 d8             	mov    rax,rbx
    4539:	48 83 c3 04          	add    rbx,0x4
    453d:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4540:	45 85 ed             	test   r13d,r13d
    4543:	0f 85 4e 42 00 00    	jne    8797 <luaV_execute+0x6aa7>
    4549:	44 89 c5             	mov    ebp,r8d
    454c:	41 0f b6 c0          	movzx  eax,r8b
    4550:	c1 ed 08             	shr    ebp,0x8
    4553:	48 89 c2             	mov    rdx,rax
    4556:	83 e5 7f             	and    ebp,0x7f
    4559:	48 c1 e5 04          	shl    rbp,0x4
    455d:	4c 01 e5             	add    rbp,r12
    4560:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (!ttisnil(ra) && RAVI_UNLIKELY(!ttisclosure(ra)))
    4564:	0f b7 45 08          	movzx  eax,WORD PTR [rbp+0x8]
    4568:	66 85 c0             	test   ax,ax
    456b:	0f 85 17 1f 00 00    	jne    6488 <luaV_execute+0x4798>
        vmbreak;
    4571:	48 89 d8             	mov    rax,rbx
    4574:	48 83 c3 04          	add    rbx,0x4
    4578:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    457b:	45 85 ed             	test   r13d,r13d
    457e:	0f 85 d6 49 00 00    	jne    8f5a <luaV_execute+0x726a>
    4584:	44 89 c5             	mov    ebp,r8d
    4587:	41 0f b6 c0          	movzx  eax,r8b
    458b:	c1 ed 08             	shr    ebp,0x8
    458e:	48 89 c2             	mov    rdx,rax
    4591:	83 e5 7f             	and    ebp,0x7f
    4594:	48 c1 e5 04          	shl    rbp,0x4
    4598:	4c 01 e5             	add    rbp,r12
    459b:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (!ttisnil(ra) && RAVI_UNLIKELY(!ttisstring(ra)))
    459f:	0f b7 45 08          	movzx  eax,WORD PTR [rbp+0x8]
    45a3:	66 85 c0             	test   ax,ax
    45a6:	0f 85 b9 1e 00 00    	jne    6465 <luaV_execute+0x4775>
        vmbreak;
    45ac:	48 89 d8             	mov    rax,rbx
    45af:	48 83 c3 04          	add    rbx,0x4
    45b3:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    45b6:	45 85 ed             	test   r13d,r13d
    45b9:	0f 85 3c 4b 00 00    	jne    90fb <luaV_execute+0x740b>
    45bf:	44 89 c5             	mov    ebp,r8d
    45c2:	41 0f b6 c0          	movzx  eax,r8b
    45c6:	c1 ed 08             	shr    ebp,0x8
    45c9:	48 89 c2             	mov    rdx,rax
    45cc:	49 8b 04 c6          	mov    rax,QWORD PTR [r14+rax*8]
    45d0:	83 e5 7f             	and    ebp,0x7f
    45d3:	48 c1 e5 04          	shl    rbp,0x4
    45d7:	4c 01 e5             	add    rbp,r12
    45da:	ff e0                	jmp    rax
        if (RAVI_UNLIKELY(!ttisfarray(ra)))
    45dc:	66 81 7d 08 25 80    	cmp    WORD PTR [rbp+0x8],0x8025
    45e2:	0f 85 42 30 00 00    	jne    762a <luaV_execute+0x593a>
        vmbreak;
    45e8:	48 89 d8             	mov    rax,rbx
    45eb:	48 83 c3 04          	add    rbx,0x4
    45ef:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    45f2:	45 85 ed             	test   r13d,r13d
    45f5:	0f 85 94 49 00 00    	jne    8f8f <luaV_execute+0x729f>
    45fb:	44 89 c5             	mov    ebp,r8d
    45fe:	41 0f b6 c0          	movzx  eax,r8b
    4602:	c1 ed 08             	shr    ebp,0x8
    4605:	48 89 c2             	mov    rdx,rax
    4608:	83 e5 7f             	and    ebp,0x7f
    460b:	48 c1 e5 04          	shl    rbp,0x4
    460f:	4c 01 e5             	add    rbp,r12
    4612:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (RAVI_UNLIKELY(!ttisiarray(ra)))
    4616:	66 81 7d 08 15 80    	cmp    WORD PTR [rbp+0x8],0x8015
    461c:	0f 85 1e 30 00 00    	jne    7640 <luaV_execute+0x5950>
        vmbreak;
    4622:	48 89 d8             	mov    rax,rbx
    4625:	48 83 c3 04          	add    rbx,0x4
    4629:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    462c:	45 85 ed             	test   r13d,r13d
    462f:	0f 85 8f 49 00 00    	jne    8fc4 <luaV_execute+0x72d4>
    4635:	44 89 c5             	mov    ebp,r8d
    4638:	41 0f b6 c0          	movzx  eax,r8b
    463c:	c1 ed 08             	shr    ebp,0x8
    463f:	48 89 c2             	mov    rdx,rax
    4642:	83 e5 7f             	and    ebp,0x7f
    4645:	48 c1 e5 04          	shl    rbp,0x4
    4649:	4c 01 e5             	add    rbp,r12
    464c:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (RAVI_UNLIKELY(!ttisLtable(ra)))
    4650:	66 81 7d 08 05 80    	cmp    WORD PTR [rbp+0x8],0x8005
    4656:	0f 85 fa 2f 00 00    	jne    7656 <luaV_execute+0x5966>
        vmbreak;
    465c:	48 89 d8             	mov    rax,rbx
    465f:	48 83 c3 04          	add    rbx,0x4
    4663:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4666:	45 85 ed             	test   r13d,r13d
    4669:	0f 85 f6 4a 00 00    	jne    9165 <luaV_execute+0x7475>
    466f:	44 89 c5             	mov    ebp,r8d
    4672:	41 0f b6 c0          	movzx  eax,r8b
    4676:	c1 ed 08             	shr    ebp,0x8
    4679:	48 89 c2             	mov    rdx,rax
    467c:	83 e5 7f             	and    ebp,0x7f
    467f:	48 c1 e5 04          	shl    rbp,0x4
    4683:	4c 01 e5             	add    rbp,r12
    4686:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (RAVI_LIKELY(tonumber(ra, &j))) { setfltvalue(ra, j); }
    468a:	66 83 7d 08 03       	cmp    WORD PTR [rbp+0x8],0x3
    468f:	0f 85 de 3b 00 00    	jne    8273 <luaV_execute+0x6583>
    4695:	f2 0f 10 45 00       	movsd  xmm0,QWORD PTR [rbp+0x0]
    469a:	f2 0f 11 84 24 00 01 	movsd  QWORD PTR [rsp+0x100],xmm0
    46a1:	00 00 
    46a3:	b8 03 00 00 00       	mov    eax,0x3
    46a8:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    46ad:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    46b1:	48 89 d8             	mov    rax,rbx
    46b4:	48 83 c3 04          	add    rbx,0x4
    46b8:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    46bb:	45 85 ed             	test   r13d,r13d
    46be:	0f 85 98 49 00 00    	jne    905c <luaV_execute+0x736c>
    46c4:	44 89 c5             	mov    ebp,r8d
    46c7:	41 0f b6 c0          	movzx  eax,r8b
    46cb:	c1 ed 08             	shr    ebp,0x8
    46ce:	48 89 c2             	mov    rdx,rax
    46d1:	83 e5 7f             	and    ebp,0x7f
    46d4:	48 c1 e5 04          	shl    rbp,0x4
    46d8:	4c 01 e5             	add    rbp,r12
    46db:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (RAVI_LIKELY(tointeger(ra, &j))) { setivalue(ra, j); }
    46df:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    46e4:	0f 85 3f 40 00 00    	jne    8729 <luaV_execute+0x6a39>
    46ea:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    46ee:	48 89 84 24 f8 00 00 	mov    QWORD PTR [rsp+0xf8],rax
    46f5:	00 
    46f6:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    46fa:	ba 13 00 00 00       	mov    edx,0x13
        vmbreak;
    46ff:	48 89 d8             	mov    rax,rbx
    4702:	48 83 c3 04          	add    rbx,0x4
        if (RAVI_LIKELY(tointeger(ra, &j))) { setivalue(ra, j); }
    4706:	66 89 55 08          	mov    WORD PTR [rbp+0x8],dx
        vmbreak;
    470a:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    470d:	45 85 ed             	test   r13d,r13d
    4710:	0f 85 32 4b 00 00    	jne    9248 <luaV_execute+0x7558>
    4716:	44 89 c5             	mov    ebp,r8d
    4719:	41 0f b6 c0          	movzx  eax,r8b
    471d:	c1 ed 08             	shr    ebp,0x8
    4720:	48 89 c2             	mov    rdx,rax
    4723:	83 e5 7f             	and    ebp,0x7f
    4726:	48 c1 e5 04          	shl    rbp,0x4
    472a:	4c 01 e5             	add    rbp,r12
    472d:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    4731:	41 c1 e8 18          	shr    r8d,0x18
    4735:	44 89 c0             	mov    eax,r8d
    4738:	48 c1 e0 04          	shl    rax,0x4
    473c:	4c 01 e0             	add    rax,r12
        if (RAVI_LIKELY(ttisLtable(rb))) {
    473f:	66 81 78 08 05 80    	cmp    WORD PTR [rax+0x8],0x8005
    4745:	0f 85 0b 2f 00 00    	jne    7656 <luaV_execute+0x5966>
          setobjs2s(L, ra, rb);
    474b:	48 8b 00             	mov    rax,QWORD PTR [rax]
    474e:	b9 05 80 ff ff       	mov    ecx,0xffff8005
    4753:	66 89 4d 08          	mov    WORD PTR [rbp+0x8],cx
    4757:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        vmbreak;
    475b:	48 89 d8             	mov    rax,rbx
    475e:	48 83 c3 04          	add    rbx,0x4
    4762:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4765:	45 85 ed             	test   r13d,r13d
    4768:	0f 85 b9 48 00 00    	jne    9027 <luaV_execute+0x7337>
    476e:	44 89 c5             	mov    ebp,r8d
    4771:	41 0f b6 c0          	movzx  eax,r8b
    4775:	c1 ed 08             	shr    ebp,0x8
    4778:	48 89 c2             	mov    rdx,rax
    477b:	83 e5 7f             	and    ebp,0x7f
    477e:	48 c1 e5 04          	shl    rbp,0x4
    4782:	4c 01 e5             	add    rbp,r12
    4785:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    4789:	41 c1 e8 18          	shr    r8d,0x18
    478d:	44 89 c0             	mov    eax,r8d
    4790:	48 c1 e0 04          	shl    rax,0x4
    4794:	4c 01 e0             	add    rax,r12
        if (RAVI_LIKELY(ttisfarray(rb))) {
    4797:	66 81 78 08 25 80    	cmp    WORD PTR [rax+0x8],0x8025
    479d:	0f 85 87 2e 00 00    	jne    762a <luaV_execute+0x593a>
          setobjs2s(L, ra, rb);
    47a3:	48 8b 00             	mov    rax,QWORD PTR [rax]
    47a6:	be 25 80 ff ff       	mov    esi,0xffff8025
    47ab:	66 89 75 08          	mov    WORD PTR [rbp+0x8],si
    47af:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        vmbreak;
    47b3:	48 89 d8             	mov    rax,rbx
    47b6:	48 83 c3 04          	add    rbx,0x4
    47ba:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    47bd:	45 85 ed             	test   r13d,r13d
    47c0:	0f 85 00 49 00 00    	jne    90c6 <luaV_execute+0x73d6>
    47c6:	44 89 c5             	mov    ebp,r8d
    47c9:	41 0f b6 c0          	movzx  eax,r8b
    47cd:	c1 ed 08             	shr    ebp,0x8
    47d0:	48 89 c2             	mov    rdx,rax
    47d3:	83 e5 7f             	and    ebp,0x7f
    47d6:	48 c1 e5 04          	shl    rbp,0x4
    47da:	4c 01 e5             	add    rbp,r12
    47dd:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    47e1:	41 c1 e8 18          	shr    r8d,0x18
    47e5:	44 89 c0             	mov    eax,r8d
    47e8:	48 c1 e0 04          	shl    rax,0x4
    47ec:	4c 01 e0             	add    rax,r12
        if (RAVI_LIKELY(ttisiarray(rb))) {
    47ef:	66 81 78 08 15 80    	cmp    WORD PTR [rax+0x8],0x8015
    47f5:	0f 85 45 2e 00 00    	jne    7640 <luaV_execute+0x5950>
          setobjs2s(L, ra, rb);
    47fb:	48 8b 00             	mov    rax,QWORD PTR [rax]
    47fe:	bf 15 80 ff ff       	mov    edi,0xffff8015
    4803:	66 89 7d 08          	mov    WORD PTR [rbp+0x8],di
    4807:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        vmbreak;
    480b:	48 89 d8             	mov    rax,rbx
    480e:	48 83 c3 04          	add    rbx,0x4
    4812:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4815:	45 85 ed             	test   r13d,r13d
    4818:	0f 85 6f 46 00 00    	jne    8e8d <luaV_execute+0x719d>
    481e:	44 89 c5             	mov    ebp,r8d
    4821:	41 0f b6 c0          	movzx  eax,r8b
    4825:	c1 ed 08             	shr    ebp,0x8
    4828:	48 89 c2             	mov    rdx,rax
    482b:	83 e5 7f             	and    ebp,0x7f
    482e:	48 c1 e5 04          	shl    rbp,0x4
    4832:	4c 01 e5             	add    rbp,r12
    4835:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    4839:	41 c1 e8 18          	shr    r8d,0x18
    483d:	44 89 c7             	mov    edi,r8d
    4840:	48 c1 e7 04          	shl    rdi,0x4
    4844:	4c 01 e7             	add    rdi,r12
        if (RAVI_LIKELY(tonumber(rb, &j))) { setfltvalue(ra, j); }
    4847:	66 83 7f 08 03       	cmp    WORD PTR [rdi+0x8],0x3
    484c:	0f 85 10 3f 00 00    	jne    8762 <luaV_execute+0x6a72>
    4852:	f2 0f 10 07          	movsd  xmm0,QWORD PTR [rdi]
    4856:	f2 0f 11 84 24 f0 00 	movsd  QWORD PTR [rsp+0xf0],xmm0
    485d:	00 00 
    485f:	41 b8 03 00 00 00    	mov    r8d,0x3
        vmbreak;
    4865:	48 89 d8             	mov    rax,rbx
        if (RAVI_LIKELY(tonumber(rb, &j))) { setfltvalue(ra, j); }
    4868:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    486d:	48 83 c3 04          	add    rbx,0x4
        if (RAVI_LIKELY(tonumber(rb, &j))) { setfltvalue(ra, j); }
    4871:	66 44 89 45 08       	mov    WORD PTR [rbp+0x8],r8w
        vmbreak;
    4876:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4879:	45 85 ed             	test   r13d,r13d
    487c:	0f 85 0f 48 00 00    	jne    9091 <luaV_execute+0x73a1>
    4882:	44 89 c5             	mov    ebp,r8d
    4885:	41 0f b6 c0          	movzx  eax,r8b
    4889:	c1 ed 08             	shr    ebp,0x8
    488c:	48 89 c2             	mov    rdx,rax
    488f:	83 e5 7f             	and    ebp,0x7f
    4892:	48 c1 e5 04          	shl    rbp,0x4
    4896:	4c 01 e5             	add    rbp,r12
    4899:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    489d:	41 c1 e8 18          	shr    r8d,0x18
    48a1:	44 89 c7             	mov    edi,r8d
    48a4:	48 c1 e7 04          	shl    rdi,0x4
    48a8:	4c 01 e7             	add    rdi,r12
        if (RAVI_LIKELY(tointeger(rb, &j))) { setivalue(ra, j); }
    48ab:	66 83 7f 08 13       	cmp    WORD PTR [rdi+0x8],0x13
    48b0:	0f 85 60 41 00 00    	jne    8a16 <luaV_execute+0x6d26>
    48b6:	48 8b 07             	mov    rax,QWORD PTR [rdi]
    48b9:	48 89 84 24 e8 00 00 	mov    QWORD PTR [rsp+0xe8],rax
    48c0:	00 
    48c1:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    48c5:	41 b9 13 00 00 00    	mov    r9d,0x13
        vmbreak;
    48cb:	48 89 d8             	mov    rax,rbx
    48ce:	48 83 c3 04          	add    rbx,0x4
        if (RAVI_LIKELY(tointeger(rb, &j))) { setivalue(ra, j); }
    48d2:	66 44 89 4d 08       	mov    WORD PTR [rbp+0x8],r9w
        vmbreak;
    48d7:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    48da:	45 85 ed             	test   r13d,r13d
    48dd:	0f 85 30 49 00 00    	jne    9213 <luaV_execute+0x7523>
    48e3:	44 89 c5             	mov    ebp,r8d
    48e6:	41 0f b6 c0          	movzx  eax,r8b
    48ea:	c1 ed 08             	shr    ebp,0x8
    48ed:	48 89 c2             	mov    rdx,rax
    48f0:	83 e5 7f             	and    ebp,0x7f
    48f3:	48 c1 e5 04          	shl    rbp,0x4
    48f7:	4c 01 e5             	add    rbp,r12
    48fa:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    48fe:	44 89 c0             	mov    eax,r8d
    4901:	c1 e8 18             	shr    eax,0x18
    4904:	a8 80                	test   al,0x80
    4906:	0f 85 9c 1d 00 00    	jne    66a8 <luaV_execute+0x49b8>
    490c:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4910:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4914:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    4918:	41 f6 c0 80          	test   r8b,0x80
    491c:	0f 84 a4 1d 00 00    	je     66c6 <luaV_execute+0x49d6>
    4922:	41 83 e0 7f          	and    r8d,0x7f
    4926:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    492b:	49 c1 e0 04          	shl    r8,0x4
    492f:	4c 01 c0             	add    rax,r8
        setfltvalue(ra, (lua_Number)(ivalue(rb)) / (lua_Number)(ivalue(rc)));
    4932:	66 0f ef c0          	pxor   xmm0,xmm0
    4936:	66 0f ef c9          	pxor   xmm1,xmm1
    493a:	41 ba 03 00 00 00    	mov    r10d,0x3
    4940:	f2 48 0f 2a 08       	cvtsi2sd xmm1,QWORD PTR [rax]
        vmbreak;
    4945:	48 89 d8             	mov    rax,rbx
    4948:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, (lua_Number)(ivalue(rb)) / (lua_Number)(ivalue(rc)));
    494c:	f2 48 0f 2a 02       	cvtsi2sd xmm0,QWORD PTR [rdx]
    4951:	66 44 89 55 08       	mov    WORD PTR [rbp+0x8],r10w
    4956:	f2 0f 5e c1          	divsd  xmm0,xmm1
    495a:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    495f:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4962:	45 85 ed             	test   r13d,r13d
    4965:	0f 85 f4 33 00 00    	jne    7d5f <luaV_execute+0x606f>
    496b:	44 89 c5             	mov    ebp,r8d
    496e:	41 0f b6 c0          	movzx  eax,r8b
    4972:	c1 ed 08             	shr    ebp,0x8
    4975:	48 89 c2             	mov    rdx,rax
    4978:	83 e5 7f             	and    ebp,0x7f
    497b:	48 c1 e5 04          	shl    rbp,0x4
    497f:	4c 01 e5             	add    rbp,r12
    4982:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4986:	44 89 c0             	mov    eax,r8d
    4989:	c1 e8 18             	shr    eax,0x18
    498c:	a8 80                	test   al,0x80
    498e:	0f 85 43 1d 00 00    	jne    66d7 <luaV_execute+0x49e7>
    4994:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4998:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    499c:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    49a0:	41 f6 c0 80          	test   r8b,0x80
    49a4:	0f 84 4b 1d 00 00    	je     66f5 <luaV_execute+0x4a05>
    49aa:	41 83 e0 7f          	and    r8d,0x7f
    49ae:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    49b3:	49 c1 e0 04          	shl    r8,0x4
    49b7:	4c 01 c0             	add    rax,r8
        setfltvalue(ra, ivalue(rb) / fltvalue(rc));
    49ba:	66 0f ef c0          	pxor   xmm0,xmm0
    49be:	41 bb 03 00 00 00    	mov    r11d,0x3
    49c4:	f2 48 0f 2a 02       	cvtsi2sd xmm0,QWORD PTR [rdx]
    49c9:	f2 0f 5e 00          	divsd  xmm0,QWORD PTR [rax]
        vmbreak;
    49cd:	48 89 d8             	mov    rax,rbx
        setfltvalue(ra, ivalue(rb) / fltvalue(rc));
    49d0:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    49d5:	66 44 89 5d 08       	mov    WORD PTR [rbp+0x8],r11w
        vmbreak;
    49da:	48 83 c3 04          	add    rbx,0x4
    49de:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    49e1:	45 85 ed             	test   r13d,r13d
    49e4:	0f 85 40 33 00 00    	jne    7d2a <luaV_execute+0x603a>
    49ea:	44 89 c5             	mov    ebp,r8d
    49ed:	41 0f b6 c0          	movzx  eax,r8b
    49f1:	c1 ed 08             	shr    ebp,0x8
    49f4:	48 89 c2             	mov    rdx,rax
    49f7:	83 e5 7f             	and    ebp,0x7f
    49fa:	48 c1 e5 04          	shl    rbp,0x4
    49fe:	4c 01 e5             	add    rbp,r12
    4a01:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4a05:	44 89 c0             	mov    eax,r8d
    4a08:	c1 e8 18             	shr    eax,0x18
    4a0b:	a8 80                	test   al,0x80
    4a0d:	0f 85 42 17 00 00    	jne    6155 <luaV_execute+0x4465>
    4a13:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4a17:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4a1b:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    4a1e:	41 f6 c0 80          	test   r8b,0x80
    4a22:	0f 84 47 17 00 00    	je     616f <luaV_execute+0x447f>
    4a28:	41 83 e0 7f          	and    r8d,0x7f
    4a2c:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
    4a31:	49 c1 e0 04          	shl    r8,0x4
    4a35:	4a 8d 14 06          	lea    rdx,[rsi+r8*1]
        setfltvalue(ra, fltvalue(rb) / ivalue(rc));
    4a39:	66 0f ef c9          	pxor   xmm1,xmm1
    4a3d:	f2 0f 10 00          	movsd  xmm0,QWORD PTR [rax]
    4a41:	41 bf 03 00 00 00    	mov    r15d,0x3
        vmbreak;
    4a47:	48 89 d8             	mov    rax,rbx
        setfltvalue(ra, fltvalue(rb) / ivalue(rc));
    4a4a:	f2 48 0f 2a 0a       	cvtsi2sd xmm1,QWORD PTR [rdx]
        vmbreak;
    4a4f:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, fltvalue(rb) / ivalue(rc));
    4a53:	66 44 89 7d 08       	mov    WORD PTR [rbp+0x8],r15w
    4a58:	f2 0f 5e c1          	divsd  xmm0,xmm1
    4a5c:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    4a61:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4a64:	45 85 ed             	test   r13d,r13d
    4a67:	0f 85 e8 3b 00 00    	jne    8655 <luaV_execute+0x6965>
    4a6d:	44 89 c5             	mov    ebp,r8d
    4a70:	41 0f b6 c0          	movzx  eax,r8b
    4a74:	c1 ed 08             	shr    ebp,0x8
    4a77:	48 89 c2             	mov    rdx,rax
    4a7a:	83 e5 7f             	and    ebp,0x7f
    4a7d:	48 c1 e5 04          	shl    rbp,0x4
    4a81:	4c 01 e5             	add    rbp,r12
    4a84:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4a88:	44 89 c0             	mov    eax,r8d
    4a8b:	c1 e8 18             	shr    eax,0x18
    4a8e:	a8 80                	test   al,0x80
    4a90:	0f 85 56 1b 00 00    	jne    65ec <luaV_execute+0x48fc>
    4a96:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4a9a:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4a9e:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    4aa2:	41 f6 c0 80          	test   r8b,0x80
    4aa6:	0f 84 5e 1b 00 00    	je     660a <luaV_execute+0x491a>
    4aac:	41 83 e0 7f          	and    r8d,0x7f
    4ab0:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    4ab5:	49 c1 e0 04          	shl    r8,0x4
    4ab9:	4c 01 c0             	add    rax,r8
        setfltvalue(ra, fltvalue(rb) / fltvalue(rc));
    4abc:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    4ac0:	f2 0f 5e 00          	divsd  xmm0,QWORD PTR [rax]
    4ac4:	b8 03 00 00 00       	mov    eax,0x3
    4ac9:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    4ace:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    4ad2:	48 89 d8             	mov    rax,rbx
    4ad5:	48 83 c3 04          	add    rbx,0x4
    4ad9:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4adc:	45 85 ed             	test   r13d,r13d
    4adf:	0f 85 c8 34 00 00    	jne    7fad <luaV_execute+0x62bd>
    4ae5:	44 89 c5             	mov    ebp,r8d
    4ae8:	41 0f b6 c0          	movzx  eax,r8b
    4aec:	c1 ed 08             	shr    ebp,0x8
    4aef:	48 89 c2             	mov    rdx,rax
    4af2:	83 e5 7f             	and    ebp,0x7f
    4af5:	48 c1 e5 04          	shl    rbp,0x4
    4af9:	4c 01 e5             	add    rbp,r12
    4afc:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4b00:	44 89 c0             	mov    eax,r8d
    4b03:	c1 e8 18             	shr    eax,0x18
    4b06:	a8 80                	test   al,0x80
    4b08:	0f 85 af 1a 00 00    	jne    65bd <luaV_execute+0x48cd>
    4b0e:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4b12:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4b16:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    4b1a:	41 f6 c0 80          	test   r8b,0x80
    4b1e:	0f 84 b7 1a 00 00    	je     65db <luaV_execute+0x48eb>
    4b24:	41 83 e0 7f          	and    r8d,0x7f
    4b28:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    4b2d:	49 c1 e0 04          	shl    r8,0x4
    4b31:	4c 01 c0             	add    rax,r8
        setivalue(ra, ivalue(rb) * ivalue(rc));
    4b34:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    4b37:	48 0f af 10          	imul   rdx,QWORD PTR [rax]
    4b3b:	b8 13 00 00 00       	mov    eax,0x13
    4b40:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    4b44:	48 89 d8             	mov    rax,rbx
    4b47:	48 83 c3 04          	add    rbx,0x4
        setivalue(ra, ivalue(rb) * ivalue(rc));
    4b4b:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    4b4f:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4b52:	45 85 ed             	test   r13d,r13d
    4b55:	0f 85 87 34 00 00    	jne    7fe2 <luaV_execute+0x62f2>
    4b5b:	44 89 c5             	mov    ebp,r8d
    4b5e:	41 0f b6 c0          	movzx  eax,r8b
    4b62:	c1 ed 08             	shr    ebp,0x8
    4b65:	48 89 c2             	mov    rdx,rax
    4b68:	83 e5 7f             	and    ebp,0x7f
    4b6b:	48 c1 e5 04          	shl    rbp,0x4
    4b6f:	4c 01 e5             	add    rbp,r12
    4b72:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4b76:	44 89 c0             	mov    eax,r8d
    4b79:	c1 e8 18             	shr    eax,0x18
    4b7c:	a8 80                	test   al,0x80
    4b7e:	0f 85 0e 1a 00 00    	jne    6592 <luaV_execute+0x48a2>
    4b84:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4b88:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4b8c:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    4b8f:	41 f6 c0 80          	test   r8b,0x80
    4b93:	0f 84 13 1a 00 00    	je     65ac <luaV_execute+0x48bc>
    4b99:	41 83 e0 7f          	and    r8d,0x7f
    4b9d:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
    4ba2:	49 c1 e0 04          	shl    r8,0x4
    4ba6:	4a 8d 14 07          	lea    rdx,[rdi+r8*1]
        setfltvalue(ra, fltvalue(rb) * ivalue(rc));
    4baa:	66 0f ef c0          	pxor   xmm0,xmm0
    4bae:	f2 48 0f 2a 02       	cvtsi2sd xmm0,QWORD PTR [rdx]
    4bb3:	f2 0f 59 00          	mulsd  xmm0,QWORD PTR [rax]
    4bb7:	b8 03 00 00 00       	mov    eax,0x3
    4bbc:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    4bc0:	48 89 d8             	mov    rax,rbx
    4bc3:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, fltvalue(rb) * ivalue(rc));
    4bc7:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    4bcc:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4bcf:	45 85 ed             	test   r13d,r13d
    4bd2:	0f 85 3f 34 00 00    	jne    8017 <luaV_execute+0x6327>
    4bd8:	44 89 c5             	mov    ebp,r8d
    4bdb:	41 0f b6 c0          	movzx  eax,r8b
    4bdf:	c1 ed 08             	shr    ebp,0x8
    4be2:	48 89 c2             	mov    rdx,rax
    4be5:	83 e5 7f             	and    ebp,0x7f
    4be8:	48 c1 e5 04          	shl    rbp,0x4
    4bec:	4c 01 e5             	add    rbp,r12
    4bef:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4bf3:	44 89 c0             	mov    eax,r8d
    4bf6:	c1 e8 18             	shr    eax,0x18
    4bf9:	a8 80                	test   al,0x80
    4bfb:	0f 85 62 19 00 00    	jne    6563 <luaV_execute+0x4873>
    4c01:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4c05:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4c09:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    4c0d:	41 f6 c0 80          	test   r8b,0x80
    4c11:	0f 84 6a 19 00 00    	je     6581 <luaV_execute+0x4891>
    4c17:	41 83 e0 7f          	and    r8d,0x7f
    4c1b:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    4c20:	49 c1 e0 04          	shl    r8,0x4
    4c24:	4c 01 c0             	add    rax,r8
        setfltvalue(ra, fltvalue(rb) * fltvalue(rc));
    4c27:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    4c2b:	f2 0f 59 00          	mulsd  xmm0,QWORD PTR [rax]
    4c2f:	b8 03 00 00 00       	mov    eax,0x3
    4c34:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    4c38:	48 89 d8             	mov    rax,rbx
    4c3b:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, fltvalue(rb) * fltvalue(rc));
    4c3f:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    4c44:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4c47:	45 85 ed             	test   r13d,r13d
    4c4a:	0f 85 fc 33 00 00    	jne    804c <luaV_execute+0x635c>
    4c50:	44 89 c5             	mov    ebp,r8d
    4c53:	41 0f b6 c0          	movzx  eax,r8b
    4c57:	c1 ed 08             	shr    ebp,0x8
    4c5a:	48 89 c2             	mov    rdx,rax
    4c5d:	83 e5 7f             	and    ebp,0x7f
    4c60:	48 c1 e5 04          	shl    rbp,0x4
    4c64:	4c 01 e5             	add    rbp,r12
    4c67:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4c6b:	44 89 c0             	mov    eax,r8d
    4c6e:	c1 e8 18             	shr    eax,0x18
    4c71:	a8 80                	test   al,0x80
    4c73:	0f 85 bb 18 00 00    	jne    6534 <luaV_execute+0x4844>
    4c79:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4c7d:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4c81:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    4c85:	41 f6 c0 80          	test   r8b,0x80
    4c89:	0f 84 c3 18 00 00    	je     6552 <luaV_execute+0x4862>
    4c8f:	41 83 e0 7f          	and    r8d,0x7f
    4c93:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    4c98:	49 c1 e0 04          	shl    r8,0x4
    4c9c:	4c 01 c0             	add    rax,r8
        setivalue(ra, ivalue(rb) - ivalue(rc));
    4c9f:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    4ca2:	48 2b 10             	sub    rdx,QWORD PTR [rax]
    4ca5:	b8 13 00 00 00       	mov    eax,0x13
    4caa:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    4cae:	48 89 d8             	mov    rax,rbx
    4cb1:	48 83 c3 04          	add    rbx,0x4
        setivalue(ra, ivalue(rb) - ivalue(rc));
    4cb5:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    4cb9:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4cbc:	45 85 ed             	test   r13d,r13d
    4cbf:	0f 85 da 34 00 00    	jne    819f <luaV_execute+0x64af>
    4cc5:	44 89 c5             	mov    ebp,r8d
    4cc8:	41 0f b6 c0          	movzx  eax,r8b
    4ccc:	c1 ed 08             	shr    ebp,0x8
    4ccf:	48 89 c2             	mov    rdx,rax
    4cd2:	83 e5 7f             	and    ebp,0x7f
    4cd5:	48 c1 e5 04          	shl    rbp,0x4
    4cd9:	4c 01 e5             	add    rbp,r12
    4cdc:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4ce0:	44 89 c0             	mov    eax,r8d
    4ce3:	c1 e8 18             	shr    eax,0x18
    4ce6:	a8 80                	test   al,0x80
    4ce8:	0f 85 17 18 00 00    	jne    6505 <luaV_execute+0x4815>
    4cee:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4cf2:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4cf6:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    4cfa:	41 f6 c0 80          	test   r8b,0x80
    4cfe:	0f 84 1f 18 00 00    	je     6523 <luaV_execute+0x4833>
    4d04:	41 83 e0 7f          	and    r8d,0x7f
    4d08:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    4d0d:	49 c1 e0 04          	shl    r8,0x4
    4d11:	4c 01 c0             	add    rax,r8
        setfltvalue(ra, ivalue(rb) - fltvalue(rc));
    4d14:	66 0f ef c0          	pxor   xmm0,xmm0
    4d18:	f2 48 0f 2a 02       	cvtsi2sd xmm0,QWORD PTR [rdx]
    4d1d:	f2 0f 5c 00          	subsd  xmm0,QWORD PTR [rax]
    4d21:	b8 03 00 00 00       	mov    eax,0x3
    4d26:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        vmbreak;
    4d2a:	48 89 d8             	mov    rax,rbx
    4d2d:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, ivalue(rb) - fltvalue(rc));
    4d31:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    4d36:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4d39:	45 85 ed             	test   r13d,r13d
    4d3c:	0f 85 92 34 00 00    	jne    81d4 <luaV_execute+0x64e4>
    4d42:	44 89 c5             	mov    ebp,r8d
    4d45:	41 0f b6 c0          	movzx  eax,r8b
    4d49:	c1 ed 08             	shr    ebp,0x8
    4d4c:	48 89 c2             	mov    rdx,rax
    4d4f:	83 e5 7f             	and    ebp,0x7f
    4d52:	48 c1 e5 04          	shl    rbp,0x4
    4d56:	4c 01 e5             	add    rbp,r12
    4d59:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4d5d:	44 89 c0             	mov    eax,r8d
    4d60:	c1 e8 18             	shr    eax,0x18
    4d63:	a8 80                	test   al,0x80
    4d65:	0f 85 6f 17 00 00    	jne    64da <luaV_execute+0x47ea>
    4d6b:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4d6f:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4d73:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    4d76:	41 f6 c0 80          	test   r8b,0x80
    4d7a:	0f 84 74 17 00 00    	je     64f4 <luaV_execute+0x4804>
    4d80:	41 83 e0 7f          	and    r8d,0x7f
    4d84:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
    4d89:	49 c1 e0 04          	shl    r8,0x4
    4d8d:	4a 8d 14 06          	lea    rdx,[rsi+r8*1]
        setfltvalue(ra, fltvalue(rb) - ivalue(rc));
    4d91:	66 0f ef c9          	pxor   xmm1,xmm1
    4d95:	f2 0f 10 00          	movsd  xmm0,QWORD PTR [rax]
        vmbreak;
    4d99:	48 89 d8             	mov    rax,rbx
    4d9c:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, fltvalue(rb) - ivalue(rc));
    4da0:	f2 48 0f 2a 0a       	cvtsi2sd xmm1,QWORD PTR [rdx]
    4da5:	ba 03 00 00 00       	mov    edx,0x3
    4daa:	66 89 55 08          	mov    WORD PTR [rbp+0x8],dx
    4dae:	f2 0f 5c c1          	subsd  xmm0,xmm1
    4db2:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    4db7:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4dba:	45 85 ed             	test   r13d,r13d
    4dbd:	0f 85 46 34 00 00    	jne    8209 <luaV_execute+0x6519>
    4dc3:	44 89 c5             	mov    ebp,r8d
    4dc6:	41 0f b6 c0          	movzx  eax,r8b
    4dca:	c1 ed 08             	shr    ebp,0x8
    4dcd:	48 89 c2             	mov    rdx,rax
    4dd0:	83 e5 7f             	and    ebp,0x7f
    4dd3:	48 c1 e5 04          	shl    rbp,0x4
    4dd7:	4c 01 e5             	add    rbp,r12
    4dda:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4dde:	44 89 c0             	mov    eax,r8d
    4de1:	c1 e8 18             	shr    eax,0x18
    4de4:	a8 80                	test   al,0x80
    4de6:	0f 85 bf 16 00 00    	jne    64ab <luaV_execute+0x47bb>
    4dec:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4df0:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4df4:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    4df8:	41 f6 c0 80          	test   r8b,0x80
    4dfc:	0f 84 c7 16 00 00    	je     64c9 <luaV_execute+0x47d9>
    4e02:	41 83 e0 7f          	and    r8d,0x7f
    4e06:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    4e0b:	49 c1 e0 04          	shl    r8,0x4
    4e0f:	4c 01 c0             	add    rax,r8
        setfltvalue(ra, fltvalue(rb) - fltvalue(rc));
    4e12:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    4e16:	f2 0f 5c 00          	subsd  xmm0,QWORD PTR [rax]
    4e1a:	b9 03 00 00 00       	mov    ecx,0x3
        vmbreak;
    4e1f:	48 89 d8             	mov    rax,rbx
        setfltvalue(ra, fltvalue(rb) - fltvalue(rc));
    4e22:	66 89 4d 08          	mov    WORD PTR [rbp+0x8],cx
        vmbreak;
    4e26:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, fltvalue(rb) - fltvalue(rc));
    4e2a:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    4e2f:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4e32:	45 85 ed             	test   r13d,r13d
    4e35:	0f 85 03 34 00 00    	jne    823e <luaV_execute+0x654e>
    4e3b:	44 89 c5             	mov    ebp,r8d
    4e3e:	41 0f b6 c0          	movzx  eax,r8b
    4e42:	c1 ed 08             	shr    ebp,0x8
    4e45:	48 89 c2             	mov    rdx,rax
    4e48:	83 e5 7f             	and    ebp,0x7f
    4e4b:	48 c1 e5 04          	shl    rbp,0x4
    4e4f:	4c 01 e5             	add    rbp,r12
    4e52:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4e56:	44 89 c0             	mov    eax,r8d
    4e59:	c1 e8 18             	shr    eax,0x18
    4e5c:	a8 80                	test   al,0x80
    4e5e:	0f 85 6f 19 00 00    	jne    67d3 <luaV_execute+0x4ae3>
    4e64:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4e68:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4e6c:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    4e6f:	41 f6 c0 80          	test   r8b,0x80
    4e73:	0f 84 74 19 00 00    	je     67ed <luaV_execute+0x4afd>
    4e79:	41 83 e0 7f          	and    r8d,0x7f
    4e7d:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
    4e82:	49 c1 e0 04          	shl    r8,0x4
    4e86:	4a 8d 14 06          	lea    rdx,[rsi+r8*1]
        setivalue(ra, ivalue(rb) + ivalue(rc));
    4e8a:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    4e8d:	be 13 00 00 00       	mov    esi,0x13
    4e92:	48 03 10             	add    rdx,QWORD PTR [rax]
        vmbreak;
    4e95:	48 89 d8             	mov    rax,rbx
        setivalue(ra, ivalue(rb) + ivalue(rc));
    4e98:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
        vmbreak;
    4e9c:	48 83 c3 04          	add    rbx,0x4
    4ea0:	44 8b 00             	mov    r8d,DWORD PTR [rax]
        setivalue(ra, ivalue(rb) + ivalue(rc));
    4ea3:	66 89 75 08          	mov    WORD PTR [rbp+0x8],si
        vmbreak;
    4ea7:	45 85 ed             	test   r13d,r13d
    4eaa:	0f 85 45 2e 00 00    	jne    7cf5 <luaV_execute+0x6005>
    4eb0:	44 89 c5             	mov    ebp,r8d
    4eb3:	41 0f b6 c0          	movzx  eax,r8b
    4eb7:	c1 ed 08             	shr    ebp,0x8
    4eba:	48 89 c2             	mov    rdx,rax
    4ebd:	83 e5 7f             	and    ebp,0x7f
    4ec0:	48 c1 e5 04          	shl    rbp,0x4
    4ec4:	4c 01 e5             	add    rbp,r12
    4ec7:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4ecb:	44 89 c0             	mov    eax,r8d
    4ece:	c1 e8 18             	shr    eax,0x18
    4ed1:	a8 80                	test   al,0x80
    4ed3:	0f 85 d6 12 00 00    	jne    61af <luaV_execute+0x44bf>
    4ed9:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4edd:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4ee1:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    4ee4:	41 f6 c0 80          	test   r8b,0x80
    4ee8:	0f 84 db 12 00 00    	je     61c9 <luaV_execute+0x44d9>
    4eee:	41 83 e0 7f          	and    r8d,0x7f
    4ef2:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    4ef7:	49 c1 e0 04          	shl    r8,0x4
    4efb:	4a 8d 14 01          	lea    rdx,[rcx+r8*1]
        setfltvalue(ra, fltvalue(rb) + ivalue(rc));
    4eff:	66 0f ef c0          	pxor   xmm0,xmm0
    4f03:	bf 03 00 00 00       	mov    edi,0x3
    4f08:	f2 48 0f 2a 02       	cvtsi2sd xmm0,QWORD PTR [rdx]
    4f0d:	f2 0f 58 00          	addsd  xmm0,QWORD PTR [rax]
        vmbreak;
    4f11:	48 89 d8             	mov    rax,rbx
        setfltvalue(ra, fltvalue(rb) + ivalue(rc));
    4f14:	66 89 7d 08          	mov    WORD PTR [rbp+0x8],di
        vmbreak;
    4f18:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, fltvalue(rb) + ivalue(rc));
    4f1c:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    4f21:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4f24:	45 85 ed             	test   r13d,r13d
    4f27:	0f 85 be 36 00 00    	jne    85eb <luaV_execute+0x68fb>
    4f2d:	44 89 c5             	mov    ebp,r8d
    4f30:	41 0f b6 c0          	movzx  eax,r8b
    4f34:	c1 ed 08             	shr    ebp,0x8
    4f37:	48 89 c2             	mov    rdx,rax
    4f3a:	83 e5 7f             	and    ebp,0x7f
    4f3d:	48 c1 e5 04          	shl    rbp,0x4
    4f41:	4c 01 e5             	add    rbp,r12
    4f44:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    4f48:	44 89 c0             	mov    eax,r8d
    4f4b:	c1 e8 18             	shr    eax,0x18
    4f4e:	a8 80                	test   al,0x80
    4f50:	0f 85 2a 12 00 00    	jne    6180 <luaV_execute+0x4490>
    4f56:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    4f5a:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    4f5e:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    4f62:	41 f6 c0 80          	test   r8b,0x80
    4f66:	0f 84 32 12 00 00    	je     619e <luaV_execute+0x44ae>
    4f6c:	41 83 e0 7f          	and    r8d,0x7f
    4f70:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    4f75:	49 c1 e0 04          	shl    r8,0x4
    4f79:	4c 01 c0             	add    rax,r8
        setfltvalue(ra, fltvalue(rb) + fltvalue(rc));
    4f7c:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    4f80:	f2 0f 58 00          	addsd  xmm0,QWORD PTR [rax]
    4f84:	41 b8 03 00 00 00    	mov    r8d,0x3
        vmbreak;
    4f8a:	48 89 d8             	mov    rax,rbx
        setfltvalue(ra, fltvalue(rb) + fltvalue(rc));
    4f8d:	66 44 89 45 08       	mov    WORD PTR [rbp+0x8],r8w
        vmbreak;
    4f92:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, fltvalue(rb) + fltvalue(rc));
    4f96:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    4f9b:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4f9e:	45 85 ed             	test   r13d,r13d
    4fa1:	0f 85 79 36 00 00    	jne    8620 <luaV_execute+0x6930>
    4fa7:	44 89 c5             	mov    ebp,r8d
    4faa:	41 0f b6 c0          	movzx  eax,r8b
    4fae:	c1 ed 08             	shr    ebp,0x8
    4fb1:	48 89 c2             	mov    rdx,rax
    4fb4:	83 e5 7f             	and    ebp,0x7f
    4fb7:	48 c1 e5 04          	shl    rbp,0x4
    4fbb:	4c 01 e5             	add    rbp,r12
    4fbe:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    4fc2:	41 c1 e8 18          	shr    r8d,0x18
        setivalue(ra, -ivalue(rb));
    4fc6:	41 b9 13 00 00 00    	mov    r9d,0x13
        TValue *rb = RB(i);
    4fcc:	44 89 c0             	mov    eax,r8d
        setivalue(ra, -ivalue(rb));
    4fcf:	48 c1 e0 04          	shl    rax,0x4
    4fd3:	49 8b 04 04          	mov    rax,QWORD PTR [r12+rax*1]
    4fd7:	66 44 89 4d 08       	mov    WORD PTR [rbp+0x8],r9w
    4fdc:	48 f7 d8             	neg    rax
    4fdf:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        vmbreak;
    4fe3:	48 89 d8             	mov    rax,rbx
    4fe6:	48 83 c3 04          	add    rbx,0x4
    4fea:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    4fed:	45 85 ed             	test   r13d,r13d
    4ff0:	0f 85 56 3a 00 00    	jne    8a4c <luaV_execute+0x6d5c>
    4ff6:	44 89 c5             	mov    ebp,r8d
    4ff9:	41 0f b6 c0          	movzx  eax,r8b
    4ffd:	c1 ed 08             	shr    ebp,0x8
    5000:	48 89 c2             	mov    rdx,rax
    5003:	83 e5 7f             	and    ebp,0x7f
    5006:	48 c1 e5 04          	shl    rbp,0x4
    500a:	4c 01 e5             	add    rbp,r12
    500d:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    5011:	41 c1 e8 18          	shr    r8d,0x18
        setfltvalue(ra, -fltvalue(rb));
    5015:	41 ba 03 00 00 00    	mov    r10d,0x3
        TValue *rb = RB(i);
    501b:	44 89 c0             	mov    eax,r8d
        setfltvalue(ra, -fltvalue(rb));
    501e:	48 c1 e0 04          	shl    rax,0x4
    5022:	f2 41 0f 10 04 04    	movsd  xmm0,QWORD PTR [r12+rax*1]
        vmbreak;
    5028:	48 89 d8             	mov    rax,rbx
        setfltvalue(ra, -fltvalue(rb));
    502b:	66 44 89 55 08       	mov    WORD PTR [rbp+0x8],r10w
        vmbreak;
    5030:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, -fltvalue(rb));
    5034:	66 0f 57 05 00 00 00 	xorpd  xmm0,XMMWORD PTR [rip+0x0]        # 503c <luaV_execute+0x334c>
    503b:	00 
    503c:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    5041:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    5044:	45 85 ed             	test   r13d,r13d
    5047:	0f 85 34 3a 00 00    	jne    8a81 <luaV_execute+0x6d91>
    504d:	44 89 c5             	mov    ebp,r8d
    5050:	41 0f b6 c0          	movzx  eax,r8b
    5054:	c1 ed 08             	shr    ebp,0x8
    5057:	48 89 c2             	mov    rdx,rax
    505a:	83 e5 7f             	and    ebp,0x7f
    505d:	48 c1 e5 04          	shl    rbp,0x4
    5061:	4c 01 e5             	add    rbp,r12
    5064:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        vmbreak;
    5068:	48 89 d8             	mov    rax,rbx
        setfltvalue(ra, 0.0);
    506b:	41 bb 03 00 00 00    	mov    r11d,0x3
        vmbreak;
    5071:	48 83 c3 04          	add    rbx,0x4
        setfltvalue(ra, 0.0);
    5075:	48 c7 45 00 00 00 00 	mov    QWORD PTR [rbp+0x0],0x0
    507c:	00 
    507d:	66 44 89 5d 08       	mov    WORD PTR [rbp+0x8],r11w
        vmbreak;
    5082:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    5085:	45 85 ed             	test   r13d,r13d
    5088:	0f 85 50 3b 00 00    	jne    8bde <luaV_execute+0x6eee>
    508e:	44 89 c5             	mov    ebp,r8d
    5091:	41 0f b6 c0          	movzx  eax,r8b
    5095:	c1 ed 08             	shr    ebp,0x8
    5098:	48 89 c2             	mov    rdx,rax
    509b:	83 e5 7f             	and    ebp,0x7f
    509e:	48 c1 e5 04          	shl    rbp,0x4
    50a2:	4c 01 e5             	add    rbp,r12
    50a5:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        vmbreak;
    50a9:	48 89 d8             	mov    rax,rbx
        setivalue(ra, 0);
    50ac:	41 bf 13 00 00 00    	mov    r15d,0x13
        vmbreak;
    50b2:	48 83 c3 04          	add    rbx,0x4
        setivalue(ra, 0);
    50b6:	48 c7 45 00 00 00 00 	mov    QWORD PTR [rbp+0x0],0x0
    50bd:	00 
    50be:	66 44 89 7d 08       	mov    WORD PTR [rbp+0x8],r15w
        vmbreak;
    50c3:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    50c6:	45 85 ed             	test   r13d,r13d
    50c9:	0f 85 44 3b 00 00    	jne    8c13 <luaV_execute+0x6f23>
    50cf:	44 89 c5             	mov    ebp,r8d
    50d2:	41 0f b6 c0          	movzx  eax,r8b
    50d6:	c1 ed 08             	shr    ebp,0x8
    50d9:	48 89 c2             	mov    rdx,rax
    50dc:	83 e5 7f             	and    ebp,0x7f
    50df:	48 c1 e5 04          	shl    rbp,0x4
    50e3:	4c 01 e5             	add    rbp,r12
    50e6:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (!ttisLtable(ra))
    50ea:	66 81 7d 08 05 80    	cmp    WORD PTR [rbp+0x8],0x8005
    50f0:	0f 85 d2 4a 00 00    	jne    9bc8 <luaV_execute+0x7ed8>
        UpVal *uv = cl->upvals[GETARG_B(i)];
    50f6:	41 c1 e8 18          	shr    r8d,0x18
    50fa:	48 8b 7c 24 20       	mov    rdi,QWORD PTR [rsp+0x20]
        setobj(L, uv->v, ra);
    50ff:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
        UpVal *uv = cl->upvals[GETARG_B(i)];
    5103:	49 63 c0             	movsxd rax,r8d
    5106:	48 8b 74 c7 20       	mov    rsi,QWORD PTR [rdi+rax*8+0x20]
        setobj(L, uv->v, ra);
    510b:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    510e:	48 89 10             	mov    QWORD PTR [rax],rdx
    5111:	0f b7 55 08          	movzx  edx,WORD PTR [rbp+0x8]
    5115:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
        luaC_upvalbarrier(L, uv);
    5119:	66 85 d2             	test   dx,dx
    511c:	0f 88 b1 18 00 00    	js     69d3 <luaV_execute+0x4ce3>
        vmbreak;
    5122:	48 89 d8             	mov    rax,rbx
    5125:	48 83 c3 04          	add    rbx,0x4
    5129:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    512c:	45 85 ed             	test   r13d,r13d
    512f:	0f 85 48 3b 00 00    	jne    8c7d <luaV_execute+0x6f8d>
    5135:	44 89 c5             	mov    ebp,r8d
    5138:	41 0f b6 c0          	movzx  eax,r8b
    513c:	c1 ed 08             	shr    ebp,0x8
    513f:	48 89 c2             	mov    rdx,rax
    5142:	83 e5 7f             	and    ebp,0x7f
    5145:	48 c1 e5 04          	shl    rbp,0x4
    5149:	4c 01 e5             	add    rbp,r12
    514c:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (!ttisfarray(ra))
    5150:	66 81 7d 08 25 80    	cmp    WORD PTR [rbp+0x8],0x8025
    5156:	0f 85 98 4a 00 00    	jne    9bf4 <luaV_execute+0x7f04>
        UpVal *uv = cl->upvals[GETARG_B(i)];
    515c:	41 c1 e8 18          	shr    r8d,0x18
    5160:	48 8b 4c 24 20       	mov    rcx,QWORD PTR [rsp+0x20]
        setobj(L, uv->v, ra);
    5165:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
        UpVal *uv = cl->upvals[GETARG_B(i)];
    5169:	49 63 c0             	movsxd rax,r8d
    516c:	48 8b 74 c1 20       	mov    rsi,QWORD PTR [rcx+rax*8+0x20]
        setobj(L, uv->v, ra);
    5171:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    5174:	48 89 10             	mov    QWORD PTR [rax],rdx
    5177:	0f b7 55 08          	movzx  edx,WORD PTR [rbp+0x8]
    517b:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
        luaC_upvalbarrier(L, uv);
    517f:	66 85 d2             	test   dx,dx
    5182:	0f 88 2f 18 00 00    	js     69b7 <luaV_execute+0x4cc7>
        vmbreak;
    5188:	48 89 d8             	mov    rax,rbx
    518b:	48 83 c3 04          	add    rbx,0x4
    518f:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    5192:	45 85 ed             	test   r13d,r13d
    5195:	0f 85 17 3b 00 00    	jne    8cb2 <luaV_execute+0x6fc2>
    519b:	44 89 c5             	mov    ebp,r8d
    519e:	41 0f b6 c0          	movzx  eax,r8b
    51a2:	c1 ed 08             	shr    ebp,0x8
    51a5:	48 89 c2             	mov    rdx,rax
    51a8:	83 e5 7f             	and    ebp,0x7f
    51ab:	48 c1 e5 04          	shl    rbp,0x4
    51af:	4c 01 e5             	add    rbp,r12
    51b2:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (!ttisiarray(ra))
    51b6:	66 81 7d 08 15 80    	cmp    WORD PTR [rbp+0x8],0x8015
    51bc:	0f 85 1c 4a 00 00    	jne    9bde <luaV_execute+0x7eee>
        UpVal *uv = cl->upvals[GETARG_B(i)];
    51c2:	41 c1 e8 18          	shr    r8d,0x18
    51c6:	48 8b 74 24 20       	mov    rsi,QWORD PTR [rsp+0x20]
        setobj(L, uv->v, ra);
    51cb:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
        UpVal *uv = cl->upvals[GETARG_B(i)];
    51cf:	49 63 c0             	movsxd rax,r8d
    51d2:	48 8b 74 c6 20       	mov    rsi,QWORD PTR [rsi+rax*8+0x20]
        setobj(L, uv->v, ra);
    51d7:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    51da:	48 89 10             	mov    QWORD PTR [rax],rdx
    51dd:	0f b7 55 08          	movzx  edx,WORD PTR [rbp+0x8]
    51e1:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
        luaC_upvalbarrier(L, uv);
    51e5:	66 85 d2             	test   dx,dx
    51e8:	0f 88 43 17 00 00    	js     6931 <luaV_execute+0x4c41>
        vmbreak;
    51ee:	48 89 d8             	mov    rax,rbx
    51f1:	48 83 c3 04          	add    rbx,0x4
    51f5:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    51f8:	45 85 ed             	test   r13d,r13d
    51fb:	0f 85 b5 38 00 00    	jne    8ab6 <luaV_execute+0x6dc6>
    5201:	44 89 c5             	mov    ebp,r8d
    5204:	41 0f b6 c0          	movzx  eax,r8b
    5208:	c1 ed 08             	shr    ebp,0x8
    520b:	48 89 c2             	mov    rdx,rax
    520e:	83 e5 7f             	and    ebp,0x7f
    5211:	48 c1 e5 04          	shl    rbp,0x4
    5215:	4c 01 e5             	add    rbp,r12
    5218:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (tonumber(ra, &na)) {
    521c:	66 83 7d 08 03       	cmp    WORD PTR [rbp+0x8],0x3
    5221:	0f 85 c4 38 00 00    	jne    8aeb <luaV_execute+0x6dfb>
    5227:	f2 0f 10 45 00       	movsd  xmm0,QWORD PTR [rbp+0x0]
    522c:	f2 0f 11 84 24 e0 00 	movsd  QWORD PTR [rsp+0xe0],xmm0
    5233:	00 00 
          UpVal *uv = cl->upvals[GETARG_B(i)];
    5235:	41 c1 e8 18          	shr    r8d,0x18
          setfltvalue(uv->v, na);
    5239:	48 8b 7c 24 20       	mov    rdi,QWORD PTR [rsp+0x20]
    523e:	ba 03 00 00 00       	mov    edx,0x3
          UpVal *uv = cl->upvals[GETARG_B(i)];
    5243:	49 63 c0             	movsxd rax,r8d
          setfltvalue(uv->v, na);
    5246:	48 8b 44 c7 20       	mov    rax,QWORD PTR [rdi+rax*8+0x20]
    524b:	48 8b 00             	mov    rax,QWORD PTR [rax]
    524e:	f2 0f 11 00          	movsd  QWORD PTR [rax],xmm0
    5252:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
        vmbreak;
    5256:	48 89 d8             	mov    rax,rbx
    5259:	48 83 c3 04          	add    rbx,0x4
    525d:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    5260:	45 85 ed             	test   r13d,r13d
    5263:	0f 85 47 37 00 00    	jne    89b0 <luaV_execute+0x6cc0>
    5269:	44 89 c5             	mov    ebp,r8d
    526c:	41 0f b6 c0          	movzx  eax,r8b
    5270:	c1 ed 08             	shr    ebp,0x8
    5273:	48 89 c2             	mov    rdx,rax
    5276:	83 e5 7f             	and    ebp,0x7f
    5279:	48 c1 e5 04          	shl    rbp,0x4
    527d:	4c 01 e5             	add    rbp,r12
    5280:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        if (tointeger(ra, &ia)) {
    5284:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    5289:	0f 85 56 37 00 00    	jne    89e5 <luaV_execute+0x6cf5>
    528f:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
    5293:	48 89 94 24 d8 00 00 	mov    QWORD PTR [rsp+0xd8],rdx
    529a:	00 
          UpVal *uv = cl->upvals[GETARG_B(i)];
    529b:	41 c1 e8 18          	shr    r8d,0x18
          setivalue(uv->v, ia);
    529f:	48 8b 4c 24 20       	mov    rcx,QWORD PTR [rsp+0x20]
          UpVal *uv = cl->upvals[GETARG_B(i)];
    52a4:	49 63 c0             	movsxd rax,r8d
          setivalue(uv->v, ia);
    52a7:	48 8b 44 c1 20       	mov    rax,QWORD PTR [rcx+rax*8+0x20]
    52ac:	b9 13 00 00 00       	mov    ecx,0x13
    52b1:	48 8b 00             	mov    rax,QWORD PTR [rax]
    52b4:	48 89 10             	mov    QWORD PTR [rax],rdx
    52b7:	66 89 48 08          	mov    WORD PTR [rax+0x8],cx
        vmbreak;
    52bb:	48 89 d8             	mov    rax,rbx
    52be:	48 83 c3 04          	add    rbx,0x4
    52c2:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    52c5:	45 85 ed             	test   r13d,r13d
    52c8:	0f 85 fe 34 00 00    	jne    87cc <luaV_execute+0x6adc>
    52ce:	44 89 c5             	mov    ebp,r8d
    52d1:	41 0f b6 c0          	movzx  eax,r8b
    52d5:	c1 ed 08             	shr    ebp,0x8
    52d8:	48 89 c2             	mov    rdx,rax
    52db:	83 e5 7f             	and    ebp,0x7f
    52de:	48 c1 e5 04          	shl    rbp,0x4
    52e2:	4c 01 e5             	add    rbp,r12
    52e5:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    52e9:	44 89 c0             	mov    eax,r8d
        Table *t = hvalue(ra);
    52ec:	48 8b 75 00          	mov    rsi,QWORD PTR [rbp+0x0]
        TValue *rb = RKB(i);
    52f0:	c1 e8 18             	shr    eax,0x18
    52f3:	a8 80                	test   al,0x80
    52f5:	0f 84 b1 14 00 00    	je     67ac <luaV_execute+0x4abc>
    52fb:	83 e0 7f             	and    eax,0x7f
    52fe:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5303:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5307:	48 c1 e0 04          	shl    rax,0x4
    530b:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    530f:	41 f6 c0 80          	test   r8b,0x80
    5313:	0f 84 a9 14 00 00    	je     67c2 <luaV_execute+0x4ad2>
    5319:	41 83 e0 7f          	and    r8d,0x7f
    531d:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    5322:	49 c1 e0 04          	shl    r8,0x4
    5326:	4c 01 c0             	add    rax,r8
        lua_Integer idx = ivalue(rb);
    5329:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
        raviH_set_float_inline(L, t, idx, fltvalue(rc));
    532c:	48 8b 7e 38          	mov    rdi,QWORD PTR [rsi+0x38]
    5330:	f2 0f 10 00          	movsd  xmm0,QWORD PTR [rax]
    5334:	89 ca                	mov    edx,ecx
    5336:	39 4e 40             	cmp    DWORD PTR [rsi+0x40],ecx
    5339:	0f 86 bb 1d 00 00    	jbe    70fa <luaV_execute+0x540a>
    533f:	f2 0f 11 04 d7       	movsd  QWORD PTR [rdi+rdx*8],xmm0
        vmbreak;
    5344:	48 89 d8             	mov    rax,rbx
    5347:	48 83 c3 04          	add    rbx,0x4
    534b:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    534e:	45 85 ed             	test   r13d,r13d
    5351:	0f 85 5f 2d 00 00    	jne    80b6 <luaV_execute+0x63c6>
    5357:	44 89 c5             	mov    ebp,r8d
    535a:	41 0f b6 c0          	movzx  eax,r8b
    535e:	c1 ed 08             	shr    ebp,0x8
    5361:	48 89 c2             	mov    rdx,rax
    5364:	83 e5 7f             	and    ebp,0x7f
    5367:	48 c1 e5 04          	shl    rbp,0x4
    536b:	4c 01 e5             	add    rbp,r12
    536e:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    5372:	44 89 c0             	mov    eax,r8d
        Table *t = hvalue(ra);
    5375:	48 8b 6d 00          	mov    rbp,QWORD PTR [rbp+0x0]
        TValue *rb = RKB(i);
    5379:	c1 e8 18             	shr    eax,0x18
    537c:	a8 80                	test   al,0x80
    537e:	0f 85 a9 13 00 00    	jne    672d <luaV_execute+0x4a3d>
    5384:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    5388:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    538c:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    538f:	41 f6 c0 80          	test   r8b,0x80
    5393:	0f 84 ae 13 00 00    	je     6747 <luaV_execute+0x4a57>
    5399:	41 83 e0 7f          	and    r8d,0x7f
    539d:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        lua_Integer idx = ivalue(rb);
    53a2:	4c 8b 38             	mov    r15,QWORD PTR [rax]
        TValue *rc = RKC(i);
    53a5:	49 c1 e0 04          	shl    r8,0x4
    53a9:	4a 8d 3c 06          	lea    rdi,[rsi+r8*1]
        if (ttisfloat(rc)) { raviH_set_float_inline(L, t, idx, fltvalue(rc)); }
    53ad:	0f b7 47 08          	movzx  eax,WORD PTR [rdi+0x8]
    53b1:	66 83 f8 03          	cmp    ax,0x3
    53b5:	0f 84 a9 13 00 00    	je     6764 <luaV_execute+0x4a74>
        else if (ttisinteger(rc)) {
    53bb:	66 83 f8 13          	cmp    ax,0x13
    53bf:	0f 85 44 1d 00 00    	jne    7109 <luaV_execute+0x5419>
          raviH_set_float_inline(L, t, idx, ((lua_Number)ivalue(rc)));
    53c5:	66 0f ef c0          	pxor   xmm0,xmm0
    53c9:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    53cd:	44 89 fa             	mov    edx,r15d
    53d0:	f2 48 0f 2a 07       	cvtsi2sd xmm0,QWORD PTR [rdi]
    53d5:	44 39 7d 40          	cmp    DWORD PTR [rbp+0x40],r15d
    53d9:	0f 86 5a 1f 00 00    	jbe    7339 <luaV_execute+0x5649>
    53df:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    53e4:	e9 95 13 00 00       	jmp    677e <luaV_execute+0x4a8e>
        TValue *rb = RKB(i);
    53e9:	44 89 c0             	mov    eax,r8d
        Table *t = hvalue(ra);
    53ec:	48 8b 75 00          	mov    rsi,QWORD PTR [rbp+0x0]
        TValue *rb = RKB(i);
    53f0:	c1 e8 18             	shr    eax,0x18
    53f3:	a8 80                	test   al,0x80
    53f5:	0f 84 0b 13 00 00    	je     6706 <luaV_execute+0x4a16>
    53fb:	83 e0 7f             	and    eax,0x7f
    53fe:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5403:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5407:	48 c1 e0 04          	shl    rax,0x4
    540b:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    540f:	41 f6 c0 80          	test   r8b,0x80
    5413:	0f 84 03 13 00 00    	je     671c <luaV_execute+0x4a2c>
    5419:	41 83 e0 7f          	and    r8d,0x7f
    541d:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    5422:	49 c1 e0 04          	shl    r8,0x4
    5426:	4c 01 c0             	add    rax,r8
        lua_Integer idx = ivalue(rb);
    5429:	48 8b 3a             	mov    rdi,QWORD PTR [rdx]
        raviH_set_int_inline(L, t, idx, ivalue(rc));
    542c:	4c 8b 46 38          	mov    r8,QWORD PTR [rsi+0x38]
    5430:	48 8b 08             	mov    rcx,QWORD PTR [rax]
    5433:	89 fa                	mov    edx,edi
    5435:	39 7e 40             	cmp    DWORD PTR [rsi+0x40],edi
    5438:	0f 86 ad 1c 00 00    	jbe    70eb <luaV_execute+0x53fb>
    543e:	49 89 0c d0          	mov    QWORD PTR [r8+rdx*8],rcx
        vmbreak;
    5442:	48 89 d8             	mov    rax,rbx
    5445:	48 83 c3 04          	add    rbx,0x4
    5449:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    544c:	45 85 ed             	test   r13d,r13d
    544f:	0f 85 2c 2c 00 00    	jne    8081 <luaV_execute+0x6391>
    5455:	44 89 c5             	mov    ebp,r8d
    5458:	41 0f b6 c0          	movzx  eax,r8b
    545c:	c1 ed 08             	shr    ebp,0x8
    545f:	48 89 c2             	mov    rdx,rax
    5462:	83 e5 7f             	and    ebp,0x7f
    5465:	48 c1 e5 04          	shl    rbp,0x4
    5469:	4c 01 e5             	add    rbp,r12
    546c:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    5470:	44 89 c0             	mov    eax,r8d
        Table *t = hvalue(ra);
    5473:	48 8b 6d 00          	mov    rbp,QWORD PTR [rbp+0x0]
        TValue *rb = RKB(i);
    5477:	c1 e8 18             	shr    eax,0x18
    547a:	a8 80                	test   al,0x80
    547c:	0f 85 56 0c 00 00    	jne    60d8 <luaV_execute+0x43e8>
    5482:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    5486:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    548a:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    548d:	41 f6 c0 80          	test   r8b,0x80
    5491:	0f 84 5b 0c 00 00    	je     60f2 <luaV_execute+0x4402>
    5497:	41 83 e0 7f          	and    r8d,0x7f
    549b:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        lua_Integer idx = ivalue(rb);
    54a0:	4c 8b 38             	mov    r15,QWORD PTR [rax]
        TValue *rc = RKC(i);
    54a3:	49 c1 e0 04          	shl    r8,0x4
    54a7:	4a 8d 3c 06          	lea    rdi,[rsi+r8*1]
        if (ttisinteger(rc)) { raviH_set_int_inline(L, t, idx, ivalue(rc)); }
    54ab:	0f b7 47 08          	movzx  eax,WORD PTR [rdi+0x8]
    54af:	66 83 f8 13          	cmp    ax,0x13
    54b3:	0f 84 56 0c 00 00    	je     610f <luaV_execute+0x441f>
        else if (ttisfloat(rc)) {
    54b9:	66 83 f8 03          	cmp    ax,0x3
    54bd:	0f 85 ef 1b 00 00    	jne    70b2 <luaV_execute+0x53c2>
          raviH_set_int_inline(L, t, idx, (lua_Integer)fltvalue(rc));
    54c3:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    54c7:	f2 48 0f 2c 0f       	cvttsd2si rcx,QWORD PTR [rdi]
    54cc:	44 89 fa             	mov    edx,r15d
    54cf:	44 39 7d 40          	cmp    DWORD PTR [rbp+0x40],r15d
    54d3:	0f 86 20 1f 00 00    	jbe    73f9 <luaV_execute+0x5709>
    54d9:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    54dd:	e9 45 0c 00 00       	jmp    6127 <luaV_execute+0x4437>
        TValue *rb = RB(i);
    54e2:	44 89 c2             	mov    edx,r8d
        TValue *rc = RKC(i);
    54e5:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RB(i);
    54e9:	c1 ea 18             	shr    edx,0x18
    54ec:	48 c1 e2 04          	shl    rdx,0x4
    54f0:	4c 01 e2             	add    rdx,r12
        TValue *rc = RKC(i);
    54f3:	41 f6 c0 80          	test   r8b,0x80
    54f7:	0f 85 c6 0b 00 00    	jne    60c3 <luaV_execute+0x43d3>
    54fd:	45 0f b6 c0          	movzx  r8d,r8b
    5501:	49 c1 e0 04          	shl    r8,0x4
    5505:	4b 8d 04 04          	lea    rax,[r12+r8*1]
        Table *t = hvalue(rb);
    5509:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
        lua_Integer idx = ivalue(rc);
    550c:	48 8b 00             	mov    rax,QWORD PTR [rax]
        raviH_get_float_inline(L, t, idx, ra);
    550f:	48 8b 4a 38          	mov    rcx,QWORD PTR [rdx+0x38]
    5513:	39 42 40             	cmp    DWORD PTR [rdx+0x40],eax
    5516:	0f 86 14 48 00 00    	jbe    9d30 <luaV_execute+0x8040>
    551c:	89 c0                	mov    eax,eax
    551e:	be 03 00 00 00       	mov    esi,0x3
    5523:	f2 0f 10 04 c1       	movsd  xmm0,QWORD PTR [rcx+rax*8]
        vmbreak;
    5528:	48 89 d8             	mov    rax,rbx
        raviH_get_float_inline(L, t, idx, ra);
    552b:	66 89 75 08          	mov    WORD PTR [rbp+0x8],si
        vmbreak;
    552f:	48 83 c3 04          	add    rbx,0x4
        raviH_get_float_inline(L, t, idx, ra);
    5533:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
        vmbreak;
    5538:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    553b:	45 85 ed             	test   r13d,r13d
    553e:	0f 85 ff 2f 00 00    	jne    8543 <luaV_execute+0x6853>
    5544:	44 89 c5             	mov    ebp,r8d
    5547:	41 0f b6 c0          	movzx  eax,r8b
    554b:	c1 ed 08             	shr    ebp,0x8
    554e:	48 89 c2             	mov    rdx,rax
    5551:	83 e5 7f             	and    ebp,0x7f
    5554:	48 c1 e5 04          	shl    rbp,0x4
    5558:	4c 01 e5             	add    rbp,r12
    555b:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RB(i);
    555f:	44 89 c2             	mov    edx,r8d
        TValue *rc = RKC(i);
    5562:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RB(i);
    5566:	c1 ea 18             	shr    edx,0x18
    5569:	48 c1 e2 04          	shl    rdx,0x4
    556d:	4c 01 e2             	add    rdx,r12
        TValue *rc = RKC(i);
    5570:	41 f6 c0 80          	test   r8b,0x80
    5574:	0f 85 34 0b 00 00    	jne    60ae <luaV_execute+0x43be>
    557a:	45 0f b6 c0          	movzx  r8d,r8b
    557e:	49 c1 e0 04          	shl    r8,0x4
    5582:	4b 8d 04 04          	lea    rax,[r12+r8*1]
        Table *t = hvalue(rb);
    5586:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
        lua_Integer idx = ivalue(rc);
    5589:	48 8b 00             	mov    rax,QWORD PTR [rax]
        raviH_get_int_inline(L, t, idx, ra);
    558c:	48 8b 4a 38          	mov    rcx,QWORD PTR [rdx+0x38]
    5590:	39 42 40             	cmp    DWORD PTR [rdx+0x40],eax
    5593:	0f 86 97 47 00 00    	jbe    9d30 <luaV_execute+0x8040>
    5599:	89 c0                	mov    eax,eax
    559b:	bf 13 00 00 00       	mov    edi,0x13
    55a0:	48 8b 04 c1          	mov    rax,QWORD PTR [rcx+rax*8]
    55a4:	66 89 7d 08          	mov    WORD PTR [rbp+0x8],di
    55a8:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
        vmbreak;
    55ac:	48 89 d8             	mov    rax,rbx
    55af:	48 83 c3 04          	add    rbx,0x4
    55b3:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    55b6:	45 85 ed             	test   r13d,r13d
    55b9:	0f 85 b9 2f 00 00    	jne    8578 <luaV_execute+0x6888>
    55bf:	44 89 c5             	mov    ebp,r8d
    55c2:	41 0f b6 c0          	movzx  eax,r8b
    55c6:	c1 ed 08             	shr    ebp,0x8
    55c9:	48 89 c2             	mov    rdx,rax
    55cc:	83 e5 7f             	and    ebp,0x7f
    55cf:	48 c1 e5 04          	shl    rbp,0x4
    55d3:	4c 01 e5             	add    rbp,r12
    55d6:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
    55da:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
        return luaO_nilobject;  /* not found */
    55e0:	4c 8d 05 00 00 00 00 	lea    r8,[rip+0x0]        # 55e7 <luaV_execute+0x38f7>
          if (RAVI_LIKELY(!ttisnil(v))) {
    55e7:	41 0f b7 40 08       	movzx  eax,WORD PTR [r8+0x8]
    55ec:	66 85 c0             	test   ax,ax
    55ef:	0f 85 13 c8 ff ff    	jne    1e08 <luaV_execute+0x118>
            Protect(luaV_finishget(L, rb, rc, ra, v));
    55f5:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    55fa:	48 89 e9             	mov    rcx,rbp
    55fd:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    5602:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    5606:	48 89 ef             	mov    rdi,rbp
    5609:	e8 00 00 00 00       	call   560e <luaV_execute+0x391e>
    560e:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    5615:	00 
        vmbreak;
    5616:	48 89 d8             	mov    rax,rbx
            Protect(luaV_finishget(L, rb, rc, ra, v));
    5619:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
        vmbreak;
    561d:	48 83 c3 04          	add    rbx,0x4
    5621:	44 8b 00             	mov    r8d,DWORD PTR [rax]
            Protect(luaV_finishget(L, rb, rc, ra, v));
    5624:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    5628:	45 85 ed             	test   r13d,r13d
    562b:	0f 84 f5 c7 ff ff    	je     1e26 <luaV_execute+0x136>
    5631:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    5636:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    563b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    5640:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    5644:	48 89 ef             	mov    rdi,rbp
    5647:	e8 00 00 00 00       	call   564c <luaV_execute+0x395c>
    564c:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    5650:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    5655:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    565c:	00 
    565d:	41 83 e5 0c          	and    r13d,0xc
    5661:	e9 c0 c7 ff ff       	jmp    1e26 <luaV_execute+0x136>
    5666:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
    566d:	00 00 00 
    5670:	4c 8d 05 00 00 00 00 	lea    r8,[rip+0x0]        # 5677 <luaV_execute+0x3987>
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
    5677:	66 41 83 78 08 00    	cmp    WORD PTR [r8+0x8],0x0
    567d:	0f 85 eb c9 ff ff    	jne    206e <luaV_execute+0x37e>
    5683:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    5688:	48 89 ee             	mov    rsi,rbp
    568b:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    5690:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    5694:	48 89 ef             	mov    rdi,rbp
    5697:	e8 00 00 00 00       	call   569c <luaV_execute+0x39ac>
    569c:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    56a3:	00 
        vmbreak;
    56a4:	48 89 d8             	mov    rax,rbx
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
    56a7:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
        vmbreak;
    56ab:	48 83 c3 04          	add    rbx,0x4
    56af:	44 8b 00             	mov    r8d,DWORD PTR [rax]
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
    56b2:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    56b6:	45 85 ed             	test   r13d,r13d
    56b9:	0f 84 dc c9 ff ff    	je     209b <luaV_execute+0x3ab>
    56bf:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    56c4:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    56c9:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    56ce:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    56d2:	48 89 ef             	mov    rdi,rbp
    56d5:	e8 00 00 00 00       	call   56da <luaV_execute+0x39ea>
    56da:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    56de:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    56e3:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    56ea:	00 
    56eb:	41 83 e5 0c          	and    r13d,0xc
    56ef:	e9 a7 c9 ff ff       	jmp    209b <luaV_execute+0x3ab>
        TValue *rc = RKC(i);
    56f4:	45 0f b6 c0          	movzx  r8d,r8b
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
    56f8:	0f b7 45 08          	movzx  eax,WORD PTR [rbp+0x8]
        TValue *rc = RKC(i);
    56fc:	49 c1 e0 04          	shl    r8,0x4
    5700:	4b 8d 0c 04          	lea    rcx,[r12+r8*1]
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
    5704:	66 3d 05 80          	cmp    ax,0x8005
    5708:	0f 84 18 c9 ff ff    	je     2026 <luaV_execute+0x336>
    570e:	83 e0 0f             	and    eax,0xf
    5711:	66 83 f8 05          	cmp    ax,0x5
    5715:	0f 84 1d 43 00 00    	je     9a38 <luaV_execute+0x7d48>
    571b:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    5720:	48 89 ee             	mov    rsi,rbp
    5723:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    5728:	45 31 c0             	xor    r8d,r8d
    572b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    572f:	48 89 ef             	mov    rdi,rbp
    5732:	e8 00 00 00 00       	call   5737 <luaV_execute+0x3a47>
    5737:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    573e:	00 
    573f:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    5743:	41 83 e5 0c          	and    r13d,0xc
    5747:	e9 3c c9 ff ff       	jmp    2088 <luaV_execute+0x398>
        TValue *rb = RKB(i);
    574c:	48 c1 e2 04          	shl    rdx,0x4
    5750:	4c 01 e2             	add    rdx,r12
    5753:	e9 a1 c8 ff ff       	jmp    1ff9 <luaV_execute+0x309>
        TValue *rb = RKB(i);
    5758:	83 e0 7f             	and    eax,0x7f
    575b:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5760:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5764:	48 c1 e0 04          	shl    rax,0x4
    5768:	4c 8d 0c 06          	lea    r9,[rsi+rax*1]
        TValue *rc = RKC(i);
    576c:	41 f6 c0 80          	test   r8b,0x80
    5770:	0f 85 0a e0 ff ff    	jne    3780 <luaV_execute+0x1a90>
    5776:	45 0f b6 c0          	movzx  r8d,r8b
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    577a:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
        TValue *rc = RKC(i);
    577f:	49 c1 e0 04          	shl    r8,0x4
    5783:	4f 8d 14 04          	lea    r10,[r12+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5787:	66 83 f8 13          	cmp    ax,0x13
    578b:	0f 85 0f e0 ff ff    	jne    37a0 <luaV_execute+0x1ab0>
    5791:	49 8b 01             	mov    rax,QWORD PTR [r9]
    5794:	48 89 44 24 70       	mov    QWORD PTR [rsp+0x70],rax
    5799:	41 0f b7 42 08       	movzx  eax,WORD PTR [r10+0x8]
    579e:	66 83 f8 13          	cmp    ax,0x13
    57a2:	0f 84 2e 1f 00 00    	je     76d6 <luaV_execute+0x59e6>
  if (!ttisfloat(obj))
    57a8:	66 83 f8 03          	cmp    ax,0x3
    57ac:	0f 85 0b e0 ff ff    	jne    37bd <luaV_execute+0x1acd>
    57b2:	31 d2                	xor    edx,edx
    57b4:	48 8d 74 24 78       	lea    rsi,[rsp+0x78]
    57b9:	4c 89 d7             	mov    rdi,r10
    57bc:	e8 cf aa ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    57c1:	85 c0                	test   eax,eax
    57c3:	0f 84 f4 df ff ff    	je     37bd <luaV_execute+0x1acd>
    57c9:	48 8b 44 24 78       	mov    rax,QWORD PTR [rsp+0x78]
          setivalue(ra, intop(|, ib, ic));
    57ce:	48 0b 44 24 70       	or     rax,QWORD PTR [rsp+0x70]
    57d3:	41 ba 13 00 00 00    	mov    r10d,0x13
    57d9:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    57dd:	66 44 89 55 08       	mov    WORD PTR [rbp+0x8],r10w
    57e2:	e9 0b e0 ff ff       	jmp    37f2 <luaV_execute+0x1b02>
        TValue *rb = RKB(i);
    57e7:	83 e0 7f             	and    eax,0x7f
    57ea:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    57ef:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    57f3:	48 c1 e0 04          	shl    rax,0x4
    57f7:	4c 8d 0c 01          	lea    r9,[rcx+rax*1]
        TValue *rc = RKC(i);
    57fb:	41 f6 c0 80          	test   r8b,0x80
    57ff:	0f 85 b4 de ff ff    	jne    36b9 <luaV_execute+0x19c9>
    5805:	45 0f b6 c0          	movzx  r8d,r8b
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5809:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
        TValue *rc = RKC(i);
    580e:	49 c1 e0 04          	shl    r8,0x4
    5812:	4f 8d 14 04          	lea    r10,[r12+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5816:	66 83 f8 13          	cmp    ax,0x13
    581a:	0f 85 b9 de ff ff    	jne    36d9 <luaV_execute+0x19e9>
    5820:	49 8b 01             	mov    rax,QWORD PTR [r9]
    5823:	48 89 84 24 80 00 00 	mov    QWORD PTR [rsp+0x80],rax
    582a:	00 
    582b:	41 0f b7 42 08       	movzx  eax,WORD PTR [r10+0x8]
    5830:	66 83 f8 13          	cmp    ax,0x13
    5834:	0f 84 e1 1e 00 00    	je     771b <luaV_execute+0x5a2b>
  if (!ttisfloat(obj))
    583a:	66 83 f8 03          	cmp    ax,0x3
    583e:	0f 85 b5 de ff ff    	jne    36f9 <luaV_execute+0x1a09>
    5844:	31 d2                	xor    edx,edx
    5846:	48 8d b4 24 88 00 00 	lea    rsi,[rsp+0x88]
    584d:	00 
    584e:	4c 89 d7             	mov    rdi,r10
    5851:	e8 3a aa ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5856:	85 c0                	test   eax,eax
    5858:	0f 84 9b de ff ff    	je     36f9 <luaV_execute+0x1a09>
    585e:	48 8b 84 24 88 00 00 	mov    rax,QWORD PTR [rsp+0x88]
    5865:	00 
          setivalue(ra, intop(^, ib, ic));
    5866:	48 33 84 24 80 00 00 	xor    rax,QWORD PTR [rsp+0x80]
    586d:	00 
    586e:	41 b9 13 00 00 00    	mov    r9d,0x13
    5874:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    5878:	66 44 89 4d 08       	mov    WORD PTR [rbp+0x8],r9w
    587d:	e9 ac de ff ff       	jmp    372e <luaV_execute+0x1a3e>
        TValue *rb = RKB(i);
    5882:	83 e0 7f             	and    eax,0x7f
    5885:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    588a:	48 c1 e0 04          	shl    rax,0x4
    588e:	48 8d 34 01          	lea    rsi,[rcx+rax*1]
        TValue *rc = RKC(i);
    5892:	44 89 c0             	mov    eax,r8d
    5895:	c1 e8 10             	shr    eax,0x10
    5898:	a8 80                	test   al,0x80
    589a:	0f 85 40 d7 ff ff    	jne    2fe0 <luaV_execute+0x12f0>
    58a0:	0f b6 c0             	movzx  eax,al
    58a3:	48 c1 e0 04          	shl    rax,0x4
    58a7:	49 8d 14 04          	lea    rdx,[r12+rax*1]
    58ab:	e9 40 d7 ff ff       	jmp    2ff0 <luaV_execute+0x1300>
        TValue *rc = RKC(i);
    58b0:	44 89 c2             	mov    edx,r8d
        TValue *rb = RKB(i);
    58b3:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    58b6:	c1 ea 10             	shr    edx,0x10
        TValue *rb = RKB(i);
    58b9:	48 c1 e0 04          	shl    rax,0x4
    58bd:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    58c2:	f6 c2 80             	test   dl,0x80
    58c5:	0f 85 55 d6 ff ff    	jne    2f20 <luaV_execute+0x1230>
    58cb:	0f b6 d2             	movzx  edx,dl
        if (ttisinteger(rb) && ttisinteger(rc))
    58ce:	0f b7 70 08          	movzx  esi,WORD PTR [rax+0x8]
        TValue *rc = RKC(i);
    58d2:	48 c1 e2 04          	shl    rdx,0x4
    58d6:	4c 01 e2             	add    rdx,r12
        if (ttisinteger(rb) && ttisinteger(rc))
    58d9:	66 83 fe 13          	cmp    si,0x13
    58dd:	0f 85 57 d6 ff ff    	jne    2f3a <luaV_execute+0x124a>
    58e3:	0f b7 4a 08          	movzx  ecx,WORD PTR [rdx+0x8]
    58e7:	66 83 f9 13          	cmp    cx,0x13
    58eb:	0f 84 72 1a 00 00    	je     7363 <luaV_execute+0x5673>
        else if (ttisnumber(rb) && ttisnumber(rc))
    58f1:	89 cf                	mov    edi,ecx
    58f3:	83 e7 0f             	and    edi,0xf
    58f6:	66 83 ff 03          	cmp    di,0x3
    58fa:	0f 85 49 d6 ff ff    	jne    2f49 <luaV_execute+0x1259>
          res = LTnum(rb, rc);
    5900:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    5903:	48 8b 38             	mov    rdi,QWORD PTR [rax]
    5906:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    590b:	e8 90 ab ff ff       	call   4a0 <LTnum.isra.2>
    5910:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
        if (res != GETARG_A(i))
    5915:	41 c1 e8 08          	shr    r8d,0x8
    5919:	41 83 e0 7f          	and    r8d,0x7f
    591d:	41 39 c0             	cmp    r8d,eax
    5920:	0f 85 67 d6 ff ff    	jne    2f8d <luaV_execute+0x129d>
          donextjump(ci);
    5926:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    5928:	89 e8                	mov    eax,ebp
    592a:	c1 e8 08             	shr    eax,0x8
    592d:	83 e0 7f             	and    eax,0x7f
    5930:	0f 85 a0 1a 00 00    	jne    73d6 <luaV_execute+0x56e6>
    5936:	89 e8                	mov    eax,ebp
    5938:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    593d:	c1 e8 10             	shr    eax,0x10
    5940:	2d 00 80 00 00       	sub    eax,0x8000
    5945:	44 0f b6 ae c8 00 00 	movzx  r13d,BYTE PTR [rsi+0xc8]
    594c:	00 
    594d:	48 98                	cdqe   
    594f:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    5954:	41 83 e5 0c          	and    r13d,0xc
    5958:	e9 34 d6 ff ff       	jmp    2f91 <luaV_execute+0x12a1>
        TValue *rb = RKB(i);
    595d:	83 e0 7f             	and    eax,0x7f
    5960:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5965:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5969:	48 c1 e0 04          	shl    rax,0x4
    596d:	48 8d 34 07          	lea    rsi,[rdi+rax*1]
        TValue *rc = RKC(i);
    5971:	41 f6 c0 80          	test   r8b,0x80
    5975:	0f 85 ba da ff ff    	jne    3435 <luaV_execute+0x1745>
    597b:	45 0f b6 c0          	movzx  r8d,r8b
        if (ttisinteger(rb) && ttisinteger(rc)) {
    597f:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
        TValue *rc = RKC(i);
    5983:	49 c1 e0 04          	shl    r8,0x4
    5987:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    598b:	66 83 f8 13          	cmp    ax,0x13
    598f:	0f 85 bf da ff ff    	jne    3454 <luaV_execute+0x1764>
    5995:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    5999:	66 83 f8 13          	cmp    ax,0x13
    599d:	0f 84 e2 39 00 00    	je     9385 <luaV_execute+0x7695>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    59a3:	48 8b 0e             	mov    rcx,QWORD PTR [rsi]
    59a6:	66 83 f8 03          	cmp    ax,0x3
    59aa:	0f 85 79 13 00 00    	jne    6d29 <luaV_execute+0x5039>
    59b0:	66 0f ef c0          	pxor   xmm0,xmm0
    59b4:	f2 48 0f 2a c1       	cvtsi2sd xmm0,rcx
    59b9:	f2 0f 10 0a          	movsd  xmm1,QWORD PTR [rdx]
          setfltvalue(ra, luai_numidiv(L, nb, nc));
    59bd:	f2 0f 5e c1          	divsd  xmm0,xmm1
    59c1:	f2 0f 10 1d 00 00 00 	movsd  xmm3,QWORD PTR [rip+0x0]        # 59c9 <luaV_execute+0x3cd9>
    59c8:	00 
    59c9:	f2 0f 10 25 00 00 00 	movsd  xmm4,QWORD PTR [rip+0x0]        # 59d1 <luaV_execute+0x3ce1>
    59d0:	00 
    59d1:	66 0f 28 d0          	movapd xmm2,xmm0
    59d5:	66 0f 28 c8          	movapd xmm1,xmm0
    59d9:	66 0f 54 d3          	andpd  xmm2,xmm3
    59dd:	66 0f 2e e2          	ucomisd xmm4,xmm2
    59e1:	76 37                	jbe    5a1a <luaV_execute+0x3d2a>
    59e3:	f2 48 0f 2c c0       	cvttsd2si rax,xmm0
    59e8:	66 0f ef d2          	pxor   xmm2,xmm2
    59ec:	f2 0f 10 25 00 00 00 	movsd  xmm4,QWORD PTR [rip+0x0]        # 59f4 <luaV_execute+0x3d04>
    59f3:	00 
    59f4:	66 0f 55 d8          	andnpd xmm3,xmm0
    59f8:	f2 48 0f 2a d0       	cvtsi2sd xmm2,rax
    59fd:	66 0f 28 ea          	movapd xmm5,xmm2
    5a01:	f2 0f c2 e8 06       	cmpnlesd xmm5,xmm0
    5a06:	66 0f 28 cd          	movapd xmm1,xmm5
    5a0a:	66 0f 54 cc          	andpd  xmm1,xmm4
    5a0e:	f2 0f 5c d1          	subsd  xmm2,xmm1
    5a12:	66 0f 28 ca          	movapd xmm1,xmm2
    5a16:	66 0f 56 cb          	orpd   xmm1,xmm3
    5a1a:	b8 03 00 00 00       	mov    eax,0x3
    5a1f:	f2 0f 11 4d 00       	movsd  QWORD PTR [rbp+0x0],xmm1
    5a24:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    5a28:	e9 2b 13 00 00       	jmp    6d58 <luaV_execute+0x5068>
        int res = l_isfalse(rb);  /* next assignment may change this value */
    5a2d:	31 d2                	xor    edx,edx
    5a2f:	66 83 f9 01          	cmp    cx,0x1
    5a33:	0f 85 fd d7 ff ff    	jne    3236 <luaV_execute+0x1546>
    5a39:	8b 00                	mov    eax,DWORD PTR [rax]
    5a3b:	31 d2                	xor    edx,edx
    5a3d:	85 c0                	test   eax,eax
    5a3f:	0f 94 c2             	sete   dl
    5a42:	e9 ef d7 ff ff       	jmp    3236 <luaV_execute+0x1546>
        TValue *rb = RKB(i);
    5a47:	83 e0 7f             	and    eax,0x7f
    5a4a:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5a4f:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5a53:	48 c1 e0 04          	shl    rax,0x4
    5a57:	4c 8d 0c 07          	lea    r9,[rdi+rax*1]
        TValue *rc = RKC(i);
    5a5b:	41 f6 c0 80          	test   r8b,0x80
    5a5f:	0f 85 8d db ff ff    	jne    35f2 <luaV_execute+0x1902>
    5a65:	45 0f b6 c0          	movzx  r8d,r8b
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5a69:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
        TValue *rc = RKC(i);
    5a6e:	49 c1 e0 04          	shl    r8,0x4
    5a72:	4f 8d 14 04          	lea    r10,[r12+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5a76:	66 83 f8 13          	cmp    ax,0x13
    5a7a:	0f 85 92 db ff ff    	jne    3612 <luaV_execute+0x1922>
    5a80:	49 8b 01             	mov    rax,QWORD PTR [r9]
    5a83:	48 89 84 24 90 00 00 	mov    QWORD PTR [rsp+0x90],rax
    5a8a:	00 
    5a8b:	41 0f b7 42 08       	movzx  eax,WORD PTR [r10+0x8]
    5a90:	66 83 f8 13          	cmp    ax,0x13
    5a94:	0f 84 71 1c 00 00    	je     770b <luaV_execute+0x5a1b>
  if (!ttisfloat(obj))
    5a9a:	66 83 f8 03          	cmp    ax,0x3
    5a9e:	0f 85 8e db ff ff    	jne    3632 <luaV_execute+0x1942>
    5aa4:	31 d2                	xor    edx,edx
    5aa6:	48 8d b4 24 98 00 00 	lea    rsi,[rsp+0x98]
    5aad:	00 
    5aae:	4c 89 d7             	mov    rdi,r10
    5ab1:	e8 da a7 ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5ab6:	85 c0                	test   eax,eax
    5ab8:	0f 84 74 db ff ff    	je     3632 <luaV_execute+0x1942>
    5abe:	48 8b 8c 24 98 00 00 	mov    rcx,QWORD PTR [rsp+0x98]
    5ac5:	00 
          setivalue(ra, luaV_shiftl(ib, ic));
    5ac6:	48 8b 94 24 90 00 00 	mov    rdx,QWORD PTR [rsp+0x90]
    5acd:	00 
  if (y < 0) {  /* shift right? */
    5ace:	48 85 c9             	test   rcx,rcx
    5ad1:	0f 88 05 22 00 00    	js     7cdc <luaV_execute+0x5fec>
    else return intop(<<, x, y);
    5ad7:	48 d3 e2             	shl    rdx,cl
    5ada:	b8 00 00 00 00       	mov    eax,0x0
    5adf:	48 83 f9 40          	cmp    rcx,0x40
    5ae3:	48 0f 4c c2          	cmovl  rax,rdx
          setivalue(ra, luaV_shiftl(ib, ic));
    5ae7:	41 b8 13 00 00 00    	mov    r8d,0x13
    5aed:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    5af1:	66 44 89 45 08       	mov    WORD PTR [rbp+0x8],r8w
    5af6:	e9 6c db ff ff       	jmp    3667 <luaV_execute+0x1977>
        TValue *rb = RKB(i);
    5afb:	83 e0 7f             	and    eax,0x7f
    5afe:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5b03:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5b07:	48 c1 e0 04          	shl    rax,0x4
    5b0b:	4c 8d 0c 06          	lea    r9,[rsi+rax*1]
        TValue *rc = RKC(i);
    5b0f:	41 f6 c0 80          	test   r8b,0x80
    5b13:	0f 85 12 da ff ff    	jne    352b <luaV_execute+0x183b>
    5b19:	45 0f b6 c0          	movzx  r8d,r8b
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5b1d:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
        TValue *rc = RKC(i);
    5b22:	49 c1 e0 04          	shl    r8,0x4
    5b26:	4f 8d 14 04          	lea    r10,[r12+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5b2a:	66 83 f8 13          	cmp    ax,0x13
    5b2e:	0f 85 17 da ff ff    	jne    354b <luaV_execute+0x185b>
    5b34:	49 8b 01             	mov    rax,QWORD PTR [r9]
    5b37:	48 89 84 24 a0 00 00 	mov    QWORD PTR [rsp+0xa0],rax
    5b3e:	00 
    5b3f:	41 0f b7 42 08       	movzx  eax,WORD PTR [r10+0x8]
    5b44:	66 83 f8 13          	cmp    ax,0x13
    5b48:	0f 84 ea 1b 00 00    	je     7738 <luaV_execute+0x5a48>
  if (!ttisfloat(obj))
    5b4e:	66 83 f8 03          	cmp    ax,0x3
    5b52:	0f 85 13 da ff ff    	jne    356b <luaV_execute+0x187b>
    5b58:	31 d2                	xor    edx,edx
    5b5a:	48 8d b4 24 a8 00 00 	lea    rsi,[rsp+0xa8]
    5b61:	00 
    5b62:	4c 89 d7             	mov    rdi,r10
    5b65:	e8 26 a7 ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    5b6a:	85 c0                	test   eax,eax
    5b6c:	0f 84 f9 d9 ff ff    	je     356b <luaV_execute+0x187b>
    5b72:	48 8b 8c 24 a8 00 00 	mov    rcx,QWORD PTR [rsp+0xa8]
    5b79:	00 
          setivalue(ra, luaV_shiftl(ib, -ic));
    5b7a:	48 8b 94 24 a0 00 00 	mov    rdx,QWORD PTR [rsp+0xa0]
    5b81:	00 
  if (y < 0) {  /* shift right? */
    5b82:	48 85 c9             	test   rcx,rcx
    5b85:	0f 8f 3c 21 00 00    	jg     7cc7 <luaV_execute+0x5fd7>
    if (y <= -NBITS) return 0;
    5b8b:	31 c0                	xor    eax,eax
    if (y >= NBITS) return 0;
    5b8d:	48 83 f9 c1          	cmp    rcx,0xffffffffffffffc1
    5b91:	7c 09                	jl     5b9c <luaV_execute+0x3eac>
          setivalue(ra, luaV_shiftl(ib, -ic));
    5b93:	48 f7 d9             	neg    rcx
    else return intop(<<, x, y);
    5b96:	48 89 d0             	mov    rax,rdx
    5b99:	48 d3 e0             	shl    rax,cl
          setivalue(ra, luaV_shiftl(ib, -ic));
    5b9c:	bf 13 00 00 00       	mov    edi,0x13
    5ba1:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    5ba5:	66 89 7d 08          	mov    WORD PTR [rbp+0x8],di
    5ba9:	e9 f2 d9 ff ff       	jmp    35a0 <luaV_execute+0x18b0>
        TValue *rb = RKB(i);
    5bae:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    5bb2:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5bb6:	49 8d 34 04          	lea    rsi,[r12+rax*1]
        TValue *rc = RKC(i);
    5bba:	41 f6 c0 80          	test   r8b,0x80
    5bbe:	0f 85 f0 d8 ff ff    	jne    34b4 <luaV_execute+0x17c4>
    5bc4:	45 0f b6 c0          	movzx  r8d,r8b
        if (ttisinteger(rb) && ttisinteger(rc)) {
    5bc8:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
        TValue *rc = RKC(i);
    5bcc:	49 c1 e0 04          	shl    r8,0x4
    5bd0:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    5bd4:	66 83 f8 13          	cmp    ax,0x13
    5bd8:	0f 85 f5 d8 ff ff    	jne    34d3 <luaV_execute+0x17e3>
    5bde:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    5be2:	66 83 f8 13          	cmp    ax,0x13
    5be6:	0f 84 bb 37 00 00    	je     93a7 <luaV_execute+0x76b7>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    5bec:	48 8b 0e             	mov    rcx,QWORD PTR [rsi]
    5bef:	66 83 f8 03          	cmp    ax,0x3
    5bf3:	0f 85 d3 10 00 00    	jne    6ccc <luaV_execute+0x4fdc>
    5bf9:	66 0f ef c0          	pxor   xmm0,xmm0
    5bfd:	f2 48 0f 2a c1       	cvtsi2sd xmm0,rcx
    5c02:	f2 0f 10 12          	movsd  xmm2,QWORD PTR [rdx]
          luai_nummod(L, nb, nc, m);
    5c06:	66 0f 28 ca          	movapd xmm1,xmm2
    5c0a:	f2 0f 11 54 24 28    	movsd  QWORD PTR [rsp+0x28],xmm2
    5c10:	e8 00 00 00 00       	call   5c15 <luaV_execute+0x3f25>
    5c15:	f2 0f 10 54 24 28    	movsd  xmm2,QWORD PTR [rsp+0x28]
    5c1b:	66 0f ef db          	pxor   xmm3,xmm3
    5c1f:	66 0f 28 ca          	movapd xmm1,xmm2
    5c23:	f2 0f 59 c8          	mulsd  xmm1,xmm0
    5c27:	66 0f 2f d9          	comisd xmm3,xmm1
    5c2b:	76 04                	jbe    5c31 <luaV_execute+0x3f41>
    5c2d:	f2 0f 58 c2          	addsd  xmm0,xmm2
          setfltvalue(ra, m);
    5c31:	b9 03 00 00 00       	mov    ecx,0x3
    5c36:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    5c3b:	66 89 4d 08          	mov    WORD PTR [rbp+0x8],cx
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    5c3f:	e9 b7 10 00 00       	jmp    6cfb <luaV_execute+0x500b>
        TValue *rc = RKC(i);
    5c44:	44 89 c2             	mov    edx,r8d
        TValue *rb = RKB(i);
    5c47:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    5c4a:	c1 ea 10             	shr    edx,0x10
        TValue *rb = RKB(i);
    5c4d:	48 c1 e0 04          	shl    rax,0x4
    5c51:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5c56:	f6 c2 80             	test   dl,0x80
    5c59:	0f 85 01 d2 ff ff    	jne    2e60 <luaV_execute+0x1170>
    5c5f:	0f b6 d2             	movzx  edx,dl
        if (ttisinteger(rb) && ttisinteger(rc))
    5c62:	0f b7 70 08          	movzx  esi,WORD PTR [rax+0x8]
        TValue *rc = RKC(i);
    5c66:	48 c1 e2 04          	shl    rdx,0x4
    5c6a:	4c 01 e2             	add    rdx,r12
        if (ttisinteger(rb) && ttisinteger(rc))
    5c6d:	66 83 fe 13          	cmp    si,0x13
    5c71:	0f 85 03 d2 ff ff    	jne    2e7a <luaV_execute+0x118a>
    5c77:	0f b7 4a 08          	movzx  ecx,WORD PTR [rdx+0x8]
    5c7b:	66 83 f9 13          	cmp    cx,0x13
    5c7f:	0f 84 2d 16 00 00    	je     72b2 <luaV_execute+0x55c2>
        else if (ttisnumber(rb) && ttisnumber(rc))
    5c85:	89 cf                	mov    edi,ecx
    5c87:	83 e7 0f             	and    edi,0xf
    5c8a:	66 83 ff 03          	cmp    di,0x3
    5c8e:	0f 85 f5 d1 ff ff    	jne    2e89 <luaV_execute+0x1199>
          res = LEnum(rb, rc);
    5c94:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    5c97:	48 8b 38             	mov    rdi,QWORD PTR [rax]
    5c9a:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    5c9f:	e8 ac a6 ff ff       	call   350 <LEnum.isra.4>
    5ca4:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
        if (res != GETARG_A(i))
    5ca9:	41 c1 e8 08          	shr    r8d,0x8
    5cad:	41 83 e0 7f          	and    r8d,0x7f
    5cb1:	41 39 c0             	cmp    r8d,eax
    5cb4:	0f 85 13 d2 ff ff    	jne    2ecd <luaV_execute+0x11dd>
          donextjump(ci);
    5cba:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    5cbc:	89 e8                	mov    eax,ebp
    5cbe:	c1 e8 08             	shr    eax,0x8
    5cc1:	83 e0 7f             	and    eax,0x7f
    5cc4:	0f 85 1c 16 00 00    	jne    72e6 <luaV_execute+0x55f6>
    5cca:	89 e8                	mov    eax,ebp
    5ccc:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    5cd1:	c1 e8 10             	shr    eax,0x10
    5cd4:	2d 00 80 00 00       	sub    eax,0x8000
    5cd9:	44 0f b6 ae c8 00 00 	movzx  r13d,BYTE PTR [rsi+0xc8]
    5ce0:	00 
    5ce1:	48 98                	cdqe   
    5ce3:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    5ce8:	41 83 e5 0c          	and    r13d,0xc
    5cec:	e9 e0 d1 ff ff       	jmp    2ed1 <luaV_execute+0x11e1>
        if (GETARG_C(i) ? l_isfalse(ra) : !l_isfalse(ra))
    5cf1:	0f b7 45 08          	movzx  eax,WORD PTR [rbp+0x8]
    5cf5:	66 85 c0             	test   ax,ax
    5cf8:	74 17                	je     5d11 <luaV_execute+0x4021>
    5cfa:	66 83 f8 01          	cmp    ax,0x1
    5cfe:	0f 85 09 d1 ff ff    	jne    2e0d <luaV_execute+0x111d>
    5d04:	44 8b 5d 00          	mov    r11d,DWORD PTR [rbp+0x0]
    5d08:	45 85 db             	test   r11d,r11d
    5d0b:	0f 85 fc d0 ff ff    	jne    2e0d <luaV_execute+0x111d>
          donextjump(ci);
    5d11:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    5d13:	89 e8                	mov    eax,ebp
    5d15:	c1 e8 08             	shr    eax,0x8
    5d18:	83 e0 7f             	and    eax,0x7f
    5d1b:	0f 85 27 1a 00 00    	jne    7748 <luaV_execute+0x5a58>
    5d21:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
    5d26:	c1 ed 10             	shr    ebp,0x10
    5d29:	8d 85 00 80 ff ff    	lea    eax,[rbp-0x8000]
    5d2f:	44 0f b6 a9 c8 00 00 	movzx  r13d,BYTE PTR [rcx+0xc8]
    5d36:	00 
    5d37:	48 98                	cdqe   
    5d39:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    5d3e:	41 83 e5 0c          	and    r13d,0xc
    5d42:	e9 ca d0 ff ff       	jmp    2e11 <luaV_execute+0x1121>
        TValue *rb = RKB(i);
    5d47:	83 e0 7f             	and    eax,0x7f
    5d4a:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5d4f:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5d53:	48 c1 e0 04          	shl    rax,0x4
    5d57:	48 01 c6             	add    rsi,rax
        TValue *rc = RKC(i);
    5d5a:	41 f6 c0 80          	test   r8b,0x80
    5d5e:	0f 85 27 d6 ff ff    	jne    338b <luaV_execute+0x169b>
    5d64:	45 0f b6 c0          	movzx  r8d,r8b
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    5d68:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
        TValue *rc = RKC(i);
    5d6c:	49 c1 e0 04          	shl    r8,0x4
    5d70:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    5d74:	66 83 f8 03          	cmp    ax,0x3
    5d78:	0f 85 2c d6 ff ff    	jne    33aa <luaV_execute+0x16ba>
    5d7e:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    5d82:	f2 0f 10 06          	movsd  xmm0,QWORD PTR [rsi]
    5d86:	66 83 f8 03          	cmp    ax,0x3
    5d8a:	0f 84 2f 18 00 00    	je     75bf <luaV_execute+0x58cf>
    5d90:	66 83 f8 13          	cmp    ax,0x13
    5d94:	0f 85 1a d6 ff ff    	jne    33b4 <luaV_execute+0x16c4>
    5d9a:	66 0f ef c9          	pxor   xmm1,xmm1
    5d9e:	f2 48 0f 2a 0a       	cvtsi2sd xmm1,QWORD PTR [rdx]
          setfltvalue(ra, luai_numpow(L, nb, nc));
    5da3:	e8 00 00 00 00       	call   5da8 <luaV_execute+0x40b8>
    5da8:	b8 03 00 00 00       	mov    eax,0x3
    5dad:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    5db2:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    5db6:	e9 28 d6 ff ff       	jmp    33e3 <luaV_execute+0x16f3>
        if (GETARG_C(i) ? l_isfalse(rb) : !l_isfalse(rb))
    5dbb:	0f b7 50 08          	movzx  edx,WORD PTR [rax+0x8]
    5dbf:	66 85 d2             	test   dx,dx
    5dc2:	74 16                	je     5dda <luaV_execute+0x40ea>
    5dc4:	66 83 fa 01          	cmp    dx,0x1
    5dc8:	0f 85 f6 cf ff ff    	jne    2dc4 <luaV_execute+0x10d4>
    5dce:	44 8b 08             	mov    r9d,DWORD PTR [rax]
    5dd1:	45 85 c9             	test   r9d,r9d
    5dd4:	0f 85 ea cf ff ff    	jne    2dc4 <luaV_execute+0x10d4>
          setobjs2s(L, ra, rb);
    5dda:	48 8b 10             	mov    rdx,QWORD PTR [rax]
    5ddd:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
    5de1:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
    5de5:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
          donextjump(ci);
    5de9:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    5deb:	89 e8                	mov    eax,ebp
    5ded:	c1 e8 08             	shr    eax,0x8
    5df0:	83 e0 7f             	and    eax,0x7f
    5df3:	0f 85 72 19 00 00    	jne    776b <luaV_execute+0x5a7b>
    5df9:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    5dfe:	c1 ed 10             	shr    ebp,0x10
    5e01:	8d 85 00 80 ff ff    	lea    eax,[rbp-0x8000]
    5e07:	44 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [rdi+0xc8]
    5e0e:	00 
    5e0f:	48 98                	cdqe   
    5e11:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    5e16:	41 83 e5 0c          	and    r13d,0xc
    5e1a:	e9 a9 cf ff ff       	jmp    2dc8 <luaV_execute+0x10d8>
          L->top = ra+b;  /* top signals number of arguments */
    5e1f:	45 89 c0             	mov    r8d,r8d
    5e22:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    5e27:	49 c1 e0 04          	shl    r8,0x4
    5e2b:	4a 8d 44 05 00       	lea    rax,[rbp+r8*1+0x0]
    5e30:	48 89 46 10          	mov    QWORD PTR [rsi+0x10],rax
    5e34:	e9 e9 ce ff ff       	jmp    2d22 <luaV_execute+0x1032>
        TValue *rb = RKB(i);
    5e39:	83 e0 7f             	and    eax,0x7f
    5e3c:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
    5e41:	48 c1 e0 04          	shl    rax,0x4
    5e45:	48 8d 14 06          	lea    rdx,[rsi+rax*1]
        TValue *rc = RKC(i);
    5e49:	44 89 c0             	mov    eax,r8d
    5e4c:	c1 e8 10             	shr    eax,0x10
    5e4f:	a8 80                	test   al,0x80
    5e51:	0f 85 d0 c5 ff ff    	jne    2427 <luaV_execute+0x737>
    5e57:	0f b6 c0             	movzx  eax,al
    5e5a:	48 c1 e0 04          	shl    rax,0x4
    5e5e:	4c 01 e0             	add    rax,r12
    5e61:	e9 cd c5 ff ff       	jmp    2433 <luaV_execute+0x743>
        TValue *rb = RKB(i);
    5e66:	83 e0 7f             	and    eax,0x7f
    5e69:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5e6e:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5e72:	48 c1 e0 04          	shl    rax,0x4
    5e76:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    5e7a:	41 f6 c0 80          	test   r8b,0x80
    5e7e:	0f 85 ec cb ff ff    	jne    2a70 <luaV_execute+0xd80>
    5e84:	45 0f b6 c0          	movzx  r8d,r8b
    5e88:	49 c1 e0 04          	shl    r8,0x4
    5e8c:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    5e90:	e9 eb cb ff ff       	jmp    2a80 <luaV_execute+0xd90>
        TValue *rb = RKB(i);
    5e95:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    5e98:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5e9c:	48 c1 e0 04          	shl    rax,0x4
    5ea0:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5ea5:	41 f6 c0 80          	test   r8b,0x80
    5ea9:	0f 85 41 c7 ff ff    	jne    25f0 <luaV_execute+0x900>
    5eaf:	45 0f b6 c0          	movzx  r8d,r8b
    5eb3:	49 c1 e0 04          	shl    r8,0x4
    5eb7:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        lua_Integer ic = ivalue(rc);
    5ebb:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
        setivalue(ra, luaV_shiftl(ivalue(rb), -ic));
    5ebe:	48 8b 10             	mov    rdx,QWORD PTR [rax]
  if (y < 0) {  /* shift right? */
    5ec1:	48 85 c9             	test   rcx,rcx
    5ec4:	0f 8e 46 c7 ff ff    	jle    2610 <luaV_execute+0x920>
    else return intop(>>, x, -y);
    5eca:	48 d3 ea             	shr    rdx,cl
    5ecd:	b8 00 00 00 00       	mov    eax,0x0
    5ed2:	48 83 f9 40          	cmp    rcx,0x40
    5ed6:	48 0f 4c c2          	cmovl  rax,rdx
    5eda:	e9 42 c7 ff ff       	jmp    2621 <luaV_execute+0x931>
        TValue *rb = RKB(i);
    5edf:	83 e0 7f             	and    eax,0x7f
    5ee2:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
    5ee7:	48 c1 e0 04          	shl    rax,0x4
    5eeb:	48 8d 14 07          	lea    rdx,[rdi+rax*1]
        TValue *rc = RKC(i);
    5eef:	44 89 c0             	mov    eax,r8d
    5ef2:	c1 e8 10             	shr    eax,0x10
    5ef5:	a8 80                	test   al,0x80
    5ef7:	0f 85 2b c6 ff ff    	jne    2528 <luaV_execute+0x838>
    5efd:	0f b6 c0             	movzx  eax,al
    5f00:	48 c1 e0 04          	shl    rax,0x4
    5f04:	4c 01 e0             	add    rax,r12
    5f07:	e9 28 c6 ff ff       	jmp    2534 <luaV_execute+0x844>
        TValue *rc = RKC(i);
    5f0c:	41 83 e0 7f          	and    r8d,0x7f
    5f10:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    5f15:	49 c1 e0 04          	shl    r8,0x4
    5f19:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
    5f1d:	e9 f5 c7 ff ff       	jmp    2717 <luaV_execute+0xa27>
        TValue *rb = RKB(i);
    5f22:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    5f26:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    5f2a:	4c 01 e0             	add    rax,r12
        TValue *rc = RKC(i);
    5f2d:	41 f6 c0 80          	test   r8b,0x80
    5f31:	0f 85 4d c7 ff ff    	jne    2684 <luaV_execute+0x994>
    5f37:	45 0f b6 c0          	movzx  r8d,r8b
    5f3b:	49 c1 e0 04          	shl    r8,0x4
    5f3f:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        setivalue(ra, luaV_shiftl(ivalue(rb), ivalue(rc)));
    5f43:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
    5f46:	48 8b 10             	mov    rdx,QWORD PTR [rax]
  if (y < 0) {  /* shift right? */
    5f49:	48 85 c9             	test   rcx,rcx
    5f4c:	0f 89 52 c7 ff ff    	jns    26a4 <luaV_execute+0x9b4>
    if (y <= -NBITS) return 0;
    5f52:	31 c0                	xor    eax,eax
    5f54:	48 83 f9 c1          	cmp    rcx,0xffffffffffffffc1
    5f58:	0f 8c 56 c7 ff ff    	jl     26b4 <luaV_execute+0x9c4>
    else return intop(>>, x, -y);
    5f5e:	f7 d9                	neg    ecx
    5f60:	48 89 d0             	mov    rax,rdx
    5f63:	48 d3 e8             	shr    rax,cl
    5f66:	e9 49 c7 ff ff       	jmp    26b4 <luaV_execute+0x9c4>
        TValue *rb = RKB(i);
    5f6b:	83 e0 7f             	and    eax,0x7f
    5f6e:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    5f73:	48 c1 e0 04          	shl    rax,0x4
    5f77:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    5f7b:	44 89 c0             	mov    eax,r8d
    5f7e:	c1 e8 10             	shr    eax,0x10
    5f81:	a8 80                	test   al,0x80
    5f83:	0f 85 1a c5 ff ff    	jne    24a3 <luaV_execute+0x7b3>
    5f89:	0f b6 c0             	movzx  eax,al
    5f8c:	48 c1 e0 04          	shl    rax,0x4
    5f90:	4c 01 e0             	add    rax,r12
    5f93:	e9 17 c5 ff ff       	jmp    24af <luaV_execute+0x7bf>
        TValue *rc = RKC(i);
    5f98:	44 89 c2             	mov    edx,r8d
        TValue *rb = RKB(i);
    5f9b:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    5f9e:	c1 ea 10             	shr    edx,0x10
        TValue *rb = RKB(i);
    5fa1:	48 c1 e0 04          	shl    rax,0x4
    5fa5:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    5faa:	f6 c2 80             	test   dl,0x80
    5fad:	0f 85 90 c1 ff ff    	jne    2143 <luaV_execute+0x453>
    5fb3:	0f b6 d2             	movzx  edx,dl
    5fb6:	48 c1 e2 04          	shl    rdx,0x4
    5fba:	4c 01 e2             	add    rdx,r12
    5fbd:	e9 8d c1 ff ff       	jmp    214f <luaV_execute+0x45f>
        TValue *rb = RKB(i);
    5fc2:	83 e0 7f             	and    eax,0x7f
    5fc5:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    5fca:	48 c1 e0 04          	shl    rax,0x4
    5fce:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    5fd2:	44 89 c0             	mov    eax,r8d
    5fd5:	c1 e8 10             	shr    eax,0x10
    5fd8:	a8 80                	test   al,0x80
    5fda:	0f 85 4d c3 ff ff    	jne    232d <luaV_execute+0x63d>
    5fe0:	0f b6 c0             	movzx  eax,al
    5fe3:	48 c1 e0 04          	shl    rax,0x4
    5fe7:	4c 01 e0             	add    rax,r12
    5fea:	e9 4a c3 ff ff       	jmp    2339 <luaV_execute+0x649>
        TValue *rc = RKC(i);
    5fef:	44 89 c2             	mov    edx,r8d
        TValue *rb = RKB(i);
    5ff2:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    5ff5:	c1 ea 10             	shr    edx,0x10
        TValue *rb = RKB(i);
    5ff8:	48 c1 e0 04          	shl    rax,0x4
    5ffc:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6001:	f6 c2 80             	test   dl,0x80
    6004:	0f 85 9f c3 ff ff    	jne    23a9 <luaV_execute+0x6b9>
    600a:	0f b6 d2             	movzx  edx,dl
    600d:	48 c1 e2 04          	shl    rdx,0x4
    6011:	4c 01 e2             	add    rdx,r12
    6014:	e9 9c c3 ff ff       	jmp    23b5 <luaV_execute+0x6c5>
        if (b != 0) L->top = ra+b;  /* else previous instruction set top */
    6019:	45 89 c0             	mov    r8d,r8d
    601c:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
    6021:	49 c1 e0 04          	shl    r8,0x4
    6025:	4a 8d 44 05 00       	lea    rax,[rbp+r8*1+0x0]
    602a:	48 89 41 10          	mov    QWORD PTR [rcx+0x10],rax
    602e:	e9 6e cc ff ff       	jmp    2ca1 <luaV_execute+0xfb1>
        int nres = (b != 0 ? b - 1 : cast_int(L->top - ra));
    6033:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
    6038:	48 8b 40 10          	mov    rax,QWORD PTR [rax+0x10]
    603c:	48 89 c1             	mov    rcx,rax
    603f:	48 89 44 24 10       	mov    QWORD PTR [rsp+0x10],rax
    6044:	48 29 e9             	sub    rcx,rbp
    6047:	48 c1 f9 04          	sar    rcx,0x4
    604b:	e9 fb cb ff ff       	jmp    2c4b <luaV_execute+0xf5b>
        TValue *rb = RKB(i);
    6050:	83 e0 7f             	and    eax,0x7f
    6053:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6058:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    605c:	48 c1 e0 04          	shl    rax,0x4
    6060:	48 8d 14 06          	lea    rdx,[rsi+rax*1]
        TValue *rc = RKC(i);
    6064:	41 f6 c0 80          	test   r8b,0x80
    6068:	0f 85 18 c9 ff ff    	jne    2986 <luaV_execute+0xc96>
    606e:	45 0f b6 c0          	movzx  r8d,r8b
    6072:	49 c1 e0 04          	shl    r8,0x4
    6076:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    607a:	e9 17 c9 ff ff       	jmp    2996 <luaV_execute+0xca6>
        TValue *rb = RKB(i);
    607f:	83 e0 7f             	and    eax,0x7f
    6082:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6087:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    608b:	48 c1 e0 04          	shl    rax,0x4
    608f:	48 8d 14 07          	lea    rdx,[rdi+rax*1]
        TValue *rc = RKC(i);
    6093:	41 f6 c0 80          	test   r8b,0x80
    6097:	0f 85 5e c9 ff ff    	jne    29fb <luaV_execute+0xd0b>
    609d:	45 0f b6 c0          	movzx  r8d,r8b
    60a1:	49 c1 e0 04          	shl    r8,0x4
    60a5:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    60a9:	e9 5d c9 ff ff       	jmp    2a0b <luaV_execute+0xd1b>
        TValue *rc = RKC(i);
    60ae:	41 83 e0 7f          	and    r8d,0x7f
    60b2:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    60b7:	49 c1 e0 04          	shl    r8,0x4
    60bb:	4c 01 c0             	add    rax,r8
    60be:	e9 c3 f4 ff ff       	jmp    5586 <luaV_execute+0x3896>
        TValue *rc = RKC(i);
    60c3:	41 83 e0 7f          	and    r8d,0x7f
    60c7:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    60cc:	49 c1 e0 04          	shl    r8,0x4
    60d0:	4c 01 c0             	add    rax,r8
    60d3:	e9 31 f4 ff ff       	jmp    5509 <luaV_execute+0x3819>
        TValue *rb = RKB(i);
    60d8:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    60db:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    60df:	48 c1 e0 04          	shl    rax,0x4
    60e3:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    60e8:	41 f6 c0 80          	test   r8b,0x80
    60ec:	0f 85 a5 f3 ff ff    	jne    5497 <luaV_execute+0x37a7>
    60f2:	45 0f b6 c0          	movzx  r8d,r8b
        lua_Integer idx = ivalue(rb);
    60f6:	4c 8b 38             	mov    r15,QWORD PTR [rax]
        TValue *rc = RKC(i);
    60f9:	49 c1 e0 04          	shl    r8,0x4
    60fd:	4b 8d 3c 04          	lea    rdi,[r12+r8*1]
        if (ttisinteger(rc)) { raviH_set_int_inline(L, t, idx, ivalue(rc)); }
    6101:	0f b7 47 08          	movzx  eax,WORD PTR [rdi+0x8]
    6105:	66 83 f8 13          	cmp    ax,0x13
    6109:	0f 85 aa f3 ff ff    	jne    54b9 <luaV_execute+0x37c9>
    610f:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    6113:	44 89 fa             	mov    edx,r15d
    6116:	44 39 7d 40          	cmp    DWORD PTR [rbp+0x40],r15d
    611a:	0f 86 7d 11 00 00    	jbe    729d <luaV_execute+0x55ad>
    6120:	48 8b 0f             	mov    rcx,QWORD PTR [rdi]
    6123:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
        vmbreak;
    6127:	48 89 d8             	mov    rax,rbx
    612a:	48 83 c3 04          	add    rbx,0x4
    612e:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    6131:	45 85 ed             	test   r13d,r13d
    6134:	0f 85 c4 1c 00 00    	jne    7dfe <luaV_execute+0x610e>
    613a:	44 89 c5             	mov    ebp,r8d
    613d:	41 0f b6 c0          	movzx  eax,r8b
    6141:	c1 ed 08             	shr    ebp,0x8
    6144:	48 89 c2             	mov    rdx,rax
    6147:	83 e5 7f             	and    ebp,0x7f
    614a:	48 c1 e5 04          	shl    rbp,0x4
    614e:	4c 01 e5             	add    rbp,r12
    6151:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    6155:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    6158:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    615c:	48 c1 e0 04          	shl    rax,0x4
    6160:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6165:	41 f6 c0 80          	test   r8b,0x80
    6169:	0f 85 b9 e8 ff ff    	jne    4a28 <luaV_execute+0x2d38>
    616f:	45 0f b6 c0          	movzx  r8d,r8b
    6173:	49 c1 e0 04          	shl    r8,0x4
    6177:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
    617b:	e9 b9 e8 ff ff       	jmp    4a39 <luaV_execute+0x2d49>
        TValue *rb = RKB(i);
    6180:	83 e0 7f             	and    eax,0x7f
    6183:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6188:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    618c:	48 c1 e0 04          	shl    rax,0x4
    6190:	48 8d 14 07          	lea    rdx,[rdi+rax*1]
        TValue *rc = RKC(i);
    6194:	41 f6 c0 80          	test   r8b,0x80
    6198:	0f 85 ce ed ff ff    	jne    4f6c <luaV_execute+0x327c>
    619e:	45 0f b6 c0          	movzx  r8d,r8b
    61a2:	49 c1 e0 04          	shl    r8,0x4
    61a6:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    61aa:	e9 cd ed ff ff       	jmp    4f7c <luaV_execute+0x328c>
        TValue *rb = RKB(i);
    61af:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    61b2:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    61b6:	48 c1 e0 04          	shl    rax,0x4
    61ba:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    61bf:	41 f6 c0 80          	test   r8b,0x80
    61c3:	0f 85 25 ed ff ff    	jne    4eee <luaV_execute+0x31fe>
    61c9:	45 0f b6 c0          	movzx  r8d,r8b
    61cd:	49 c1 e0 04          	shl    r8,0x4
    61d1:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
    61d5:	e9 25 ed ff ff       	jmp    4eff <luaV_execute+0x320f>
        TValue *rc = RKC(i);
    61da:	41 83 e0 7f          	and    r8d,0x7f
    61de:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    61e3:	49 c1 e0 04          	shl    r8,0x4
    61e7:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
    61eb:	e9 10 e0 ff ff       	jmp    4200 <luaV_execute+0x2510>
        TValue *rc = RKC(i);
    61f0:	41 83 e0 7f          	and    r8d,0x7f
    61f4:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    61f9:	49 c1 e0 04          	shl    r8,0x4
    61fd:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
    6201:	e9 4f df ff ff       	jmp    4155 <luaV_execute+0x2465>
        TValue *rb = RKB(i);
    6206:	83 e0 7f             	and    eax,0x7f
    6209:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    620e:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6212:	48 c1 e0 04          	shl    rax,0x4
    6216:	48 8d 34 01          	lea    rsi,[rcx+rax*1]
        TValue *rc = RKC(i);
    621a:	41 f6 c0 80          	test   r8b,0x80
    621e:	0f 85 83 de ff ff    	jne    40a7 <luaV_execute+0x23b7>
    6224:	45 0f b6 c0          	movzx  r8d,r8b
        if (ttisinteger(rb) && ttisinteger(rc)) {
    6228:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
        TValue *rc = RKC(i);
    622c:	49 c1 e0 04          	shl    r8,0x4
    6230:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    6234:	66 83 f8 13          	cmp    ax,0x13
    6238:	0f 85 88 de ff ff    	jne    40c6 <luaV_execute+0x23d6>
    623e:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    6242:	66 83 f8 13          	cmp    ax,0x13
    6246:	0f 84 bf 11 00 00    	je     740b <luaV_execute+0x571b>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    624c:	48 8b 0e             	mov    rcx,QWORD PTR [rsi]
    624f:	66 83 f8 03          	cmp    ax,0x3
    6253:	0f 85 77 de ff ff    	jne    40d0 <luaV_execute+0x23e0>
    6259:	66 0f ef c9          	pxor   xmm1,xmm1
    625d:	f2 48 0f 2a c9       	cvtsi2sd xmm1,rcx
    6262:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    6266:	e9 a7 0f 00 00       	jmp    7212 <luaV_execute+0x5522>
        TValue *rb = RKB(i);
    626b:	83 e0 7f             	and    eax,0x7f
    626e:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6273:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6277:	48 c1 e0 04          	shl    rax,0x4
    627b:	48 8d 34 07          	lea    rsi,[rdi+rax*1]
        TValue *rc = RKC(i);
    627f:	41 f6 c0 80          	test   r8b,0x80
    6283:	0f 85 74 dd ff ff    	jne    3ffd <luaV_execute+0x230d>
    6289:	45 0f b6 c0          	movzx  r8d,r8b
        if (ttisinteger(rb) && ttisinteger(rc)) {
    628d:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
        TValue *rc = RKC(i);
    6291:	49 c1 e0 04          	shl    r8,0x4
    6295:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    6299:	66 83 f8 13          	cmp    ax,0x13
    629d:	0f 85 79 dd ff ff    	jne    401c <luaV_execute+0x232c>
    62a3:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    62a7:	66 83 f8 13          	cmp    ax,0x13
    62ab:	0f 84 9a 10 00 00    	je     734b <luaV_execute+0x565b>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    62b1:	48 8b 0e             	mov    rcx,QWORD PTR [rsi]
    62b4:	66 83 f8 03          	cmp    ax,0x3
    62b8:	0f 85 68 dd ff ff    	jne    4026 <luaV_execute+0x2336>
    62be:	66 0f ef c0          	pxor   xmm0,xmm0
    62c2:	f2 48 0f 2a c1       	cvtsi2sd xmm0,rcx
    62c7:	f2 0f 10 0a          	movsd  xmm1,QWORD PTR [rdx]
    62cb:	e9 ca 0e 00 00       	jmp    719a <luaV_execute+0x54aa>
        TValue *rb = RKB(i);
    62d0:	83 e0 7f             	and    eax,0x7f
    62d3:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    62d8:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    62dc:	48 c1 e0 04          	shl    rax,0x4
    62e0:	48 01 c6             	add    rsi,rax
        TValue *rc = RKC(i);
    62e3:	41 f6 c0 80          	test   r8b,0x80
    62e7:	0f 85 66 dc ff ff    	jne    3f53 <luaV_execute+0x2263>
    62ed:	45 0f b6 c0          	movzx  r8d,r8b
        if (ttisinteger(rb) && ttisinteger(rc)) {
    62f1:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
        TValue *rc = RKC(i);
    62f5:	49 c1 e0 04          	shl    r8,0x4
    62f9:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        if (ttisinteger(rb) && ttisinteger(rc)) {
    62fd:	66 83 f8 13          	cmp    ax,0x13
    6301:	0f 85 6b dc ff ff    	jne    3f72 <luaV_execute+0x2282>
    6307:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    630b:	66 83 f8 13          	cmp    ax,0x13
    630f:	0f 84 0e 11 00 00    	je     7423 <luaV_execute+0x5733>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    6315:	48 8b 0e             	mov    rcx,QWORD PTR [rsi]
    6318:	66 83 f8 03          	cmp    ax,0x3
    631c:	0f 85 5a dc ff ff    	jne    3f7c <luaV_execute+0x228c>
    6322:	66 0f ef c9          	pxor   xmm1,xmm1
    6326:	f2 48 0f 2a c9       	cvtsi2sd xmm1,rcx
    632b:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    632f:	e9 a2 0e 00 00       	jmp    71d6 <luaV_execute+0x54e6>
        TValue *rb = RKB(i);
    6334:	83 e0 7f             	and    eax,0x7f
    6337:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    633c:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6340:	48 c1 e0 04          	shl    rax,0x4
    6344:	48 8d 34 01          	lea    rsi,[rcx+rax*1]
        TValue *rc = RKC(i);
    6348:	41 f6 c0 80          	test   r8b,0x80
    634c:	0f 85 57 db ff ff    	jne    3ea9 <luaV_execute+0x21b9>
    6352:	45 0f b6 c0          	movzx  r8d,r8b
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    6356:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
        TValue *rc = RKC(i);
    635a:	49 c1 e0 04          	shl    r8,0x4
    635e:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    6362:	66 83 f8 03          	cmp    ax,0x3
    6366:	0f 85 5c db ff ff    	jne    3ec8 <luaV_execute+0x21d8>
    636c:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    6370:	f2 0f 10 06          	movsd  xmm0,QWORD PTR [rsi]
    6374:	66 83 f8 03          	cmp    ax,0x3
    6378:	0f 84 61 12 00 00    	je     75df <luaV_execute+0x58ef>
    637e:	66 83 f8 13          	cmp    ax,0x13
    6382:	0f 85 4a db ff ff    	jne    3ed2 <luaV_execute+0x21e2>
    6388:	66 0f ef c9          	pxor   xmm1,xmm1
    638c:	f2 48 0f 2a 0a       	cvtsi2sd xmm1,QWORD PTR [rdx]
          setfltvalue(ra, luai_numdiv(L, nb, nc));
    6391:	f2 0f 5e c1          	divsd  xmm0,xmm1
    6395:	41 bf 03 00 00 00    	mov    r15d,0x3
    639b:	66 44 89 7d 08       	mov    WORD PTR [rbp+0x8],r15w
    63a0:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    63a5:	e9 57 db ff ff       	jmp    3f01 <luaV_execute+0x2211>
        TValue *rb = RKB(i);
    63aa:	83 e0 7f             	and    eax,0x7f
    63ad:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    63b2:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    63b6:	48 c1 e0 04          	shl    rax,0x4
    63ba:	4c 8d 0c 07          	lea    r9,[rdi+rax*1]
        TValue *rc = RKC(i);
    63be:	41 f6 c0 80          	test   r8b,0x80
    63c2:	0f 85 1d da ff ff    	jne    3de5 <luaV_execute+0x20f5>
    63c8:	45 0f b6 c0          	movzx  r8d,r8b
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    63cc:	41 0f b7 41 08       	movzx  eax,WORD PTR [r9+0x8]
        TValue *rc = RKC(i);
    63d1:	49 c1 e0 04          	shl    r8,0x4
    63d5:	4f 8d 14 04          	lea    r10,[r12+r8*1]
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    63d9:	66 83 f8 13          	cmp    ax,0x13
    63dd:	0f 85 22 da ff ff    	jne    3e05 <luaV_execute+0x2115>
    63e3:	49 8b 01             	mov    rax,QWORD PTR [r9]
    63e6:	48 89 44 24 60       	mov    QWORD PTR [rsp+0x60],rax
    63eb:	41 0f b7 42 08       	movzx  eax,WORD PTR [r10+0x8]
    63f0:	66 83 f8 13          	cmp    ax,0x13
    63f4:	0f 84 31 13 00 00    	je     772b <luaV_execute+0x5a3b>
  if (!ttisfloat(obj))
    63fa:	66 83 f8 03          	cmp    ax,0x3
    63fe:	0f 85 1e da ff ff    	jne    3e22 <luaV_execute+0x2132>
    6404:	31 d2                	xor    edx,edx
    6406:	48 8d 74 24 68       	lea    rsi,[rsp+0x68]
    640b:	4c 89 d7             	mov    rdi,r10
    640e:	e8 7d 9e ff ff       	call   290 <luaV_flttointeger.part.7>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    6413:	85 c0                	test   eax,eax
    6415:	0f 84 07 da ff ff    	je     3e22 <luaV_execute+0x2132>
    641b:	48 8b 44 24 68       	mov    rax,QWORD PTR [rsp+0x68]
          setivalue(ra, intop(&, ib, ic));
    6420:	48 23 44 24 60       	and    rax,QWORD PTR [rsp+0x60]
    6425:	41 bb 13 00 00 00    	mov    r11d,0x13
    642b:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    642f:	66 44 89 5d 08       	mov    WORD PTR [rbp+0x8],r11w
    6434:	e9 1e da ff ff       	jmp    3e57 <luaV_execute+0x2167>
        TValue *rc = RKC(i);                           /* key */
    6439:	41 83 e0 7f          	and    r8d,0x7f
    643d:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    6442:	49 c1 e0 04          	shl    r8,0x4
    6446:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
    644a:	e9 95 d8 ff ff       	jmp    3ce4 <luaV_execute+0x1ff4>
        TValue *rc = RKC(i);                           /* key */
    644f:	41 83 e0 7f          	and    r8d,0x7f
    6453:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    6458:	49 c1 e0 04          	shl    r8,0x4
    645c:	4a 8d 14 00          	lea    rdx,[rax+r8*1]
    6460:	e9 cf d7 ff ff       	jmp    3c34 <luaV_execute+0x1f44>
        if (!ttisnil(ra) && RAVI_UNLIKELY(!ttisstring(ra)))
    6465:	83 e0 0f             	and    eax,0xf
    6468:	66 83 f8 04          	cmp    ax,0x4
    646c:	0f 84 3a e1 ff ff    	je     45ac <luaV_execute+0x28bc>
    6472:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "string expected");        
    6477:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 647e <luaV_execute+0x478e>
    647e:	31 c0                	xor    eax,eax
    6480:	4c 89 ef             	mov    rdi,r13
    6483:	e8 00 00 00 00       	call   6488 <luaV_execute+0x4798>
        if (!ttisnil(ra) && RAVI_UNLIKELY(!ttisclosure(ra)))
    6488:	83 e0 1f             	and    eax,0x1f
    648b:	66 83 f8 06          	cmp    ax,0x6
    648f:	0f 84 dc e0 ff ff    	je     4571 <luaV_execute+0x2881>
    6495:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "closure expected");
    649a:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 64a1 <luaV_execute+0x47b1>
    64a1:	31 c0                	xor    eax,eax
    64a3:	4c 89 ef             	mov    rdi,r13
    64a6:	e8 00 00 00 00       	call   64ab <luaV_execute+0x47bb>
        TValue *rb = RKB(i);
    64ab:	83 e0 7f             	and    eax,0x7f
    64ae:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    64b3:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    64b7:	48 c1 e0 04          	shl    rax,0x4
    64bb:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    64bf:	41 f6 c0 80          	test   r8b,0x80
    64c3:	0f 85 39 e9 ff ff    	jne    4e02 <luaV_execute+0x3112>
    64c9:	45 0f b6 c0          	movzx  r8d,r8b
    64cd:	49 c1 e0 04          	shl    r8,0x4
    64d1:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    64d5:	e9 38 e9 ff ff       	jmp    4e12 <luaV_execute+0x3122>
        TValue *rb = RKB(i);
    64da:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    64dd:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    64e1:	48 c1 e0 04          	shl    rax,0x4
    64e5:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    64ea:	41 f6 c0 80          	test   r8b,0x80
    64ee:	0f 85 8c e8 ff ff    	jne    4d80 <luaV_execute+0x3090>
    64f4:	45 0f b6 c0          	movzx  r8d,r8b
    64f8:	49 c1 e0 04          	shl    r8,0x4
    64fc:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
    6500:	e9 8c e8 ff ff       	jmp    4d91 <luaV_execute+0x30a1>
        TValue *rb = RKB(i);
    6505:	83 e0 7f             	and    eax,0x7f
    6508:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    650d:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6511:	48 c1 e0 04          	shl    rax,0x4
    6515:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    6519:	41 f6 c0 80          	test   r8b,0x80
    651d:	0f 85 e1 e7 ff ff    	jne    4d04 <luaV_execute+0x3014>
    6523:	45 0f b6 c0          	movzx  r8d,r8b
    6527:	49 c1 e0 04          	shl    r8,0x4
    652b:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    652f:	e9 e0 e7 ff ff       	jmp    4d14 <luaV_execute+0x3024>
        TValue *rb = RKB(i);
    6534:	83 e0 7f             	and    eax,0x7f
    6537:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    653c:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6540:	48 c1 e0 04          	shl    rax,0x4
    6544:	48 8d 14 07          	lea    rdx,[rdi+rax*1]
        TValue *rc = RKC(i);
    6548:	41 f6 c0 80          	test   r8b,0x80
    654c:	0f 85 3d e7 ff ff    	jne    4c8f <luaV_execute+0x2f9f>
    6552:	45 0f b6 c0          	movzx  r8d,r8b
    6556:	49 c1 e0 04          	shl    r8,0x4
    655a:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    655e:	e9 3c e7 ff ff       	jmp    4c9f <luaV_execute+0x2faf>
        TValue *rb = RKB(i);
    6563:	83 e0 7f             	and    eax,0x7f
    6566:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    656b:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    656f:	48 c1 e0 04          	shl    rax,0x4
    6573:	48 8d 14 06          	lea    rdx,[rsi+rax*1]
        TValue *rc = RKC(i);
    6577:	41 f6 c0 80          	test   r8b,0x80
    657b:	0f 85 96 e6 ff ff    	jne    4c17 <luaV_execute+0x2f27>
    6581:	45 0f b6 c0          	movzx  r8d,r8b
    6585:	49 c1 e0 04          	shl    r8,0x4
    6589:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    658d:	e9 95 e6 ff ff       	jmp    4c27 <luaV_execute+0x2f37>
        TValue *rb = RKB(i);
    6592:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    6595:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6599:	48 c1 e0 04          	shl    rax,0x4
    659d:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    65a2:	41 f6 c0 80          	test   r8b,0x80
    65a6:	0f 85 ed e5 ff ff    	jne    4b99 <luaV_execute+0x2ea9>
    65ac:	45 0f b6 c0          	movzx  r8d,r8b
    65b0:	49 c1 e0 04          	shl    r8,0x4
    65b4:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
    65b8:	e9 ed e5 ff ff       	jmp    4baa <luaV_execute+0x2eba>
        TValue *rb = RKB(i);
    65bd:	83 e0 7f             	and    eax,0x7f
    65c0:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    65c5:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    65c9:	48 c1 e0 04          	shl    rax,0x4
    65cd:	48 8d 14 06          	lea    rdx,[rsi+rax*1]
        TValue *rc = RKC(i);
    65d1:	41 f6 c0 80          	test   r8b,0x80
    65d5:	0f 85 49 e5 ff ff    	jne    4b24 <luaV_execute+0x2e34>
    65db:	45 0f b6 c0          	movzx  r8d,r8b
    65df:	49 c1 e0 04          	shl    r8,0x4
    65e3:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    65e7:	e9 48 e5 ff ff       	jmp    4b34 <luaV_execute+0x2e44>
        TValue *rb = RKB(i);
    65ec:	83 e0 7f             	and    eax,0x7f
    65ef:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    65f4:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    65f8:	48 c1 e0 04          	shl    rax,0x4
    65fc:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    6600:	41 f6 c0 80          	test   r8b,0x80
    6604:	0f 85 a2 e4 ff ff    	jne    4aac <luaV_execute+0x2dbc>
    660a:	45 0f b6 c0          	movzx  r8d,r8b
    660e:	49 c1 e0 04          	shl    r8,0x4
    6612:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    6616:	e9 a1 e4 ff ff       	jmp    4abc <luaV_execute+0x2dcc>
        TValue *rb = RKB(i);
    661b:	83 e0 7f             	and    eax,0x7f
    661e:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6623:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6627:	48 c1 e0 04          	shl    rax,0x4
    662b:	4c 8d 3c 07          	lea    r15,[rdi+rax*1]
        TValue *rc = RKC(i);
    662f:	41 f6 c0 80          	test   r8b,0x80
    6633:	0f 85 57 d4 ff ff    	jne    3a90 <luaV_execute+0x1da0>
    6639:	45 0f b6 c0          	movzx  r8d,r8b
    663d:	49 c1 e0 04          	shl    r8,0x4
    6641:	4b 8d 0c 04          	lea    rcx,[r12+r8*1]
    6645:	e9 57 d4 ff ff       	jmp    3aa1 <luaV_execute+0x1db1>
        TValue *rb = RKB(i);
    664a:	83 e0 7f             	and    eax,0x7f
    664d:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6652:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6656:	48 c1 e0 04          	shl    rax,0x4
    665a:	4c 8d 3c 06          	lea    r15,[rsi+rax*1]
        TValue *rc = RKC(i);
    665e:	41 f6 c0 80          	test   r8b,0x80
    6662:	0f 85 6d d3 ff ff    	jne    39d5 <luaV_execute+0x1ce5>
    6668:	45 0f b6 c0          	movzx  r8d,r8b
    666c:	49 c1 e0 04          	shl    r8,0x4
    6670:	4b 8d 0c 04          	lea    rcx,[r12+r8*1]
    6674:	e9 6d d3 ff ff       	jmp    39e6 <luaV_execute+0x1cf6>
        TValue *rb = RKB(i);
    6679:	83 e0 7f             	and    eax,0x7f
    667c:	48 8b 74 24 10       	mov    rsi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    6681:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6685:	48 c1 e0 04          	shl    rax,0x4
    6689:	48 8d 2c 06          	lea    rbp,[rsi+rax*1]
        TValue *rc = RKC(i);
    668d:	41 f6 c0 80          	test   r8b,0x80
    6691:	0f 85 d4 d4 ff ff    	jne    3b6b <luaV_execute+0x1e7b>
    6697:	45 0f b6 c0          	movzx  r8d,r8b
    669b:	49 c1 e0 04          	shl    r8,0x4
    669f:	4b 8d 0c 04          	lea    rcx,[r12+r8*1]
    66a3:	e9 d4 d4 ff ff       	jmp    3b7c <luaV_execute+0x1e8c>
        TValue *rb = RKB(i);
    66a8:	83 e0 7f             	and    eax,0x7f
    66ab:	48 8b 7c 24 10       	mov    rdi,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    66b0:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    66b4:	48 c1 e0 04          	shl    rax,0x4
    66b8:	48 8d 14 07          	lea    rdx,[rdi+rax*1]
        TValue *rc = RKC(i);
    66bc:	41 f6 c0 80          	test   r8b,0x80
    66c0:	0f 85 5c e2 ff ff    	jne    4922 <luaV_execute+0x2c32>
    66c6:	45 0f b6 c0          	movzx  r8d,r8b
    66ca:	49 c1 e0 04          	shl    r8,0x4
    66ce:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    66d2:	e9 5b e2 ff ff       	jmp    4932 <luaV_execute+0x2c42>
        TValue *rb = RKB(i);
    66d7:	83 e0 7f             	and    eax,0x7f
    66da:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    66df:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    66e3:	48 c1 e0 04          	shl    rax,0x4
    66e7:	48 8d 14 01          	lea    rdx,[rcx+rax*1]
        TValue *rc = RKC(i);
    66eb:	41 f6 c0 80          	test   r8b,0x80
    66ef:	0f 85 b5 e2 ff ff    	jne    49aa <luaV_execute+0x2cba>
    66f5:	45 0f b6 c0          	movzx  r8d,r8b
    66f9:	49 c1 e0 04          	shl    r8,0x4
    66fd:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    6701:	e9 b4 e2 ff ff       	jmp    49ba <luaV_execute+0x2cca>
        TValue *rb = RKB(i);
    6706:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    670a:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    670e:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    6712:	41 f6 c0 80          	test   r8b,0x80
    6716:	0f 85 fd ec ff ff    	jne    5419 <luaV_execute+0x3729>
    671c:	45 0f b6 c0          	movzx  r8d,r8b
    6720:	49 c1 e0 04          	shl    r8,0x4
    6724:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    6728:	e9 fc ec ff ff       	jmp    5429 <luaV_execute+0x3739>
        TValue *rb = RKB(i);
    672d:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    6730:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    6734:	48 c1 e0 04          	shl    rax,0x4
    6738:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    673d:	41 f6 c0 80          	test   r8b,0x80
    6741:	0f 85 52 ec ff ff    	jne    5399 <luaV_execute+0x36a9>
    6747:	45 0f b6 c0          	movzx  r8d,r8b
        lua_Integer idx = ivalue(rb);
    674b:	4c 8b 38             	mov    r15,QWORD PTR [rax]
        TValue *rc = RKC(i);
    674e:	49 c1 e0 04          	shl    r8,0x4
    6752:	4b 8d 3c 04          	lea    rdi,[r12+r8*1]
        if (ttisfloat(rc)) { raviH_set_float_inline(L, t, idx, fltvalue(rc)); }
    6756:	0f b7 47 08          	movzx  eax,WORD PTR [rdi+0x8]
    675a:	66 83 f8 03          	cmp    ax,0x3
    675e:	0f 85 57 ec ff ff    	jne    53bb <luaV_execute+0x36cb>
    6764:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    6768:	44 89 fa             	mov    edx,r15d
    676b:	44 39 7d 40          	cmp    DWORD PTR [rbp+0x40],r15d
    676f:	0f 86 ff 0b 00 00    	jbe    7374 <luaV_execute+0x5684>
    6775:	f2 0f 10 07          	movsd  xmm0,QWORD PTR [rdi]
    6779:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
        vmbreak;
    677e:	48 89 d8             	mov    rax,rbx
    6781:	48 83 c3 04          	add    rbx,0x4
    6785:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    6788:	45 85 ed             	test   r13d,r13d
    678b:	0f 85 a2 16 00 00    	jne    7e33 <luaV_execute+0x6143>
    6791:	44 89 c5             	mov    ebp,r8d
    6794:	41 0f b6 c0          	movzx  eax,r8b
    6798:	c1 ed 08             	shr    ebp,0x8
    679b:	48 89 c2             	mov    rdx,rax
    679e:	83 e5 7f             	and    ebp,0x7f
    67a1:	48 c1 e5 04          	shl    rbp,0x4
    67a5:	4c 01 e5             	add    rbp,r12
    67a8:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        TValue *rb = RKB(i);
    67ac:	48 c1 e0 04          	shl    rax,0x4
        TValue *rc = RKC(i);
    67b0:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    67b4:	49 8d 14 04          	lea    rdx,[r12+rax*1]
        TValue *rc = RKC(i);
    67b8:	41 f6 c0 80          	test   r8b,0x80
    67bc:	0f 85 57 eb ff ff    	jne    5319 <luaV_execute+0x3629>
    67c2:	45 0f b6 c0          	movzx  r8d,r8b
    67c6:	49 c1 e0 04          	shl    r8,0x4
    67ca:	4b 8d 04 04          	lea    rax,[r12+r8*1]
    67ce:	e9 56 eb ff ff       	jmp    5329 <luaV_execute+0x3639>
        TValue *rb = RKB(i);
    67d3:	83 e0 7f             	and    eax,0x7f
        TValue *rc = RKC(i);
    67d6:	41 c1 e8 10          	shr    r8d,0x10
        TValue *rb = RKB(i);
    67da:	48 c1 e0 04          	shl    rax,0x4
    67de:	48 03 44 24 10       	add    rax,QWORD PTR [rsp+0x10]
        TValue *rc = RKC(i);
    67e3:	41 f6 c0 80          	test   r8b,0x80
    67e7:	0f 85 8c e6 ff ff    	jne    4e79 <luaV_execute+0x3189>
    67ed:	45 0f b6 c0          	movzx  r8d,r8b
    67f1:	49 c1 e0 04          	shl    r8,0x4
    67f5:	4b 8d 14 04          	lea    rdx,[r12+r8*1]
    67f9:	e9 8c e6 ff ff       	jmp    4e8a <luaV_execute+0x319a>
    67fe:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    6803:	48 8b 40 28          	mov    rax,QWORD PTR [rax+0x28]
    6807:	e9 2d b5 ff ff       	jmp    1d39 <luaV_execute+0x49>
            pc += GETARG_sBx(i);  /* jump back */
    680c:	41 c1 e8 10          	shr    r8d,0x10
            setivalue(ra + 3, idx);  /* ...and external index */
    6810:	be 13 00 00 00       	mov    esi,0x13
            chgivalue(ra, idx);  /* update internal index... */
    6815:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
            pc += GETARG_sBx(i);  /* jump back */
    6819:	44 89 c1             	mov    ecx,r8d
            setivalue(ra + 3, idx);  /* ...and external index */
    681c:	48 89 55 30          	mov    QWORD PTR [rbp+0x30],rdx
            pc += GETARG_sBx(i);  /* jump back */
    6820:	48 8d 84 8b 00 00 fe 	lea    rax,[rbx+rcx*4-0x20000]
    6827:	ff 
            setivalue(ra + 3, idx);  /* ...and external index */
    6828:	66 89 75 38          	mov    WORD PTR [rbp+0x38],si
    682c:	e9 be c3 ff ff       	jmp    2bef <luaV_execute+0xeff>
        if (cl->p->sizep > 0) luaF_close(L, base);
    6831:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6836:	4c 89 e6             	mov    rsi,r12
    6839:	e8 00 00 00 00       	call   683e <luaV_execute+0x4b4e>
    683e:	e9 f2 c3 ff ff       	jmp    2c35 <luaV_execute+0xf45>
        checkGC(L, ra + 1);
    6843:	48 83 c5 10          	add    rbp,0x10
    6847:	4c 89 ff             	mov    rdi,r15
    684a:	49 89 6f 10          	mov    QWORD PTR [r15+0x10],rbp
    684e:	e8 00 00 00 00       	call   6853 <luaV_execute+0x4b63>
    6853:	48 8b 7c 24 18       	mov    rdi,QWORD PTR [rsp+0x18]
    6858:	48 8b 47 08          	mov    rax,QWORD PTR [rdi+0x8]
    685c:	4c 8b 67 20          	mov    r12,QWORD PTR [rdi+0x20]
    6860:	48 89 5f 28          	mov    QWORD PTR [rdi+0x28],rbx
    6864:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    686b:	00 
    686c:	49 89 47 10          	mov    QWORD PTR [r15+0x10],rax
    6870:	41 83 e5 0c          	and    r13d,0xc
    6874:	e9 52 bf ff ff       	jmp    27cb <luaV_execute+0xadb>
        checkGC(L, ra + 1);
    6879:	48 83 c5 10          	add    rbp,0x10
    687d:	4c 89 ff             	mov    rdi,r15
    6880:	49 89 6f 10          	mov    QWORD PTR [r15+0x10],rbp
    6884:	e8 00 00 00 00       	call   6889 <luaV_execute+0x4b99>
    6889:	48 8b 7c 24 18       	mov    rdi,QWORD PTR [rsp+0x18]
    688e:	48 8b 47 08          	mov    rax,QWORD PTR [rdi+0x8]
    6892:	4c 8b 67 20          	mov    r12,QWORD PTR [rdi+0x20]
    6896:	48 89 5f 28          	mov    QWORD PTR [rdi+0x28],rbx
    689a:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    68a1:	00 
    68a2:	49 89 47 10          	mov    QWORD PTR [r15+0x10],rax
    68a6:	41 83 e5 0c          	and    r13d,0xc
    68aa:	e9 42 b8 ff ff       	jmp    20f1 <luaV_execute+0x401>
        checkGC(L, ra + 1);
    68af:	48 83 c5 10          	add    rbp,0x10
    68b3:	49 89 6f 10          	mov    QWORD PTR [r15+0x10],rbp
    68b7:	e8 00 00 00 00       	call   68bc <luaV_execute+0x4bcc>
    68bc:	48 8b 74 24 18       	mov    rsi,QWORD PTR [rsp+0x18]
    68c1:	48 8b 46 08          	mov    rax,QWORD PTR [rsi+0x8]
    68c5:	4c 8b 66 20          	mov    r12,QWORD PTR [rsi+0x20]
    68c9:	48 89 5e 28          	mov    QWORD PTR [rsi+0x28],rbx
    68cd:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    68d4:	00 
    68d5:	49 89 47 10          	mov    QWORD PTR [r15+0x10],rax
    68d9:	41 83 e5 0c          	and    r13d,0xc
    68dd:	e9 18 da ff ff       	jmp    42fa <luaV_execute+0x260a>
        checkGC(L, ra + 1);
    68e2:	48 83 c5 10          	add    rbp,0x10
    68e6:	49 89 6f 10          	mov    QWORD PTR [r15+0x10],rbp
    68ea:	e8 00 00 00 00       	call   68ef <luaV_execute+0x4bff>
    68ef:	48 8b 4c 24 18       	mov    rcx,QWORD PTR [rsp+0x18]
    68f4:	48 8b 41 08          	mov    rax,QWORD PTR [rcx+0x8]
    68f8:	4c 8b 61 20          	mov    r12,QWORD PTR [rcx+0x20]
    68fc:	48 89 59 28          	mov    QWORD PTR [rcx+0x28],rbx
    6900:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    6907:	00 
    6908:	49 89 47 10          	mov    QWORD PTR [r15+0x10],rax
    690c:	41 83 e5 0c          	and    r13d,0xc
    6910:	e9 1f c0 ff ff       	jmp    2934 <luaV_execute+0xc44>
        luaC_upvalbarrier(L, uv);
    6915:	48 8d 56 10          	lea    rdx,[rsi+0x10]
    6919:	48 39 d0             	cmp    rax,rdx
    691c:	0f 85 71 d4 ff ff    	jne    3d93 <luaV_execute+0x20a3>
    6922:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6927:	e8 00 00 00 00       	call   692c <luaV_execute+0x4c3c>
    692c:	e9 62 d4 ff ff       	jmp    3d93 <luaV_execute+0x20a3>
        luaC_upvalbarrier(L, uv);
    6931:	48 8d 56 10          	lea    rdx,[rsi+0x10]
    6935:	48 39 d0             	cmp    rax,rdx
    6938:	0f 85 b0 e8 ff ff    	jne    51ee <luaV_execute+0x34fe>
    693e:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6943:	e8 00 00 00 00       	call   6948 <luaV_execute+0x4c58>
    6948:	e9 a1 e8 ff ff       	jmp    51ee <luaV_execute+0x34fe>
          Protect(luaD_checkstack(L, n));
    694d:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    6952:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    6957:	49 63 d7             	movsxd rdx,r15d
    695a:	48 89 58 28          	mov    QWORD PTR [rax+0x28],rbx
    695e:	48 8b 46 30          	mov    rax,QWORD PTR [rsi+0x30]
    6962:	48 2b 46 10          	sub    rax,QWORD PTR [rsi+0x10]
    6966:	48 c1 f8 04          	sar    rax,0x4
    696a:	48 39 d0             	cmp    rax,rdx
    696d:	0f 8e eb 08 00 00    	jle    725e <luaV_execute+0x556e>
    6973:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
          ra = RA(i);  /* previous call may change the stack */
    6978:	41 c1 e8 08          	shr    r8d,0x8
          Protect(luaD_checkstack(L, n));
    697c:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
          b = n;  /* get all var. arguments */
    6981:	44 89 fe             	mov    esi,r15d
          ra = RA(i);  /* previous call may change the stack */
    6984:	44 89 c5             	mov    ebp,r8d
          Protect(luaD_checkstack(L, n));
    6987:	4c 8b 60 20          	mov    r12,QWORD PTR [rax+0x20]
          ra = RA(i);  /* previous call may change the stack */
    698b:	83 e5 7f             	and    ebp,0x7f
          L->top = ra + n;
    698e:	49 63 c7             	movsxd rax,r15d
          Protect(luaD_checkstack(L, n));
    6991:	44 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [rdi+0xc8]
    6998:	00 
          ra = RA(i);  /* previous call may change the stack */
    6999:	48 c1 e5 04          	shl    rbp,0x4
          L->top = ra + n;
    699d:	48 c1 e0 04          	shl    rax,0x4
          ra = RA(i);  /* previous call may change the stack */
    69a1:	4c 01 e5             	add    rbp,r12
          Protect(luaD_checkstack(L, n));
    69a4:	41 83 e5 0c          	and    r13d,0xc
          L->top = ra + n;
    69a8:	48 01 e8             	add    rax,rbp
    69ab:	48 89 47 10          	mov    QWORD PTR [rdi+0x10],rax
    69af:	44 89 ff             	mov    edi,r15d
    69b2:	e9 88 be ff ff       	jmp    283f <luaV_execute+0xb4f>
        luaC_upvalbarrier(L, uv);
    69b7:	48 8d 56 10          	lea    rdx,[rsi+0x10]
    69bb:	48 39 d0             	cmp    rax,rdx
    69be:	0f 85 c4 e7 ff ff    	jne    5188 <luaV_execute+0x3498>
    69c4:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    69c9:	e8 00 00 00 00       	call   69ce <luaV_execute+0x4cde>
    69ce:	e9 b5 e7 ff ff       	jmp    5188 <luaV_execute+0x3498>
        luaC_upvalbarrier(L, uv);
    69d3:	48 8d 56 10          	lea    rdx,[rsi+0x10]
    69d7:	48 39 d0             	cmp    rax,rdx
    69da:	0f 85 42 e7 ff ff    	jne    5122 <luaV_execute+0x3432>
    69e0:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    69e5:	e8 00 00 00 00       	call   69ea <luaV_execute+0x4cfa>
    69ea:	e9 33 e7 ff ff       	jmp    5122 <luaV_execute+0x3432>
        if (ttisinteger(init) && ttisinteger(pstep) &&
    69ef:	66 41 83 7f 08 13    	cmp    WORD PTR [r15+0x8],0x13
    69f5:	0f 85 07 c1 ff ff    	jne    2b02 <luaV_execute+0xe12>
            luaV_forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {
    69fb:	49 8b 17             	mov    rdx,QWORD PTR [r15]
    69fe:	4c 89 cf             	mov    rdi,r9
    6a01:	48 8d 4c 24 54       	lea    rcx,[rsp+0x54]
    6a06:	44 89 44 24 30       	mov    DWORD PTR [rsp+0x30],r8d
    6a0b:	48 8d b4 24 b8 00 00 	lea    rsi,[rsp+0xb8]
    6a12:	00 
    6a13:	4c 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],r9
    6a18:	e8 00 00 00 00       	call   6a1d <luaV_execute+0x4d2d>
        if (ttisinteger(init) && ttisinteger(pstep) &&
    6a1d:	4c 8b 4c 24 28       	mov    r9,QWORD PTR [rsp+0x28]
    6a22:	44 8b 44 24 30       	mov    r8d,DWORD PTR [rsp+0x30]
    6a27:	85 c0                	test   eax,eax
    6a29:	0f 84 d3 c0 ff ff    	je     2b02 <luaV_execute+0xe12>
          lua_Integer initv = (stopnow ? 0 : ivalue(init));
    6a2f:	8b 54 24 54          	mov    edx,DWORD PTR [rsp+0x54]
    6a33:	31 c0                	xor    eax,eax
    6a35:	85 d2                	test   edx,edx
    6a37:	75 04                	jne    6a3d <luaV_execute+0x4d4d>
    6a39:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
          setivalue(plimit, ilimit);
    6a3d:	48 8b 94 24 b8 00 00 	mov    rdx,QWORD PTR [rsp+0xb8]
    6a44:	00 
    6a45:	41 bb 13 00 00 00    	mov    r11d,0x13
    6a4b:	66 45 89 59 08       	mov    WORD PTR [r9+0x8],r11w
    6a50:	49 89 11             	mov    QWORD PTR [r9],rdx
          setivalue(init, intop(-, initv, ivalue(pstep)));
    6a53:	49 2b 07             	sub    rax,QWORD PTR [r15]
    6a56:	41 bf 13 00 00 00    	mov    r15d,0x13
    6a5c:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    6a60:	66 44 89 7d 08       	mov    WORD PTR [rbp+0x8],r15w
            luaV_forlimit(plimit, &ilimit, ivalue(pstep), &stopnow)) {
    6a65:	e9 17 c1 ff ff       	jmp    2b81 <luaV_execute+0xe91>
        last = ((c-1)*LFIELDS_PER_FLUSH) + n;
    6a6a:	89 f1                	mov    ecx,esi
          if (last > h->sizearray)  /* needs more space? */
    6a6c:	41 39 71 0c          	cmp    DWORD PTR [r9+0xc],esi
    6a70:	0f 82 93 08 00 00    	jb     7309 <luaV_execute+0x5619>
    6a76:	49 63 c0             	movsxd rax,r8d
    6a79:	44 89 7c 24 28       	mov    DWORD PTR [rsp+0x28],r15d
    6a7e:	48 c1 e0 04          	shl    rax,0x4
    6a82:	48 89 c3             	mov    rbx,rax
          for (; n > 0; n--) {
    6a85:	45 85 c0             	test   r8d,r8d
    6a88:	0f 8e e1 ce ff ff    	jle    396f <luaV_execute+0x1c7f>
    6a8e:	44 89 6c 24 48       	mov    DWORD PTR [rsp+0x48],r13d
    6a93:	4c 8b 6c 24 30       	mov    r13,QWORD PTR [rsp+0x30]
    6a98:	4c 89 4c 24 40       	mov    QWORD PTR [rsp+0x40],r9
    6a9d:	4c 89 64 24 30       	mov    QWORD PTR [rsp+0x30],r12
    6aa2:	eb 12                	jmp    6ab6 <luaV_execute+0x4dc6>
    6aa4:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    6aa8:	48 83 eb 10          	sub    rbx,0x10
    6aac:	44 89 e1             	mov    ecx,r12d
    6aaf:	44 3b 64 24 28       	cmp    r12d,DWORD PTR [rsp+0x28]
    6ab4:	74 52                	je     6b08 <luaV_execute+0x4e18>
            TValue *val = ra + n;
    6ab6:	4c 8d 7c 1d 00       	lea    r15,[rbp+rbx*1+0x0]
            luaH_setint(L, h, last--, val);
    6abb:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6ac0:	89 ca                	mov    edx,ecx
    6ac2:	44 8d 61 ff          	lea    r12d,[rcx-0x1]
    6ac6:	4c 89 ee             	mov    rsi,r13
    6ac9:	4c 89 f9             	mov    rcx,r15
    6acc:	e8 00 00 00 00       	call   6ad1 <luaV_execute+0x4de1>
            luaC_barrierback(L, h, val);
    6ad1:	66 41 83 7f 08 00    	cmp    WORD PTR [r15+0x8],0x0
    6ad7:	79 cf                	jns    6aa8 <luaV_execute+0x4db8>
    6ad9:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
    6ade:	f6 40 09 04          	test   BYTE PTR [rax+0x9],0x4
    6ae2:	74 c4                	je     6aa8 <luaV_execute+0x4db8>
    6ae4:	49 8b 07             	mov    rax,QWORD PTR [r15]
    6ae7:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
    6aeb:	74 bb                	je     6aa8 <luaV_execute+0x4db8>
    6aed:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6af2:	4c 89 ee             	mov    rsi,r13
    6af5:	48 83 eb 10          	sub    rbx,0x10
    6af9:	e8 00 00 00 00       	call   6afe <luaV_execute+0x4e0e>
    6afe:	44 89 e1             	mov    ecx,r12d
          for (; n > 0; n--) {
    6b01:	44 3b 64 24 28       	cmp    r12d,DWORD PTR [rsp+0x28]
    6b06:	75 ae                	jne    6ab6 <luaV_execute+0x4dc6>
    6b08:	44 8b 6c 24 48       	mov    r13d,DWORD PTR [rsp+0x48]
    6b0d:	4c 8b 64 24 30       	mov    r12,QWORD PTR [rsp+0x30]
    6b12:	e9 58 ce ff ff       	jmp    396f <luaV_execute+0x1c7f>
          setivalue(ra, intop(-, 0, ib));
    6b17:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    6b1a:	48 f7 d8             	neg    rax
    6b1d:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    6b21:	b8 13 00 00 00       	mov    eax,0x13
    6b26:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    6b2a:	e9 0a c8 ff ff       	jmp    3339 <luaV_execute+0x1649>
        if (tointegerns(rb, &ib)) {
    6b2f:	49 8b 01             	mov    rax,QWORD PTR [r9]
    6b32:	48 89 84 24 b0 00 00 	mov    QWORD PTR [rsp+0xb0],rax
    6b39:	00 
    6b3a:	e9 7c c7 ff ff       	jmp    32bb <luaV_execute+0x15cb>
        dojump(ci, i, 0);
    6b3f:	48 8b 74 24 18       	mov    rsi,QWORD PTR [rsp+0x18]
    6b44:	48 98                	cdqe   
    6b46:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6b4b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    6b50:	48 c1 e0 04          	shl    rax,0x4
    6b54:	48 8b 56 20          	mov    rdx,QWORD PTR [rsi+0x20]
    6b58:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    6b5d:	e8 00 00 00 00       	call   6b62 <luaV_execute+0x4e72>
    6b62:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    6b67:	e9 03 c5 ff ff       	jmp    306f <luaV_execute+0x137f>
          luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
    6b6c:	44 89 ff             	mov    edi,r15d
    6b6f:	48 89 74 24 38       	mov    QWORD PTR [rsp+0x38],rsi
    6b74:	e8 00 00 00 00       	call   6b79 <luaV_execute+0x4e89>
    6b79:	8b 7c 24 28          	mov    edi,DWORD PTR [rsp+0x28]
    6b7d:	89 44 24 30          	mov    DWORD PTR [rsp+0x30],eax
    6b81:	e8 00 00 00 00       	call   6b86 <luaV_execute+0x4e96>
    6b86:	8b 4c 24 30          	mov    ecx,DWORD PTR [rsp+0x30]
    6b8a:	48 8b 74 24 38       	mov    rsi,QWORD PTR [rsp+0x38]
    6b8f:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6b94:	89 c2                	mov    edx,eax
    6b96:	e8 00 00 00 00       	call   6b9b <luaV_execute+0x4eab>
    6b9b:	e9 43 d7 ff ff       	jmp    42e3 <luaV_execute+0x25f3>
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    6ba0:	48 8b 74 24 28       	mov    rsi,QWORD PTR [rsp+0x28]
    6ba5:	31 d2                	xor    edx,edx
                  lua_Integer i = 0;
    6ba7:	48 c7 84 24 d0 00 00 	mov    QWORD PTR [rsp+0xd0],0x0
    6bae:	00 00 00 00 00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    6bb3:	e8 00 00 00 00       	call   6bb8 <luaV_execute+0x4ec8>
                  if (luaV_tointeger_(val, &i)) {
    6bb8:	85 c0                	test   eax,eax
    6bba:	0f 84 30 31 00 00    	je     9cf0 <luaV_execute+0x8000>
                    raviH_set_int_inline(L, h, u, i);
    6bc0:	48 8b 8c 24 d0 00 00 	mov    rcx,QWORD PTR [rsp+0xd0]
    6bc7:	00 
    6bc8:	44 89 ea             	mov    edx,r13d
    6bcb:	45 39 6c 24 40       	cmp    DWORD PTR [r12+0x40],r13d
    6bd0:	0f 86 92 00 00 00    	jbe    6c68 <luaV_execute+0x4f78>
    6bd6:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    6bdb:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    6bdf:	e9 34 cd ff ff       	jmp    3918 <luaV_execute+0x1c28>
    6be4:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
                  raviH_set_float_inline(L, h, u, fltvalue(val));
    6be8:	44 89 ea             	mov    edx,r13d
    6beb:	45 39 6c 24 40       	cmp    DWORD PTR [r12+0x40],r13d
    6bf0:	0f 86 aa 00 00 00    	jbe    6ca0 <luaV_execute+0x4fb0>
    6bf6:	f2 0f 10 07          	movsd  xmm0,QWORD PTR [rdi]
    6bfa:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    6bff:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    6c04:	e9 0f cd ff ff       	jmp    3918 <luaV_execute+0x1c28>
    6c09:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
                  if (luaV_tonumber_(val, &d)) {
    6c10:	48 8b 74 24 28       	mov    rsi,QWORD PTR [rsp+0x28]
                  lua_Number d = 0.0;
    6c15:	48 c7 84 24 d0 00 00 	mov    QWORD PTR [rsp+0xd0],0x0
    6c1c:	00 00 00 00 00 
                  if (luaV_tonumber_(val, &d)) {
    6c21:	e8 00 00 00 00       	call   6c26 <luaV_execute+0x4f36>
    6c26:	85 c0                	test   eax,eax
    6c28:	0f 84 cc 30 00 00    	je     9cfa <luaV_execute+0x800a>
                    raviH_set_float_inline(L, h, u, d);
    6c2e:	f2 0f 10 84 24 d0 00 	movsd  xmm0,QWORD PTR [rsp+0xd0]
    6c35:	00 00 
    6c37:	44 89 ea             	mov    edx,r13d
    6c3a:	45 39 6c 24 40       	cmp    DWORD PTR [r12+0x40],r13d
    6c3f:	0f 86 41 01 00 00    	jbe    6d86 <luaV_execute+0x5096>
    6c45:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    6c4a:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    6c4f:	e9 c4 cc ff ff       	jmp    3918 <luaV_execute+0x1c28>
                  raviH_set_int_inline(L, h, u, ivalue(val));
    6c54:	48 8b 74 24 30       	mov    rsi,QWORD PTR [rsp+0x30]
    6c59:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6c5e:	e8 00 00 00 00       	call   6c63 <luaV_execute+0x4f73>
    6c63:	e9 b0 cc ff ff       	jmp    3918 <luaV_execute+0x1c28>
                    raviH_set_int_inline(L, h, u, i);
    6c68:	48 8b 74 24 30       	mov    rsi,QWORD PTR [rsp+0x30]
    6c6d:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6c72:	e8 00 00 00 00       	call   6c77 <luaV_execute+0x4f87>
    6c77:	e9 9c cc ff ff       	jmp    3918 <luaV_execute+0x1c28>
    6c7c:	4c 8d 05 00 00 00 00 	lea    r8,[rip+0x0]        # 6c83 <luaV_execute+0x4f93>
    6c83:	e9 d2 d5 ff ff       	jmp    425a <luaV_execute+0x256a>
    6c88:	4c 8d 05 00 00 00 00 	lea    r8,[rip+0x0]        # 6c8f <luaV_execute+0x4f9f>
    6c8f:	e9 76 b5 ff ff       	jmp    220a <luaV_execute+0x51a>
    6c94:	4c 8d 05 00 00 00 00 	lea    r8,[rip+0x0]        # 6c9b <luaV_execute+0x4fab>
    6c9b:	e9 22 b6 ff ff       	jmp    22c2 <luaV_execute+0x5d2>
                  raviH_set_float_inline(L, h, u, fltvalue(val));
    6ca0:	f2 0f 10 07          	movsd  xmm0,QWORD PTR [rdi]
    6ca4:	48 8b 74 24 30       	mov    rsi,QWORD PTR [rsp+0x30]
    6ca9:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6cae:	e8 00 00 00 00       	call   6cb3 <luaV_execute+0x4fc3>
    6cb3:	e9 60 cc ff ff       	jmp    3918 <luaV_execute+0x1c28>
                  raviH_set_float_inline(L, h, u, (lua_Number)(ivalue(val)));
    6cb8:	48 8b 74 24 30       	mov    rsi,QWORD PTR [rsp+0x30]
    6cbd:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6cc2:	e8 00 00 00 00       	call   6cc7 <luaV_execute+0x4fd7>
    6cc7:	e9 4c cc ff ff       	jmp    3918 <luaV_execute+0x1c28>
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_MOD)); }
    6ccc:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    6cd1:	48 89 e9             	mov    rcx,rbp
    6cd4:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    6cd9:	41 b8 09 00 00 00    	mov    r8d,0x9
    6cdf:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    6ce3:	48 89 ef             	mov    rdi,rbp
    6ce6:	e8 00 00 00 00       	call   6ceb <luaV_execute+0x4ffb>
    6ceb:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    6cf2:	00 
    6cf3:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    6cf7:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    6cfb:	48 89 d8             	mov    rax,rbx
    6cfe:	48 83 c3 04          	add    rbx,0x4
    6d02:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    6d05:	45 85 ed             	test   r13d,r13d
    6d08:	0f 85 cd 17 00 00    	jne    84db <luaV_execute+0x67eb>
    6d0e:	44 89 c5             	mov    ebp,r8d
    6d11:	41 0f b6 c0          	movzx  eax,r8b
    6d15:	c1 ed 08             	shr    ebp,0x8
    6d18:	48 89 c2             	mov    rdx,rax
    6d1b:	83 e5 7f             	and    ebp,0x7f
    6d1e:	48 c1 e5 04          	shl    rbp,0x4
    6d22:	4c 01 e5             	add    rbp,r12
    6d25:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_IDIV)); }
    6d29:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    6d2e:	48 89 e9             	mov    rcx,rbp
    6d31:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    6d36:	41 b8 0c 00 00 00    	mov    r8d,0xc
    6d3c:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    6d40:	48 89 ef             	mov    rdi,rbp
    6d43:	e8 00 00 00 00       	call   6d48 <luaV_execute+0x5058>
    6d48:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    6d4f:	00 
    6d50:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    6d54:	41 83 e5 0c          	and    r13d,0xc
        vmbreak;
    6d58:	48 89 d8             	mov    rax,rbx
    6d5b:	48 83 c3 04          	add    rbx,0x4
    6d5f:	44 8b 00             	mov    r8d,DWORD PTR [rax]
    6d62:	45 85 ed             	test   r13d,r13d
    6d65:	0f 85 1d 16 00 00    	jne    8388 <luaV_execute+0x6698>
    6d6b:	44 89 c5             	mov    ebp,r8d
    6d6e:	41 0f b6 c0          	movzx  eax,r8b
    6d72:	c1 ed 08             	shr    ebp,0x8
    6d75:	48 89 c2             	mov    rdx,rax
    6d78:	83 e5 7f             	and    ebp,0x7f
    6d7b:	48 c1 e5 04          	shl    rbp,0x4
    6d7f:	4c 01 e5             	add    rbp,r12
    6d82:	41 ff 24 c6          	jmp    QWORD PTR [r14+rax*8]
                    raviH_set_float_inline(L, h, u, d);
    6d86:	48 8b 74 24 30       	mov    rsi,QWORD PTR [rsp+0x30]
    6d8b:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6d90:	e8 00 00 00 00       	call   6d95 <luaV_execute+0x50a5>
    6d95:	e9 7e cb ff ff       	jmp    3918 <luaV_execute+0x1c28>
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
    6d9a:	48 8b 75 00          	mov    rsi,QWORD PTR [rbp+0x0]
    6d9e:	f6 46 09 04          	test   BYTE PTR [rsi+0x9],0x4
    6da2:	0f 84 e0 b2 ff ff    	je     2088 <luaV_execute+0x398>
    6da8:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    6dab:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
    6daf:	0f 84 d3 b2 ff ff    	je     2088 <luaV_execute+0x398>
    6db5:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6dba:	e8 00 00 00 00       	call   6dbf <luaV_execute+0x50cf>
    6dbf:	e9 c4 b2 ff ff       	jmp    2088 <luaV_execute+0x398>
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    6dc4:	48 89 d6             	mov    rsi,rdx
    6dc7:	48 89 54 24 28       	mov    QWORD PTR [rsp+0x28],rdx
    6dcc:	e8 00 00 00 00       	call   6dd1 <luaV_execute+0x50e1>
    6dd1:	48 8b 54 24 28       	mov    rdx,QWORD PTR [rsp+0x28]
    6dd6:	e9 46 cf ff ff       	jmp    3d21 <luaV_execute+0x2031>
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    6ddb:	49 8b 3f             	mov    rdi,QWORD PTR [r15]
    6dde:	48 89 d6             	mov    rsi,rdx
    6de1:	48 89 54 24 28       	mov    QWORD PTR [rsp+0x28],rdx
    6de6:	e8 00 00 00 00       	call   6deb <luaV_execute+0x50fb>
    6deb:	48 8b 54 24 28       	mov    rdx,QWORD PTR [rsp+0x28]
    6df0:	e9 7c ce ff ff       	jmp    3c71 <luaV_execute+0x1f81>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    6df5:	48 89 ee             	mov    rsi,rbp
    6df8:	48 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],rcx
    6dfd:	e8 00 00 00 00       	call   6e02 <luaV_execute+0x5112>
    6e02:	48 8b 4c 24 28       	mov    rcx,QWORD PTR [rsp+0x28]
    6e07:	66 83 78 08 00       	cmp    WORD PTR [rax+0x8],0x0
    6e0c:	0f 85 b3 cd ff ff    	jne    3bc5 <luaV_execute+0x1ed5>
    6e12:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    6e17:	4c 89 fe             	mov    rsi,r15
    6e1a:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    6e1f:	49 89 c0             	mov    r8,rax
    6e22:	48 89 ea             	mov    rdx,rbp
    6e25:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    6e2a:	4c 89 ff             	mov    rdi,r15
    6e2d:	e8 00 00 00 00       	call   6e32 <luaV_execute+0x5142>
    6e32:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    6e39:	00 
    6e3a:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    6e3f:	41 83 e5 0c          	and    r13d,0xc
    6e43:	e9 96 cd ff ff       	jmp    3bde <luaV_execute+0x1eee>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    6e48:	48 8b 7d 00          	mov    rdi,QWORD PTR [rbp+0x0]
    6e4c:	4c 89 fe             	mov    rsi,r15
    6e4f:	48 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],rcx
    6e54:	e8 00 00 00 00       	call   6e59 <luaV_execute+0x5169>
    6e59:	48 8b 4c 24 28       	mov    rcx,QWORD PTR [rsp+0x28]
    6e5e:	66 83 78 08 00       	cmp    WORD PTR [rax+0x8],0x0
    6e63:	0f 85 81 cc ff ff    	jne    3aea <luaV_execute+0x1dfa>
    6e69:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    6e6e:	4c 89 fa             	mov    rdx,r15
    6e71:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    6e76:	49 89 c0             	mov    r8,rax
    6e79:	48 89 ee             	mov    rsi,rbp
    6e7c:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    6e81:	4c 89 ff             	mov    rdi,r15
    6e84:	e8 00 00 00 00       	call   6e89 <luaV_execute+0x5199>
    6e89:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    6e90:	00 
    6e91:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    6e96:	41 83 e5 0c          	and    r13d,0xc
    6e9a:	e9 64 cc ff ff       	jmp    3b03 <luaV_execute+0x1e13>
          Protect(luaT_trybinTM(L, rb, rb, ra, TM_BNOT));
    6e9f:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    6ea4:	48 89 e9             	mov    rcx,rbp
    6ea7:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    6eac:	4c 89 ca             	mov    rdx,r9
    6eaf:	41 b8 13 00 00 00    	mov    r8d,0x13
    6eb5:	4c 89 ce             	mov    rsi,r9
    6eb8:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    6ebc:	48 89 ef             	mov    rdi,rbp
    6ebf:	e8 00 00 00 00       	call   6ec4 <luaV_execute+0x51d4>
    6ec4:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    6ecb:	00 
    6ecc:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    6ed0:	41 83 e5 0c          	and    r13d,0xc
    6ed4:	e9 f2 c3 ff ff       	jmp    32cb <luaV_execute+0x15db>
        if (luaV_fastget(L, rb, key, aux, luaH_getstr)) {
    6ed9:	49 8b 3f             	mov    rdi,QWORD PTR [r15]
    6edc:	48 89 54 24 28       	mov    QWORD PTR [rsp+0x28],rdx
    6ee1:	e8 00 00 00 00       	call   6ee6 <luaV_execute+0x51f6>
    6ee6:	48 8b 54 24 28       	mov    rdx,QWORD PTR [rsp+0x28]
    6eeb:	49 89 c0             	mov    r8,rax
    6eee:	0f b7 40 08          	movzx  eax,WORD PTR [rax+0x8]
    6ef2:	66 85 c0             	test   ax,ax
    6ef5:	0f 84 82 d2 ff ff    	je     417d <luaV_execute+0x248d>
          setobj2s(L, ra, aux);
    6efb:	49 8b 10             	mov    rdx,QWORD PTR [r8]
    6efe:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    6f02:	48 89 55 00          	mov    QWORD PTR [rbp+0x0],rdx
    6f06:	e9 a0 d2 ff ff       	jmp    41ab <luaV_execute+0x24bb>
          donextjump(ci);
    6f0b:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    6f0d:	89 e8                	mov    eax,ebp
    6f0f:	c1 e8 08             	shr    eax,0x8
    6f12:	83 e0 7f             	and    eax,0x7f
    6f15:	0f 85 21 05 00 00    	jne    743c <luaV_execute+0x574c>
    6f1b:	89 e8                	mov    eax,ebp
    6f1d:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    6f22:	c1 e8 10             	shr    eax,0x10
    6f25:	2d 00 80 00 00       	sub    eax,0x8000
    6f2a:	44 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [rdi+0xc8]
    6f31:	00 
    6f32:	48 98                	cdqe   
    6f34:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    6f39:	41 83 e5 0c          	and    r13d,0xc
    6f3d:	e9 96 b4 ff ff       	jmp    23d8 <luaV_execute+0x6e8>
          donextjump(ci);
    6f42:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    6f44:	89 e8                	mov    eax,ebp
    6f46:	c1 e8 08             	shr    eax,0x8
    6f49:	83 e0 7f             	and    eax,0x7f
    6f4c:	0f 85 28 03 00 00    	jne    727a <luaV_execute+0x558a>
    6f52:	89 e8                	mov    eax,ebp
    6f54:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    6f59:	c1 e8 10             	shr    eax,0x10
    6f5c:	2d 00 80 00 00       	sub    eax,0x8000
    6f61:	44 0f b6 ae c8 00 00 	movzx  r13d,BYTE PTR [rsi+0xc8]
    6f68:	00 
    6f69:	48 98                	cdqe   
    6f6b:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    6f70:	41 83 e5 0c          	and    r13d,0xc
    6f74:	e9 e1 b3 ff ff       	jmp    235a <luaV_execute+0x66a>
          donextjump(ci);
    6f79:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    6f7b:	89 e8                	mov    eax,ebp
    6f7d:	c1 e8 08             	shr    eax,0x8
    6f80:	83 e0 7f             	and    eax,0x7f
    6f83:	0f 85 3a 03 00 00    	jne    72c3 <luaV_execute+0x55d3>
    6f89:	89 e8                	mov    eax,ebp
    6f8b:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
    6f90:	c1 e8 10             	shr    eax,0x10
    6f93:	2d 00 80 00 00       	sub    eax,0x8000
    6f98:	44 0f b6 a9 c8 00 00 	movzx  r13d,BYTE PTR [rcx+0xc8]
    6f9f:	00 
    6fa0:	48 98                	cdqe   
    6fa2:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    6fa7:	41 83 e5 0c          	and    r13d,0xc
    6fab:	e9 a4 b4 ff ff       	jmp    2454 <luaV_execute+0x764>
          donextjump(ci);
    6fb0:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    6fb2:	89 e8                	mov    eax,ebp
    6fb4:	c1 e8 08             	shr    eax,0x8
    6fb7:	83 e0 7f             	and    eax,0x7f
    6fba:	0f 85 ed 03 00 00    	jne    73ad <luaV_execute+0x56bd>
    6fc0:	89 e8                	mov    eax,ebp
    6fc2:	c1 e8 10             	shr    eax,0x10
    6fc5:	2d 00 80 00 00       	sub    eax,0x8000
    6fca:	48 98                	cdqe   
    6fcc:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    6fd1:	e9 5f c0 ff ff       	jmp    3035 <luaV_execute+0x1345>
          donextjump(ci);
    6fd6:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    6fd8:	89 e8                	mov    eax,ebp
    6fda:	c1 e8 08             	shr    eax,0x8
    6fdd:	83 e0 7f             	and    eax,0x7f
    6fe0:	0f 85 a4 03 00 00    	jne    738a <luaV_execute+0x569a>
    6fe6:	89 e8                	mov    eax,ebp
    6fe8:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
    6fed:	c1 e8 10             	shr    eax,0x10
    6ff0:	2d 00 80 00 00       	sub    eax,0x8000
    6ff5:	44 0f b6 a9 c8 00 00 	movzx  r13d,BYTE PTR [rcx+0xc8]
    6ffc:	00 
    6ffd:	48 98                	cdqe   
    6fff:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    7004:	41 83 e5 0c          	and    r13d,0xc
    7008:	e9 65 b1 ff ff       	jmp    2172 <luaV_execute+0x482>
          donextjump(ci);
    700d:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    700f:	89 e8                	mov    eax,ebp
    7011:	c1 e8 08             	shr    eax,0x8
    7014:	83 e0 7f             	and    eax,0x7f
    7017:	0f 85 65 04 00 00    	jne    7482 <luaV_execute+0x5792>
    701d:	89 e8                	mov    eax,ebp
    701f:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    7024:	c1 e8 10             	shr    eax,0x10
    7027:	2d 00 80 00 00       	sub    eax,0x8000
    702c:	44 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [rdi+0xc8]
    7033:	00 
    7034:	48 98                	cdqe   
    7036:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    703b:	41 83 e5 0c          	and    r13d,0xc
    703f:	e9 95 b4 ff ff       	jmp    24d9 <luaV_execute+0x7e9>
          donextjump(ci);
    7044:	8b 2b                	mov    ebp,DWORD PTR [rbx]
    7046:	89 e8                	mov    eax,ebp
    7048:	c1 e8 08             	shr    eax,0x8
    704b:	83 e0 7f             	and    eax,0x7f
    704e:	0f 85 0b 04 00 00    	jne    745f <luaV_execute+0x576f>
    7054:	89 e8                	mov    eax,ebp
    7056:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    705b:	c1 e8 10             	shr    eax,0x10
    705e:	2d 00 80 00 00       	sub    eax,0x8000
    7063:	44 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [rdi+0xc8]
    706a:	00 
    706b:	48 98                	cdqe   
    706d:	48 8d 44 83 04       	lea    rax,[rbx+rax*4+0x4]
    7072:	41 83 e5 0c          	and    r13d,0xc
    7076:	e9 da b4 ff ff       	jmp    2555 <luaV_execute+0x865>
          Protect(luaT_trybinTM(L, rb, rb, ra, TM_UNM));
    707b:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7080:	48 89 e9             	mov    rcx,rbp
    7083:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7088:	48 89 f2             	mov    rdx,rsi
    708b:	41 b8 12 00 00 00    	mov    r8d,0x12
    7091:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7095:	48 89 ef             	mov    rdi,rbp
    7098:	e8 00 00 00 00       	call   709d <luaV_execute+0x53ad>
    709d:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    70a4:	00 
    70a5:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    70a9:	41 83 e5 0c          	and    r13d,0xc
    70ad:	e9 87 c2 ff ff       	jmp    3339 <luaV_execute+0x1649>
          if (tointeger(rc, &j)) { raviH_set_int_inline(L, t, idx, j); }
    70b2:	31 d2                	xor    edx,edx
    70b4:	48 8d b4 24 d0 00 00 	lea    rsi,[rsp+0xd0]
    70bb:	00 
    70bc:	e8 00 00 00 00       	call   70c1 <luaV_execute+0x53d1>
    70c1:	85 c0                	test   eax,eax
    70c3:	0f 84 51 2c 00 00    	je     9d1a <luaV_execute+0x802a>
    70c9:	48 8b 8c 24 d0 00 00 	mov    rcx,QWORD PTR [rsp+0xd0]
    70d0:	00 
    70d1:	44 89 fa             	mov    edx,r15d
    70d4:	44 39 7d 40          	cmp    DWORD PTR [rbp+0x40],r15d
    70d8:	0f 86 d9 03 00 00    	jbe    74b7 <luaV_execute+0x57c7>
    70de:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    70e2:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    70e6:	e9 3c f0 ff ff       	jmp    6127 <luaV_execute+0x4437>
        raviH_set_int_inline(L, t, idx, ivalue(rc));
    70eb:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    70f0:	e8 00 00 00 00       	call   70f5 <luaV_execute+0x5405>
    70f5:	e9 48 e3 ff ff       	jmp    5442 <luaV_execute+0x3752>
        raviH_set_float_inline(L, t, idx, fltvalue(rc));
    70fa:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    70ff:	e8 00 00 00 00       	call   7104 <luaV_execute+0x5414>
    7104:	e9 3b e2 ff ff       	jmp    5344 <luaV_execute+0x3654>
          if (tonumber(rc, &j)) { raviH_set_float_inline(L, t, idx, j); }
    7109:	48 8d b4 24 d0 00 00 	lea    rsi,[rsp+0xd0]
    7110:	00 
    7111:	e8 00 00 00 00       	call   7116 <luaV_execute+0x5426>
    7116:	85 c0                	test   eax,eax
    7118:	0f 84 bc 2b 00 00    	je     9cda <luaV_execute+0x7fea>
    711e:	f2 0f 10 84 24 d0 00 	movsd  xmm0,QWORD PTR [rsp+0xd0]
    7125:	00 00 
    7127:	44 89 fa             	mov    edx,r15d
    712a:	44 39 7d 40          	cmp    DWORD PTR [rbp+0x40],r15d
    712e:	0f 86 71 03 00 00    	jbe    74a5 <luaV_execute+0x57b5>
    7134:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    7138:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    713d:	e9 3c f6 ff ff       	jmp    677e <luaV_execute+0x4a8e>
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    7142:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    7147:	4c 89 fa             	mov    rdx,r15
    714a:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    714f:	48 89 ee             	mov    rsi,rbp
    7152:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    7157:	4c 89 ff             	mov    rdi,r15
    715a:	e8 00 00 00 00       	call   715f <luaV_execute+0x546f>
    715f:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    7166:	00 
    7167:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    716c:	41 83 e5 0c          	and    r13d,0xc
    7170:	e9 c9 c8 ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    7175:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    7179:	f2 0f 10 06          	movsd  xmm0,QWORD PTR [rsi]
    717d:	66 83 f8 03          	cmp    ax,0x3
    7181:	0f 84 40 f1 ff ff    	je     62c7 <luaV_execute+0x45d7>
    7187:	66 83 f8 13          	cmp    ax,0x13
    718b:	0f 85 95 ce ff ff    	jne    4026 <luaV_execute+0x2336>
    7191:	66 0f ef c9          	pxor   xmm1,xmm1
    7195:	f2 48 0f 2a 0a       	cvtsi2sd xmm1,QWORD PTR [rdx]
          setfltvalue(ra, luai_numsub(L, nb, nc));
    719a:	f2 0f 5c c1          	subsd  xmm0,xmm1
    719e:	b8 03 00 00 00       	mov    eax,0x3
    71a3:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    71a7:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    71ac:	e9 a4 ce ff ff       	jmp    4055 <luaV_execute+0x2365>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    71b1:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    71b5:	f2 0f 10 0e          	movsd  xmm1,QWORD PTR [rsi]
    71b9:	66 83 f8 03          	cmp    ax,0x3
    71bd:	0f 84 68 f1 ff ff    	je     632b <luaV_execute+0x463b>
    71c3:	66 83 f8 13          	cmp    ax,0x13
    71c7:	0f 85 af cd ff ff    	jne    3f7c <luaV_execute+0x228c>
    71cd:	66 0f ef c0          	pxor   xmm0,xmm0
    71d1:	f2 48 0f 2a 02       	cvtsi2sd xmm0,QWORD PTR [rdx]
          setfltvalue(ra, luai_nummul(L, nb, nc));
    71d6:	f2 0f 59 c1          	mulsd  xmm0,xmm1
    71da:	b8 03 00 00 00       	mov    eax,0x3
    71df:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    71e3:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    71e8:	e9 be cd ff ff       	jmp    3fab <luaV_execute+0x22bb>
        else if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    71ed:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    71f1:	f2 0f 10 0e          	movsd  xmm1,QWORD PTR [rsi]
    71f5:	66 83 f8 03          	cmp    ax,0x3
    71f9:	0f 84 63 f0 ff ff    	je     6262 <luaV_execute+0x4572>
    71ff:	66 83 f8 13          	cmp    ax,0x13
    7203:	0f 85 c7 ce ff ff    	jne    40d0 <luaV_execute+0x23e0>
    7209:	66 0f ef c0          	pxor   xmm0,xmm0
    720d:	f2 48 0f 2a 02       	cvtsi2sd xmm0,QWORD PTR [rdx]
          setfltvalue(ra, luai_numadd(L, nb, nc));
    7212:	f2 0f 58 c1          	addsd  xmm0,xmm1
    7216:	b8 03 00 00 00       	mov    eax,0x3
    721b:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
    721f:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    7224:	e9 d6 ce ff ff       	jmp    40ff <luaV_execute+0x240f>
        if (GETARG_C(i) ? l_isfalse(rb) : !l_isfalse(rb))
    7229:	66 83 fa 01          	cmp    dx,0x1
    722d:	0f 85 a7 eb ff ff    	jne    5dda <luaV_execute+0x40ea>
    7233:	44 8b 10             	mov    r10d,DWORD PTR [rax]
    7236:	45 85 d2             	test   r10d,r10d
    7239:	0f 84 85 bb ff ff    	je     2dc4 <luaV_execute+0x10d4>
    723f:	e9 96 eb ff ff       	jmp    5dda <luaV_execute+0x40ea>
        if (GETARG_C(i) ? l_isfalse(ra) : !l_isfalse(ra))
    7244:	66 83 f8 01          	cmp    ax,0x1
    7248:	0f 85 c3 ea ff ff    	jne    5d11 <luaV_execute+0x4021>
    724e:	8b 6d 00             	mov    ebp,DWORD PTR [rbp+0x0]
    7251:	85 ed                	test   ebp,ebp
    7253:	0f 84 b4 bb ff ff    	je     2e0d <luaV_execute+0x111d>
    7259:	e9 b3 ea ff ff       	jmp    5d11 <luaV_execute+0x4021>
          Protect(luaD_checkstack(L, n));
    725e:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    7263:	44 89 fe             	mov    esi,r15d
    7266:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    726b:	e8 00 00 00 00       	call   7270 <luaV_execute+0x5580>
    7270:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7275:	e9 f9 f6 ff ff       	jmp    6973 <luaV_execute+0x4c83>
          donextjump(ci);
    727a:	48 8b 7c 24 18       	mov    rdi,QWORD PTR [rsp+0x18]
    727f:	48 98                	cdqe   
    7281:	48 c1 e0 04          	shl    rax,0x4
    7285:	48 8b 57 20          	mov    rdx,QWORD PTR [rdi+0x20]
    7289:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    728e:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    7293:	e8 00 00 00 00       	call   7298 <luaV_execute+0x55a8>
    7298:	e9 b5 fc ff ff       	jmp    6f52 <luaV_execute+0x5262>
        if (ttisinteger(rc)) { raviH_set_int_inline(L, t, idx, ivalue(rc)); }
    729d:	48 8b 0f             	mov    rcx,QWORD PTR [rdi]
    72a0:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    72a5:	48 89 ee             	mov    rsi,rbp
    72a8:	e8 00 00 00 00       	call   72ad <luaV_execute+0x55bd>
    72ad:	e9 75 ee ff ff       	jmp    6127 <luaV_execute+0x4437>
          res = (ivalue(rb) <= ivalue(rc));
    72b2:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    72b5:	48 39 30             	cmp    QWORD PTR [rax],rsi
    72b8:	0f 9e c0             	setle  al
    72bb:	0f b6 c0             	movzx  eax,al
    72be:	e9 f9 bb ff ff       	jmp    2ebc <luaV_execute+0x11cc>
          donextjump(ci);
    72c3:	48 8b 4c 24 18       	mov    rcx,QWORD PTR [rsp+0x18]
    72c8:	48 98                	cdqe   
    72ca:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    72cf:	48 c1 e0 04          	shl    rax,0x4
    72d3:	48 8b 51 20          	mov    rdx,QWORD PTR [rcx+0x20]
    72d7:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    72dc:	e8 00 00 00 00       	call   72e1 <luaV_execute+0x55f1>
    72e1:	e9 a3 fc ff ff       	jmp    6f89 <luaV_execute+0x5299>
          donextjump(ci);
    72e6:	48 8b 4c 24 18       	mov    rcx,QWORD PTR [rsp+0x18]
    72eb:	48 98                	cdqe   
    72ed:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    72f2:	48 c1 e0 04          	shl    rax,0x4
    72f6:	48 8b 51 20          	mov    rdx,QWORD PTR [rcx+0x20]
    72fa:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    72ff:	e8 00 00 00 00       	call   7304 <luaV_execute+0x5614>
    7304:	e9 c1 e9 ff ff       	jmp    5cca <luaV_execute+0x3fda>
            luaH_resizearray(L, h, last);  /* pre-allocate it at once */
    7309:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    730e:	89 f2                	mov    edx,esi
    7310:	89 74 24 40          	mov    DWORD PTR [rsp+0x40],esi
    7314:	4c 89 ce             	mov    rsi,r9
    7317:	44 89 44 24 48       	mov    DWORD PTR [rsp+0x48],r8d
    731c:	4c 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],r9
    7321:	e8 00 00 00 00       	call   7326 <luaV_execute+0x5636>
    7326:	4c 8b 4c 24 28       	mov    r9,QWORD PTR [rsp+0x28]
    732b:	8b 4c 24 40          	mov    ecx,DWORD PTR [rsp+0x40]
    732f:	44 8b 44 24 48       	mov    r8d,DWORD PTR [rsp+0x48]
    7334:	e9 3d f7 ff ff       	jmp    6a76 <luaV_execute+0x4d86>
          raviH_set_float_inline(L, t, idx, ((lua_Number)ivalue(rc)));
    7339:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    733e:	48 89 ee             	mov    rsi,rbp
    7341:	e8 00 00 00 00       	call   7346 <luaV_execute+0x5656>
    7346:	e9 33 f4 ff ff       	jmp    677e <luaV_execute+0x4a8e>
          setivalue(ra, intop(-, ib, ic));
    734b:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    734e:	48 2b 02             	sub    rax,QWORD PTR [rdx]
    7351:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    7355:	b8 13 00 00 00       	mov    eax,0x13
    735a:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        if (ttisinteger(rb) && ttisinteger(rc)) {
    735e:	e9 f2 cc ff ff       	jmp    4055 <luaV_execute+0x2365>
          res = (ivalue(rb) < ivalue(rc));
    7363:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    7366:	48 39 30             	cmp    QWORD PTR [rax],rsi
    7369:	0f 9c c0             	setl   al
    736c:	0f b6 c0             	movzx  eax,al
    736f:	e9 08 bc ff ff       	jmp    2f7c <luaV_execute+0x128c>
        if (ttisfloat(rc)) { raviH_set_float_inline(L, t, idx, fltvalue(rc)); }
    7374:	f2 0f 10 07          	movsd  xmm0,QWORD PTR [rdi]
    7378:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    737d:	48 89 ee             	mov    rsi,rbp
    7380:	e8 00 00 00 00       	call   7385 <luaV_execute+0x5695>
    7385:	e9 f4 f3 ff ff       	jmp    677e <luaV_execute+0x4a8e>
          donextjump(ci);
    738a:	48 8b 4c 24 18       	mov    rcx,QWORD PTR [rsp+0x18]
    738f:	48 98                	cdqe   
    7391:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    7396:	48 c1 e0 04          	shl    rax,0x4
    739a:	48 8b 51 20          	mov    rdx,QWORD PTR [rcx+0x20]
    739e:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    73a3:	e8 00 00 00 00       	call   73a8 <luaV_execute+0x56b8>
    73a8:	e9 39 fc ff ff       	jmp    6fe6 <luaV_execute+0x52f6>
          donextjump(ci);
    73ad:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    73b2:	48 98                	cdqe   
    73b4:	48 c1 e0 04          	shl    rax,0x4
    73b8:	49 8d 74 04 f0       	lea    rsi,[r12+rax*1-0x10]
    73bd:	4c 89 ff             	mov    rdi,r15
    73c0:	e8 00 00 00 00       	call   73c5 <luaV_execute+0x56d5>
    73c5:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    73cc:	00 
    73cd:	41 83 e5 0c          	and    r13d,0xc
    73d1:	e9 ea fb ff ff       	jmp    6fc0 <luaV_execute+0x52d0>
          donextjump(ci);
    73d6:	48 8b 4c 24 18       	mov    rcx,QWORD PTR [rsp+0x18]
    73db:	48 98                	cdqe   
    73dd:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    73e2:	48 c1 e0 04          	shl    rax,0x4
    73e6:	48 8b 51 20          	mov    rdx,QWORD PTR [rcx+0x20]
    73ea:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    73ef:	e8 00 00 00 00       	call   73f4 <luaV_execute+0x5704>
    73f4:	e9 3d e5 ff ff       	jmp    5936 <luaV_execute+0x3c46>
          raviH_set_int_inline(L, t, idx, (lua_Integer)fltvalue(rc));
    73f9:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    73fe:	48 89 ee             	mov    rsi,rbp
    7401:	e8 00 00 00 00       	call   7406 <luaV_execute+0x5716>
    7406:	e9 1c ed ff ff       	jmp    6127 <luaV_execute+0x4437>
          setivalue(ra, intop(+, ib, ic));
    740b:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    740e:	48 03 06             	add    rax,QWORD PTR [rsi]
    7411:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    7415:	b8 13 00 00 00       	mov    eax,0x13
    741a:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        if (ttisinteger(rb) && ttisinteger(rc)) {
    741e:	e9 dc cc ff ff       	jmp    40ff <luaV_execute+0x240f>
          setivalue(ra, intop(*, ib, ic));
    7423:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    7426:	48 0f af 02          	imul   rax,QWORD PTR [rdx]
    742a:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    742e:	b8 13 00 00 00       	mov    eax,0x13
    7433:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
        if (ttisinteger(rb) && ttisinteger(rc)) {
    7437:	e9 6f cb ff ff       	jmp    3fab <luaV_execute+0x22bb>
          donextjump(ci);
    743c:	48 8b 74 24 18       	mov    rsi,QWORD PTR [rsp+0x18]
    7441:	48 98                	cdqe   
    7443:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    7448:	48 c1 e0 04          	shl    rax,0x4
    744c:	48 8b 56 20          	mov    rdx,QWORD PTR [rsi+0x20]
    7450:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    7455:	e8 00 00 00 00       	call   745a <luaV_execute+0x576a>
    745a:	e9 bc fa ff ff       	jmp    6f1b <luaV_execute+0x522b>
          donextjump(ci);
    745f:	48 8b 74 24 18       	mov    rsi,QWORD PTR [rsp+0x18]
    7464:	48 98                	cdqe   
    7466:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    746b:	48 c1 e0 04          	shl    rax,0x4
    746f:	48 8b 56 20          	mov    rdx,QWORD PTR [rsi+0x20]
    7473:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    7478:	e8 00 00 00 00       	call   747d <luaV_execute+0x578d>
    747d:	e9 d2 fb ff ff       	jmp    7054 <luaV_execute+0x5364>
          donextjump(ci);
    7482:	48 8b 7c 24 18       	mov    rdi,QWORD PTR [rsp+0x18]
    7487:	48 98                	cdqe   
    7489:	48 c1 e0 04          	shl    rax,0x4
    748d:	48 8b 57 20          	mov    rdx,QWORD PTR [rdi+0x20]
    7491:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    7496:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    749b:	e8 00 00 00 00       	call   74a0 <luaV_execute+0x57b0>
    74a0:	e9 78 fb ff ff       	jmp    701d <luaV_execute+0x532d>
          if (tonumber(rc, &j)) { raviH_set_float_inline(L, t, idx, j); }
    74a5:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    74aa:	48 89 ee             	mov    rsi,rbp
    74ad:	e8 00 00 00 00       	call   74b2 <luaV_execute+0x57c2>
    74b2:	e9 c7 f2 ff ff       	jmp    677e <luaV_execute+0x4a8e>
          if (tointeger(rc, &j)) { raviH_set_int_inline(L, t, idx, j); }
    74b7:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    74bc:	48 89 ee             	mov    rsi,rbp
    74bf:	e8 00 00 00 00       	call   74c4 <luaV_execute+0x57d4>
    74c4:	e9 5e ec ff ff       	jmp    6127 <luaV_execute+0x4437>
    vmfetch;
    74c9:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    74ce:	48 89 5f 28          	mov    QWORD PTR [rdi+0x28],rbx
    74d2:	49 89 ff             	mov    r15,rdi
    74d5:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    74da:	48 89 ef             	mov    rdi,rbp
    74dd:	e8 00 00 00 00       	call   74e2 <luaV_execute+0x57f2>
    74e2:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    74e6:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    74eb:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    74f2:	00 
    74f3:	41 83 e5 0c          	and    r13d,0xc
    74f7:	e9 71 a8 ff ff       	jmp    1d6d <luaV_execute+0x7d>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    74fc:	49 8b 37             	mov    rsi,QWORD PTR [r15]
    74ff:	f6 46 09 04          	test   BYTE PTR [rsi+0x9],0x4
    7503:	0f 84 d5 c6 ff ff    	je     3bde <luaV_execute+0x1eee>
    7509:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    750c:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
    7510:	0f 84 c8 c6 ff ff    	je     3bde <luaV_execute+0x1eee>
    7516:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    751b:	e8 00 00 00 00       	call   7520 <luaV_execute+0x5830>
    7520:	e9 b9 c6 ff ff       	jmp    3bde <luaV_execute+0x1eee>
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    7525:	f6 42 09 04          	test   BYTE PTR [rdx+0x9],0x4
    7529:	0f 84 0f c5 ff ff    	je     3a3e <luaV_execute+0x1d4e>
    752f:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    7532:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
    7536:	0f 84 02 c5 ff ff    	je     3a3e <luaV_execute+0x1d4e>
    753c:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    7541:	48 89 d6             	mov    rsi,rdx
    7544:	e8 00 00 00 00       	call   7549 <luaV_execute+0x5859>
    7549:	e9 f0 c4 ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    754e:	48 8b 75 00          	mov    rsi,QWORD PTR [rbp+0x0]
    7552:	f6 46 09 04          	test   BYTE PTR [rsi+0x9],0x4
    7556:	0f 84 a7 c5 ff ff    	je     3b03 <luaV_execute+0x1e13>
    755c:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    755f:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
    7563:	0f 84 9a c5 ff ff    	je     3b03 <luaV_execute+0x1e13>
    7569:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    756e:	e8 00 00 00 00       	call   7573 <luaV_execute+0x5883>
    7573:	e9 8b c5 ff ff       	jmp    3b03 <luaV_execute+0x1e13>
    7578:	0f b7 4a 08          	movzx  ecx,WORD PTR [rdx+0x8]
        else if (ttisnumber(rb) && ttisnumber(rc))
    757c:	89 cf                	mov    edi,ecx
    757e:	83 e7 0f             	and    edi,0xf
    7581:	66 83 ff 03          	cmp    di,0x3
    7585:	0f 85 fe b8 ff ff    	jne    2e89 <luaV_execute+0x1199>
    758b:	e9 04 e7 ff ff       	jmp    5c94 <luaV_execute+0x3fa4>
    7590:	0f b7 4a 08          	movzx  ecx,WORD PTR [rdx+0x8]
        else if (ttisnumber(rb) && ttisnumber(rc))
    7594:	89 cf                	mov    edi,ecx
    7596:	83 e7 0f             	and    edi,0xf
    7599:	66 83 ff 03          	cmp    di,0x3
    759d:	0f 85 a6 b9 ff ff    	jne    2f49 <luaV_execute+0x1259>
    75a3:	e9 58 e3 ff ff       	jmp    5900 <luaV_execute+0x3c10>
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    75a8:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    75ac:	66 0f ef c0          	pxor   xmm0,xmm0
    75b0:	f2 48 0f 2a 06       	cvtsi2sd xmm0,QWORD PTR [rsi]
    75b5:	66 83 f8 03          	cmp    ax,0x3
    75b9:	0f 85 d1 e7 ff ff    	jne    5d90 <luaV_execute+0x40a0>
    75bf:	f2 0f 10 0a          	movsd  xmm1,QWORD PTR [rdx]
    75c3:	e9 db e7 ff ff       	jmp    5da3 <luaV_execute+0x40b3>
        if (tonumberns(rb, nb) && tonumberns(rc, nc)) {
    75c8:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    75cc:	66 0f ef c0          	pxor   xmm0,xmm0
    75d0:	f2 48 0f 2a 06       	cvtsi2sd xmm0,QWORD PTR [rsi]
    75d5:	66 83 f8 03          	cmp    ax,0x3
    75d9:	0f 85 9f ed ff ff    	jne    637e <luaV_execute+0x468e>
    75df:	f2 0f 10 0a          	movsd  xmm1,QWORD PTR [rdx]
    75e3:	e9 a9 ed ff ff       	jmp    6391 <luaV_execute+0x46a1>
        lua_Integer initv = ivalue(pinit);
    75e8:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
        lua_Integer istep = RAVI_LIKELY((op == OP_RAVI_FORPREP_I1)) ? 1 : ivalue(pstep);
    75ec:	48 8b 55 20          	mov    rdx,QWORD PTR [rbp+0x20]
    75f0:	e9 5e a8 ff ff       	jmp    1e53 <luaV_execute+0x163>
        vmbreak;
    75f5:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    75fa:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    75ff:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7604:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7608:	48 89 ef             	mov    rdi,rbp
    760b:	e8 00 00 00 00       	call   7610 <luaV_execute+0x5920>
    7610:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7614:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7619:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7620:	00 
    7621:	41 83 e5 0c          	and    r13d,0xc
    7625:	e9 57 a8 ff ff       	jmp    1e81 <luaV_execute+0x191>
    762a:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "number[] expected");
    762f:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 7636 <luaV_execute+0x5946>
    7636:	31 c0                	xor    eax,eax
    7638:	4c 89 ef             	mov    rdi,r13
    763b:	e8 00 00 00 00       	call   7640 <luaV_execute+0x5950>
    7640:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "integer[] expected");
    7645:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 764c <luaV_execute+0x595c>
    764c:	31 c0                	xor    eax,eax
    764e:	4c 89 ef             	mov    rdi,r13
    7651:	e8 00 00 00 00       	call   7656 <luaV_execute+0x5966>
    7656:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "table expected");
    765b:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 7662 <luaV_execute+0x5972>
    7662:	31 c0                	xor    eax,eax
    7664:	4c 89 ef             	mov    rdi,r13
    7667:	e8 00 00 00 00       	call   766c <luaV_execute+0x597c>
        vmbreak;
    766c:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7671:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7676:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    767b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    767f:	48 89 ef             	mov    rdi,rbp
    7682:	e8 00 00 00 00       	call   7687 <luaV_execute+0x5997>
    7687:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    768b:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7690:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7697:	00 
    7698:	41 83 e5 0c          	and    r13d,0xc
    769c:	e9 46 a8 ff ff       	jmp    1ee7 <luaV_execute+0x1f7>
        vmbreak;
    76a1:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    76a6:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    76ab:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    76b0:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    76b4:	48 89 ef             	mov    rdi,rbp
    76b7:	e8 00 00 00 00       	call   76bc <luaV_execute+0x59cc>
    76bc:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    76c0:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    76c5:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    76cc:	00 
    76cd:	41 83 e5 0c          	and    r13d,0xc
    76d1:	e9 ed a8 ff ff       	jmp    1fc3 <luaV_execute+0x2d3>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    76d6:	49 8b 02             	mov    rax,QWORD PTR [r10]
    76d9:	48 89 44 24 78       	mov    QWORD PTR [rsp+0x78],rax
    76de:	e9 eb e0 ff ff       	jmp    57ce <luaV_execute+0x3ade>
          savepc(L);  /* in case of allocation errors */
    76e3:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
          pushclosure(L, p, cl->upvals, base, ra);  /* create a new one */
    76e8:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    76ed:	49 89 e8             	mov    r8,rbp
    76f0:	4c 89 e1             	mov    rcx,r12
    76f3:	4c 8d 4d 08          	lea    r9,[rbp+0x8]
    76f7:	4c 89 fa             	mov    rdx,r15
    76fa:	4c 89 de             	mov    rsi,r11
          savepc(L);  /* in case of allocation errors */
    76fd:	48 89 58 28          	mov    QWORD PTR [rax+0x28],rbx
          pushclosure(L, p, cl->upvals, base, ra);  /* create a new one */
    7701:	e8 ba 8a ff ff       	call   1c0 <pushclosure.isra.6>
    7706:	e9 12 b2 ff ff       	jmp    291d <luaV_execute+0xc2d>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    770b:	49 8b 0a             	mov    rcx,QWORD PTR [r10]
    770e:	48 89 8c 24 98 00 00 	mov    QWORD PTR [rsp+0x98],rcx
    7715:	00 
    7716:	e9 ab e3 ff ff       	jmp    5ac6 <luaV_execute+0x3dd6>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    771b:	49 8b 02             	mov    rax,QWORD PTR [r10]
    771e:	48 89 84 24 88 00 00 	mov    QWORD PTR [rsp+0x88],rax
    7725:	00 
    7726:	e9 3b e1 ff ff       	jmp    5866 <luaV_execute+0x3b76>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    772b:	49 8b 02             	mov    rax,QWORD PTR [r10]
    772e:	48 89 44 24 68       	mov    QWORD PTR [rsp+0x68],rax
    7733:	e9 e8 ec ff ff       	jmp    6420 <luaV_execute+0x4730>
        if (tointegerns(rb, &ib) && tointegerns(rc, &ic)) {
    7738:	49 8b 0a             	mov    rcx,QWORD PTR [r10]
    773b:	48 89 8c 24 a8 00 00 	mov    QWORD PTR [rsp+0xa8],rcx
    7742:	00 
    7743:	e9 32 e4 ff ff       	jmp    5b7a <luaV_execute+0x3e8a>
          donextjump(ci);
    7748:	48 8b 74 24 18       	mov    rsi,QWORD PTR [rsp+0x18]
    774d:	48 98                	cdqe   
    774f:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    7754:	48 c1 e0 04          	shl    rax,0x4
    7758:	48 8b 56 20          	mov    rdx,QWORD PTR [rsi+0x20]
    775c:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    7761:	e8 00 00 00 00       	call   7766 <luaV_execute+0x5a76>
    7766:	e9 b6 e5 ff ff       	jmp    5d21 <luaV_execute+0x4031>
          donextjump(ci);
    776b:	48 8b 7c 24 18       	mov    rdi,QWORD PTR [rsp+0x18]
    7770:	48 98                	cdqe   
    7772:	48 c1 e0 04          	shl    rax,0x4
    7776:	48 8b 57 20          	mov    rdx,QWORD PTR [rdi+0x20]
    777a:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    777f:	48 8d 74 02 f0       	lea    rsi,[rdx+rax*1-0x10]
    7784:	e8 00 00 00 00       	call   7789 <luaV_execute+0x5a99>
    7789:	e9 6b e6 ff ff       	jmp    5df9 <luaV_execute+0x4109>
        vmbreak;
    778e:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7793:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7798:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    779d:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    77a1:	48 89 ef             	mov    rdi,rbp
    77a4:	e8 00 00 00 00       	call   77a9 <luaV_execute+0x5ab9>
    77a9:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    77ad:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    77b2:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    77b9:	00 
    77ba:	41 83 e5 0c          	and    r13d,0xc
    77be:	e9 33 bc ff ff       	jmp    33f6 <luaV_execute+0x1706>
        vmbreak;
    77c3:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    77c8:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    77cd:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    77d2:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    77d6:	48 89 ef             	mov    rdi,rbp
    77d9:	e8 00 00 00 00       	call   77de <luaV_execute+0x5aee>
    77de:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    77e2:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    77e7:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    77ee:	00 
    77ef:	41 83 e5 0c          	and    r13d,0xc
    77f3:	e9 49 bf ff ff       	jmp    3741 <luaV_execute+0x1a51>
        vmbreak;
    77f8:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    77fd:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7802:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7807:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    780b:	48 89 ef             	mov    rdi,rbp
    780e:	e8 00 00 00 00       	call   7813 <luaV_execute+0x5b23>
    7813:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7817:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    781c:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7823:	00 
    7824:	41 83 e5 0c          	and    r13d,0xc
    7828:	e9 59 ca ff ff       	jmp    4286 <luaV_execute+0x2596>
        vmbreak;
    782d:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7832:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7837:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    783c:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7840:	48 89 ef             	mov    rdi,rbp
    7843:	e8 00 00 00 00       	call   7848 <luaV_execute+0x5b58>
    7848:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    784c:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7851:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7858:	00 
    7859:	41 83 e5 0c          	and    r13d,0xc
    785d:	e9 08 c6 ff ff       	jmp    3e6a <luaV_execute+0x217a>
        vmbreak;
    7862:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7867:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    786c:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7871:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7875:	48 89 ef             	mov    rdi,rbp
    7878:	e8 00 00 00 00       	call   787d <luaV_execute+0x5b8d>
    787d:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7881:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7886:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    788d:	00 
    788e:	41 83 e5 0c          	and    r13d,0xc
    7892:	e9 e3 bd ff ff       	jmp    367a <luaV_execute+0x198a>
        vmbreak;
    7897:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    789c:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    78a1:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    78a6:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    78aa:	48 89 ef             	mov    rdi,rbp
    78ad:	e8 00 00 00 00       	call   78b2 <luaV_execute+0x5bc2>
    78b2:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    78b6:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    78bb:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    78c2:	00 
    78c3:	41 83 e5 0c          	and    r13d,0xc
    78c7:	e9 e7 bc ff ff       	jmp    35b3 <luaV_execute+0x18c3>
        vmbreak;
    78cc:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    78d1:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    78d6:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    78db:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    78df:	48 89 ef             	mov    rdi,rbp
    78e2:	e8 00 00 00 00       	call   78e7 <luaV_execute+0x5bf7>
    78e7:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    78eb:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    78f0:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    78f7:	00 
    78f8:	41 83 e5 0c          	and    r13d,0xc
    78fc:	e9 04 bf ff ff       	jmp    3805 <luaV_execute+0x1b15>
        vmbreak;
    7901:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7906:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    790b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7910:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7914:	48 89 ef             	mov    rdi,rbp
    7917:	e8 00 00 00 00       	call   791c <luaV_execute+0x5c2c>
    791c:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7920:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7925:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    792c:	00 
    792d:	41 83 e5 0c          	and    r13d,0xc
    7931:	e9 fb b0 ff ff       	jmp    2a31 <luaV_execute+0xd41>
        vmbreak;
    7936:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    793b:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7940:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7945:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7949:	48 89 ef             	mov    rdi,rbp
    794c:	e8 00 00 00 00       	call   7951 <luaV_execute+0x5c61>
    7951:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7955:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    795a:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7961:	00 
    7962:	41 83 e5 0c          	and    r13d,0xc
    7966:	e9 51 b0 ff ff       	jmp    29bc <luaV_execute+0xccc>
        vmbreak;
    796b:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7970:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7975:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    797a:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    797e:	48 89 ef             	mov    rdi,rbp
    7981:	e8 00 00 00 00       	call   7986 <luaV_execute+0x5c96>
    7986:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    798a:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    798f:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7996:	00 
    7997:	41 83 e5 0c          	and    r13d,0xc
    799b:	e9 48 aa ff ff       	jmp    23e8 <luaV_execute+0x6f8>
        vmbreak;
    79a0:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    79a5:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    79aa:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    79af:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    79b3:	48 89 ef             	mov    rdi,rbp
    79b6:	e8 00 00 00 00       	call   79bb <luaV_execute+0x5ccb>
    79bb:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    79bf:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    79c4:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    79cb:	00 
    79cc:	41 83 e5 0c          	and    r13d,0xc
    79d0:	e9 95 a9 ff ff       	jmp    236a <luaV_execute+0x67a>
        vmbreak;
    79d5:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    79da:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    79df:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    79e4:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    79e8:	48 89 ef             	mov    rdi,rbp
    79eb:	e8 00 00 00 00       	call   79f0 <luaV_execute+0x5d00>
    79f0:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    79f4:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    79f9:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7a00:	00 
    7a01:	41 83 e5 0c          	and    r13d,0xc
    7a05:	e9 78 a7 ff ff       	jmp    2182 <luaV_execute+0x492>
        vmbreak;
    7a0a:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7a0f:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7a14:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7a19:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7a1d:	48 89 ef             	mov    rdi,rbp
    7a20:	e8 00 00 00 00       	call   7a25 <luaV_execute+0x5d35>
    7a25:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7a29:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7a2e:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7a35:	00 
    7a36:	41 83 e5 0c          	and    r13d,0xc
    7a3a:	e9 aa aa ff ff       	jmp    24e9 <luaV_execute+0x7f9>
        vmbreak;
    7a3f:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7a44:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7a49:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7a4e:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7a52:	48 89 ef             	mov    rdi,rbp
    7a55:	e8 00 00 00 00       	call   7a5a <luaV_execute+0x5d6a>
    7a5a:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7a5e:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7a63:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7a6a:	00 
    7a6b:	41 83 e5 0c          	and    r13d,0xc
    7a6f:	e9 01 ad ff ff       	jmp    2775 <luaV_execute+0xa85>
        GETTABLE_INLINE_PROTECTED_I(L, rb, rc, ra);
    7a74:	66 3d 25 80          	cmp    ax,0x8025
    7a78:	0f 84 57 19 00 00    	je     93d5 <luaV_execute+0x76e5>
    7a7e:	66 3d 15 80          	cmp    ax,0x8015
    7a82:	0f 85 4f 1b 00 00    	jne    95d7 <luaV_execute+0x78e7>
    7a88:	49 8b 0f             	mov    rcx,QWORD PTR [r15]
    7a8b:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    7a8e:	48 8b 51 38          	mov    rdx,QWORD PTR [rcx+0x38]
    7a92:	39 41 40             	cmp    DWORD PTR [rcx+0x40],eax
    7a95:	0f 86 95 22 00 00    	jbe    9d30 <luaV_execute+0x8040>
    7a9b:	89 c0                	mov    eax,eax
    7a9d:	41 b8 13 00 00 00    	mov    r8d,0x13
    7aa3:	48 8b 04 c2          	mov    rax,QWORD PTR [rdx+rax*8]
    7aa7:	66 44 89 45 08       	mov    WORD PTR [rbp+0x8],r8w
    7aac:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    7ab0:	e9 ad ac ff ff       	jmp    2762 <luaV_execute+0xa72>
        vmbreak;
    7ab5:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7aba:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7abf:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7ac4:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7ac8:	48 89 ef             	mov    rdi,rbp
    7acb:	e8 00 00 00 00       	call   7ad0 <luaV_execute+0x5de0>
    7ad0:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7ad4:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7ad9:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7ae0:	00 
    7ae1:	41 83 e5 0c          	and    r13d,0xc
    7ae5:	e9 7b aa ff ff       	jmp    2565 <luaV_execute+0x875>
        vmbreak;
    7aea:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7aef:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7af4:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7af9:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7afd:	48 89 ef             	mov    rdi,rbp
    7b00:	e8 00 00 00 00       	call   7b05 <luaV_execute+0x5e15>
    7b05:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7b09:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7b0e:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7b15:	00 
    7b16:	41 83 e5 0c          	and    r13d,0xc
    7b1a:	e9 22 ab ff ff       	jmp    2641 <luaV_execute+0x951>
        vmbreak;
    7b1f:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7b24:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7b29:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7b2e:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7b32:	48 89 ef             	mov    rdi,rbp
    7b35:	e8 00 00 00 00       	call   7b3a <luaV_execute+0x5e4a>
    7b3a:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7b3e:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7b43:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7b4a:	00 
    7b4b:	41 83 e5 0c          	and    r13d,0xc
    7b4f:	e9 80 ab ff ff       	jmp    26d4 <luaV_execute+0x9e4>
        vmbreak;
    7b54:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7b59:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7b5e:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7b63:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7b67:	48 89 ef             	mov    rdi,rbp
    7b6a:	e8 00 00 00 00       	call   7b6f <luaV_execute+0x5e7f>
    7b6f:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7b73:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7b78:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7b7f:	00 
    7b80:	41 83 e5 0c          	and    r13d,0xc
    7b84:	e9 1d af ff ff       	jmp    2aa6 <luaV_execute+0xdb6>
        vmbreak;
    7b89:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7b8e:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7b93:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7b98:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7b9c:	48 89 ef             	mov    rdi,rbp
    7b9f:	e8 00 00 00 00       	call   7ba4 <luaV_execute+0x5eb4>
    7ba4:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7ba8:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7bad:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7bb4:	00 
    7bb5:	41 83 e5 0c          	and    r13d,0xc
    7bb9:	e9 a6 a8 ff ff       	jmp    2464 <luaV_execute+0x774>
        vmbreak;
    7bbe:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7bc3:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7bc8:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7bcd:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7bd1:	48 89 ef             	mov    rdi,rbp
    7bd4:	e8 00 00 00 00       	call   7bd9 <luaV_execute+0x5ee9>
    7bd9:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7bdd:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7be2:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7be9:	00 
    7bea:	41 83 e5 0c          	and    r13d,0xc
    7bee:	e9 e1 ae ff ff       	jmp    2ad4 <luaV_execute+0xde4>
        vmbreak;
    7bf3:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7bf8:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7bfd:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7c02:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7c06:	48 89 ef             	mov    rdi,rbp
    7c09:	e8 00 00 00 00       	call   7c0e <luaV_execute+0x5f1e>
    7c0e:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7c12:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7c17:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7c1e:	00 
    7c1f:	41 83 e5 0c          	and    r13d,0xc
    7c23:	e9 b0 b1 ff ff       	jmp    2dd8 <luaV_execute+0x10e8>
        vmbreak;
    7c28:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7c2d:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7c32:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7c37:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7c3b:	48 89 ef             	mov    rdi,rbp
    7c3e:	e8 00 00 00 00       	call   7c43 <luaV_execute+0x5f53>
    7c43:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7c47:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7c4c:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7c53:	00 
    7c54:	41 83 e5 0c          	and    r13d,0xc
    7c58:	e9 c4 b1 ff ff       	jmp    2e21 <luaV_execute+0x1131>
        vmbreak;
    7c5d:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7c62:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7c67:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7c6c:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7c70:	48 89 ef             	mov    rdi,rbp
    7c73:	e8 00 00 00 00       	call   7c78 <luaV_execute+0x5f88>
    7c78:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7c7c:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7c81:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7c88:	00 
    7c89:	41 83 e5 0c          	and    r13d,0xc
    7c8d:	e9 84 be ff ff       	jmp    3b16 <luaV_execute+0x1e26>
        vmbreak;
    7c92:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7c97:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7c9c:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7ca1:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7ca5:	48 89 ef             	mov    rdi,rbp
    7ca8:	e8 00 00 00 00       	call   7cad <luaV_execute+0x5fbd>
    7cad:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7cb1:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7cb6:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7cbd:	00 
    7cbe:	41 83 e5 0c          	and    r13d,0xc
    7cc2:	e9 1a b2 ff ff       	jmp    2ee1 <luaV_execute+0x11f1>
    else return intop(>>, x, -y);
    7cc7:	48 d3 ea             	shr    rdx,cl
    7cca:	b8 00 00 00 00       	mov    eax,0x0
    7ccf:	48 83 f9 40          	cmp    rcx,0x40
    7cd3:	48 0f 4c c2          	cmovl  rax,rdx
    7cd7:	e9 c0 de ff ff       	jmp    5b9c <luaV_execute+0x3eac>
    if (y <= -NBITS) return 0;
    7cdc:	31 c0                	xor    eax,eax
    7cde:	48 83 f9 c1          	cmp    rcx,0xffffffffffffffc1
    7ce2:	0f 8c ff dd ff ff    	jl     5ae7 <luaV_execute+0x3df7>
    else return intop(>>, x, -y);
    7ce8:	f7 d9                	neg    ecx
    7cea:	48 89 d0             	mov    rax,rdx
    7ced:	48 d3 e8             	shr    rax,cl
    7cf0:	e9 f2 dd ff ff       	jmp    5ae7 <luaV_execute+0x3df7>
        vmbreak;
    7cf5:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7cfa:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7cff:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7d04:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7d08:	48 89 ef             	mov    rdi,rbp
    7d0b:	e8 00 00 00 00       	call   7d10 <luaV_execute+0x6020>
    7d10:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7d14:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7d19:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7d20:	00 
    7d21:	41 83 e5 0c          	and    r13d,0xc
    7d25:	e9 86 d1 ff ff       	jmp    4eb0 <luaV_execute+0x31c0>
        vmbreak;
    7d2a:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7d2f:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7d34:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7d39:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7d3d:	48 89 ef             	mov    rdi,rbp
    7d40:	e8 00 00 00 00       	call   7d45 <luaV_execute+0x6055>
    7d45:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7d49:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7d4e:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7d55:	00 
    7d56:	41 83 e5 0c          	and    r13d,0xc
    7d5a:	e9 8b cc ff ff       	jmp    49ea <luaV_execute+0x2cfa>
        vmbreak;
    7d5f:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7d64:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7d69:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7d6e:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7d72:	48 89 ef             	mov    rdi,rbp
    7d75:	e8 00 00 00 00       	call   7d7a <luaV_execute+0x608a>
    7d7a:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7d7e:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7d83:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7d8a:	00 
    7d8b:	41 83 e5 0c          	and    r13d,0xc
    7d8f:	e9 d7 cb ff ff       	jmp    496b <luaV_execute+0x2c7b>
        vmbreak;
    7d94:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7d99:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7d9e:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7da3:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7da7:	48 89 ef             	mov    rdi,rbp
    7daa:	e8 00 00 00 00       	call   7daf <luaV_execute+0x60bf>
    7daf:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7db3:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7db8:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7dbf:	00 
    7dc0:	41 83 e5 0c          	and    r13d,0xc
    7dc4:	e9 4b c1 ff ff       	jmp    3f14 <luaV_execute+0x2224>
        vmbreak;
    7dc9:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7dce:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7dd3:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7dd8:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7ddc:	48 89 ef             	mov    rdi,rbp
    7ddf:	e8 00 00 00 00       	call   7de4 <luaV_execute+0x60f4>
    7de4:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7de8:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7ded:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7df4:	00 
    7df5:	41 83 e5 0c          	and    r13d,0xc
    7df9:	e9 a3 b1 ff ff       	jmp    2fa1 <luaV_execute+0x12b1>
        vmbreak;
    7dfe:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7e03:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7e08:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7e0d:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7e11:	48 89 ef             	mov    rdi,rbp
    7e14:	e8 00 00 00 00       	call   7e19 <luaV_execute+0x6129>
    7e19:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7e1d:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7e22:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7e29:	00 
    7e2a:	41 83 e5 0c          	and    r13d,0xc
    7e2e:	e9 07 e3 ff ff       	jmp    613a <luaV_execute+0x444a>
        vmbreak;
    7e33:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7e38:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7e3d:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7e42:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7e46:	48 89 ef             	mov    rdi,rbp
    7e49:	e8 00 00 00 00       	call   7e4e <luaV_execute+0x615e>
    7e4e:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7e52:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7e57:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7e5e:	00 
    7e5f:	41 83 e5 0c          	and    r13d,0xc
    7e63:	e9 29 e9 ff ff       	jmp    6791 <luaV_execute+0x4aa1>
        vmbreak;
    7e68:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7e6d:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7e72:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7e77:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7e7b:	48 89 ef             	mov    rdi,rbp
    7e7e:	e8 00 00 00 00       	call   7e83 <luaV_execute+0x6193>
    7e83:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7e87:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7e8c:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7e93:	00 
    7e94:	41 83 e5 0c          	and    r13d,0xc
    7e98:	e9 54 bd ff ff       	jmp    3bf1 <luaV_execute+0x1f01>
        vmbreak;
    7e9d:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7ea2:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7ea7:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7eac:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7eb0:	48 89 ef             	mov    rdi,rbp
    7eb3:	e8 00 00 00 00       	call   7eb8 <luaV_execute+0x61c8>
    7eb8:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7ebc:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7ec1:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7ec8:	00 
    7ec9:	41 83 e5 0c          	and    r13d,0xc
    7ecd:	e9 7f bb ff ff       	jmp    3a51 <luaV_execute+0x1d61>
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    7ed2:	66 3d 25 80          	cmp    ax,0x8025
    7ed6:	0f 84 34 16 00 00    	je     9510 <luaV_execute+0x7820>
    7edc:	66 3d 15 80          	cmp    ax,0x8015
    7ee0:	0f 85 bb 16 00 00    	jne    95a1 <luaV_execute+0x78b1>
    7ee6:	66 83 79 08 13       	cmp    WORD PTR [rcx+0x8],0x13
    7eeb:	48 8b 6d 00          	mov    rbp,QWORD PTR [rbp+0x0]
    7eef:	0f 85 f4 18 00 00    	jne    97e9 <luaV_execute+0x7af9>
    7ef5:	49 8b 07             	mov    rax,QWORD PTR [r15]
    7ef8:	48 8b 75 38          	mov    rsi,QWORD PTR [rbp+0x38]
    7efc:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
    7eff:	89 c2                	mov    edx,eax
    7f01:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    7f04:	0f 86 a6 1d 00 00    	jbe    9cb0 <luaV_execute+0x7fc0>
    7f0a:	48 89 0c d6          	mov    QWORD PTR [rsi+rdx*8],rcx
    7f0e:	e9 2b bb ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    7f13:	66 3d 25 80          	cmp    ax,0x8025
    7f17:	0f 84 6c 15 00 00    	je     9489 <luaV_execute+0x7799>
    7f1d:	66 3d 15 80          	cmp    ax,0x8015
    7f21:	0f 85 88 17 00 00    	jne    96af <luaV_execute+0x79bf>
    7f27:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    7f2c:	4d 8b 3f             	mov    r15,QWORD PTR [r15]
    7f2f:	0f 85 be 1a 00 00    	jne    99f3 <luaV_execute+0x7d03>
    7f35:	66 83 79 08 13       	cmp    WORD PTR [rcx+0x8],0x13
    7f3a:	0f 85 51 1a 00 00    	jne    9991 <luaV_execute+0x7ca1>
    7f40:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    7f44:	49 8b 77 38          	mov    rsi,QWORD PTR [r15+0x38]
    7f48:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
    7f4b:	89 c2                	mov    edx,eax
    7f4d:	41 39 47 40          	cmp    DWORD PTR [r15+0x40],eax
    7f51:	0f 86 17 1e 00 00    	jbe    9d6e <luaV_execute+0x807e>
    7f57:	48 89 0c d6          	mov    QWORD PTR [rsi+rdx*8],rcx
    7f5b:	e9 7e bc ff ff       	jmp    3bde <luaV_execute+0x1eee>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    7f60:	66 3d 25 80          	cmp    ax,0x8025
    7f64:	0f 84 9a 14 00 00    	je     9404 <luaV_execute+0x7714>
    7f6a:	66 3d 15 80          	cmp    ax,0x8015
    7f6e:	0f 85 cf 16 00 00    	jne    9643 <luaV_execute+0x7953>
    7f74:	66 41 83 7f 08 13    	cmp    WORD PTR [r15+0x8],0x13
    7f7a:	48 8b 6d 00          	mov    rbp,QWORD PTR [rbp+0x0]
    7f7e:	0f 85 8b 1c 00 00    	jne    9c0f <luaV_execute+0x7f1f>
    7f84:	66 83 79 08 13       	cmp    WORD PTR [rcx+0x8],0x13
    7f89:	0f 85 0c 19 00 00    	jne    989b <luaV_execute+0x7bab>
    7f8f:	49 8b 07             	mov    rax,QWORD PTR [r15]
    7f92:	48 8b 75 38          	mov    rsi,QWORD PTR [rbp+0x38]
    7f96:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
    7f99:	89 c2                	mov    edx,eax
    7f9b:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    7f9e:	0f 86 a2 1d 00 00    	jbe    9d46 <luaV_execute+0x8056>
    7fa4:	48 89 0c d6          	mov    QWORD PTR [rsi+rdx*8],rcx
    7fa8:	e9 56 bb ff ff       	jmp    3b03 <luaV_execute+0x1e13>
        vmbreak;
    7fad:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7fb2:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7fb7:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7fbc:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7fc0:	48 89 ef             	mov    rdi,rbp
    7fc3:	e8 00 00 00 00       	call   7fc8 <luaV_execute+0x62d8>
    7fc8:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    7fcc:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    7fd1:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    7fd8:	00 
    7fd9:	41 83 e5 0c          	and    r13d,0xc
    7fdd:	e9 03 cb ff ff       	jmp    4ae5 <luaV_execute+0x2df5>
        vmbreak;
    7fe2:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    7fe7:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    7fec:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    7ff1:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    7ff5:	48 89 ef             	mov    rdi,rbp
    7ff8:	e8 00 00 00 00       	call   7ffd <luaV_execute+0x630d>
    7ffd:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8001:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8006:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    800d:	00 
    800e:	41 83 e5 0c          	and    r13d,0xc
    8012:	e9 44 cb ff ff       	jmp    4b5b <luaV_execute+0x2e6b>
        vmbreak;
    8017:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    801c:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8021:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8026:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    802a:	48 89 ef             	mov    rdi,rbp
    802d:	e8 00 00 00 00       	call   8032 <luaV_execute+0x6342>
    8032:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8036:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    803b:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8042:	00 
    8043:	41 83 e5 0c          	and    r13d,0xc
    8047:	e9 8c cb ff ff       	jmp    4bd8 <luaV_execute+0x2ee8>
        vmbreak;
    804c:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8051:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8056:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    805b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    805f:	48 89 ef             	mov    rdi,rbp
    8062:	e8 00 00 00 00       	call   8067 <luaV_execute+0x6377>
    8067:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    806b:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8070:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8077:	00 
    8078:	41 83 e5 0c          	and    r13d,0xc
    807c:	e9 cf cb ff ff       	jmp    4c50 <luaV_execute+0x2f60>
        vmbreak;
    8081:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8086:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    808b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8090:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8094:	48 89 ef             	mov    rdi,rbp
    8097:	e8 00 00 00 00       	call   809c <luaV_execute+0x63ac>
    809c:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    80a0:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    80a5:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    80ac:	00 
    80ad:	41 83 e5 0c          	and    r13d,0xc
    80b1:	e9 9f d3 ff ff       	jmp    5455 <luaV_execute+0x3765>
        vmbreak;
    80b6:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    80bb:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    80c0:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    80c5:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    80c9:	48 89 ef             	mov    rdi,rbp
    80cc:	e8 00 00 00 00       	call   80d1 <luaV_execute+0x63e1>
    80d1:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    80d5:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    80da:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    80e1:	00 
    80e2:	41 83 e5 0c          	and    r13d,0xc
    80e6:	e9 6c d2 ff ff       	jmp    5357 <luaV_execute+0x3667>
        vmbreak;
    80eb:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    80f0:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    80f5:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    80fa:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    80fe:	48 89 ef             	mov    rdi,rbp
    8101:	e8 00 00 00 00       	call   8106 <luaV_execute+0x6416>
    8106:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    810a:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    810f:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8116:	00 
    8117:	41 83 e5 0c          	and    r13d,0xc
    811b:	e9 7c bb ff ff       	jmp    3c9c <luaV_execute+0x1fac>
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    8120:	66 3d 25 80          	cmp    ax,0x8025
    8124:	0f 84 27 13 00 00    	je     9451 <luaV_execute+0x7761>
    812a:	66 3d 15 80          	cmp    ax,0x8015
    812e:	0f 85 d9 14 00 00    	jne    960d <luaV_execute+0x791d>
    8134:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    8139:	0f 85 10 19 00 00    	jne    9a4f <luaV_execute+0x7d5f>
    813f:	49 8b 0f             	mov    rcx,QWORD PTR [r15]
    8142:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    8145:	48 8b 51 38          	mov    rdx,QWORD PTR [rcx+0x38]
    8149:	39 41 40             	cmp    DWORD PTR [rcx+0x40],eax
    814c:	0f 86 de 1b 00 00    	jbe    9d30 <luaV_execute+0x8040>
    8152:	89 c0                	mov    eax,eax
    8154:	b9 13 00 00 00       	mov    ecx,0x13
    8159:	48 8b 04 c2          	mov    rax,QWORD PTR [rdx+rax*8]
    815d:	66 89 4d 08          	mov    WORD PTR [rbp+0x8],cx
    8161:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    8165:	e9 1f bb ff ff       	jmp    3c89 <luaV_execute+0x1f99>
        vmbreak;
    816a:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    816f:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8174:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8179:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    817d:	48 89 ef             	mov    rdi,rbp
    8180:	e8 00 00 00 00       	call   8185 <luaV_execute+0x6495>
    8185:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8189:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    818e:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8195:	00 
    8196:	41 83 e5 0c          	and    r13d,0xc
    819a:	e9 ad bb ff ff       	jmp    3d4c <luaV_execute+0x205c>
        vmbreak;
    819f:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    81a4:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    81a9:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    81ae:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    81b2:	48 89 ef             	mov    rdi,rbp
    81b5:	e8 00 00 00 00       	call   81ba <luaV_execute+0x64ca>
    81ba:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    81be:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    81c3:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    81ca:	00 
    81cb:	41 83 e5 0c          	and    r13d,0xc
    81cf:	e9 f1 ca ff ff       	jmp    4cc5 <luaV_execute+0x2fd5>
        vmbreak;
    81d4:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    81d9:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    81de:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    81e3:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    81e7:	48 89 ef             	mov    rdi,rbp
    81ea:	e8 00 00 00 00       	call   81ef <luaV_execute+0x64ff>
    81ef:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    81f3:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    81f8:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    81ff:	00 
    8200:	41 83 e5 0c          	and    r13d,0xc
    8204:	e9 39 cb ff ff       	jmp    4d42 <luaV_execute+0x3052>
        vmbreak;
    8209:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    820e:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8213:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8218:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    821c:	48 89 ef             	mov    rdi,rbp
    821f:	e8 00 00 00 00       	call   8224 <luaV_execute+0x6534>
    8224:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8228:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    822d:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8234:	00 
    8235:	41 83 e5 0c          	and    r13d,0xc
    8239:	e9 85 cb ff ff       	jmp    4dc3 <luaV_execute+0x30d3>
        vmbreak;
    823e:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8243:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8248:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    824d:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8251:	48 89 ef             	mov    rdi,rbp
    8254:	e8 00 00 00 00       	call   8259 <luaV_execute+0x6569>
    8259:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    825d:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8262:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8269:	00 
    826a:	41 83 e5 0c          	and    r13d,0xc
    826e:	e9 c8 cb ff ff       	jmp    4e3b <luaV_execute+0x314b>
        if (RAVI_LIKELY(tonumber(ra, &j))) { setfltvalue(ra, j); }
    8273:	48 8d b4 24 00 01 00 	lea    rsi,[rsp+0x100]
    827a:	00 
    827b:	48 89 ef             	mov    rdi,rbp
    827e:	e8 00 00 00 00       	call   8283 <luaV_execute+0x6593>
    8283:	85 c0                	test   eax,eax
    8285:	74 0e                	je     8295 <luaV_execute+0x65a5>
    8287:	f2 0f 10 84 24 00 01 	movsd  xmm0,QWORD PTR [rsp+0x100]
    828e:	00 00 
    8290:	e9 0e c4 ff ff       	jmp    46a3 <luaV_execute+0x29b3>
    8295:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "TOFLT: number expected");
    829a:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 82a1 <luaV_execute+0x65b1>
    82a1:	31 c0                	xor    eax,eax
    82a3:	4c 89 ef             	mov    rdi,r13
    82a6:	e8 00 00 00 00       	call   82ab <luaV_execute+0x65bb>
        vmbreak;
    82ab:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    82b0:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    82b5:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    82ba:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    82be:	48 89 ef             	mov    rdi,rbp
    82c1:	e8 00 00 00 00       	call   82c6 <luaV_execute+0x65d6>
    82c6:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    82ca:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    82cf:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    82d6:	00 
    82d7:	41 83 e5 0c          	and    r13d,0xc
    82db:	e9 56 9f ff ff       	jmp    2236 <luaV_execute+0x546>
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    82e0:	83 e0 0f             	and    eax,0xf
    82e3:	66 83 f8 05          	cmp    ax,0x5
    82e7:	0f 84 53 19 00 00    	je     9c40 <luaV_execute+0x7f50>
    82ed:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    82f2:	48 89 e9             	mov    rcx,rbp
    82f5:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    82fa:	45 31 c0             	xor    r8d,r8d
    82fd:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8301:	48 89 ef             	mov    rdi,rbp
    8304:	e8 00 00 00 00       	call   8309 <luaV_execute+0x6619>
    8309:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8310:	00 
    8311:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8315:	41 83 e5 0c          	and    r13d,0xc
    8319:	e9 bd 9f ff ff       	jmp    22db <luaV_execute+0x5eb>
        vmbreak;
    831e:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8323:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8328:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    832d:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8331:	48 89 ef             	mov    rdi,rbp
    8334:	e8 00 00 00 00       	call   8339 <luaV_execute+0x6649>
    8339:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    833d:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8342:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8349:	00 
    834a:	41 83 e5 0c          	and    r13d,0xc
    834e:	e9 9b 9f ff ff       	jmp    22ee <luaV_execute+0x5fe>
        vmbreak;
    8353:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8358:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    835d:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8362:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8366:	48 89 ef             	mov    rdi,rbp
    8369:	e8 00 00 00 00       	call   836e <luaV_execute+0x667e>
    836e:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8372:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8377:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    837e:	00 
    837f:	41 83 e5 0c          	and    r13d,0xc
    8383:	e9 bd ac ff ff       	jmp    3045 <luaV_execute+0x1355>
        vmbreak;
    8388:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    838d:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8392:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8397:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    839b:	48 89 ef             	mov    rdi,rbp
    839e:	e8 00 00 00 00       	call   83a3 <luaV_execute+0x66b3>
    83a3:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    83a7:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    83ac:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    83b3:	00 
    83b4:	41 83 e5 0c          	and    r13d,0xc
    83b8:	e9 ae e9 ff ff       	jmp    6d6b <luaV_execute+0x507b>
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    83bd:	66 3d 25 80          	cmp    ax,0x8025
    83c1:	0f 84 0f 11 00 00    	je     94d6 <luaV_execute+0x77e6>
    83c7:	66 3d 15 80          	cmp    ax,0x8015
    83cb:	0f 85 a8 12 00 00    	jne    9679 <luaV_execute+0x7989>
    83d1:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    83d6:	0f 85 4a 18 00 00    	jne    9c26 <luaV_execute+0x7f36>
    83dc:	49 8b 0f             	mov    rcx,QWORD PTR [r15]
    83df:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    83e2:	48 8b 51 38          	mov    rdx,QWORD PTR [rcx+0x38]
    83e6:	39 41 40             	cmp    DWORD PTR [rcx+0x40],eax
    83e9:	0f 86 41 19 00 00    	jbe    9d30 <luaV_execute+0x8040>
    83ef:	89 c0                	mov    eax,eax
    83f1:	bf 13 00 00 00       	mov    edi,0x13
    83f6:	48 8b 04 c2          	mov    rax,QWORD PTR [rdx+rax*8]
    83fa:	66 89 7d 08          	mov    WORD PTR [rbp+0x8],di
    83fe:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    8402:	e9 32 b9 ff ff       	jmp    3d39 <luaV_execute+0x2049>
        vmbreak;
    8407:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    840c:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8411:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8416:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    841a:	48 89 ef             	mov    rdi,rbp
    841d:	e8 00 00 00 00       	call   8422 <luaV_execute+0x6732>
    8422:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8426:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    842b:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8432:	00 
    8433:	41 83 e5 0c          	and    r13d,0xc
    8437:	e9 82 bb ff ff       	jmp    3fbe <luaV_execute+0x22ce>
        vmbreak;
    843c:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8441:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8446:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    844b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    844f:	48 89 ef             	mov    rdi,rbp
    8452:	e8 00 00 00 00       	call   8457 <luaV_execute+0x6767>
    8457:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    845b:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8460:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8467:	00 
    8468:	41 83 e5 0c          	and    r13d,0xc
    846c:	e9 f7 bb ff ff       	jmp    4068 <luaV_execute+0x2378>
        vmbreak;
    8471:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8476:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    847b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8480:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8484:	48 89 ef             	mov    rdi,rbp
    8487:	e8 00 00 00 00       	call   848c <luaV_execute+0x679c>
    848c:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8490:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8495:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    849c:	00 
    849d:	41 83 e5 0c          	and    r13d,0xc
    84a1:	e9 6c bc ff ff       	jmp    4112 <luaV_execute+0x2422>
        vmbreak;
    84a6:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    84ab:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    84b0:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    84b5:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    84b9:	48 89 ef             	mov    rdi,rbp
    84bc:	e8 00 00 00 00       	call   84c1 <luaV_execute+0x67d1>
    84c1:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    84c5:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    84ca:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    84d1:	00 
    84d2:	41 83 e5 0c          	and    r13d,0xc
    84d6:	e9 e3 bc ff ff       	jmp    41be <luaV_execute+0x24ce>
        vmbreak;
    84db:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    84e0:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    84e5:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    84ea:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    84ee:	48 89 ef             	mov    rdi,rbp
    84f1:	e8 00 00 00 00       	call   84f6 <luaV_execute+0x6806>
    84f6:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    84fa:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    84ff:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8506:	00 
    8507:	41 83 e5 0c          	and    r13d,0xc
    850b:	e9 fe e7 ff ff       	jmp    6d0e <luaV_execute+0x501e>
        vmbreak;
    8510:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8515:	48 89 f5             	mov    rbp,rsi
    8518:	48 89 f7             	mov    rdi,rsi
    851b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8520:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8524:	e8 00 00 00 00       	call   8529 <luaV_execute+0x6839>
    8529:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    852d:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8532:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8539:	00 
    853a:	41 83 e5 0c          	and    r13d,0xc
    853e:	e9 53 b4 ff ff       	jmp    3996 <luaV_execute+0x1ca6>
        vmbreak;
    8543:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8548:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    854d:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8552:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8556:	48 89 ef             	mov    rdi,rbp
    8559:	e8 00 00 00 00       	call   855e <luaV_execute+0x686e>
    855e:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8562:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8567:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    856e:	00 
    856f:	41 83 e5 0c          	and    r13d,0xc
    8573:	e9 cc cf ff ff       	jmp    5544 <luaV_execute+0x3854>
        vmbreak;
    8578:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    857d:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8582:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8587:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    858b:	48 89 ef             	mov    rdi,rbp
    858e:	e8 00 00 00 00       	call   8593 <luaV_execute+0x68a3>
    8593:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8597:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    859c:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    85a3:	00 
    85a4:	41 83 e5 0c          	and    r13d,0xc
    85a8:	e9 12 d0 ff ff       	jmp    55bf <luaV_execute+0x38cf>
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    85ad:	83 e0 0f             	and    eax,0xf
    85b0:	66 83 f8 05          	cmp    ax,0x5
    85b4:	0f 84 d0 15 00 00    	je     9b8a <luaV_execute+0x7e9a>
    85ba:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    85bf:	48 89 e9             	mov    rcx,rbp
    85c2:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    85c7:	45 31 c0             	xor    r8d,r8d
    85ca:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    85ce:	48 89 ef             	mov    rdi,rbp
    85d1:	e8 00 00 00 00       	call   85d6 <luaV_execute+0x68e6>
    85d6:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    85dd:	00 
    85de:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    85e2:	41 83 e5 0c          	and    r13d,0xc
    85e6:	e9 88 bc ff ff       	jmp    4273 <luaV_execute+0x2583>
        vmbreak;
    85eb:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    85f0:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    85f5:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    85fa:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    85fe:	48 89 ef             	mov    rdi,rbp
    8601:	e8 00 00 00 00       	call   8606 <luaV_execute+0x6916>
    8606:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    860a:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    860f:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8616:	00 
    8617:	41 83 e5 0c          	and    r13d,0xc
    861b:	e9 0d c9 ff ff       	jmp    4f2d <luaV_execute+0x323d>
        vmbreak;
    8620:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8625:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    862a:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    862f:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8633:	48 89 ef             	mov    rdi,rbp
    8636:	e8 00 00 00 00       	call   863b <luaV_execute+0x694b>
    863b:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    863f:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8644:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    864b:	00 
    864c:	41 83 e5 0c          	and    r13d,0xc
    8650:	e9 52 c9 ff ff       	jmp    4fa7 <luaV_execute+0x32b7>
        vmbreak;
    8655:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    865a:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    865f:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8664:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8668:	48 89 ef             	mov    rdi,rbp
    866b:	e8 00 00 00 00       	call   8670 <luaV_execute+0x6980>
    8670:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8674:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8679:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8680:	00 
    8681:	41 83 e5 0c          	and    r13d,0xc
    8685:	e9 e3 c3 ff ff       	jmp    4a6d <luaV_execute+0x2d7d>
        vmbreak;
    868a:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    868f:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8694:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8699:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    869d:	48 89 ef             	mov    rdi,rbp
    86a0:	e8 00 00 00 00       	call   86a5 <luaV_execute+0x69b5>
    86a5:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    86a9:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    86ae:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    86b5:	00 
    86b6:	41 83 e5 0c          	and    r13d,0xc
    86ba:	e9 03 a2 ff ff       	jmp    28c2 <luaV_execute+0xbd2>
        vmbreak;
    86bf:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    86c4:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    86c9:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    86ce:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    86d2:	48 89 ef             	mov    rdi,rbp
    86d5:	e8 00 00 00 00       	call   86da <luaV_execute+0x69ea>
    86da:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    86de:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    86e3:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    86ea:	00 
    86eb:	41 83 e5 0c          	and    r13d,0xc
    86ef:	e9 ea ab ff ff       	jmp    32de <luaV_execute+0x15ee>
        vmbreak;
    86f4:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    86f9:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    86fe:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8703:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8707:	48 89 ef             	mov    rdi,rbp
    870a:	e8 00 00 00 00       	call   870f <luaV_execute+0x6a1f>
    870f:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8713:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8718:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    871f:	00 
    8720:	41 83 e5 0c          	and    r13d,0xc
    8724:	e9 23 ac ff ff       	jmp    334c <luaV_execute+0x165c>
        if (RAVI_LIKELY(tointeger(ra, &j))) { setivalue(ra, j); }
    8729:	31 d2                	xor    edx,edx
    872b:	48 8d b4 24 f8 00 00 	lea    rsi,[rsp+0xf8]
    8732:	00 
    8733:	48 89 ef             	mov    rdi,rbp
    8736:	e8 00 00 00 00       	call   873b <luaV_execute+0x6a4b>
    873b:	85 c0                	test   eax,eax
    873d:	74 0d                	je     874c <luaV_execute+0x6a5c>
    873f:	48 8b 84 24 f8 00 00 	mov    rax,QWORD PTR [rsp+0xf8]
    8746:	00 
    8747:	e9 aa bf ff ff       	jmp    46f6 <luaV_execute+0x2a06>
    874c:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "TOINT: integer expected");
    8751:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 8758 <luaV_execute+0x6a68>
    8758:	31 c0                	xor    eax,eax
    875a:	4c 89 ef             	mov    rdi,r13
    875d:	e8 00 00 00 00       	call   8762 <luaV_execute+0x6a72>
        if (RAVI_LIKELY(tonumber(rb, &j))) { setfltvalue(ra, j); }
    8762:	48 8d b4 24 f0 00 00 	lea    rsi,[rsp+0xf0]
    8769:	00 
    876a:	e8 00 00 00 00       	call   876f <luaV_execute+0x6a7f>
    876f:	85 c0                	test   eax,eax
    8771:	74 0e                	je     8781 <luaV_execute+0x6a91>
    8773:	f2 0f 10 84 24 f0 00 	movsd  xmm0,QWORD PTR [rsp+0xf0]
    877a:	00 00 
    877c:	e9 de c0 ff ff       	jmp    485f <luaV_execute+0x2b6f>
    8781:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "MOVEF: number expected");
    8786:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 878d <luaV_execute+0x6a9d>
    878d:	31 c0                	xor    eax,eax
    878f:	4c 89 ef             	mov    rdi,r13
    8792:	e8 00 00 00 00       	call   8797 <luaV_execute+0x6aa7>
        vmbreak;
    8797:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    879c:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    87a1:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    87a6:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    87aa:	48 89 ef             	mov    rdi,rbp
    87ad:	e8 00 00 00 00       	call   87b2 <luaV_execute+0x6ac2>
    87b2:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    87b6:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    87bb:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    87c2:	00 
    87c3:	41 83 e5 0c          	and    r13d,0xc
    87c7:	e9 7d bd ff ff       	jmp    4549 <luaV_execute+0x2859>
        vmbreak;
    87cc:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    87d1:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    87d6:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    87db:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    87df:	48 89 ef             	mov    rdi,rbp
    87e2:	e8 00 00 00 00       	call   87e7 <luaV_execute+0x6af7>
    87e7:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    87eb:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    87f0:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    87f7:	00 
    87f8:	41 83 e5 0c          	and    r13d,0xc
    87fc:	e9 cd ca ff ff       	jmp    52ce <luaV_execute+0x35de>
        vmbreak;
    8801:	48 8b 5c 24 18       	mov    rbx,QWORD PTR [rsp+0x18]
    8806:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    880b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8810:	4c 89 7b 28          	mov    QWORD PTR [rbx+0x28],r15
    8814:	48 89 ef             	mov    rdi,rbp
    8817:	e8 00 00 00 00       	call   881c <luaV_execute+0x6b2c>
    881c:	4c 8b 63 20          	mov    r12,QWORD PTR [rbx+0x20]
    8820:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8825:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    882c:	00 
    882d:	41 83 e5 0c          	and    r13d,0xc
    8831:	e9 a2 bc ff ff       	jmp    44d8 <luaV_execute+0x27e8>
        vmbreak;
    8836:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    883b:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8840:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8845:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8849:	48 89 ef             	mov    rdi,rbp
    884c:	e8 00 00 00 00       	call   8851 <luaV_execute+0x6b61>
    8851:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8855:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    885a:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8861:	00 
    8862:	41 83 e5 0c          	and    r13d,0xc
    8866:	e9 94 a3 ff ff       	jmp    2bff <luaV_execute+0xf0f>
        vmbreak;
    886b:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8870:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8875:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    887a:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    887e:	48 89 ef             	mov    rdi,rbp
    8881:	e8 00 00 00 00       	call   8886 <luaV_execute+0x6b96>
    8886:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    888a:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    888f:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8896:	00 
    8897:	41 83 e5 0c          	and    r13d,0xc
    889b:	e9 64 98 ff ff       	jmp    2104 <luaV_execute+0x414>
        vmbreak;
    88a0:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    88a5:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    88aa:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    88af:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    88b3:	48 89 ef             	mov    rdi,rbp
    88b6:	e8 00 00 00 00       	call   88bb <luaV_execute+0x6bcb>
    88bb:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    88bf:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    88c4:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    88cb:	00 
    88cc:	41 83 e5 0c          	and    r13d,0xc
    88d0:	e9 dd 9c ff ff       	jmp    25b2 <luaV_execute+0x8c2>
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    88d5:	83 e0 0f             	and    eax,0xf
    88d8:	66 83 f8 05          	cmp    ax,0x5
    88dc:	0f 84 d2 12 00 00    	je     9bb4 <luaV_execute+0x7ec4>
    88e2:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    88e7:	48 89 e9             	mov    rcx,rbp
    88ea:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    88ef:	45 31 c0             	xor    r8d,r8d
    88f2:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    88f6:	48 89 ef             	mov    rdi,rbp
    88f9:	e8 00 00 00 00       	call   88fe <luaV_execute+0x6c0e>
    88fe:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8905:	00 
    8906:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    890a:	41 83 e5 0c          	and    r13d,0xc
    890e:	e9 10 99 ff ff       	jmp    2223 <luaV_execute+0x533>
        vmbreak;
    8913:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8918:	48 89 cd             	mov    rbp,rcx
    891b:	48 89 cf             	mov    rdi,rcx
    891e:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8923:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8927:	e8 00 00 00 00       	call   892c <luaV_execute+0x6c3c>
    892c:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8930:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8935:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    893c:	00 
    893d:	41 83 e5 0c          	and    r13d,0xc
    8941:	e9 55 a8 ff ff       	jmp    319b <luaV_execute+0x14ab>
        vmbreak;
    8946:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    894b:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8950:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8955:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8959:	48 89 ef             	mov    rdi,rbp
    895c:	e8 00 00 00 00       	call   8961 <luaV_execute+0x6c71>
    8961:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8965:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    896a:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8971:	00 
    8972:	41 83 e5 0c          	and    r13d,0xc
    8976:	e9 c8 ba ff ff       	jmp    4443 <luaV_execute+0x2753>
        vmbreak;
    897b:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8980:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8985:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    898a:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    898e:	48 89 ef             	mov    rdi,rbp
    8991:	e8 00 00 00 00       	call   8996 <luaV_execute+0x6ca6>
    8996:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    899a:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    899f:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    89a6:	00 
    89a7:	41 83 e5 0c          	and    r13d,0xc
    89ab:	e9 e0 ba ff ff       	jmp    4490 <luaV_execute+0x27a0>
        vmbreak;
    89b0:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    89b5:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    89ba:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    89bf:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    89c3:	48 89 ef             	mov    rdi,rbp
    89c6:	e8 00 00 00 00       	call   89cb <luaV_execute+0x6cdb>
    89cb:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    89cf:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    89d4:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    89db:	00 
    89dc:	41 83 e5 0c          	and    r13d,0xc
    89e0:	e9 84 c8 ff ff       	jmp    5269 <luaV_execute+0x3579>
        if (tointeger(ra, &ia)) {
    89e5:	31 d2                	xor    edx,edx
    89e7:	48 8d b4 24 d8 00 00 	lea    rsi,[rsp+0xd8]
    89ee:	00 
    89ef:	48 89 ef             	mov    rdi,rbp
    89f2:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    89f7:	e8 00 00 00 00       	call   89fc <luaV_execute+0x6d0c>
    89fc:	85 c0                	test   eax,eax
    89fe:	0f 84 9a 11 00 00    	je     9b9e <luaV_execute+0x7eae>
    8a04:	48 8b 94 24 d8 00 00 	mov    rdx,QWORD PTR [rsp+0xd8]
    8a0b:	00 
    8a0c:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8a11:	e9 85 c8 ff ff       	jmp    529b <luaV_execute+0x35ab>
        if (RAVI_LIKELY(tointeger(rb, &j))) { setivalue(ra, j); }
    8a16:	31 d2                	xor    edx,edx
    8a18:	48 8d b4 24 e8 00 00 	lea    rsi,[rsp+0xe8]
    8a1f:	00 
    8a20:	e8 00 00 00 00       	call   8a25 <luaV_execute+0x6d35>
    8a25:	85 c0                	test   eax,eax
    8a27:	74 0d                	je     8a36 <luaV_execute+0x6d46>
    8a29:	48 8b 84 24 e8 00 00 	mov    rax,QWORD PTR [rsp+0xe8]
    8a30:	00 
    8a31:	e9 8b be ff ff       	jmp    48c1 <luaV_execute+0x2bd1>
    8a36:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L, "MOVEI: integer expected");
    8a3b:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 8a42 <luaV_execute+0x6d52>
    8a42:	31 c0                	xor    eax,eax
    8a44:	4c 89 ef             	mov    rdi,r13
    8a47:	e8 00 00 00 00       	call   8a4c <luaV_execute+0x6d5c>
        vmbreak;
    8a4c:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8a51:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8a56:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8a5b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8a5f:	48 89 ef             	mov    rdi,rbp
    8a62:	e8 00 00 00 00       	call   8a67 <luaV_execute+0x6d77>
    8a67:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8a6b:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8a70:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8a77:	00 
    8a78:	41 83 e5 0c          	and    r13d,0xc
    8a7c:	e9 75 c5 ff ff       	jmp    4ff6 <luaV_execute+0x3306>
        vmbreak;
    8a81:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8a86:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8a8b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8a90:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8a94:	48 89 ef             	mov    rdi,rbp
    8a97:	e8 00 00 00 00       	call   8a9c <luaV_execute+0x6dac>
    8a9c:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8aa0:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8aa5:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8aac:	00 
    8aad:	41 83 e5 0c          	and    r13d,0xc
    8ab1:	e9 97 c5 ff ff       	jmp    504d <luaV_execute+0x335d>
        vmbreak;
    8ab6:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8abb:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8ac0:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8ac5:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8ac9:	48 89 ef             	mov    rdi,rbp
    8acc:	e8 00 00 00 00       	call   8ad1 <luaV_execute+0x6de1>
    8ad1:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8ad5:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8ada:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8ae1:	00 
    8ae2:	41 83 e5 0c          	and    r13d,0xc
    8ae6:	e9 16 c7 ff ff       	jmp    5201 <luaV_execute+0x3511>
        if (tonumber(ra, &na)) {
    8aeb:	48 8d b4 24 e0 00 00 	lea    rsi,[rsp+0xe0]
    8af2:	00 
    8af3:	48 89 ef             	mov    rdi,rbp
    8af6:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8afb:	e8 00 00 00 00       	call   8b00 <luaV_execute+0x6e10>
    8b00:	85 c0                	test   eax,eax
    8b02:	0f 84 10 10 00 00    	je     9b18 <luaV_execute+0x7e28>
    8b08:	f2 0f 10 84 24 e0 00 	movsd  xmm0,QWORD PTR [rsp+0xe0]
    8b0f:	00 00 
    8b11:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8b16:	e9 1a c7 ff ff       	jmp    5235 <luaV_execute+0x3545>
        vmbreak;
    8b1b:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8b20:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8b25:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8b2a:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8b2e:	48 89 ef             	mov    rdi,rbp
    8b31:	e8 00 00 00 00       	call   8b36 <luaV_execute+0x6e46>
    8b36:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8b3a:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8b3f:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8b46:	00 
    8b47:	41 83 e5 0c          	and    r13d,0xc
    8b4b:	e9 56 b2 ff ff       	jmp    3da6 <luaV_execute+0x20b6>
        vmbreak;
    8b50:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8b55:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8b5a:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8b5f:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8b63:	48 89 ef             	mov    rdi,rbp
    8b66:	e8 00 00 00 00       	call   8b6b <luaV_execute+0x6e7b>
    8b6b:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8b6f:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8b74:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8b7b:	00 
    8b7c:	41 83 e5 0c          	and    r13d,0xc
    8b80:	e9 1b a0 ff ff       	jmp    2ba0 <luaV_execute+0xeb0>
          lua_Number step = fltvalue(ra + 2);
    8b85:	f2 0f 10 55 20       	movsd  xmm2,QWORD PTR [rbp+0x20]
          if (luai_numlt(0, step) ? luai_numle(idx, limit)
    8b8a:	66 0f 2f 15 00 00 00 	comisd xmm2,QWORD PTR [rip+0x0]        # 8b92 <luaV_execute+0x6ea2>
    8b91:	00 
          lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
    8b92:	f2 0f 10 45 00       	movsd  xmm0,QWORD PTR [rbp+0x0]
          lua_Number limit = fltvalue(ra + 1);
    8b97:	f2 0f 10 4d 10       	movsd  xmm1,QWORD PTR [rbp+0x10]
          lua_Number idx = luai_numadd(L, fltvalue(ra), step); /* inc. index */
    8b9c:	f2 0f 58 c2          	addsd  xmm0,xmm2
          if (luai_numlt(0, step) ? luai_numle(idx, limit)
    8ba0:	0f 86 23 08 00 00    	jbe    93c9 <luaV_execute+0x76d9>
    8ba6:	66 0f 2f c8          	comisd xmm1,xmm0
    8baa:	0f 93 c2             	setae  dl
    8bad:	48 89 d8             	mov    rax,rbx
    8bb0:	84 d2                	test   dl,dl
    8bb2:	0f 84 37 a0 ff ff    	je     2bef <luaV_execute+0xeff>
            pc += GETARG_sBx(i);  /* jump back */
    8bb8:	44 89 c2             	mov    edx,r8d
            setfltvalue(ra + 3, idx);  /* ...and external index */
    8bbb:	b9 03 00 00 00       	mov    ecx,0x3
            chgfltvalue(ra, idx);  /* update internal index... */
    8bc0:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
            pc += GETARG_sBx(i);  /* jump back */
    8bc5:	c1 ea 10             	shr    edx,0x10
            setfltvalue(ra + 3, idx);  /* ...and external index */
    8bc8:	f2 0f 11 45 30       	movsd  QWORD PTR [rbp+0x30],xmm0
            pc += GETARG_sBx(i);  /* jump back */
    8bcd:	48 8d 84 93 00 00 fe 	lea    rax,[rbx+rdx*4-0x20000]
    8bd4:	ff 
            setfltvalue(ra + 3, idx);  /* ...and external index */
    8bd5:	66 89 4d 38          	mov    WORD PTR [rbp+0x38],cx
    8bd9:	e9 11 a0 ff ff       	jmp    2bef <luaV_execute+0xeff>
        vmbreak;
    8bde:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8be3:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8be8:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8bed:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8bf1:	48 89 ef             	mov    rdi,rbp
    8bf4:	e8 00 00 00 00       	call   8bf9 <luaV_execute+0x6f09>
    8bf9:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8bfd:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8c02:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8c09:	00 
    8c0a:	41 83 e5 0c          	and    r13d,0xc
    8c0e:	e9 7b c4 ff ff       	jmp    508e <luaV_execute+0x339e>
        vmbreak;
    8c13:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8c18:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8c1d:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8c22:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8c26:	48 89 ef             	mov    rdi,rbp
    8c29:	e8 00 00 00 00       	call   8c2e <luaV_execute+0x6f3e>
    8c2e:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8c32:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8c37:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8c3e:	00 
    8c3f:	41 83 e5 0c          	and    r13d,0xc
    8c43:	e9 87 c4 ff ff       	jmp    50cf <luaV_execute+0x33df>
        vmbreak;
    8c48:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8c4d:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8c52:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8c57:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8c5b:	48 89 ef             	mov    rdi,rbp
    8c5e:	e8 00 00 00 00       	call   8c63 <luaV_execute+0x6f73>
    8c63:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8c67:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8c6c:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8c73:	00 
    8c74:	41 83 e5 0c          	and    r13d,0xc
    8c78:	e9 61 9b ff ff       	jmp    27de <luaV_execute+0xaee>
        vmbreak;
    8c7d:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8c82:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8c87:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8c8c:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8c90:	48 89 ef             	mov    rdi,rbp
    8c93:	e8 00 00 00 00       	call   8c98 <luaV_execute+0x6fa8>
    8c98:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8c9c:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8ca1:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8ca8:	00 
    8ca9:	41 83 e5 0c          	and    r13d,0xc
    8cad:	e9 83 c4 ff ff       	jmp    5135 <luaV_execute+0x3445>
        vmbreak;
    8cb2:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8cb7:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8cbc:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8cc1:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8cc5:	48 89 ef             	mov    rdi,rbp
    8cc8:	e8 00 00 00 00       	call   8ccd <luaV_execute+0x6fdd>
    8ccd:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8cd1:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8cd6:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8cdd:	00 
    8cde:	41 83 e5 0c          	and    r13d,0xc
    8ce2:	e9 b4 c4 ff ff       	jmp    519b <luaV_execute+0x34ab>
        vmbreak;
    8ce7:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8cec:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8cf1:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8cf6:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8cfa:	48 89 ef             	mov    rdi,rbp
    8cfd:	e8 00 00 00 00       	call   8d02 <luaV_execute+0x7012>
    8d02:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8d06:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8d0b:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8d12:	00 
    8d13:	41 83 e5 0c          	and    r13d,0xc
    8d17:	e9 2b 9c ff ff       	jmp    2947 <luaV_execute+0xc57>
        vmbreak;
    8d1c:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8d21:	48 89 c5             	mov    rbp,rax
    8d24:	48 89 c7             	mov    rdi,rax
    8d27:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8d2c:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8d30:	e8 00 00 00 00       	call   8d35 <luaV_execute+0x7045>
    8d35:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8d39:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8d3e:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8d45:	00 
    8d46:	41 83 e5 0c          	and    r13d,0xc
    8d4a:	e9 4d a3 ff ff       	jmp    309c <luaV_execute+0x13ac>
        vmbreak;
    8d4f:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8d54:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8d59:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8d5e:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8d62:	48 89 ef             	mov    rdi,rbp
    8d65:	e8 00 00 00 00       	call   8d6a <luaV_execute+0x707a>
    8d6a:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8d6e:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8d73:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8d7a:	00 
    8d7b:	41 83 e5 0c          	and    r13d,0xc
    8d7f:	e9 d3 a4 ff ff       	jmp    3257 <luaV_execute+0x1567>
        vmbreak;
    8d84:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8d89:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8d8e:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8d93:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8d97:	48 89 ef             	mov    rdi,rbp
    8d9a:	e8 00 00 00 00       	call   8d9f <luaV_execute+0x70af>
    8d9f:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8da3:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8da8:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8daf:	00 
    8db0:	41 83 e5 0c          	and    r13d,0xc
    8db4:	e9 42 a4 ff ff       	jmp    31fb <luaV_execute+0x150b>
        vmbreak;
    8db9:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8dbe:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8dc3:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8dc8:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8dcc:	48 89 ef             	mov    rdi,rbp
    8dcf:	e8 00 00 00 00       	call   8dd4 <luaV_execute+0x70e4>
    8dd4:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8dd8:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8ddd:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8de4:	00 
    8de5:	41 83 e5 0c          	and    r13d,0xc
    8de9:	e9 1f b5 ff ff       	jmp    430d <luaV_execute+0x261d>
        vmbreak;
    8dee:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8df3:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8df8:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8dfd:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8e01:	48 89 ef             	mov    rdi,rbp
    8e04:	e8 00 00 00 00       	call   8e09 <luaV_execute+0x7119>
    8e09:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8e0d:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8e12:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8e19:	00 
    8e1a:	41 83 e5 0c          	and    r13d,0xc
    8e1e:	e9 3b b5 ff ff       	jmp    435e <luaV_execute+0x266e>
        vmbreak;
    8e23:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8e28:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8e2d:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8e32:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8e36:	48 89 ef             	mov    rdi,rbp
    8e39:	e8 00 00 00 00       	call   8e3e <luaV_execute+0x714e>
    8e3e:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8e42:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8e47:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8e4e:	00 
    8e4f:	41 83 e5 0c          	and    r13d,0xc
    8e53:	e9 51 b5 ff ff       	jmp    43a9 <luaV_execute+0x26b9>
        vmbreak;
    8e58:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8e5d:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8e62:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8e67:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8e6b:	48 89 ef             	mov    rdi,rbp
    8e6e:	e8 00 00 00 00       	call   8e73 <luaV_execute+0x7183>
    8e73:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8e77:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8e7c:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8e83:	00 
    8e84:	41 83 e5 0c          	and    r13d,0xc
    8e88:	e9 6a b5 ff ff       	jmp    43f7 <luaV_execute+0x2707>
        vmbreak;
    8e8d:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8e92:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8e97:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8e9c:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8ea0:	48 89 ef             	mov    rdi,rbp
    8ea3:	e8 00 00 00 00       	call   8ea8 <luaV_execute+0x71b8>
    8ea8:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8eac:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8eb1:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8eb8:	00 
    8eb9:	41 83 e5 0c          	and    r13d,0xc
    8ebd:	e9 5c b9 ff ff       	jmp    481e <luaV_execute+0x2b2e>
          if (!tonumber(plimit, &nlimit))
    8ec2:	4c 89 cf             	mov    rdi,r9
    8ec5:	48 8d b4 24 c8 00 00 	lea    rsi,[rsp+0xc8]
    8ecc:	00 
    8ecd:	44 89 44 24 30       	mov    DWORD PTR [rsp+0x30],r8d
    8ed2:	4c 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],r9
    8ed7:	e8 00 00 00 00       	call   8edc <luaV_execute+0x71ec>
    8edc:	4c 8b 4c 24 28       	mov    r9,QWORD PTR [rsp+0x28]
    8ee1:	44 8b 44 24 30       	mov    r8d,DWORD PTR [rsp+0x30]
    8ee6:	85 c0                	test   eax,eax
    8ee8:	0f 84 40 0c 00 00    	je     9b2e <luaV_execute+0x7e3e>
    8eee:	f2 0f 10 84 24 c8 00 	movsd  xmm0,QWORD PTR [rsp+0xc8]
    8ef5:	00 00 
    8ef7:	e9 29 9c ff ff       	jmp    2b25 <luaV_execute+0xe35>
          if (!tonumber(pstep, &nstep))
    8efc:	48 8d b4 24 d0 00 00 	lea    rsi,[rsp+0xd0]
    8f03:	00 
    8f04:	4c 89 ff             	mov    rdi,r15
    8f07:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8f0c:	e8 00 00 00 00       	call   8f11 <luaV_execute+0x7221>
    8f11:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8f16:	85 c0                	test   eax,eax
    8f18:	0f 84 b8 0b 00 00    	je     9ad6 <luaV_execute+0x7de6>
    8f1e:	f2 0f 10 8c 24 d0 00 	movsd  xmm1,QWORD PTR [rsp+0xd0]
    8f25:	00 00 
    8f27:	e9 23 9c ff ff       	jmp    2b4f <luaV_execute+0xe5f>
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    8f2c:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8f31:	48 89 e9             	mov    rcx,rbp
    8f34:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8f39:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8f3d:	48 89 ef             	mov    rdi,rbp
    8f40:	e8 00 00 00 00       	call   8f45 <luaV_execute+0x7255>
    8f45:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8f4c:	00 
    8f4d:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8f51:	41 83 e5 0c          	and    r13d,0xc
    8f55:	e9 c9 92 ff ff       	jmp    2223 <luaV_execute+0x533>
        vmbreak;
    8f5a:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8f5f:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8f64:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8f69:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8f6d:	48 89 ef             	mov    rdi,rbp
    8f70:	e8 00 00 00 00       	call   8f75 <luaV_execute+0x7285>
    8f75:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8f79:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8f7e:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8f85:	00 
    8f86:	41 83 e5 0c          	and    r13d,0xc
    8f8a:	e9 f5 b5 ff ff       	jmp    4584 <luaV_execute+0x2894>
        vmbreak;
    8f8f:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8f94:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8f99:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8f9e:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8fa2:	48 89 ef             	mov    rdi,rbp
    8fa5:	e8 00 00 00 00       	call   8faa <luaV_execute+0x72ba>
    8faa:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8fae:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8fb3:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8fba:	00 
    8fbb:	41 83 e5 0c          	and    r13d,0xc
    8fbf:	e9 37 b6 ff ff       	jmp    45fb <luaV_execute+0x290b>
        vmbreak;
    8fc4:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8fc9:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    8fce:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    8fd3:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    8fd7:	48 89 ef             	mov    rdi,rbp
    8fda:	e8 00 00 00 00       	call   8fdf <luaV_execute+0x72ef>
    8fdf:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    8fe3:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    8fe8:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    8fef:	00 
    8ff0:	41 83 e5 0c          	and    r13d,0xc
    8ff4:	e9 3c b6 ff ff       	jmp    4635 <luaV_execute+0x2945>
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    8ff9:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    8ffe:	48 89 e9             	mov    rcx,rbp
    9001:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    9006:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    900a:	48 89 ef             	mov    rdi,rbp
    900d:	e8 00 00 00 00       	call   9012 <luaV_execute+0x7322>
    9012:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    9019:	00 
    901a:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    901e:	41 83 e5 0c          	and    r13d,0xc
    9022:	e9 b4 92 ff ff       	jmp    22db <luaV_execute+0x5eb>
        vmbreak;
    9027:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    902c:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    9031:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    9036:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    903a:	48 89 ef             	mov    rdi,rbp
    903d:	e8 00 00 00 00       	call   9042 <luaV_execute+0x7352>
    9042:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    9046:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    904b:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    9052:	00 
    9053:	41 83 e5 0c          	and    r13d,0xc
    9057:	e9 12 b7 ff ff       	jmp    476e <luaV_execute+0x2a7e>
        vmbreak;
    905c:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    9061:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    9066:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    906b:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    906f:	48 89 ef             	mov    rdi,rbp
    9072:	e8 00 00 00 00       	call   9077 <luaV_execute+0x7387>
    9077:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    907b:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    9080:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    9087:	00 
    9088:	41 83 e5 0c          	and    r13d,0xc
    908c:	e9 33 b6 ff ff       	jmp    46c4 <luaV_execute+0x29d4>
        vmbreak;
    9091:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    9096:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    909b:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    90a0:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    90a4:	48 89 ef             	mov    rdi,rbp
    90a7:	e8 00 00 00 00       	call   90ac <luaV_execute+0x73bc>
    90ac:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    90b0:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    90b5:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    90bc:	00 
    90bd:	41 83 e5 0c          	and    r13d,0xc
    90c1:	e9 bc b7 ff ff       	jmp    4882 <luaV_execute+0x2b92>
        vmbreak;
    90c6:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    90cb:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    90d0:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    90d5:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    90d9:	48 89 ef             	mov    rdi,rbp
    90dc:	e8 00 00 00 00       	call   90e1 <luaV_execute+0x73f1>
    90e1:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    90e5:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    90ea:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    90f1:	00 
    90f2:	41 83 e5 0c          	and    r13d,0xc
    90f6:	e9 cb b6 ff ff       	jmp    47c6 <luaV_execute+0x2ad6>
        vmbreak;
    90fb:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    9100:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    9105:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    910a:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    910e:	48 89 ef             	mov    rdi,rbp
    9111:	e8 00 00 00 00       	call   9116 <luaV_execute+0x7426>
    9116:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    911a:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    911f:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    9126:	00 
    9127:	41 83 e5 0c          	and    r13d,0xc
    912b:	e9 8f b4 ff ff       	jmp    45bf <luaV_execute+0x28cf>
        vmbreak;
    9130:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    9135:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    913a:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    913f:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    9143:	48 89 ef             	mov    rdi,rbp
    9146:	e8 00 00 00 00       	call   914b <luaV_execute+0x745b>
    914b:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    914f:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    9154:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    915b:	00 
    915c:	41 83 e5 0c          	and    r13d,0xc
    9160:	e9 89 9b ff ff       	jmp    2cee <luaV_execute+0xffe>
        vmbreak;
    9165:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    916a:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    916f:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    9174:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    9178:	48 89 ef             	mov    rdi,rbp
    917b:	e8 00 00 00 00       	call   9180 <luaV_execute+0x7490>
    9180:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    9184:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    9189:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    9190:	00 
    9191:	41 83 e5 0c          	and    r13d,0xc
    9195:	e9 d5 b4 ff ff       	jmp    466f <luaV_execute+0x297f>
        vmbreak;
    919a:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    919f:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    91a4:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    91a9:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    91ad:	48 89 ef             	mov    rdi,rbp
    91b0:	e8 00 00 00 00       	call   91b5 <luaV_execute+0x74c5>
    91b5:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    91b9:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    91be:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    91c5:	00 
    91c6:	41 83 e5 0c          	and    r13d,0xc
    91ca:	e9 b3 9b ff ff       	jmp    2d82 <luaV_execute+0x1092>
          if (!tonumber(init, &ninit))
    91cf:	48 8d b4 24 c0 00 00 	lea    rsi,[rsp+0xc0]
    91d6:	00 
    91d7:	48 89 ef             	mov    rdi,rbp
    91da:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    91df:	e8 00 00 00 00       	call   91e4 <luaV_execute+0x74f4>
    91e4:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    91e9:	85 c0                	test   eax,eax
    91eb:	0f 84 d1 08 00 00    	je     9ac2 <luaV_execute+0x7dd2>
    91f1:	f2 0f 10 84 24 c0 00 	movsd  xmm0,QWORD PTR [rsp+0xc0]
    91f8:	00 00 
    91fa:	f2 0f 10 8c 24 d0 00 	movsd  xmm1,QWORD PTR [rsp+0xd0]
    9201:	00 00 
    9203:	e9 67 99 ff ff       	jmp    2b6f <luaV_execute+0xe7f>
          if (RAVI_LIKELY((0 < step)) ? (idx <= limit) : (limit <= idx)) {
    9208:	48 39 ca             	cmp    rdx,rcx
    920b:	0f 9d c1             	setge  cl
    920e:	e9 d1 99 ff ff       	jmp    2be4 <luaV_execute+0xef4>
        vmbreak;
    9213:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    9218:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    921d:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    9222:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    9226:	48 89 ef             	mov    rdi,rbp
    9229:	e8 00 00 00 00       	call   922e <luaV_execute+0x753e>
    922e:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    9232:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    9237:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    923e:	00 
    923f:	41 83 e5 0c          	and    r13d,0xc
    9243:	e9 9b b6 ff ff       	jmp    48e3 <luaV_execute+0x2bf3>
        vmbreak;
    9248:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    924d:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    9252:	44 89 44 24 28       	mov    DWORD PTR [rsp+0x28],r8d
    9257:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    925b:	48 89 ef             	mov    rdi,rbp
    925e:	e8 00 00 00 00       	call   9263 <luaV_execute+0x7573>
    9263:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    9267:	44 8b 44 24 28       	mov    r8d,DWORD PTR [rsp+0x28]
    926c:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    9273:	00 
    9274:	41 83 e5 0c          	and    r13d,0xc
    9278:	e9 99 b4 ff ff       	jmp    4716 <luaV_execute+0x2a26>
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    927d:	4c 8b 7c 24 18       	mov    r15,QWORD PTR [rsp+0x18]
    9282:	48 89 e9             	mov    rcx,rbp
    9285:	48 8b 6c 24 08       	mov    rbp,QWORD PTR [rsp+0x8]
    928a:	49 89 5f 28          	mov    QWORD PTR [r15+0x28],rbx
    928e:	48 89 ef             	mov    rdi,rbp
    9291:	e8 00 00 00 00       	call   9296 <luaV_execute+0x75a6>
    9296:	44 0f b6 ad c8 00 00 	movzx  r13d,BYTE PTR [rbp+0xc8]
    929d:	00 
    929e:	4d 8b 67 20          	mov    r12,QWORD PTR [r15+0x20]
    92a2:	41 83 e5 0c          	and    r13d,0xc
    92a6:	e9 c8 af ff ff       	jmp    4273 <luaV_execute+0x2583>
        GETTABLE_INLINE_PROTECTED_I(L, rb, rc, ra);
    92ab:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    92b0:	4c 89 fe             	mov    rsi,r15
    92b3:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    92b8:	48 89 e9             	mov    rcx,rbp
    92bb:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    92c0:	4c 89 ff             	mov    rdi,r15
    92c3:	e8 00 00 00 00       	call   92c8 <luaV_execute+0x75d8>
    92c8:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    92cf:	00 
    92d0:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    92d5:	41 83 e5 0c          	and    r13d,0xc
    92d9:	e9 84 94 ff ff       	jmp    2762 <luaV_execute+0xa72>
    92de:	48 89 54 24 28       	mov    QWORD PTR [rsp+0x28],rdx
    92e3:	e8 00 00 00 00       	call   92e8 <luaV_execute+0x75f8>
    92e8:	48 8b 54 24 28       	mov    rdx,QWORD PTR [rsp+0x28]
    92ed:	49 89 c0             	mov    r8,rax
    92f0:	e9 54 94 ff ff       	jmp    2749 <luaV_execute+0xa59>
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    92f5:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    92fa:	4c 89 fe             	mov    rsi,r15
    92fd:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    9302:	49 89 c0             	mov    r8,rax
    9305:	48 89 e9             	mov    rcx,rbp
    9308:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    930d:	4c 89 ff             	mov    rdi,r15
    9310:	e8 00 00 00 00       	call   9315 <luaV_execute+0x7625>
    9315:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    931c:	00 
    931d:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    9322:	41 83 e5 0c          	and    r13d,0xc
    9326:	e9 0e aa ff ff       	jmp    3d39 <luaV_execute+0x2049>
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    932b:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    9330:	4c 89 fe             	mov    rsi,r15
    9333:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    9338:	49 89 c0             	mov    r8,rax
    933b:	48 89 e9             	mov    rcx,rbp
    933e:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    9343:	4c 89 ff             	mov    rdi,r15
    9346:	e8 00 00 00 00       	call   934b <luaV_execute+0x765b>
    934b:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    9352:	00 
    9353:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    9358:	41 83 e5 0c          	and    r13d,0xc
    935c:	e9 28 a9 ff ff       	jmp    3c89 <luaV_execute+0x1f99>
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    9361:	48 89 d7             	mov    rdi,rdx
    9364:	48 89 4c 24 30       	mov    QWORD PTR [rsp+0x30],rcx
    9369:	48 89 54 24 28       	mov    QWORD PTR [rsp+0x28],rdx
    936e:	e8 00 00 00 00       	call   9373 <luaV_execute+0x7683>
    9373:	48 8b 4c 24 30       	mov    rcx,QWORD PTR [rsp+0x30]
    9378:	48 8b 54 24 28       	mov    rdx,QWORD PTR [rsp+0x28]
    937d:	49 89 c0             	mov    r8,rax
    9380:	e9 93 a6 ff ff       	jmp    3a18 <luaV_execute+0x1d28>
          setivalue(ra, luaV_div(L, ib, ic));
    9385:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    9388:	48 8b 36             	mov    rsi,QWORD PTR [rsi]
    938b:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9390:	e8 00 00 00 00       	call   9395 <luaV_execute+0x76a5>
    9395:	ba 13 00 00 00       	mov    edx,0x13
    939a:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    939e:	66 89 55 08          	mov    WORD PTR [rbp+0x8],dx
        if (ttisinteger(rb) && ttisinteger(rc)) {
    93a2:	e9 b1 d9 ff ff       	jmp    6d58 <luaV_execute+0x5068>
          setivalue(ra, luaV_mod(L, ib, ic));
    93a7:	48 8b 36             	mov    rsi,QWORD PTR [rsi]
    93aa:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    93ad:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    93b2:	e8 00 00 00 00       	call   93b7 <luaV_execute+0x76c7>
    93b7:	be 13 00 00 00       	mov    esi,0x13
    93bc:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    93c0:	66 89 75 08          	mov    WORD PTR [rbp+0x8],si
        if (ttisinteger(rb) && ttisinteger(rc)) {
    93c4:	e9 32 d9 ff ff       	jmp    6cfb <luaV_execute+0x500b>
          if (luai_numlt(0, step) ? luai_numle(idx, limit)
    93c9:	66 0f 2f c1          	comisd xmm0,xmm1
    93cd:	0f 93 c2             	setae  dl
    93d0:	e9 d8 f7 ff ff       	jmp    8bad <luaV_execute+0x6ebd>
        GETTABLE_INLINE_PROTECTED_I(L, rb, rc, ra);
    93d5:	49 8b 0f             	mov    rcx,QWORD PTR [r15]
    93d8:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    93db:	48 8b 51 38          	mov    rdx,QWORD PTR [rcx+0x38]
    93df:	39 41 40             	cmp    DWORD PTR [rcx+0x40],eax
    93e2:	0f 86 48 09 00 00    	jbe    9d30 <luaV_execute+0x8040>
    93e8:	89 c0                	mov    eax,eax
    93ea:	41 b9 03 00 00 00    	mov    r9d,0x3
    93f0:	f2 0f 10 04 c2       	movsd  xmm0,QWORD PTR [rdx+rax*8]
    93f5:	66 44 89 4d 08       	mov    WORD PTR [rbp+0x8],r9w
    93fa:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    93ff:	e9 5e 93 ff ff       	jmp    2762 <luaV_execute+0xa72>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9404:	66 41 83 7f 08 13    	cmp    WORD PTR [r15+0x8],0x13
    940a:	48 8b 6d 00          	mov    rbp,QWORD PTR [rbp+0x0]
    940e:	0f 85 97 06 00 00    	jne    9aab <luaV_execute+0x7dbb>
    9414:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    9418:	66 83 f8 03          	cmp    ax,0x3
    941c:	0f 84 b1 05 00 00    	je     99d3 <luaV_execute+0x7ce3>
    9422:	66 83 f8 13          	cmp    ax,0x13
    9426:	0f 85 fa 04 00 00    	jne    9926 <luaV_execute+0x7c36>
    942c:	49 8b 07             	mov    rax,QWORD PTR [r15]
    942f:	66 0f ef c0          	pxor   xmm0,xmm0
    9433:	48 8b 75 38          	mov    rsi,QWORD PTR [rbp+0x38]
    9437:	f2 48 0f 2a 01       	cvtsi2sd xmm0,QWORD PTR [rcx]
    943c:	89 c2                	mov    edx,eax
    943e:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    9441:	0f 86 3b 06 00 00    	jbe    9a82 <luaV_execute+0x7d92>
    9447:	f2 0f 11 04 d6       	movsd  QWORD PTR [rsi+rdx*8],xmm0
    944c:	e9 b2 a6 ff ff       	jmp    3b03 <luaV_execute+0x1e13>
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    9451:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    9456:	0f 85 14 07 00 00    	jne    9b70 <luaV_execute+0x7e80>
    945c:	49 8b 0f             	mov    rcx,QWORD PTR [r15]
    945f:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    9462:	48 8b 51 38          	mov    rdx,QWORD PTR [rcx+0x38]
    9466:	39 41 40             	cmp    DWORD PTR [rcx+0x40],eax
    9469:	0f 86 c1 08 00 00    	jbe    9d30 <luaV_execute+0x8040>
    946f:	89 c0                	mov    eax,eax
    9471:	be 03 00 00 00       	mov    esi,0x3
    9476:	f2 0f 10 04 c2       	movsd  xmm0,QWORD PTR [rdx+rax*8]
    947b:	66 89 75 08          	mov    WORD PTR [rbp+0x8],si
    947f:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    9484:	e9 00 a8 ff ff       	jmp    3c89 <luaV_execute+0x1f99>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    9489:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    948e:	4d 8b 3f             	mov    r15,QWORD PTR [r15]
    9491:	0f 85 fd 05 00 00    	jne    9a94 <luaV_execute+0x7da4>
    9497:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    949b:	66 83 f8 03          	cmp    ax,0x3
    949f:	0f 84 ca 04 00 00    	je     996f <luaV_execute+0x7c7f>
    94a5:	66 83 f8 13          	cmp    ax,0x13
    94a9:	0f 85 35 04 00 00    	jne    98e4 <luaV_execute+0x7bf4>
    94af:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    94b3:	66 0f ef c0          	pxor   xmm0,xmm0
    94b7:	49 8b 77 38          	mov    rsi,QWORD PTR [r15+0x38]
    94bb:	f2 48 0f 2a 01       	cvtsi2sd xmm0,QWORD PTR [rcx]
    94c0:	89 c2                	mov    edx,eax
    94c2:	41 39 47 40          	cmp    DWORD PTR [r15+0x40],eax
    94c6:	0f 86 a4 05 00 00    	jbe    9a70 <luaV_execute+0x7d80>
    94cc:	f2 0f 11 04 d6       	movsd  QWORD PTR [rsi+rdx*8],xmm0
    94d1:	e9 08 a7 ff ff       	jmp    3bde <luaV_execute+0x1eee>
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    94d6:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    94db:	0f 85 73 07 00 00    	jne    9c54 <luaV_execute+0x7f64>
    94e1:	49 8b 0f             	mov    rcx,QWORD PTR [r15]
    94e4:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    94e7:	48 8b 51 38          	mov    rdx,QWORD PTR [rcx+0x38]
    94eb:	39 41 40             	cmp    DWORD PTR [rcx+0x40],eax
    94ee:	0f 86 3c 08 00 00    	jbe    9d30 <luaV_execute+0x8040>
    94f4:	89 c0                	mov    eax,eax
    94f6:	41 b8 03 00 00 00    	mov    r8d,0x3
    94fc:	f2 0f 10 04 c2       	movsd  xmm0,QWORD PTR [rdx+rax*8]
    9501:	66 44 89 45 08       	mov    WORD PTR [rbp+0x8],r8w
    9506:	f2 0f 11 45 00       	movsd  QWORD PTR [rbp+0x0],xmm0
    950b:	e9 29 a8 ff ff       	jmp    3d39 <luaV_execute+0x2049>
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    9510:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    9514:	48 8b 6d 00          	mov    rbp,QWORD PTR [rbp+0x0]
    9518:	66 83 f8 03          	cmp    ax,0x3
    951c:	0f 84 59 03 00 00    	je     987b <luaV_execute+0x7b8b>
    9522:	66 83 f8 13          	cmp    ax,0x13
    9526:	0f 85 06 03 00 00    	jne    9832 <luaV_execute+0x7b42>
    952c:	49 8b 07             	mov    rax,QWORD PTR [r15]
    952f:	66 0f ef c0          	pxor   xmm0,xmm0
    9533:	48 8b 75 38          	mov    rsi,QWORD PTR [rbp+0x38]
    9537:	f2 48 0f 2a 01       	cvtsi2sd xmm0,QWORD PTR [rcx]
    953c:	89 c2                	mov    edx,eax
    953e:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    9541:	0f 86 01 06 00 00    	jbe    9b48 <luaV_execute+0x7e58>
    9547:	f2 0f 11 04 d6       	movsd  QWORD PTR [rsi+rdx*8],xmm0
    954c:	e9 ed a4 ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
    9551:	48 89 54 24 28       	mov    QWORD PTR [rsp+0x28],rdx
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    9556:	e8 00 00 00 00       	call   955b <luaV_execute+0x786b>
    955b:	48 8b 54 24 28       	mov    rdx,QWORD PTR [rsp+0x28]
    9560:	e9 0c a7 ff ff       	jmp    3c71 <luaV_execute+0x1f81>
    9565:	48 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],rcx
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    956a:	e8 00 00 00 00       	call   956f <luaV_execute+0x787f>
    956f:	48 8b 4c 24 28       	mov    rcx,QWORD PTR [rsp+0x28]
    9574:	e9 41 a6 ff ff       	jmp    3bba <luaV_execute+0x1eca>
    9579:	48 89 54 24 28       	mov    QWORD PTR [rsp+0x28],rdx
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    957e:	e8 00 00 00 00       	call   9583 <luaV_execute+0x7893>
    9583:	48 8b 54 24 28       	mov    rdx,QWORD PTR [rsp+0x28]
    9588:	e9 94 a7 ff ff       	jmp    3d21 <luaV_execute+0x2031>
    958d:	48 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],rcx
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9592:	e8 00 00 00 00       	call   9597 <luaV_execute+0x78a7>
    9597:	48 8b 4c 24 28       	mov    rcx,QWORD PTR [rsp+0x28]
    959c:	e9 3e a5 ff ff       	jmp    3adf <luaV_execute+0x1def>
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    95a1:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    95a6:	4c 89 fa             	mov    rdx,r15
    95a9:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    95ae:	45 31 c0             	xor    r8d,r8d
    95b1:	48 89 ee             	mov    rsi,rbp
    95b4:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    95b9:	4c 89 ff             	mov    rdi,r15
    95bc:	e8 00 00 00 00       	call   95c1 <luaV_execute+0x78d1>
    95c1:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    95c8:	00 
    95c9:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    95ce:	41 83 e5 0c          	and    r13d,0xc
    95d2:	e9 67 a4 ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
        GETTABLE_INLINE_PROTECTED_I(L, rb, rc, ra);
    95d7:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    95dc:	4c 89 fe             	mov    rsi,r15
    95df:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    95e4:	45 31 c0             	xor    r8d,r8d
    95e7:	48 89 e9             	mov    rcx,rbp
    95ea:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    95ef:	4c 89 ff             	mov    rdi,r15
    95f2:	e8 00 00 00 00       	call   95f7 <luaV_execute+0x7907>
    95f7:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    95fe:	00 
    95ff:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    9604:	41 83 e5 0c          	and    r13d,0xc
    9608:	e9 55 91 ff ff       	jmp    2762 <luaV_execute+0xa72>
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    960d:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    9612:	4c 89 fe             	mov    rsi,r15
    9615:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    961a:	45 31 c0             	xor    r8d,r8d
    961d:	48 89 e9             	mov    rcx,rbp
    9620:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    9625:	4c 89 ff             	mov    rdi,r15
    9628:	e8 00 00 00 00       	call   962d <luaV_execute+0x793d>
    962d:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    9634:	00 
    9635:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    963a:	41 83 e5 0c          	and    r13d,0xc
    963e:	e9 46 a6 ff ff       	jmp    3c89 <luaV_execute+0x1f99>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9643:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    9648:	4c 89 fa             	mov    rdx,r15
    964b:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    9650:	45 31 c0             	xor    r8d,r8d
    9653:	48 89 ee             	mov    rsi,rbp
    9656:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    965b:	4c 89 ff             	mov    rdi,r15
    965e:	e8 00 00 00 00       	call   9663 <luaV_execute+0x7973>
    9663:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    966a:	00 
    966b:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    9670:	41 83 e5 0c          	and    r13d,0xc
    9674:	e9 8a a4 ff ff       	jmp    3b03 <luaV_execute+0x1e13>
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    9679:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    967e:	4c 89 fe             	mov    rsi,r15
    9681:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    9686:	45 31 c0             	xor    r8d,r8d
    9689:	48 89 e9             	mov    rcx,rbp
    968c:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    9691:	4c 89 ff             	mov    rdi,r15
    9694:	e8 00 00 00 00       	call   9699 <luaV_execute+0x79a9>
    9699:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    96a0:	00 
    96a1:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    96a6:	41 83 e5 0c          	and    r13d,0xc
    96aa:	e9 8a a6 ff ff       	jmp    3d39 <luaV_execute+0x2049>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    96af:	4c 8b 64 24 18       	mov    r12,QWORD PTR [rsp+0x18]
    96b4:	4c 89 fe             	mov    rsi,r15
    96b7:	4c 8b 7c 24 08       	mov    r15,QWORD PTR [rsp+0x8]
    96bc:	45 31 c0             	xor    r8d,r8d
    96bf:	48 89 ea             	mov    rdx,rbp
    96c2:	49 89 5c 24 28       	mov    QWORD PTR [r12+0x28],rbx
    96c7:	4c 89 ff             	mov    rdi,r15
    96ca:	e8 00 00 00 00       	call   96cf <luaV_execute+0x79df>
    96cf:	45 0f b6 af c8 00 00 	movzx  r13d,BYTE PTR [r15+0xc8]
    96d6:	00 
    96d7:	4d 8b 64 24 20       	mov    r12,QWORD PTR [r12+0x20]
    96dc:	41 83 e5 0c          	and    r13d,0xc
    96e0:	e9 f9 a4 ff ff       	jmp    3bde <luaV_execute+0x1eee>
          CallInfo *nci = L->ci;  /* called frame */
    96e5:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
          if (cl->p->sizep > 0) luaF_close(L, oci->u.l.base);
    96ea:	48 8b 4c 24 20       	mov    rcx,QWORD PTR [rsp+0x20]
          CallInfo *nci = L->ci;  /* called frame */
    96ef:	48 8b 6f 20          	mov    rbp,QWORD PTR [rdi+0x20]
          StkId nfunc = nci->func;  /* called function */
    96f3:	4c 8b 6d 00          	mov    r13,QWORD PTR [rbp+0x0]
          CallInfo *oci = nci->previous;  /* caller frame */
    96f7:	48 8b 75 10          	mov    rsi,QWORD PTR [rbp+0x10]
          StkId lim = nci->u.l.base + getproto(nfunc)->numparams;
    96fb:	48 8b 45 20          	mov    rax,QWORD PTR [rbp+0x20]
    96ff:	49 8b 55 00          	mov    rdx,QWORD PTR [r13+0x0]
          StkId ofunc = oci->func;  /* caller function */
    9703:	4c 8b 26             	mov    r12,QWORD PTR [rsi]
          CallInfo *oci = nci->previous;  /* caller frame */
    9706:	48 89 74 24 18       	mov    QWORD PTR [rsp+0x18],rsi
          StkId lim = nci->u.l.base + getproto(nfunc)->numparams;
    970b:	48 8b 52 18          	mov    rdx,QWORD PTR [rdx+0x18]
    970f:	0f b6 5a 0a          	movzx  ebx,BYTE PTR [rdx+0xa]
          if (cl->p->sizep > 0) luaF_close(L, oci->u.l.base);
    9713:	48 8b 51 18          	mov    rdx,QWORD PTR [rcx+0x18]
    9717:	44 8b 42 20          	mov    r8d,DWORD PTR [rdx+0x20]
          StkId lim = nci->u.l.base + getproto(nfunc)->numparams;
    971b:	48 c1 e3 04          	shl    rbx,0x4
    971f:	48 01 c3             	add    rbx,rax
          if (cl->p->sizep > 0) luaF_close(L, oci->u.l.base);
    9722:	45 85 c0             	test   r8d,r8d
    9725:	7e 0d                	jle    9734 <luaV_execute+0x7a44>
    9727:	48 8b 76 20          	mov    rsi,QWORD PTR [rsi+0x20]
    972b:	e8 00 00 00 00       	call   9730 <luaV_execute+0x7a40>
    9730:	48 8b 45 20          	mov    rax,QWORD PTR [rbp+0x20]
    9734:	4c 89 ea             	mov    rdx,r13
    9737:	49 8d 4c 24 08       	lea    rcx,[r12+0x8]
          for (aux = 0; nfunc + aux < lim; aux++)
    973c:	49 39 dd             	cmp    r13,rbx
    973f:	73 1c                	jae    975d <luaV_execute+0x7a6d>
            setobjs2s(L, ofunc + aux, nfunc + aux);
    9741:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    9744:	48 83 c2 10          	add    rdx,0x10
    9748:	48 83 c1 10          	add    rcx,0x10
    974c:	48 89 71 e8          	mov    QWORD PTR [rcx-0x18],rsi
    9750:	0f b7 72 f8          	movzx  esi,WORD PTR [rdx-0x8]
    9754:	66 89 71 f0          	mov    WORD PTR [rcx-0x10],si
          for (aux = 0; nfunc + aux < lim; aux++)
    9758:	48 39 d3             	cmp    rbx,rdx
    975b:	77 e4                	ja     9741 <luaV_execute+0x7a51>
          oci->u.l.base = ofunc + (nci->u.l.base - nfunc);  /* correct base */
    975d:	48 8b 4c 24 18       	mov    rcx,QWORD PTR [rsp+0x18]
          oci->top = L->top = ofunc + (L->top - nfunc);  /* correct top */
    9762:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
          oci->u.l.base = ofunc + (nci->u.l.base - nfunc);  /* correct base */
    9767:	4c 29 e8             	sub    rax,r13
    976a:	4c 01 e0             	add    rax,r12
    976d:	48 89 41 20          	mov    QWORD PTR [rcx+0x20],rax
          oci->top = L->top = ofunc + (L->top - nfunc);  /* correct top */
    9771:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
    9775:	48 89 44 24 10       	mov    QWORD PTR [rsp+0x10],rax
    977a:	4c 29 e8             	sub    rax,r13
    977d:	4c 01 e0             	add    rax,r12
    9780:	48 89 47 10          	mov    QWORD PTR [rdi+0x10],rax
    9784:	48 89 41 08          	mov    QWORD PTR [rcx+0x8],rax
          oci->u.l.savedpc = nci->u.l.savedpc;
    9788:	48 8b 45 28          	mov    rax,QWORD PTR [rbp+0x28]
          oci->callstatus |= CIST_TAIL;  /* function was tail called */
    978c:	66 83 49 42 20       	or     WORD PTR [rcx+0x42],0x20
          oci->u.l.savedpc = nci->u.l.savedpc;
    9791:	48 89 41 28          	mov    QWORD PTR [rcx+0x28],rax
          oci->jitstatus = 0;
    9795:	c6 41 46 00          	mov    BYTE PTR [rcx+0x46],0x0
          ci = L->ci = oci;  /* remove new frame */
    9799:	0f b6 97 c8 00 00 00 	movzx  edx,BYTE PTR [rdi+0xc8]
    97a0:	48 89 4f 20          	mov    QWORD PTR [rdi+0x20],rcx
          goto newframe;  /* restart luaV_execute over new Lua function */
    97a4:	e9 90 85 ff ff       	jmp    1d39 <luaV_execute+0x49>
      }
    }
  }
}
    97a9:	48 8b b4 24 08 01 00 	mov    rsi,QWORD PTR [rsp+0x108]
    97b0:	00 
    97b1:	64 48 33 34 25 28 00 	xor    rsi,QWORD PTR fs:0x28
    97b8:	00 00 
    97ba:	0f 85 4a 04 00 00    	jne    9c0a <luaV_execute+0x7f1a>
    97c0:	48 81 c4 18 01 00 00 	add    rsp,0x118
    97c7:	5b                   	pop    rbx
    97c8:	5d                   	pop    rbp
    97c9:	41 5c                	pop    r12
    97cb:	41 5d                	pop    r13
    97cd:	41 5e                	pop    r14
    97cf:	41 5f                	pop    r15
    97d1:	c3                   	ret    
          ci = L->ci;
    97d2:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
    97d7:	48 8b 40 20          	mov    rax,QWORD PTR [rax+0x20]
    97db:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
          goto newframe; /* restart luaV_execute over new Lua function */
    97e0:	48 8b 40 28          	mov    rax,QWORD PTR [rax+0x28]
    97e4:	e9 50 85 ff ff       	jmp    1d39 <luaV_execute+0x49>
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    97e9:	31 d2                	xor    edx,edx
    97eb:	48 89 cf             	mov    rdi,rcx
    97ee:	48 8d b4 24 c8 00 00 	lea    rsi,[rsp+0xc8]
    97f5:	00 
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    97f6:	48 c7 84 24 c8 00 00 	mov    QWORD PTR [rsp+0xc8],0x0
    97fd:	00 00 00 00 00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    9802:	e8 00 00 00 00       	call   9807 <luaV_execute+0x7b17>
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    9807:	85 c0                	test   eax,eax
    9809:	0f 84 f3 02 00 00    	je     9b02 <luaV_execute+0x7e12>
    980f:	49 8b 07             	mov    rax,QWORD PTR [r15]
    9812:	48 8b 8c 24 c8 00 00 	mov    rcx,QWORD PTR [rsp+0xc8]
    9819:	00 
    981a:	89 c2                	mov    edx,eax
    981c:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    981f:	0f 86 cb 02 00 00    	jbe    9af0 <luaV_execute+0x7e00>
    9825:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    9829:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    982d:	e9 0c a2 ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
    9832:	48 8d b4 24 c8 00 00 	lea    rsi,[rsp+0xc8]
    9839:	00 
    983a:	48 89 cf             	mov    rdi,rcx
    983d:	48 c7 84 24 c8 00 00 	mov    QWORD PTR [rsp+0xc8],0x0
    9844:	00 00 00 00 00 
    9849:	e8 00 00 00 00       	call   984e <luaV_execute+0x7b5e>
    984e:	85 c0                	test   eax,eax
    9850:	0f 84 cc 01 00 00    	je     9a22 <luaV_execute+0x7d32>
    9856:	f2 0f 10 84 24 c8 00 	movsd  xmm0,QWORD PTR [rsp+0xc8]
    985d:	00 00 
    985f:	49 8b 07             	mov    rax,QWORD PTR [r15]
    9862:	89 c2                	mov    edx,eax
    9864:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    9867:	0f 86 a3 01 00 00    	jbe    9a10 <luaV_execute+0x7d20>
    986d:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    9871:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    9876:	e9 c3 a1 ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
    987b:	49 8b 07             	mov    rax,QWORD PTR [r15]
    987e:	48 8b 75 38          	mov    rsi,QWORD PTR [rbp+0x38]
    9882:	89 c2                	mov    edx,eax
    9884:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    9887:	0f 86 cb 04 00 00    	jbe    9d58 <luaV_execute+0x8068>
    988d:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    9891:	f2 0f 11 04 d6       	movsd  QWORD PTR [rsi+rdx*8],xmm0
    9896:	e9 a3 a1 ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    989b:	31 d2                	xor    edx,edx
    989d:	48 89 cf             	mov    rdi,rcx
    98a0:	48 8d b4 24 c0 00 00 	lea    rsi,[rsp+0xc0]
    98a7:	00 
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    98a8:	48 c7 84 24 c0 00 00 	mov    QWORD PTR [rsp+0xc0],0x0
    98af:	00 00 00 00 00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    98b4:	e8 00 00 00 00       	call   98b9 <luaV_execute+0x7bc9>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    98b9:	85 c0                	test   eax,eax
    98bb:	0f 84 c1 03 00 00    	je     9c82 <luaV_execute+0x7f92>
    98c1:	49 8b 07             	mov    rax,QWORD PTR [r15]
    98c4:	48 8b 8c 24 c0 00 00 	mov    rcx,QWORD PTR [rsp+0xc0]
    98cb:	00 
    98cc:	89 c2                	mov    edx,eax
    98ce:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    98d1:	0f 86 99 03 00 00    	jbe    9c70 <luaV_execute+0x7f80>
    98d7:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    98db:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    98df:	e9 1f a2 ff ff       	jmp    3b03 <luaV_execute+0x1e13>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    98e4:	48 8d 74 24 58       	lea    rsi,[rsp+0x58]
    98e9:	48 89 cf             	mov    rdi,rcx
    98ec:	48 c7 44 24 58 00 00 	mov    QWORD PTR [rsp+0x58],0x0
    98f3:	00 00 
    98f5:	e8 00 00 00 00       	call   98fa <luaV_execute+0x7c0a>
    98fa:	85 c0                	test   eax,eax
    98fc:	0f 84 a6 04 00 00    	je     9da8 <luaV_execute+0x80b8>
    9902:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    9906:	f2 0f 10 44 24 58    	movsd  xmm0,QWORD PTR [rsp+0x58]
    990c:	89 c2                	mov    edx,eax
    990e:	41 39 47 40          	cmp    DWORD PTR [r15+0x40],eax
    9912:	0f 86 7e 04 00 00    	jbe    9d96 <luaV_execute+0x80a6>
    9918:	49 8b 47 38          	mov    rax,QWORD PTR [r15+0x38]
    991c:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    9921:	e9 b8 a2 ff ff       	jmp    3bde <luaV_execute+0x1eee>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9926:	48 8d b4 24 c0 00 00 	lea    rsi,[rsp+0xc0]
    992d:	00 
    992e:	48 89 cf             	mov    rdi,rcx
    9931:	48 c7 84 24 c0 00 00 	mov    QWORD PTR [rsp+0xc0],0x0
    9938:	00 00 00 00 00 
    993d:	e8 00 00 00 00       	call   9942 <luaV_execute+0x7c52>
    9942:	85 c0                	test   eax,eax
    9944:	0f 84 58 03 00 00    	je     9ca2 <luaV_execute+0x7fb2>
    994a:	f2 0f 10 84 24 c0 00 	movsd  xmm0,QWORD PTR [rsp+0xc0]
    9951:	00 00 
    9953:	49 8b 07             	mov    rax,QWORD PTR [r15]
    9956:	89 c2                	mov    edx,eax
    9958:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    995b:	0f 86 2f 03 00 00    	jbe    9c90 <luaV_execute+0x7fa0>
    9961:	48 8b 45 38          	mov    rax,QWORD PTR [rbp+0x38]
    9965:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    996a:	e9 94 a1 ff ff       	jmp    3b03 <luaV_execute+0x1e13>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    996f:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    9973:	49 8b 77 38          	mov    rsi,QWORD PTR [r15+0x38]
    9977:	89 c2                	mov    edx,eax
    9979:	41 39 47 40          	cmp    DWORD PTR [r15+0x40],eax
    997d:	0f 86 fd 03 00 00    	jbe    9d80 <luaV_execute+0x8090>
    9983:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    9987:	f2 0f 11 04 d6       	movsd  QWORD PTR [rsi+rdx*8],xmm0
    998c:	e9 4d a2 ff ff       	jmp    3bde <luaV_execute+0x1eee>
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    9991:	31 d2                	xor    edx,edx
    9993:	48 8d 74 24 58       	lea    rsi,[rsp+0x58]
    9998:	48 89 cf             	mov    rdi,rcx
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    999b:	48 c7 44 24 58 00 00 	mov    QWORD PTR [rsp+0x58],0x0
    99a2:	00 00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    99a4:	e8 00 00 00 00       	call   99a9 <luaV_execute+0x7cb9>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    99a9:	85 c0                	test   eax,eax
    99ab:	0f 84 19 04 00 00    	je     9dca <luaV_execute+0x80da>
    99b1:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    99b5:	48 8b 4c 24 58       	mov    rcx,QWORD PTR [rsp+0x58]
    99ba:	89 c2                	mov    edx,eax
    99bc:	41 39 47 40          	cmp    DWORD PTR [r15+0x40],eax
    99c0:	0f 86 f2 03 00 00    	jbe    9db8 <luaV_execute+0x80c8>
    99c6:	49 8b 47 38          	mov    rax,QWORD PTR [r15+0x38]
    99ca:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    99ce:	e9 0b a2 ff ff       	jmp    3bde <luaV_execute+0x1eee>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    99d3:	49 8b 07             	mov    rax,QWORD PTR [r15]
    99d6:	48 8b 75 38          	mov    rsi,QWORD PTR [rbp+0x38]
    99da:	89 c2                	mov    edx,eax
    99dc:	39 45 40             	cmp    DWORD PTR [rbp+0x40],eax
    99df:	0f 86 75 01 00 00    	jbe    9b5a <luaV_execute+0x7e6a>
    99e5:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    99e9:	f2 0f 11 04 d6       	movsd  QWORD PTR [rsi+rdx*8],xmm0
    99ee:	e9 10 a1 ff ff       	jmp    3b03 <luaV_execute+0x1e13>
    99f3:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    99f8:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 99ff <luaV_execute+0x7d0f>
    99ff:	48 89 ee             	mov    rsi,rbp
    9a02:	4c 89 ef             	mov    rdi,r13
    9a05:	e8 00 00 00 00       	call   9a0a <luaV_execute+0x7d1a>
    9a0a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    9a10:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9a15:	48 89 ee             	mov    rsi,rbp
    9a18:	e8 00 00 00 00       	call   9a1d <luaV_execute+0x7d2d>
    9a1d:	e9 1c a0 ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
    9a22:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    9a27:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9a2e <luaV_execute+0x7d3e>
    9a2e:	4c 89 ef             	mov    rdi,r13
    9a31:	31 c0                	xor    eax,eax
    9a33:	e8 00 00 00 00       	call   9a38 <luaV_execute+0x7d48>
    9a38:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        SETTABLE_INLINE_SSKEY_PROTECTED(L, ra, rb, rc);
    9a3d:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9a44 <luaV_execute+0x7d54>
    9a44:	48 89 ee             	mov    rsi,rbp
    9a47:	4c 89 ef             	mov    rdi,r13
    9a4a:	e8 00 00 00 00       	call   9a4f <luaV_execute+0x7d5f>
    9a4f:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    9a54:	49 89 d7             	mov    r15,rdx
    9a57:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9a5e <luaV_execute+0x7d6e>
    9a5e:	4c 89 fe             	mov    rsi,r15
    9a61:	4c 89 ef             	mov    rdi,r13
    9a64:	e8 00 00 00 00       	call   9a69 <luaV_execute+0x7d79>
    9a69:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    9a70:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9a75:	4c 89 fe             	mov    rsi,r15
    9a78:	e8 00 00 00 00       	call   9a7d <luaV_execute+0x7d8d>
    9a7d:	e9 5c a1 ff ff       	jmp    3bde <luaV_execute+0x1eee>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9a82:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9a87:	48 89 ee             	mov    rsi,rbp
    9a8a:	e8 00 00 00 00       	call   9a8f <luaV_execute+0x7d9f>
    9a8f:	e9 6f a0 ff ff       	jmp    3b03 <luaV_execute+0x1e13>
    9a94:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    9a99:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9aa0 <luaV_execute+0x7db0>
    9aa0:	48 89 ee             	mov    rsi,rbp
    9aa3:	4c 89 ef             	mov    rdi,r13
    9aa6:	e8 00 00 00 00       	call   9aab <luaV_execute+0x7dbb>
    9aab:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9ab0:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9ab7 <luaV_execute+0x7dc7>
    9ab7:	4c 89 fe             	mov    rsi,r15
    9aba:	4c 89 ef             	mov    rdi,r13
    9abd:	e8 00 00 00 00       	call   9ac2 <luaV_execute+0x7dd2>
    9ac2:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
            luaG_runerror(L, "'for' initial value must be a number");
    9ac7:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9ace <luaV_execute+0x7dde>
    9ace:	4c 89 ef             	mov    rdi,r13
    9ad1:	e8 00 00 00 00       	call   9ad6 <luaV_execute+0x7de6>
    9ad6:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
            luaG_runerror(L, "'for' step must be a number");
    9adb:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9ae2 <luaV_execute+0x7df2>
    9ae2:	4c 89 ef             	mov    rdi,r13
    9ae5:	e8 00 00 00 00       	call   9aea <luaV_execute+0x7dfa>
    9aea:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    9af0:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9af5:	48 89 ee             	mov    rsi,rbp
    9af8:	e8 00 00 00 00       	call   9afd <luaV_execute+0x7e0d>
    9afd:	e9 3c 9f ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
    9b02:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    9b07:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9b0e <luaV_execute+0x7e1e>
    9b0e:	4c 89 ef             	mov    rdi,r13
    9b11:	31 c0                	xor    eax,eax
    9b13:	e8 00 00 00 00       	call   9b18 <luaV_execute+0x7e28>
    9b18:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(
    9b1d:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9b24 <luaV_execute+0x7e34>
    9b24:	31 c0                	xor    eax,eax
    9b26:	4c 89 ef             	mov    rdi,r13
    9b29:	e8 00 00 00 00       	call   9b2e <luaV_execute+0x7e3e>
    9b2e:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
            luaG_runerror(L, "'for' limit must be a number");
    9b33:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9b3a <luaV_execute+0x7e4a>
    9b3a:	4c 89 ef             	mov    rdi,r13
    9b3d:	e8 00 00 00 00       	call   9b42 <luaV_execute+0x7e52>
    9b42:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    9b48:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9b4d:	48 89 ee             	mov    rsi,rbp
    9b50:	e8 00 00 00 00       	call   9b55 <luaV_execute+0x7e65>
    9b55:	e9 e4 9e ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9b5a:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    9b5e:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9b63:	48 89 ee             	mov    rsi,rbp
    9b66:	e8 00 00 00 00       	call   9b6b <luaV_execute+0x7e7b>
    9b6b:	e9 93 9f ff ff       	jmp    3b03 <luaV_execute+0x1e13>
    9b70:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        GETTABLE_INLINE_PROTECTED(L, rb, rc, ra);
    9b75:	49 89 d7             	mov    r15,rdx
    9b78:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9b7f <luaV_execute+0x7e8f>
    9b7f:	4c 89 fe             	mov    rsi,r15
    9b82:	4c 89 ef             	mov    rdi,r13
    9b85:	e8 00 00 00 00       	call   9b8a <luaV_execute+0x7e9a>
    9b8a:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    9b8f:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9b96 <luaV_execute+0x7ea6>
    9b96:	4c 89 ef             	mov    rdi,r13
    9b99:	e8 00 00 00 00       	call   9b9e <luaV_execute+0x7eae>
    9b9e:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(
    9ba3:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9baa <luaV_execute+0x7eba>
    9baa:	31 c0                	xor    eax,eax
    9bac:	4c 89 ef             	mov    rdi,r13
    9baf:	e8 00 00 00 00       	call   9bb4 <luaV_execute+0x7ec4>
    9bb4:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    9bb9:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9bc0 <luaV_execute+0x7ed0>
    9bc0:	4c 89 ef             	mov    rdi,r13
    9bc3:	e8 00 00 00 00       	call   9bc8 <luaV_execute+0x7ed8>
    9bc8:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(
    9bcd:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9bd4 <luaV_execute+0x7ee4>
    9bd4:	31 c0                	xor    eax,eax
    9bd6:	4c 89 ef             	mov    rdi,r13
    9bd9:	e8 00 00 00 00       	call   9bde <luaV_execute+0x7eee>
    9bde:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(L,
    9be3:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9bea <luaV_execute+0x7efa>
    9bea:	31 c0                	xor    eax,eax
    9bec:	4c 89 ef             	mov    rdi,r13
    9bef:	e8 00 00 00 00       	call   9bf4 <luaV_execute+0x7f04>
    9bf4:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
          luaG_runerror(
    9bf9:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9c00 <luaV_execute+0x7f10>
    9c00:	31 c0                	xor    eax,eax
    9c02:	4c 89 ef             	mov    rdi,r13
    9c05:	e8 00 00 00 00       	call   9c0a <luaV_execute+0x7f1a>
}
    9c0a:	e8 00 00 00 00       	call   9c0f <luaV_execute+0x7f1f>
    9c0f:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9c14:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9c1b <luaV_execute+0x7f2b>
    9c1b:	4c 89 fe             	mov    rsi,r15
    9c1e:	4c 89 ef             	mov    rdi,r13
    9c21:	e8 00 00 00 00       	call   9c26 <luaV_execute+0x7f36>
    9c26:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    9c2b:	49 89 d7             	mov    r15,rdx
    9c2e:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9c35 <luaV_execute+0x7f45>
    9c35:	4c 89 fe             	mov    rsi,r15
    9c38:	4c 89 ef             	mov    rdi,r13
    9c3b:	e8 00 00 00 00       	call   9c40 <luaV_execute+0x7f50>
    9c40:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        GETTABLE_INLINE_SSKEY_PROTECTED(L, rb, rc, ra);
    9c45:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9c4c <luaV_execute+0x7f5c>
    9c4c:	4c 89 ef             	mov    rdi,r13
    9c4f:	e8 00 00 00 00       	call   9c54 <luaV_execute+0x7f64>
    9c54:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    9c59:	49 89 d7             	mov    r15,rdx
    9c5c:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # 9c63 <luaV_execute+0x7f73>
    9c63:	4c 89 fe             	mov    rsi,r15
    9c66:	4c 89 ef             	mov    rdi,r13
    9c69:	e8 00 00 00 00       	call   9c6e <luaV_execute+0x7f7e>
    9c6e:	66 90                	xchg   ax,ax
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9c70:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9c75:	48 89 ee             	mov    rsi,rbp
    9c78:	e8 00 00 00 00       	call   9c7d <luaV_execute+0x7f8d>
    9c7d:	e9 81 9e ff ff       	jmp    3b03 <luaV_execute+0x1e13>
    9c82:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
    9c87:	e9 7b fe ff ff       	jmp    9b07 <luaV_execute+0x7e17>
    9c8c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    9c90:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9c95:	48 89 ee             	mov    rsi,rbp
    9c98:	e8 00 00 00 00       	call   9c9d <luaV_execute+0x7fad>
    9c9d:	e9 61 9e ff ff       	jmp    3b03 <luaV_execute+0x1e13>
    9ca2:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
    9ca7:	e9 7b fd ff ff       	jmp    9a27 <luaV_execute+0x7d37>
    9cac:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    9cb0:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9cb5:	48 89 ee             	mov    rsi,rbp
    9cb8:	e8 00 00 00 00       	call   9cbd <luaV_execute+0x7fcd>
    9cbd:	e9 7c 9d ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
    9cc2:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
            luaG_runerror(L, "type mismatch: expected %s", getstr(key));
    9cc7:	49 8d 50 18          	lea    rdx,[r8+0x18]
    9ccb:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9cd2 <luaV_execute+0x7fe2>
    9cd2:	4c 89 ef             	mov    rdi,r13
    9cd5:	e8 00 00 00 00       	call   9cda <luaV_execute+0x7fea>
    9cda:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
            luaG_runerror(L, "number expected");
    9cdf:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9ce6 <luaV_execute+0x7ff6>
    9ce6:	31 c0                	xor    eax,eax
    9ce8:	4c 89 ef             	mov    rdi,r13
    9ceb:	e8 00 00 00 00       	call   9cf0 <luaV_execute+0x8000>
    9cf0:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
    9cf5:	e9 0d fe ff ff       	jmp    9b07 <luaV_execute+0x7e17>
    9cfa:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
    9cff:	e9 23 fd ff ff       	jmp    9a27 <luaV_execute+0x7d37>
    9d04:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
            luaG_runerror(L, "type name must be string");
    9d09:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9d10 <luaV_execute+0x8020>
    9d10:	31 c0                	xor    eax,eax
    9d12:	4c 89 ef             	mov    rdi,r13
    9d15:	e8 00 00 00 00       	call   9d1a <luaV_execute+0x802a>
    9d1a:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
            luaG_runerror(L, "integer expected");
    9d1f:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9d26 <luaV_execute+0x8036>
    9d26:	31 c0                	xor    eax,eax
    9d28:	4c 89 ef             	mov    rdi,r13
    9d2b:	e8 00 00 00 00       	call   9d30 <luaV_execute+0x8040>
    9d30:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
        GETTABLE_INLINE_PROTECTED(L, upval, rc, ra);
    9d35:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9d3c <luaV_execute+0x804c>
    9d3c:	31 c0                	xor    eax,eax
    9d3e:	4c 89 ef             	mov    rdi,r13
    9d41:	e8 00 00 00 00       	call   9d46 <luaV_execute+0x8056>
        SETTABLE_INLINE_PROTECTED(L, ra, rb, rc);
    9d46:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9d4b:	48 89 ee             	mov    rsi,rbp
    9d4e:	e8 00 00 00 00       	call   9d53 <luaV_execute+0x8063>
    9d53:	e9 ab 9d ff ff       	jmp    3b03 <luaV_execute+0x1e13>
        SETTABLE_INLINE_PROTECTED_I(L, ra, rb, rc);
    9d58:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    9d5c:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9d61:	48 89 ee             	mov    rsi,rbp
    9d64:	e8 00 00 00 00       	call   9d69 <luaV_execute+0x8079>
    9d69:	e9 d0 9c ff ff       	jmp    3a3e <luaV_execute+0x1d4e>
        SETTABLE_INLINE_PROTECTED(L, upval, rb, rc);
    9d6e:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9d73:	4c 89 fe             	mov    rsi,r15
    9d76:	e8 00 00 00 00       	call   9d7b <luaV_execute+0x808b>
    9d7b:	e9 5e 9e ff ff       	jmp    3bde <luaV_execute+0x1eee>
    9d80:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    9d84:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9d89:	4c 89 fe             	mov    rsi,r15
    9d8c:	e8 00 00 00 00       	call   9d91 <luaV_execute+0x80a1>
    9d91:	e9 48 9e ff ff       	jmp    3bde <luaV_execute+0x1eee>
    9d96:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9d9b:	4c 89 fe             	mov    rsi,r15
    9d9e:	e8 00 00 00 00       	call   9da3 <luaV_execute+0x80b3>
    9da3:	e9 36 9e ff ff       	jmp    3bde <luaV_execute+0x1eee>
    9da8:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
    9dad:	e9 75 fc ff ff       	jmp    9a27 <luaV_execute+0x7d37>
    9db2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    9db8:	48 8b 7c 24 08       	mov    rdi,QWORD PTR [rsp+0x8]
    9dbd:	4c 89 fe             	mov    rsi,r15
    9dc0:	e8 00 00 00 00       	call   9dc5 <luaV_execute+0x80d5>
    9dc5:	e9 14 9e ff ff       	jmp    3bde <luaV_execute+0x1eee>
    9dca:	4c 8b 6c 24 08       	mov    r13,QWORD PTR [rsp+0x8]
    9dcf:	e9 33 fd ff ff       	jmp    9b07 <luaV_execute+0x7e17>
    9dd4:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    9ddb:	00 00 00 00 
    9ddf:	90                   	nop

0000000000009de0 <ravi_dump_value>:

void ravi_dump_value(lua_State *L, const TValue *stack_ptr) {
  (void)L;
  if (ttisCclosure(stack_ptr))
    9de0:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    9de4:	66 3d 26 80          	cmp    ax,0x8026
    9de8:	0f 84 b2 00 00 00    	je     9ea0 <ravi_dump_value+0xc0>
    printf("C closure\n");
  else if (ttislcf(stack_ptr))
    9dee:	66 83 f8 16          	cmp    ax,0x16
    9df2:	0f 84 f8 00 00 00    	je     9ef0 <ravi_dump_value+0x110>
    printf("light C function\n");
  else if (ttisfcf(stack_ptr))
    9df8:	89 c2                	mov    edx,eax
    9dfa:	83 e2 7f             	and    edx,0x7f
    9dfd:	66 83 fa 46          	cmp    dx,0x46
    9e01:	0f 84 01 01 00 00    	je     9f08 <ravi_dump_value+0x128>
	printf("fast C function\n");
  else if (ttisLclosure(stack_ptr))
    9e07:	66 3d 06 80          	cmp    ax,0x8006
    9e0b:	0f 84 a7 00 00 00    	je     9eb8 <ravi_dump_value+0xd8>
    printf("Lua closure\n");
  else if (ttisfunction(stack_ptr))
    9e11:	89 c2                	mov    edx,eax
    9e13:	83 e2 0f             	and    edx,0xf
    9e16:	66 83 fa 06          	cmp    dx,0x6
    9e1a:	0f 84 00 01 00 00    	je     9f20 <ravi_dump_value+0x140>
    printf("function\n");
  else if (ttislngstring(stack_ptr) || ttisshrstring(stack_ptr) ||
    9e20:	89 c1                	mov    ecx,eax
    9e22:	83 e1 ef             	and    ecx,0xffffffef
    9e25:	66 81 f9 04 80       	cmp    cx,0x8004
    9e2a:	0f 84 a0 00 00 00    	je     9ed0 <ravi_dump_value+0xf0>
    9e30:	66 83 fa 04          	cmp    dx,0x4
    9e34:	0f 84 96 00 00 00    	je     9ed0 <ravi_dump_value+0xf0>
    ttisstring(stack_ptr))
    printf("'%s'\n", svalue(stack_ptr));
  else if (ttistable(stack_ptr))
    9e3a:	66 83 fa 05          	cmp    dx,0x5
    9e3e:	0f 84 24 01 00 00    	je     9f68 <ravi_dump_value+0x188>
    printf("table\n");
  else if (ttisnil(stack_ptr))
    9e44:	66 85 c0             	test   ax,ax
    9e47:	0f 84 03 01 00 00    	je     9f50 <ravi_dump_value+0x170>
    printf("nil\n");
  else if (ttisfloat(stack_ptr))
    9e4d:	66 83 f8 03          	cmp    ax,0x3
    9e51:	0f 84 29 01 00 00    	je     9f80 <ravi_dump_value+0x1a0>
    printf("%.6f\n", fltvalue(stack_ptr));
  else if (ttisinteger(stack_ptr))
    9e57:	66 83 f8 13          	cmp    ax,0x13
    9e5b:	0f 84 d7 00 00 00    	je     9f38 <ravi_dump_value+0x158>
    printf("%lld\n", (long long)ivalue(stack_ptr));
  else if (ttislightuserdata(stack_ptr))
    9e61:	66 83 f8 02          	cmp    ax,0x2
    9e65:	0f 84 2f 01 00 00    	je     9f9a <ravi_dump_value+0x1ba>
    printf("light user data\n");
  else if (ttisfulluserdata(stack_ptr))
    9e6b:	66 3d 07 80          	cmp    ax,0x8007
    9e6f:	0f 84 4b 01 00 00    	je     9fc0 <ravi_dump_value+0x1e0>
    printf("full user data\n");
  else if (ttisboolean(stack_ptr))
    9e75:	66 83 f8 01          	cmp    ax,0x1
    9e79:	0f 84 54 01 00 00    	je     9fd3 <ravi_dump_value+0x1f3>
    printf("boolean\n");
  else if (ttisthread(stack_ptr))
    9e7f:	66 3d 08 80          	cmp    ax,0x8008
    9e83:	0f 84 24 01 00 00    	je     9fad <ravi_dump_value+0x1cd>
}

__fortify_function int
printf (const char *__restrict __fmt, ...)
{
  return __printf_chk (__USE_FORTIFY_LEVEL - 1, __fmt, __va_arg_pack ());
    9e89:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9e90 <ravi_dump_value+0xb0>
    9e90:	bf 01 00 00 00       	mov    edi,0x1
    9e95:	31 c0                	xor    eax,eax
    9e97:	e9 00 00 00 00       	jmp    9e9c <ravi_dump_value+0xbc>
    9e9c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    9ea0:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9ea7 <ravi_dump_value+0xc7>
    9ea7:	bf 01 00 00 00       	mov    edi,0x1
    9eac:	31 c0                	xor    eax,eax
    9eae:	e9 00 00 00 00       	jmp    9eb3 <ravi_dump_value+0xd3>
    9eb3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    9eb8:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9ebf <ravi_dump_value+0xdf>
    9ebf:	bf 01 00 00 00       	mov    edi,0x1
    9ec4:	31 c0                	xor    eax,eax
    9ec6:	e9 00 00 00 00       	jmp    9ecb <ravi_dump_value+0xeb>
    9ecb:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    printf("'%s'\n", svalue(stack_ptr));
    9ed0:	48 8b 16             	mov    rdx,QWORD PTR [rsi]
    9ed3:	bf 01 00 00 00       	mov    edi,0x1
    9ed8:	31 c0                	xor    eax,eax
    9eda:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9ee1 <ravi_dump_value+0x101>
    9ee1:	48 83 c2 18          	add    rdx,0x18
    9ee5:	e9 00 00 00 00       	jmp    9eea <ravi_dump_value+0x10a>
    9eea:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    9ef0:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9ef7 <ravi_dump_value+0x117>
    9ef7:	bf 01 00 00 00       	mov    edi,0x1
    9efc:	31 c0                	xor    eax,eax
    9efe:	e9 00 00 00 00       	jmp    9f03 <ravi_dump_value+0x123>
    9f03:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    9f08:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9f0f <ravi_dump_value+0x12f>
    9f0f:	bf 01 00 00 00       	mov    edi,0x1
    9f14:	31 c0                	xor    eax,eax
    9f16:	e9 00 00 00 00       	jmp    9f1b <ravi_dump_value+0x13b>
    9f1b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    9f20:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9f27 <ravi_dump_value+0x147>
    9f27:	bf 01 00 00 00       	mov    edi,0x1
    9f2c:	31 c0                	xor    eax,eax
    9f2e:	e9 00 00 00 00       	jmp    9f33 <ravi_dump_value+0x153>
    9f33:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    9f38:	48 8b 16             	mov    rdx,QWORD PTR [rsi]
    9f3b:	bf 01 00 00 00       	mov    edi,0x1
    9f40:	31 c0                	xor    eax,eax
    9f42:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9f49 <ravi_dump_value+0x169>
    9f49:	e9 00 00 00 00       	jmp    9f4e <ravi_dump_value+0x16e>
    9f4e:	66 90                	xchg   ax,ax
    9f50:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9f57 <ravi_dump_value+0x177>
    9f57:	bf 01 00 00 00       	mov    edi,0x1
    9f5c:	e9 00 00 00 00       	jmp    9f61 <ravi_dump_value+0x181>
    9f61:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    9f68:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9f6f <ravi_dump_value+0x18f>
    9f6f:	bf 01 00 00 00       	mov    edi,0x1
    9f74:	31 c0                	xor    eax,eax
    9f76:	e9 00 00 00 00       	jmp    9f7b <ravi_dump_value+0x19b>
    9f7b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    9f80:	f2 0f 10 06          	movsd  xmm0,QWORD PTR [rsi]
    9f84:	bf 01 00 00 00       	mov    edi,0x1
    9f89:	b8 01 00 00 00       	mov    eax,0x1
    9f8e:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9f95 <ravi_dump_value+0x1b5>
    9f95:	e9 00 00 00 00       	jmp    9f9a <ravi_dump_value+0x1ba>
    9f9a:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9fa1 <ravi_dump_value+0x1c1>
    9fa1:	bf 01 00 00 00       	mov    edi,0x1
    9fa6:	31 c0                	xor    eax,eax
    9fa8:	e9 00 00 00 00       	jmp    9fad <ravi_dump_value+0x1cd>
    9fad:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9fb4 <ravi_dump_value+0x1d4>
    9fb4:	bf 01 00 00 00       	mov    edi,0x1
    9fb9:	31 c0                	xor    eax,eax
    9fbb:	e9 00 00 00 00       	jmp    9fc0 <ravi_dump_value+0x1e0>
    9fc0:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9fc7 <ravi_dump_value+0x1e7>
    9fc7:	bf 01 00 00 00       	mov    edi,0x1
    9fcc:	31 c0                	xor    eax,eax
    9fce:	e9 00 00 00 00       	jmp    9fd3 <ravi_dump_value+0x1f3>
    9fd3:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # 9fda <ravi_dump_value+0x1fa>
    9fda:	bf 01 00 00 00       	mov    edi,0x1
    9fdf:	31 c0                	xor    eax,eax
    9fe1:	e9 00 00 00 00       	jmp    9fe6 <ravi_dump_value+0x206>
    9fe6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
    9fed:	00 00 00 

0000000000009ff0 <ravi_dump_stack>:
      funcpos, (int)(p->maxstacksize), (int)(p->numparams), p->sizelocvars);
  printf("---> called from \n");
}

void ravi_dump_stack(lua_State *L, const char *s) {
  if (!s)
    9ff0:	48 85 f6             	test   rsi,rsi
    9ff3:	0f 84 e7 03 00 00    	je     a3e0 <ravi_dump_stack+0x3f0>
void ravi_dump_stack(lua_State *L, const char *s) {
    9ff9:	41 57                	push   r15
    9ffb:	31 c0                	xor    eax,eax
    9ffd:	41 56                	push   r14
    9fff:	49 89 fe             	mov    r14,rdi
    a002:	41 55                	push   r13
    a004:	41 54                	push   r12
    a006:	55                   	push   rbp
    a007:	53                   	push   rbx
    a008:	48 89 f3             	mov    rbx,rsi
    a00b:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a012 <ravi_dump_stack+0x22>
    a012:	48 83 ec 18          	sub    rsp,0x18
    return;
  CallInfo *ci = L->ci;
    a016:	48 8b 6f 20          	mov    rbp,QWORD PTR [rdi+0x20]
    a01a:	bf 01 00 00 00       	mov    edi,0x1
    a01f:	e8 00 00 00 00       	call   a024 <ravi_dump_stack+0x34>
    a024:	48 89 da             	mov    rdx,rbx
    a027:	bf 01 00 00 00       	mov    edi,0x1
    a02c:	31 c0                	xor    eax,eax
    a02e:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a035 <ravi_dump_stack+0x45>
    a035:	e8 00 00 00 00       	call   a03a <ravi_dump_stack+0x4a>
    a03a:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a041 <ravi_dump_stack+0x51>
    a041:	bf 01 00 00 00       	mov    edi,0x1
    a046:	31 c0                	xor    eax,eax
    a048:	e8 00 00 00 00       	call   a04d <ravi_dump_stack+0x5d>
  printf("=======================\n");
  printf("Stack dump %s\n", s);
  printf("=======================\n");
  printf("L->top = %d\n", (int)(L->top - L->stack));
    a04d:	49 8b 56 10          	mov    rdx,QWORD PTR [r14+0x10]
    a051:	49 2b 56 38          	sub    rdx,QWORD PTR [r14+0x38]
    a055:	31 c0                	xor    eax,eax
    a057:	48 c1 fa 04          	sar    rdx,0x4
    a05b:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a062 <ravi_dump_stack+0x72>
    a062:	bf 01 00 00 00       	mov    edi,0x1
    a067:	e8 00 00 00 00       	call   a06c <ravi_dump_stack+0x7c>
  while (ci) {
    a06c:	48 85 ed             	test   rbp,rbp
    a06f:	0f 84 a5 00 00 00    	je     a11a <ravi_dump_stack+0x12a>
    printf("stack[%d] reg[%d] = %s %s", i, (int)(stack_ptr-base), (stack_ptr == base ? "(base) " : ""), (stack_ptr == L->top ? "(L->top) " : ""));
    a075:	4c 8d 3d 00 00 00 00 	lea    r15,[rip+0x0]        # a07c <ravi_dump_stack+0x8c>
    a07c:	eb 3b                	jmp    a0b9 <ravi_dump_stack+0xc9>
    a07e:	66 90                	xchg   ax,ax
  switch (func_type) {
    a080:	66 83 f8 46          	cmp    ax,0x46
    a084:	75 2a                	jne    a0b0 <ravi_dump_stack+0xc0>
    a086:	8b 14 24             	mov    edx,DWORD PTR [rsp]
    a089:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a090 <ravi_dump_stack+0xa0>
    a090:	bf 01 00 00 00       	mov    edi,0x1
    a095:	31 c0                	xor    eax,eax
    a097:	e8 00 00 00 00       	call   a09c <ravi_dump_stack+0xac>
    a09c:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a0a3 <ravi_dump_stack+0xb3>
    a0a3:	bf 01 00 00 00       	mov    edi,0x1
    a0a8:	31 c0                	xor    eax,eax
    a0aa:	e8 00 00 00 00       	call   a0af <ravi_dump_stack+0xbf>
    a0af:	90                   	nop
    ravi_dump_ci(L, ci);
    ci = ci->previous;
    a0b0:	48 8b 6d 10          	mov    rbp,QWORD PTR [rbp+0x10]
  while (ci) {
    a0b4:	48 85 ed             	test   rbp,rbp
    a0b7:	74 61                	je     a11a <ravi_dump_stack+0x12a>
  StkId func = ci->func;
    a0b9:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
  int funcpos = ci->func - L->stack;
    a0bd:	49 8b 4e 38          	mov    rcx,QWORD PTR [r14+0x38]
    a0c1:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    a0c5:	49 89 d4             	mov    r12,rdx
    a0c8:	49 29 cc             	sub    r12,rcx
    a0cb:	49 c1 fc 04          	sar    r12,0x4
    a0cf:	83 e0 7f             	and    eax,0x7f
    a0d2:	4c 89 24 24          	mov    QWORD PTR [rsp],r12
  switch (func_type) {
    a0d6:	66 83 f8 16          	cmp    ax,0x16
    a0da:	0f 84 68 02 00 00    	je     a348 <ravi_dump_stack+0x358>
    a0e0:	76 5e                	jbe    a140 <ravi_dump_stack+0x150>
    a0e2:	66 83 f8 26          	cmp    ax,0x26
    a0e6:	75 98                	jne    a080 <ravi_dump_stack+0x90>
    a0e8:	8b 14 24             	mov    edx,DWORD PTR [rsp]
    a0eb:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a0f2 <ravi_dump_stack+0x102>
    a0f2:	bf 01 00 00 00       	mov    edi,0x1
    a0f7:	31 c0                	xor    eax,eax
    a0f9:	e8 00 00 00 00       	call   a0fe <ravi_dump_stack+0x10e>
    a0fe:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a105 <ravi_dump_stack+0x115>
    a105:	bf 01 00 00 00       	mov    edi,0x1
    a10a:	31 c0                	xor    eax,eax
    a10c:	e8 00 00 00 00       	call   a111 <ravi_dump_stack+0x121>
    ci = ci->previous;
    a111:	48 8b 6d 10          	mov    rbp,QWORD PTR [rbp+0x10]
  while (ci) {
    a115:	48 85 ed             	test   rbp,rbp
    a118:	75 9f                	jne    a0b9 <ravi_dump_stack+0xc9>
  }
  printf("\n");
}
    a11a:	48 83 c4 18          	add    rsp,0x18
    a11e:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a125 <ravi_dump_stack+0x135>
    a125:	bf 01 00 00 00       	mov    edi,0x1
    a12a:	31 c0                	xor    eax,eax
    a12c:	5b                   	pop    rbx
    a12d:	5d                   	pop    rbp
    a12e:	41 5c                	pop    r12
    a130:	41 5d                	pop    r13
    a132:	41 5e                	pop    r14
    a134:	41 5f                	pop    r15
    a136:	e9 00 00 00 00       	jmp    a13b <ravi_dump_stack+0x14b>
    a13b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  switch (func_type) {
    a140:	66 83 f8 06          	cmp    ax,0x6
    a144:	0f 85 66 ff ff ff    	jne    a0b0 <ravi_dump_stack+0xc0>
  StkId stack_ptr = ci->top - 1;
    a14a:	48 8b 5d 08          	mov    rbx,QWORD PTR [rbp+0x8]
    p = clLvalue(func)->p;
    a14e:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    base = ci->u.l.base;
    a151:	4c 8b 6d 20          	mov    r13,QWORD PTR [rbp+0x20]
  StkId stack_ptr = ci->top - 1;
    a155:	4c 8d 63 f0          	lea    r12,[rbx-0x10]
    p = clLvalue(func)->p;
    a159:	48 8b 40 18          	mov    rax,QWORD PTR [rax+0x18]
    i = ci->top - L->stack - 1;
    a15d:	48 29 cb             	sub    rbx,rcx
    a160:	48 c1 fb 04          	sar    rbx,0x4
    p = clLvalue(func)->p;
    a164:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
    i = ci->top - L->stack - 1;
    a169:	83 eb 01             	sub    ebx,0x1
  for (; stack_ptr >= base; stack_ptr--, i--) {
    a16c:	4d 39 ec             	cmp    r12,r13
    a16f:	0f 82 13 01 00 00    	jb     a288 <ravi_dump_stack+0x298>
    a175:	0f 1f 00             	nop    DWORD PTR [rax]
    printf("stack[%d] reg[%d] = %s %s", i, (int)(stack_ptr-base), (stack_ptr == base ? "(base) " : ""), (stack_ptr == L->top ? "(L->top) " : ""));
    a178:	4d 39 66 10          	cmp    QWORD PTR [r14+0x10],r12
    a17c:	4c 8d 0d 00 00 00 00 	lea    r9,[rip+0x0]        # a183 <ravi_dump_stack+0x193>
    a183:	4c 89 e1             	mov    rcx,r12
    a186:	89 da                	mov    edx,ebx
    a188:	4d 0f 45 cf          	cmovne r9,r15
    a18c:	4c 8d 05 00 00 00 00 	lea    r8,[rip+0x0]        # a193 <ravi_dump_stack+0x1a3>
    a193:	4d 39 e5             	cmp    r13,r12
    a196:	bf 01 00 00 00       	mov    edi,0x1
    a19b:	4d 0f 45 c7          	cmovne r8,r15
    a19f:	4c 29 e9             	sub    rcx,r13
    a1a2:	31 c0                	xor    eax,eax
    a1a4:	48 c1 f9 04          	sar    rcx,0x4
    a1a8:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a1af <ravi_dump_stack+0x1bf>
    a1af:	e8 00 00 00 00       	call   a1b4 <ravi_dump_stack+0x1c4>
    if (ttisCclosure(stack_ptr))
    a1b4:	41 0f b7 44 24 08    	movzx  eax,WORD PTR [r12+0x8]
    a1ba:	66 3d 26 80          	cmp    ax,0x8026
    a1be:	0f 84 0c 01 00 00    	je     a2d0 <ravi_dump_stack+0x2e0>
    else if (ttislcf(stack_ptr))
    a1c4:	66 83 f8 16          	cmp    ax,0x16
    a1c8:	0f 84 4a 01 00 00    	je     a318 <ravi_dump_stack+0x328>
    else if (ttisfcf(stack_ptr))
    a1ce:	89 c2                	mov    edx,eax
    a1d0:	83 e2 7f             	and    edx,0x7f
    a1d3:	66 83 fa 46          	cmp    dx,0x46
    a1d7:	0f 84 53 01 00 00    	je     a330 <ravi_dump_stack+0x340>
    else if (ttisLclosure(stack_ptr))
    a1dd:	66 3d 06 80          	cmp    ax,0x8006
    a1e1:	0f 84 99 01 00 00    	je     a380 <ravi_dump_stack+0x390>
    else if (ttisfunction(stack_ptr))
    a1e7:	89 c2                	mov    edx,eax
    a1e9:	83 e2 0f             	and    edx,0xf
    a1ec:	66 83 fa 06          	cmp    dx,0x6
    a1f0:	0f 84 aa 01 00 00    	je     a3a0 <ravi_dump_stack+0x3b0>
    else if (ttislngstring(stack_ptr) || ttisshrstring(stack_ptr) ||
    a1f6:	89 c1                	mov    ecx,eax
    a1f8:	83 e1 ef             	and    ecx,0xffffffef
    a1fb:	66 81 f9 04 80       	cmp    cx,0x8004
    a200:	0f 84 f2 00 00 00    	je     a2f8 <ravi_dump_stack+0x308>
    a206:	66 83 fa 04          	cmp    dx,0x4
    a20a:	0f 84 e8 00 00 00    	je     a2f8 <ravi_dump_stack+0x308>
    else if (ttistable(stack_ptr))
    a210:	66 83 fa 05          	cmp    dx,0x5
    a214:	0f 84 ce 01 00 00    	je     a3e8 <ravi_dump_stack+0x3f8>
    else if (ttisnil(stack_ptr))
    a21a:	66 85 c0             	test   ax,ax
    a21d:	0f 84 9d 01 00 00    	je     a3c0 <ravi_dump_stack+0x3d0>
    else if (ttisfloat(stack_ptr))
    a223:	66 83 f8 03          	cmp    ax,0x3
    a227:	0f 84 13 02 00 00    	je     a440 <ravi_dump_stack+0x450>
    else if (ttisinteger(stack_ptr))
    a22d:	66 83 f8 13          	cmp    ax,0x13
    a231:	0f 84 c9 01 00 00    	je     a400 <ravi_dump_stack+0x410>
    else if (ttislightuserdata(stack_ptr))
    a237:	66 83 f8 02          	cmp    ax,0x2
    a23b:	0f 84 27 02 00 00    	je     a468 <ravi_dump_stack+0x478>
    else if (ttisfulluserdata(stack_ptr))
    a241:	66 3d 07 80          	cmp    ax,0x8007
    a245:	0f 84 35 02 00 00    	je     a480 <ravi_dump_stack+0x490>
    else if (ttisboolean(stack_ptr))
    a24b:	66 83 f8 01          	cmp    ax,0x1
    a24f:	0f 84 43 02 00 00    	je     a498 <ravi_dump_stack+0x4a8>
    else if (ttisthread(stack_ptr))
    a255:	66 3d 08 80          	cmp    ax,0x8008
    a259:	0f 84 c1 01 00 00    	je     a420 <ravi_dump_stack+0x430>
    a25f:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a266 <ravi_dump_stack+0x276>
    a266:	31 c0                	xor    eax,eax
  for (; stack_ptr >= base; stack_ptr--, i--) {
    a268:	49 83 ec 10          	sub    r12,0x10
    a26c:	83 eb 01             	sub    ebx,0x1
    a26f:	bf 01 00 00 00       	mov    edi,0x1
    a274:	e8 00 00 00 00       	call   a279 <ravi_dump_stack+0x289>
    a279:	4d 39 e5             	cmp    r13,r12
    a27c:	0f 86 f6 fe ff ff    	jbe    a178 <ravi_dump_stack+0x188>
    a282:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  printf(
    a288:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
    a28d:	8b 14 24             	mov    edx,DWORD PTR [rsp]
    a290:	bf 01 00 00 00       	mov    edi,0x1
    a295:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a29c <ravi_dump_stack+0x2ac>
    a29c:	0f b6 48 0c          	movzx  ecx,BYTE PTR [rax+0xc]
    a2a0:	44 8b 48 24          	mov    r9d,DWORD PTR [rax+0x24]
    a2a4:	44 0f b6 40 0a       	movzx  r8d,BYTE PTR [rax+0xa]
    a2a9:	31 c0                	xor    eax,eax
    a2ab:	e8 00 00 00 00       	call   a2b0 <ravi_dump_stack+0x2c0>
    a2b0:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a2b7 <ravi_dump_stack+0x2c7>
    a2b7:	bf 01 00 00 00       	mov    edi,0x1
    a2bc:	31 c0                	xor    eax,eax
    a2be:	e8 00 00 00 00       	call   a2c3 <ravi_dump_stack+0x2d3>
    a2c3:	e9 e8 fd ff ff       	jmp    a0b0 <ravi_dump_stack+0xc0>
    a2c8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    a2cf:	00 
    a2d0:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a2d7 <ravi_dump_stack+0x2e7>
    a2d7:	bf 01 00 00 00       	mov    edi,0x1
    a2dc:	31 c0                	xor    eax,eax
    a2de:	e8 00 00 00 00       	call   a2e3 <ravi_dump_stack+0x2f3>
  for (; stack_ptr >= base; stack_ptr--, i--) {
    a2e3:	49 83 ec 10          	sub    r12,0x10
    a2e7:	83 eb 01             	sub    ebx,0x1
    a2ea:	4d 39 e5             	cmp    r13,r12
    a2ed:	0f 86 85 fe ff ff    	jbe    a178 <ravi_dump_stack+0x188>
    a2f3:	eb 93                	jmp    a288 <ravi_dump_stack+0x298>
    a2f5:	0f 1f 00             	nop    DWORD PTR [rax]
      printf("'%s'\n", svalue(stack_ptr));
    a2f8:	49 8b 04 24          	mov    rax,QWORD PTR [r12]
    a2fc:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a303 <ravi_dump_stack+0x313>
    a303:	bf 01 00 00 00       	mov    edi,0x1
    a308:	48 8d 50 18          	lea    rdx,[rax+0x18]
    a30c:	31 c0                	xor    eax,eax
    a30e:	e8 00 00 00 00       	call   a313 <ravi_dump_stack+0x323>
    a313:	eb ce                	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a315:	0f 1f 00             	nop    DWORD PTR [rax]
    a318:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a31f <ravi_dump_stack+0x32f>
    a31f:	bf 01 00 00 00       	mov    edi,0x1
    a324:	31 c0                	xor    eax,eax
    a326:	e8 00 00 00 00       	call   a32b <ravi_dump_stack+0x33b>
    a32b:	eb b6                	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a32d:	0f 1f 00             	nop    DWORD PTR [rax]
    a330:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a337 <ravi_dump_stack+0x347>
    a337:	bf 01 00 00 00       	mov    edi,0x1
    a33c:	31 c0                	xor    eax,eax
    a33e:	e8 00 00 00 00       	call   a343 <ravi_dump_stack+0x353>
    a343:	eb 9e                	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a345:	0f 1f 00             	nop    DWORD PTR [rax]
    a348:	8b 14 24             	mov    edx,DWORD PTR [rsp]
    a34b:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a352 <ravi_dump_stack+0x362>
    a352:	bf 01 00 00 00       	mov    edi,0x1
    a357:	31 c0                	xor    eax,eax
    a359:	e8 00 00 00 00       	call   a35e <ravi_dump_stack+0x36e>
    a35e:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a365 <ravi_dump_stack+0x375>
    a365:	bf 01 00 00 00       	mov    edi,0x1
    a36a:	31 c0                	xor    eax,eax
    a36c:	e8 00 00 00 00       	call   a371 <ravi_dump_stack+0x381>
    a371:	e9 3a fd ff ff       	jmp    a0b0 <ravi_dump_stack+0xc0>
    a376:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
    a37d:	00 00 00 
    a380:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a387 <ravi_dump_stack+0x397>
    a387:	bf 01 00 00 00       	mov    edi,0x1
    a38c:	31 c0                	xor    eax,eax
    a38e:	e8 00 00 00 00       	call   a393 <ravi_dump_stack+0x3a3>
    a393:	e9 4b ff ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a398:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    a39f:	00 
    a3a0:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a3a7 <ravi_dump_stack+0x3b7>
    a3a7:	bf 01 00 00 00       	mov    edi,0x1
    a3ac:	31 c0                	xor    eax,eax
    a3ae:	e8 00 00 00 00       	call   a3b3 <ravi_dump_stack+0x3c3>
    a3b3:	e9 2b ff ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a3b8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    a3bf:	00 
    a3c0:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a3c7 <ravi_dump_stack+0x3d7>
    a3c7:	bf 01 00 00 00       	mov    edi,0x1
    a3cc:	e8 00 00 00 00       	call   a3d1 <ravi_dump_stack+0x3e1>
    a3d1:	e9 0d ff ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a3d6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
    a3dd:	00 00 00 
    a3e0:	c3                   	ret    
    a3e1:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    a3e8:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a3ef <ravi_dump_stack+0x3ff>
    a3ef:	bf 01 00 00 00       	mov    edi,0x1
    a3f4:	31 c0                	xor    eax,eax
    a3f6:	e8 00 00 00 00       	call   a3fb <ravi_dump_stack+0x40b>
    a3fb:	e9 e3 fe ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a400:	49 8b 14 24          	mov    rdx,QWORD PTR [r12]
    a404:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a40b <ravi_dump_stack+0x41b>
    a40b:	bf 01 00 00 00       	mov    edi,0x1
    a410:	31 c0                	xor    eax,eax
    a412:	e8 00 00 00 00       	call   a417 <ravi_dump_stack+0x427>
    a417:	e9 c7 fe ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a41c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    a420:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a427 <ravi_dump_stack+0x437>
    a427:	bf 01 00 00 00       	mov    edi,0x1
    a42c:	31 c0                	xor    eax,eax
    a42e:	e8 00 00 00 00       	call   a433 <ravi_dump_stack+0x443>
    a433:	e9 ab fe ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a438:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    a43f:	00 
    a440:	f2 41 0f 10 04 24    	movsd  xmm0,QWORD PTR [r12]
    a446:	bf 01 00 00 00       	mov    edi,0x1
    a44b:	b8 01 00 00 00       	mov    eax,0x1
    a450:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a457 <ravi_dump_stack+0x467>
    a457:	e8 00 00 00 00       	call   a45c <ravi_dump_stack+0x46c>
    a45c:	e9 82 fe ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a461:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    a468:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a46f <ravi_dump_stack+0x47f>
    a46f:	bf 01 00 00 00       	mov    edi,0x1
    a474:	31 c0                	xor    eax,eax
    a476:	e8 00 00 00 00       	call   a47b <ravi_dump_stack+0x48b>
    a47b:	e9 63 fe ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a480:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a487 <ravi_dump_stack+0x497>
    a487:	bf 01 00 00 00       	mov    edi,0x1
    a48c:	31 c0                	xor    eax,eax
    a48e:	e8 00 00 00 00       	call   a493 <ravi_dump_stack+0x4a3>
    a493:	e9 4b fe ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>
    a498:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a49f <ravi_dump_stack+0x4af>
    a49f:	bf 01 00 00 00       	mov    edi,0x1
    a4a4:	31 c0                	xor    eax,eax
    a4a6:	e8 00 00 00 00       	call   a4ab <ravi_dump_stack+0x4bb>
    a4ab:	e9 33 fe ff ff       	jmp    a2e3 <ravi_dump_stack+0x2f3>

000000000000a4b0 <ravi_dump_stacktop>:

void ravi_dump_stacktop(lua_State *L, const char *s) {
  CallInfo *ci = L->ci;
    a4b0:	48 8b 4f 20          	mov    rcx,QWORD PTR [rdi+0x20]
  int funcpos = (int)(ci->func - L->stack);
    a4b4:	48 8b 47 38          	mov    rax,QWORD PTR [rdi+0x38]
void ravi_dump_stacktop(lua_State *L, const char *s) {
    a4b8:	48 89 f2             	mov    rdx,rsi
    a4bb:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a4c2 <ravi_dump_stacktop+0x12>
  int top = (int)(L->top - L->stack);
    a4c2:	4c 8b 47 10          	mov    r8,QWORD PTR [rdi+0x10]
    a4c6:	bf 01 00 00 00       	mov    edi,0x1
  int ci_top = (int)(ci->top - L->stack);
    a4cb:	4c 8b 49 08          	mov    r9,QWORD PTR [rcx+0x8]
  int funcpos = (int)(ci->func - L->stack);
    a4cf:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
  int top = (int)(L->top - L->stack);
    a4d2:	49 29 c0             	sub    r8,rax
  int ci_top = (int)(ci->top - L->stack);
    a4d5:	49 29 c1             	sub    r9,rax
  int funcpos = (int)(ci->func - L->stack);
    a4d8:	48 29 c1             	sub    rcx,rax
  int top = (int)(L->top - L->stack);
    a4db:	49 c1 f8 04          	sar    r8,0x4
    a4df:	31 c0                	xor    eax,eax
  int ci_top = (int)(ci->top - L->stack);
    a4e1:	49 c1 f9 04          	sar    r9,0x4
  int funcpos = (int)(ci->func - L->stack);
    a4e5:	48 c1 f9 04          	sar    rcx,0x4
    a4e9:	e9 00 00 00 00       	jmp    a4ee <ravi_dump_stacktop+0x3e>
    a4ee:	66 90                	xchg   ax,ax

000000000000a4f0 <ravi_debug_trace>:
/*
** This function is called from JIT compiled code when JIT trace is
** enabled; the function needs to update the savedpc and
** call luaG_traceexec() if necessary
*/
void ravi_debug_trace(lua_State *L, int opCode, int pc) {
    a4f0:	41 57                	push   r15
    a4f2:	41 56                	push   r14
    a4f4:	4c 63 f2             	movsxd r14,edx
    a4f7:	41 55                	push   r13
    a4f9:	41 54                	push   r12
    a4fb:	55                   	push   rbp
    a4fc:	53                   	push   rbx
    a4fd:	48 89 fb             	mov    rbx,rdi
    a500:	48 81 ec 98 00 00 00 	sub    rsp,0x98
    a507:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    a50e:	00 00 
    a510:	48 89 84 24 88 00 00 	mov    QWORD PTR [rsp+0x88],rax
    a517:	00 
    a518:	31 c0                	xor    eax,eax
  RAVI_DEBUG_STACK(
    a51a:	48 8b 47 20          	mov    rax,QWORD PTR [rdi+0x20]
    a51e:	48 8b 28             	mov    rbp,QWORD PTR [rax]
    a521:	48 8b 55 00          	mov    rdx,QWORD PTR [rbp+0x0]
    a525:	48 8b 52 18          	mov    rdx,QWORD PTR [rdx+0x18]
    a529:	48 8b 52 38          	mov    rdx,QWORD PTR [rdx+0x38]
    a52d:	f6 05 00 00 00 00 08 	test   BYTE PTR [rip+0x0],0x8        # a534 <ravi_debug_trace+0x44>
    a534:	75 42                	jne    a578 <ravi_debug_trace+0x88>
  // required if someone wishes to set a line hook. The second option
  // is very expensive and will inhibit optimizations, hence it is optional.
  // This is the setting that is done below - and then the hook is invoked
  // See issue #15
  LClosure *closure = clLvalue(L->ci->func);
  L->ci->u.l.savedpc = &closure->p->code[pc + 1];
    a536:	4a 8d 54 b2 04       	lea    rdx,[rdx+r14*4+0x4]
    a53b:	48 89 50 28          	mov    QWORD PTR [rax+0x28],rdx
  if (L->hookmask & (LUA_MASKLINE | LUA_MASKCOUNT)) luaG_traceexec(L);
    a53f:	f6 83 c8 00 00 00 0c 	test   BYTE PTR [rbx+0xc8],0xc
    a546:	0f 85 e4 00 00 00    	jne    a630 <ravi_debug_trace+0x140>
}
    a54c:	48 8b 84 24 88 00 00 	mov    rax,QWORD PTR [rsp+0x88]
    a553:	00 
    a554:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    a55b:	00 00 
    a55d:	0f 85 da 00 00 00    	jne    a63d <ravi_debug_trace+0x14d>
    a563:	48 81 c4 98 00 00 00 	add    rsp,0x98
    a56a:	5b                   	pop    rbx
    a56b:	5d                   	pop    rbp
    a56c:	41 5c                	pop    r12
    a56e:	41 5d                	pop    r13
    a570:	41 5e                	pop    r14
    a572:	41 5f                	pop    r15
    a574:	c3                   	ret    
    a575:	0f 1f 00             	nop    DWORD PTR [rax]
  RAVI_DEBUG_STACK(
    a578:	4c 8b 47 38          	mov    r8,QWORD PTR [rdi+0x38]
    a57c:	4c 8b 50 20          	mov    r10,QWORD PTR [rax+0x20]
    a580:	48 8d 4c 24 20       	lea    rcx,[rsp+0x20]
    a585:	89 74 24 1c          	mov    DWORD PTR [rsp+0x1c],esi
    a589:	4c 8b 67 10          	mov    r12,QWORD PTR [rdi+0x10]
    a58d:	4c 8b 68 08          	mov    r13,QWORD PTR [rax+0x8]
    a591:	48 89 cf             	mov    rdi,rcx
    a594:	48 89 0c 24          	mov    QWORD PTR [rsp],rcx
    a598:	4d 29 c2             	sub    r10,r8
    a59b:	42 8b 14 b2          	mov    edx,DWORD PTR [rdx+r14*4]
    a59f:	be 64 00 00 00       	mov    esi,0x64
    a5a4:	4c 89 44 24 10       	mov    QWORD PTR [rsp+0x10],r8
    a5a9:	49 c1 fa 04          	sar    r10,0x4
    a5ad:	4d 29 c4             	sub    r12,r8
    a5b0:	4d 29 c5             	sub    r13,r8
    a5b3:	4c 89 54 24 08       	mov    QWORD PTR [rsp+0x8],r10
    a5b8:	49 c1 fc 04          	sar    r12,0x4
    a5bc:	49 c1 fd 04          	sar    r13,0x4
    a5c0:	e8 00 00 00 00       	call   a5c5 <ravi_debug_trace+0xd5>
    a5c5:	4c 8b 54 24 08       	mov    r10,QWORD PTR [rsp+0x8]
    a5ca:	4c 8b 44 24 10       	mov    r8,QWORD PTR [rsp+0x10]
    a5cf:	48 8d 05 00 00 00 00 	lea    rax,[rip+0x0]        # a5d6 <ravi_debug_trace+0xe6>
    a5d6:	4c 63 4c 24 1c       	movsxd r9,DWORD PTR [rsp+0x1c]
    a5db:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # a5e2 <ravi_debug_trace+0xf2>
    a5e2:	bf 01 00 00 00       	mov    edi,0x1
    a5e7:	45 29 d5             	sub    r13d,r10d
    a5ea:	45 29 d4             	sub    r12d,r10d
    a5ed:	4c 29 c5             	sub    rbp,r8
    a5f0:	41 55                	push   r13
    a5f2:	4a 8b 14 c8          	mov    rdx,QWORD PTR [rax+r9*8]
    a5f6:	49 89 e8             	mov    r8,rbp
    a5f9:	45 89 f1             	mov    r9d,r14d
    a5fc:	41 54                	push   r12
    a5fe:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    a603:	49 c1 f8 04          	sar    r8,0x4
    a607:	31 c0                	xor    eax,eax
    a609:	e8 00 00 00 00       	call   a60e <ravi_debug_trace+0x11e>
    a60e:	48 8b 43 20          	mov    rax,QWORD PTR [rbx+0x20]
    a612:	59                   	pop    rcx
    a613:	5e                   	pop    rsi
    a614:	48 8b 10             	mov    rdx,QWORD PTR [rax]
    a617:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    a61a:	48 8b 52 18          	mov    rdx,QWORD PTR [rdx+0x18]
    a61e:	48 8b 52 38          	mov    rdx,QWORD PTR [rdx+0x38]
    a622:	e9 0f ff ff ff       	jmp    a536 <ravi_debug_trace+0x46>
    a627:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
    a62e:	00 00 
  if (L->hookmask & (LUA_MASKLINE | LUA_MASKCOUNT)) luaG_traceexec(L);
    a630:	48 89 df             	mov    rdi,rbx
    a633:	e8 00 00 00 00       	call   a638 <ravi_debug_trace+0x148>
}
    a638:	e9 0f ff ff ff       	jmp    a54c <ravi_debug_trace+0x5c>
    a63d:	e8 00 00 00 00       	call   a642 <ravi_debug_trace+0x152>
    a642:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    a649:	00 00 00 00 
    a64d:	0f 1f 00             	nop    DWORD PTR [rax]

000000000000a650 <raviV_op_newarrayint>:

void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra) {
    a650:	41 54                	push   r12
    a652:	49 89 f4             	mov    r12,rsi
  Table *t = raviH_new(L, RAVI_TARRAYINT, 0);
    a655:	be 03 00 00 00       	mov    esi,0x3
void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra) {
    a65a:	55                   	push   rbp
    a65b:	48 89 d5             	mov    rbp,rdx
  Table *t = raviH_new(L, RAVI_TARRAYINT, 0);
    a65e:	31 d2                	xor    edx,edx
void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra) {
    a660:	53                   	push   rbx
    a661:	48 89 fb             	mov    rbx,rdi
  Table *t = raviH_new(L, RAVI_TARRAYINT, 0);
    a664:	e8 00 00 00 00       	call   a669 <raviV_op_newarrayint+0x19>
  setiarrayvalue(L, ra, t);
    a669:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    a66d:	b8 15 80 ff ff       	mov    eax,0xffff8015
    a672:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
  checkGC_(L, ra + 1);
    a676:	48 8b 43 18          	mov    rax,QWORD PTR [rbx+0x18]
    a67a:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    a67f:	7e 19                	jle    a69a <raviV_op_newarrayint+0x4a>
    a681:	48 83 c5 10          	add    rbp,0x10
    a685:	48 89 df             	mov    rdi,rbx
    a688:	48 89 6b 10          	mov    QWORD PTR [rbx+0x10],rbp
    a68c:	e8 00 00 00 00       	call   a691 <raviV_op_newarrayint+0x41>
    a691:	49 8b 44 24 08       	mov    rax,QWORD PTR [r12+0x8]
    a696:	48 89 43 10          	mov    QWORD PTR [rbx+0x10],rax
}
    a69a:	5b                   	pop    rbx
    a69b:	5d                   	pop    rbp
    a69c:	41 5c                	pop    r12
    a69e:	c3                   	ret    
    a69f:	90                   	nop

000000000000a6a0 <raviV_op_newarrayfloat>:

void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra) {
    a6a0:	41 54                	push   r12
    a6a2:	49 89 f4             	mov    r12,rsi
  Table *t = raviH_new(L, RAVI_TARRAYFLT, 0);
    a6a5:	be 04 00 00 00       	mov    esi,0x4
void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra) {
    a6aa:	55                   	push   rbp
    a6ab:	48 89 d5             	mov    rbp,rdx
  Table *t = raviH_new(L, RAVI_TARRAYFLT, 0);
    a6ae:	31 d2                	xor    edx,edx
void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra) {
    a6b0:	53                   	push   rbx
    a6b1:	48 89 fb             	mov    rbx,rdi
  Table *t = raviH_new(L, RAVI_TARRAYFLT, 0);
    a6b4:	e8 00 00 00 00       	call   a6b9 <raviV_op_newarrayfloat+0x19>
  setfarrayvalue(L, ra, t);
    a6b9:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    a6bd:	b8 25 80 ff ff       	mov    eax,0xffff8025
    a6c2:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
  checkGC_(L, ra + 1);
    a6c6:	48 8b 43 18          	mov    rax,QWORD PTR [rbx+0x18]
    a6ca:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    a6cf:	7e 19                	jle    a6ea <raviV_op_newarrayfloat+0x4a>
    a6d1:	48 83 c5 10          	add    rbp,0x10
    a6d5:	48 89 df             	mov    rdi,rbx
    a6d8:	48 89 6b 10          	mov    QWORD PTR [rbx+0x10],rbp
    a6dc:	e8 00 00 00 00       	call   a6e1 <raviV_op_newarrayfloat+0x41>
    a6e1:	49 8b 44 24 08       	mov    rax,QWORD PTR [r12+0x8]
    a6e6:	48 89 43 10          	mov    QWORD PTR [rbx+0x10],rax
}
    a6ea:	5b                   	pop    rbx
    a6eb:	5d                   	pop    rbp
    a6ec:	41 5c                	pop    r12
    a6ee:	c3                   	ret    
    a6ef:	90                   	nop

000000000000a6f0 <raviV_op_newtable>:

void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int c) {
    a6f0:	41 57                	push   r15
    a6f2:	45 89 c7             	mov    r15d,r8d
    a6f5:	41 56                	push   r14
    a6f7:	49 89 f6             	mov    r14,rsi
    a6fa:	41 55                	push   r13
    a6fc:	41 89 cd             	mov    r13d,ecx
    a6ff:	41 54                	push   r12
    a701:	55                   	push   rbp
    a702:	48 89 d5             	mov    rbp,rdx
    a705:	53                   	push   rbx
    a706:	48 89 fb             	mov    rbx,rdi
    a709:	48 83 ec 08          	sub    rsp,0x8
  Table *t = luaH_new(L);
    a70d:	e8 00 00 00 00       	call   a712 <raviV_op_newtable+0x22>
    a712:	49 89 c4             	mov    r12,rax
  sethvalue(L, ra, t);
    a715:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    a719:	b8 05 80 ff ff       	mov    eax,0xffff8005
    a71e:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
  if (b != 0 || c != 0) luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
    a722:	44 89 e8             	mov    eax,r13d
    a725:	44 09 f8             	or     eax,r15d
    a728:	75 36                	jne    a760 <raviV_op_newtable+0x70>
  checkGC_(L, ra + 1);
    a72a:	48 8b 43 18          	mov    rax,QWORD PTR [rbx+0x18]
    a72e:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    a733:	7e 18                	jle    a74d <raviV_op_newtable+0x5d>
    a735:	48 83 c5 10          	add    rbp,0x10
    a739:	48 89 df             	mov    rdi,rbx
    a73c:	48 89 6b 10          	mov    QWORD PTR [rbx+0x10],rbp
    a740:	e8 00 00 00 00       	call   a745 <raviV_op_newtable+0x55>
    a745:	49 8b 46 08          	mov    rax,QWORD PTR [r14+0x8]
    a749:	48 89 43 10          	mov    QWORD PTR [rbx+0x10],rax
}
    a74d:	48 83 c4 08          	add    rsp,0x8
    a751:	5b                   	pop    rbx
    a752:	5d                   	pop    rbp
    a753:	41 5c                	pop    r12
    a755:	41 5d                	pop    r13
    a757:	41 5e                	pop    r14
    a759:	41 5f                	pop    r15
    a75b:	c3                   	ret    
    a75c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  if (b != 0 || c != 0) luaH_resize(L, t, luaO_fb2int(b), luaO_fb2int(c));
    a760:	44 89 ff             	mov    edi,r15d
    a763:	e8 00 00 00 00       	call   a768 <raviV_op_newtable+0x78>
    a768:	44 89 ef             	mov    edi,r13d
    a76b:	41 89 c7             	mov    r15d,eax
    a76e:	e8 00 00 00 00       	call   a773 <raviV_op_newtable+0x83>
    a773:	44 89 f9             	mov    ecx,r15d
    a776:	4c 89 e6             	mov    rsi,r12
    a779:	48 89 df             	mov    rdi,rbx
    a77c:	89 c2                	mov    edx,eax
    a77e:	e8 00 00 00 00       	call   a783 <raviV_op_newtable+0x93>
    a783:	eb a5                	jmp    a72a <raviV_op_newtable+0x3a>
    a785:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    a78c:	00 00 00 00 

000000000000a790 <raviV_op_setlist>:

void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c) {
    a790:	41 57                	push   r15
    a792:	41 56                	push   r14
    a794:	41 55                	push   r13
    a796:	49 89 d5             	mov    r13,rdx
    a799:	41 54                	push   r12
    a79b:	55                   	push   rbp
    a79c:	48 89 fd             	mov    rbp,rdi
    a79f:	53                   	push   rbx
    a7a0:	48 83 ec 38          	sub    rsp,0x38
    a7a4:	48 89 74 24 10       	mov    QWORD PTR [rsp+0x10],rsi
    a7a9:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    a7b0:	00 00 
    a7b2:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
    a7b7:	31 c0                	xor    eax,eax
  int n = b;
  unsigned int last;
  Table *h;
  if (n == 0) n = cast_int(L->top - ra) - 1;
    a7b9:	85 c9                	test   ecx,ecx
    a7bb:	75 0e                	jne    a7cb <raviV_op_setlist+0x3b>
    a7bd:	48 8b 4f 10          	mov    rcx,QWORD PTR [rdi+0x10]
    a7c1:	48 29 d1             	sub    rcx,rdx
    a7c4:	48 c1 f9 04          	sar    rcx,0x4
    a7c8:	83 e9 01             	sub    ecx,0x1
  h = hvalue(ra);
    a7cb:	4d 8b 65 00          	mov    r12,QWORD PTR [r13+0x0]
  last = ((c - 1) * LFIELDS_PER_FLUSH) + n;
    a7cf:	41 8d 58 ff          	lea    ebx,[r8-0x1]
    a7d3:	6b db 32             	imul   ebx,ebx,0x32
  if (h->ravi_array.array_type == RAVI_TTABLE) {
    a7d6:	41 0f b6 44 24 48    	movzx  eax,BYTE PTR [r12+0x48]
  h = hvalue(ra);
    a7dc:	4c 89 64 24 08       	mov    QWORD PTR [rsp+0x8],r12
  last = ((c - 1) * LFIELDS_PER_FLUSH) + n;
    a7e1:	44 8d 34 0b          	lea    r14d,[rbx+rcx*1]
  if (h->ravi_array.array_type == RAVI_TTABLE) {
    a7e5:	3c 06                	cmp    al,0x6
    a7e7:	0f 84 03 02 00 00    	je     a9f0 <raviV_op_setlist+0x260>
      luaH_setint(L, h, last--, val);
      luaC_barrierback(L, h, val);
    }
  }
  else {
    int i = last - n + 1;
    a7ed:	83 c3 01             	add    ebx,0x1
    for (; i <= (int)last; i++) {
    a7f0:	41 39 de             	cmp    r14d,ebx
    a7f3:	0f 8c 9f 00 00 00    	jl     a898 <raviV_op_setlist+0x108>
    a7f9:	4c 63 c3             	movsxd r8,ebx
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    a7fc:	4c 8d 7c 24 20       	lea    r15,[rsp+0x20]
    a801:	49 c1 e0 04          	shl    r8,0x4
    a805:	4d 01 c5             	add    r13,r8
    a808:	eb 58                	jmp    a862 <raviV_op_setlist+0xd2>
    a80a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
      TValue *val = ra + i;
      unsigned int u = (unsigned int)(i);
      switch (h->ravi_array.array_type) {
    a810:	3c 04                	cmp    al,0x4
    a812:	75 3c                	jne    a850 <raviV_op_setlist+0xc0>
            else
              luaG_runerror(L, "value cannot be converted to integer");
          }
        } break;
        case RAVI_TARRAYFLT: {
          if (ttisfloat(val)) {
    a814:	41 0f b7 45 08       	movzx  eax,WORD PTR [r13+0x8]
    a819:	66 83 f8 03          	cmp    ax,0x3
    a81d:	0f 84 ed 00 00 00    	je     a910 <raviV_op_setlist+0x180>
            raviH_set_float_inline(L, h, u, fltvalue(val));
          }
          else if (ttisinteger(val)) {
    a823:	66 83 f8 13          	cmp    ax,0x13
    a827:	0f 85 0b 01 00 00    	jne    a938 <raviV_op_setlist+0x1a8>
            raviH_set_float_inline(L, h, u, (lua_Number)(ivalue(val)));
    a82d:	66 0f ef c0          	pxor   xmm0,xmm0
    a831:	89 da                	mov    edx,ebx
    a833:	f2 49 0f 2a 45 00    	cvtsi2sd xmm0,QWORD PTR [r13+0x0]
    a839:	41 3b 5c 24 40       	cmp    ebx,DWORD PTR [r12+0x40]
    a83e:	0f 83 7c 01 00 00    	jae    a9c0 <raviV_op_setlist+0x230>
    a844:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    a849:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    a84e:	66 90                	xchg   ax,ax
    for (; i <= (int)last; i++) {
    a850:	83 c3 01             	add    ebx,0x1
    a853:	49 83 c5 10          	add    r13,0x10
    a857:	41 39 de             	cmp    r14d,ebx
    a85a:	7c 3c                	jl     a898 <raviV_op_setlist+0x108>
    a85c:	41 0f b6 44 24 48    	movzx  eax,BYTE PTR [r12+0x48]
      switch (h->ravi_array.array_type) {
    a862:	3c 03                	cmp    al,0x3
    a864:	75 aa                	jne    a810 <raviV_op_setlist+0x80>
          if (ttisinteger(val)) { raviH_set_int_inline(L, h, u, ivalue(val)); }
    a866:	66 41 83 7d 08 13    	cmp    WORD PTR [r13+0x8],0x13
    a86c:	75 62                	jne    a8d0 <raviV_op_setlist+0x140>
    a86e:	49 8b 4d 00          	mov    rcx,QWORD PTR [r13+0x0]
    a872:	89 da                	mov    edx,ebx
    a874:	41 39 5c 24 40       	cmp    DWORD PTR [r12+0x40],ebx
    a879:	0f 86 f9 00 00 00    	jbe    a978 <raviV_op_setlist+0x1e8>
    a87f:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    for (; i <= (int)last; i++) {
    a884:	83 c3 01             	add    ebx,0x1
    a887:	49 83 c5 10          	add    r13,0x10
          if (ttisinteger(val)) { raviH_set_int_inline(L, h, u, ivalue(val)); }
    a88b:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    for (; i <= (int)last; i++) {
    a88f:	41 39 de             	cmp    r14d,ebx
    a892:	7d c8                	jge    a85c <raviV_op_setlist+0xcc>
    a894:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
        } break;
        default: lua_assert(0);
      }
    }
  }
  L->top = ci->top; /* correct top (in case of previous open call) */
    a898:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    a89d:	48 8b 40 08          	mov    rax,QWORD PTR [rax+0x8]
    a8a1:	48 89 45 10          	mov    QWORD PTR [rbp+0x10],rax
}
    a8a5:	48 8b 44 24 28       	mov    rax,QWORD PTR [rsp+0x28]
    a8aa:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    a8b1:	00 00 
    a8b3:	0f 85 d3 01 00 00    	jne    aa8c <raviV_op_setlist+0x2fc>
    a8b9:	48 83 c4 38          	add    rsp,0x38
    a8bd:	5b                   	pop    rbx
    a8be:	5d                   	pop    rbp
    a8bf:	41 5c                	pop    r12
    a8c1:	41 5d                	pop    r13
    a8c3:	41 5e                	pop    r14
    a8c5:	41 5f                	pop    r15
    a8c7:	c3                   	ret    
    a8c8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    a8cf:	00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    a8d0:	31 d2                	xor    edx,edx
    a8d2:	4c 89 fe             	mov    rsi,r15
    a8d5:	4c 89 ef             	mov    rdi,r13
            lua_Integer i = 0;
    a8d8:	48 c7 44 24 20 00 00 	mov    QWORD PTR [rsp+0x20],0x0
    a8df:	00 00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    a8e1:	e8 00 00 00 00       	call   a8e6 <raviV_op_setlist+0x156>
            if (luaV_tointeger_(val, &i)) { raviH_set_int_inline(L, h, u, i); }
    a8e6:	85 c0                	test   eax,eax
    a8e8:	0f 84 8d 01 00 00    	je     aa7b <raviV_op_setlist+0x2eb>
    a8ee:	48 8b 4c 24 20       	mov    rcx,QWORD PTR [rsp+0x20]
    a8f3:	89 da                	mov    edx,ebx
    a8f5:	41 39 5c 24 40       	cmp    DWORD PTR [r12+0x40],ebx
    a8fa:	0f 86 90 00 00 00    	jbe    a990 <raviV_op_setlist+0x200>
    a900:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    a905:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    a909:	e9 42 ff ff ff       	jmp    a850 <raviV_op_setlist+0xc0>
    a90e:	66 90                	xchg   ax,ax
            raviH_set_float_inline(L, h, u, fltvalue(val));
    a910:	89 da                	mov    edx,ebx
    a912:	41 39 5c 24 40       	cmp    DWORD PTR [r12+0x40],ebx
    a917:	0f 86 8b 00 00 00    	jbe    a9a8 <raviV_op_setlist+0x218>
    a91d:	f2 41 0f 10 45 00    	movsd  xmm0,QWORD PTR [r13+0x0]
    a923:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    a928:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    a92d:	e9 1e ff ff ff       	jmp    a850 <raviV_op_setlist+0xc0>
    a932:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
            lua_Number d = 0.0;
    a938:	48 c7 44 24 20 00 00 	mov    QWORD PTR [rsp+0x20],0x0
    a93f:	00 00 
            if (luaV_tonumber_(val, &d)) { raviH_set_float_inline(L, h, u, d); }
    a941:	4c 89 fe             	mov    rsi,r15
    a944:	4c 89 ef             	mov    rdi,r13
    a947:	e8 00 00 00 00       	call   a94c <raviV_op_setlist+0x1bc>
    a94c:	85 c0                	test   eax,eax
    a94e:	0f 84 3d 01 00 00    	je     aa91 <raviV_op_setlist+0x301>
    a954:	f2 0f 10 44 24 20    	movsd  xmm0,QWORD PTR [rsp+0x20]
    a95a:	89 da                	mov    edx,ebx
    a95c:	41 3b 5c 24 40       	cmp    ebx,DWORD PTR [r12+0x40]
    a961:	73 75                	jae    a9d8 <raviV_op_setlist+0x248>
    a963:	49 8b 44 24 38       	mov    rax,QWORD PTR [r12+0x38]
    a968:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    a96d:	e9 de fe ff ff       	jmp    a850 <raviV_op_setlist+0xc0>
    a972:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
          if (ttisinteger(val)) { raviH_set_int_inline(L, h, u, ivalue(val)); }
    a978:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    a97d:	48 89 ef             	mov    rdi,rbp
    a980:	e8 00 00 00 00       	call   a985 <raviV_op_setlist+0x1f5>
    a985:	e9 c6 fe ff ff       	jmp    a850 <raviV_op_setlist+0xc0>
    a98a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
            if (luaV_tointeger_(val, &i)) { raviH_set_int_inline(L, h, u, i); }
    a990:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    a995:	48 89 ef             	mov    rdi,rbp
    a998:	e8 00 00 00 00       	call   a99d <raviV_op_setlist+0x20d>
    a99d:	e9 ae fe ff ff       	jmp    a850 <raviV_op_setlist+0xc0>
    a9a2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
            raviH_set_float_inline(L, h, u, fltvalue(val));
    a9a8:	f2 41 0f 10 45 00    	movsd  xmm0,QWORD PTR [r13+0x0]
    a9ae:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    a9b3:	48 89 ef             	mov    rdi,rbp
    a9b6:	e8 00 00 00 00       	call   a9bb <raviV_op_setlist+0x22b>
    a9bb:	e9 90 fe ff ff       	jmp    a850 <raviV_op_setlist+0xc0>
            raviH_set_float_inline(L, h, u, (lua_Number)(ivalue(val)));
    a9c0:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    a9c5:	48 89 ef             	mov    rdi,rbp
    a9c8:	e8 00 00 00 00       	call   a9cd <raviV_op_setlist+0x23d>
    a9cd:	e9 7e fe ff ff       	jmp    a850 <raviV_op_setlist+0xc0>
    a9d2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
            if (luaV_tonumber_(val, &d)) { raviH_set_float_inline(L, h, u, d); }
    a9d8:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    a9dd:	48 89 ef             	mov    rdi,rbp
    a9e0:	e8 00 00 00 00       	call   a9e5 <raviV_op_setlist+0x255>
    a9e5:	e9 66 fe ff ff       	jmp    a850 <raviV_op_setlist+0xc0>
    a9ea:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    if (last > h->sizearray)        /* needs more space? */
    a9f0:	45 39 74 24 0c       	cmp    DWORD PTR [r12+0xc],r14d
    a9f5:	72 69                	jb     aa60 <raviV_op_setlist+0x2d0>
    for (; n > 0; n--) {
    a9f7:	85 c9                	test   ecx,ecx
    a9f9:	0f 8e 99 fe ff ff    	jle    a898 <raviV_op_setlist+0x108>
    a9ff:	48 63 c9             	movsxd rcx,ecx
    aa02:	48 c1 e1 04          	shl    rcx,0x4
    aa06:	49 01 cd             	add    r13,rcx
    aa09:	eb 15                	jmp    aa20 <raviV_op_setlist+0x290>
    aa0b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    aa10:	49 83 ed 10          	sub    r13,0x10
    aa14:	45 89 fe             	mov    r14d,r15d
    aa17:	44 39 fb             	cmp    ebx,r15d
    aa1a:	0f 84 78 fe ff ff    	je     a898 <raviV_op_setlist+0x108>
      luaH_setint(L, h, last--, val);
    aa20:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    aa25:	44 89 f2             	mov    edx,r14d
    aa28:	4c 89 e9             	mov    rcx,r13
    aa2b:	48 89 ef             	mov    rdi,rbp
    aa2e:	45 8d 7e ff          	lea    r15d,[r14-0x1]
    aa32:	e8 00 00 00 00       	call   aa37 <raviV_op_setlist+0x2a7>
      luaC_barrierback(L, h, val);
    aa37:	66 41 83 7d 08 00    	cmp    WORD PTR [r13+0x8],0x0
    aa3d:	79 d1                	jns    aa10 <raviV_op_setlist+0x280>
    aa3f:	41 f6 44 24 09 04    	test   BYTE PTR [r12+0x9],0x4
    aa45:	74 c9                	je     aa10 <raviV_op_setlist+0x280>
    aa47:	49 8b 45 00          	mov    rax,QWORD PTR [r13+0x0]
    aa4b:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
    aa4f:	74 bf                	je     aa10 <raviV_op_setlist+0x280>
    aa51:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
    aa56:	48 89 ef             	mov    rdi,rbp
    aa59:	e8 00 00 00 00       	call   aa5e <raviV_op_setlist+0x2ce>
    aa5e:	eb b0                	jmp    aa10 <raviV_op_setlist+0x280>
      luaH_resizearray(L, h, last); /* pre-allocate it at once */
    aa60:	44 89 f2             	mov    edx,r14d
    aa63:	4c 89 e6             	mov    rsi,r12
    aa66:	48 89 ef             	mov    rdi,rbp
    aa69:	89 4c 24 1c          	mov    DWORD PTR [rsp+0x1c],ecx
    aa6d:	e8 00 00 00 00       	call   aa72 <raviV_op_setlist+0x2e2>
    aa72:	8b 4c 24 1c          	mov    ecx,DWORD PTR [rsp+0x1c]
    aa76:	e9 7c ff ff ff       	jmp    a9f7 <raviV_op_setlist+0x267>
              luaG_runerror(L, "value cannot be converted to integer");
    aa7b:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # aa82 <raviV_op_setlist+0x2f2>
    aa82:	48 89 ef             	mov    rdi,rbp
    aa85:	31 c0                	xor    eax,eax
    aa87:	e8 00 00 00 00       	call   aa8c <raviV_op_setlist+0x2fc>
}
    aa8c:	e8 00 00 00 00       	call   aa91 <raviV_op_setlist+0x301>
              luaG_runerror(L, "value cannot be converted to number");
    aa91:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # aa98 <raviV_op_setlist+0x308>
    aa98:	48 89 ef             	mov    rdi,rbp
    aa9b:	31 c0                	xor    eax,eax
    aa9d:	e8 00 00 00 00       	call   aaa2 <raviV_op_setlist+0x312>
    aaa2:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    aaa9:	00 00 00 00 
    aaad:	0f 1f 00             	nop    DWORD PTR [rax]

000000000000aab0 <raviV_op_concat>:

void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c) {
    aab0:	41 55                	push   r13
  StkId rb, ra;
  StkId base = ci->u.l.base;
  L->top = base + c + 1; /* mark the end of concat operands */
    aab2:	49 63 c0             	movsxd rax,r8d
void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c) {
    aab5:	4c 63 e9             	movsxd r13,ecx
    aab8:	41 54                	push   r12
  L->top = base + c + 1; /* mark the end of concat operands */
    aaba:	48 83 c0 01          	add    rax,0x1
  Protect_base(luaV_concat(L, c - b + 1));
    aabe:	45 29 e8             	sub    r8d,r13d
void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c) {
    aac1:	49 89 f4             	mov    r12,rsi
    aac4:	55                   	push   rbp
  L->top = base + c + 1; /* mark the end of concat operands */
    aac5:	48 c1 e0 04          	shl    rax,0x4
void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c) {
    aac9:	48 89 fd             	mov    rbp,rdi
  ra = base + a; /* 'luav_concat' may invoke TMs and move the stack */
  rb = base + b;
    aacc:	49 c1 e5 04          	shl    r13,0x4
void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c) {
    aad0:	53                   	push   rbx
    aad1:	48 63 da             	movsxd rbx,edx
  ra = base + a; /* 'luav_concat' may invoke TMs and move the stack */
    aad4:	48 c1 e3 04          	shl    rbx,0x4
void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c) {
    aad8:	48 83 ec 08          	sub    rsp,0x8
  L->top = base + c + 1; /* mark the end of concat operands */
    aadc:	48 03 46 20          	add    rax,QWORD PTR [rsi+0x20]
  Protect_base(luaV_concat(L, c - b + 1));
    aae0:	41 8d 70 01          	lea    esi,[r8+0x1]
  L->top = base + c + 1; /* mark the end of concat operands */
    aae4:	48 89 47 10          	mov    QWORD PTR [rdi+0x10],rax
  Protect_base(luaV_concat(L, c - b + 1));
    aae8:	e8 00 00 00 00       	call   aaed <raviV_op_concat+0x3d>
    aaed:	49 8b 44 24 20       	mov    rax,QWORD PTR [r12+0x20]
  rb = base + b;
    aaf2:	4a 8d 0c 28          	lea    rcx,[rax+r13*1]
  ra = base + a; /* 'luav_concat' may invoke TMs and move the stack */
    aaf6:	48 8d 14 18          	lea    rdx,[rax+rbx*1]
  setobjs2s(L, ra, rb);
    aafa:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    aafd:	48 89 02             	mov    QWORD PTR [rdx],rax
    ab00:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    ab04:	66 89 42 08          	mov    WORD PTR [rdx+0x8],ax
  checkGC_protectbase(L, (ra >= rb ? ra + 1 : rb));
    ab08:	48 8b 45 18          	mov    rax,QWORD PTR [rbp+0x18]
    ab0c:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    ab11:	7e 17                	jle    ab2a <raviV_op_concat+0x7a>
    ab13:	48 8d 42 10          	lea    rax,[rdx+0x10]
    ab17:	48 39 ca             	cmp    rdx,rcx
    ab1a:	48 89 ef             	mov    rdi,rbp
    ab1d:	48 0f 43 c8          	cmovae rcx,rax
    ab21:	48 89 4d 10          	mov    QWORD PTR [rbp+0x10],rcx
    ab25:	e8 00 00 00 00       	call   ab2a <raviV_op_concat+0x7a>
  L->top = ci->top; /* restore top */
    ab2a:	49 8b 44 24 08       	mov    rax,QWORD PTR [r12+0x8]
    ab2f:	48 89 45 10          	mov    QWORD PTR [rbp+0x10],rax
}
    ab33:	48 83 c4 08          	add    rsp,0x8
    ab37:	5b                   	pop    rbx
    ab38:	5d                   	pop    rbp
    ab39:	41 5c                	pop    r12
    ab3b:	41 5d                	pop    r13
    ab3d:	c3                   	ret    
    ab3e:	66 90                	xchg   ax,ax

000000000000ab40 <raviV_op_closure>:

void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx) {
    ab40:	41 56                	push   r14
  StkId base = ci->u.l.base;
  Proto *p = cl->p->p[Bx];
    ab42:	4d 63 c0             	movsxd r8,r8d
  LClosure *ncl = getcached(p, cl->upvals, base); /* cached closure */
    ab45:	4c 8d 5a 20          	lea    r11,[rdx+0x20]
void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx) {
    ab49:	41 55                	push   r13
    ab4b:	49 89 fd             	mov    r13,rdi
    ab4e:	41 54                	push   r12
    ab50:	55                   	push   rbp
    ab51:	48 89 f5             	mov    rbp,rsi
    ab54:	53                   	push   rbx
  Proto *p = cl->p->p[Bx];
    ab55:	48 8b 42 18          	mov    rax,QWORD PTR [rdx+0x18]
void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx) {
    ab59:	48 63 d9             	movsxd rbx,ecx
  StkId base = ci->u.l.base;
    ab5c:	4c 8b 76 20          	mov    r14,QWORD PTR [rsi+0x20]
  LClosure *ncl = getcached(p, cl->upvals, base); /* cached closure */
    ab60:	4c 89 de             	mov    rsi,r11
  StkId ra = base + a;
    ab63:	48 c1 e3 04          	shl    rbx,0x4
  Proto *p = cl->p->p[Bx];
    ab67:	48 8b 40 40          	mov    rax,QWORD PTR [rax+0x40]
  LClosure *ncl = getcached(p, cl->upvals, base); /* cached closure */
    ab6b:	4c 89 f2             	mov    rdx,r14
  StkId ra = base + a;
    ab6e:	4c 01 f3             	add    rbx,r14
  Proto *p = cl->p->p[Bx];
    ab71:	4e 8b 24 c0          	mov    r12,QWORD PTR [rax+r8*8]
  LClosure *ncl = getcached(p, cl->upvals, base); /* cached closure */
    ab75:	4c 89 e7             	mov    rdi,r12
    ab78:	e8 83 54 ff ff       	call   0 <getcached>
  if (ncl == NULL) /* no match? */ {
    ab7d:	48 85 c0             	test   rax,rax
    ab80:	74 3e                	je     abc0 <raviV_op_closure+0x80>
    pushclosure(L, p, cl->upvals, base, ra); /* create a new one */
  }
  else {
    setclLvalue(L, ra, ncl); /* push cashed closure */
    ab82:	48 89 03             	mov    QWORD PTR [rbx],rax
    ab85:	b8 06 80 ff ff       	mov    eax,0xffff8006
    ab8a:	66 89 43 08          	mov    WORD PTR [rbx+0x8],ax
  }
  checkGC_protectbase(L, ra + 1);
    ab8e:	49 8b 45 18          	mov    rax,QWORD PTR [r13+0x18]
    ab92:	48 83 78 18 00       	cmp    QWORD PTR [rax+0x18],0x0
    ab97:	7e 18                	jle    abb1 <raviV_op_closure+0x71>
    ab99:	48 83 c3 10          	add    rbx,0x10
    ab9d:	4c 89 ef             	mov    rdi,r13
    aba0:	49 89 5d 10          	mov    QWORD PTR [r13+0x10],rbx
    aba4:	e8 00 00 00 00       	call   aba9 <raviV_op_closure+0x69>
    aba9:	48 8b 45 08          	mov    rax,QWORD PTR [rbp+0x8]
    abad:	49 89 45 10          	mov    QWORD PTR [r13+0x10],rax
}
    abb1:	5b                   	pop    rbx
    abb2:	5d                   	pop    rbp
    abb3:	41 5c                	pop    r12
    abb5:	41 5d                	pop    r13
    abb7:	41 5e                	pop    r14
    abb9:	c3                   	ret    
    abba:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    pushclosure(L, p, cl->upvals, base, ra); /* create a new one */
    abc0:	4c 8d 4b 08          	lea    r9,[rbx+0x8]
    abc4:	49 89 d8             	mov    r8,rbx
    abc7:	4c 89 f1             	mov    rcx,r14
    abca:	4c 89 da             	mov    rdx,r11
    abcd:	4c 89 e6             	mov    rsi,r12
    abd0:	4c 89 ef             	mov    rdi,r13
    abd3:	e8 e8 55 ff ff       	call   1c0 <pushclosure.isra.6>
    abd8:	eb b4                	jmp    ab8e <raviV_op_closure+0x4e>
    abda:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

000000000000abe0 <raviV_op_vararg>:

void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int b) {
    abe0:	41 55                	push   r13
    abe2:	41 54                	push   r12
    abe4:	55                   	push   rbp
    abe5:	53                   	push   rbx
    abe6:	48 83 ec 08          	sub    rsp,0x8
  StkId base = ci->u.l.base;
    abea:	48 8b 46 20          	mov    rax,QWORD PTR [rsi+0x20]
  StkId ra;
  int j;
  int n = cast_int(base - ci->func) - cl->p->numparams - 1;
    abee:	48 8b 52 18          	mov    rdx,QWORD PTR [rdx+0x18]
    abf2:	48 89 c3             	mov    rbx,rax
    abf5:	0f b6 52 0a          	movzx  edx,BYTE PTR [rdx+0xa]
    abf9:	48 2b 1e             	sub    rbx,QWORD PTR [rsi]
    abfc:	48 c1 fb 04          	sar    rbx,0x4
    ac00:	29 d3                	sub    ebx,edx
  if (n < 0) /* less arguments than parameters? */
    ac02:	ba 00 00 00 00       	mov    edx,0x0
    ac07:	83 eb 01             	sub    ebx,0x1
    ac0a:	0f 48 da             	cmovs  ebx,edx
    n = 0;   /* no vararg arguments */
  b = b - 1;
  if (b < 0) { /* B == 0? */
    ac0d:	48 63 d1             	movsxd rdx,ecx
    ac10:	48 c1 e2 04          	shl    rdx,0x4
    ac14:	49 89 d5             	mov    r13,rdx
    ac17:	41 83 e8 01          	sub    r8d,0x1
    ac1b:	0f 88 8f 00 00 00    	js     acb0 <raviV_op_vararg+0xd0>
    Protect_base(luaD_checkstack(L, n));
    ra = base + a; /* previous call may change the stack */
    L->top = ra + n;
  }
  else {
    ra = base + a;
    ac21:	41 39 d8             	cmp    r8d,ebx
    ac24:	41 89 d9             	mov    r9d,ebx
    ac27:	48 8d 14 10          	lea    rdx,[rax+rdx*1]
    ac2b:	45 0f 4e c8          	cmovle r9d,r8d
  }
  for (j = 0; j < b && j < n; j++) 
    ac2f:	45 85 c9             	test   r9d,r9d
    ac32:	74 37                	je     ac6b <raviV_op_vararg+0x8b>
    ac34:	48 63 db             	movsxd rbx,ebx
    ac37:	41 8d 79 ff          	lea    edi,[r9-0x1]
    ac3b:	48 c1 e3 04          	shl    rbx,0x4
    ac3f:	48 83 c7 01          	add    rdi,0x1
    ac43:	48 29 d8             	sub    rax,rbx
    ac46:	48 c1 e7 04          	shl    rdi,0x4
    ac4a:	48 89 c1             	mov    rcx,rax
    ac4d:	31 c0                	xor    eax,eax
    ac4f:	90                   	nop
    setobjs2s(L, ra + j, base - n + j);
    ac50:	48 8b 34 01          	mov    rsi,QWORD PTR [rcx+rax*1]
    ac54:	48 89 34 02          	mov    QWORD PTR [rdx+rax*1],rsi
    ac58:	0f b7 74 01 08       	movzx  esi,WORD PTR [rcx+rax*1+0x8]
    ac5d:	66 89 74 02 08       	mov    WORD PTR [rdx+rax*1+0x8],si
    ac62:	48 83 c0 10          	add    rax,0x10
  for (j = 0; j < b && j < n; j++) 
    ac66:	48 39 f8             	cmp    rax,rdi
    ac69:	75 e5                	jne    ac50 <raviV_op_vararg+0x70>
  for (; j < b; j++) /* complete required results with nil */
    ac6b:	45 39 c8             	cmp    r8d,r9d
    ac6e:	7e 2f                	jle    ac9f <raviV_op_vararg+0xbf>
    ac70:	41 8d 48 ff          	lea    ecx,[r8-0x1]
    ac74:	49 63 f1             	movsxd rsi,r9d
    ac77:	44 29 c9             	sub    ecx,r9d
    ac7a:	48 89 f0             	mov    rax,rsi
    ac7d:	48 01 f1             	add    rcx,rsi
    ac80:	48 c1 e0 04          	shl    rax,0x4
    ac84:	48 c1 e1 04          	shl    rcx,0x4
    ac88:	48 01 d0             	add    rax,rdx
    ac8b:	48 8d 54 0a 10       	lea    rdx,[rdx+rcx*1+0x10]
    setnilvalue(ra + j);
    ac90:	31 c9                	xor    ecx,ecx
    ac92:	48 83 c0 10          	add    rax,0x10
    ac96:	66 89 48 f8          	mov    WORD PTR [rax-0x8],cx
  for (; j < b; j++) /* complete required results with nil */
    ac9a:	48 39 d0             	cmp    rax,rdx
    ac9d:	75 f1                	jne    ac90 <raviV_op_vararg+0xb0>
}
    ac9f:	48 83 c4 08          	add    rsp,0x8
    aca3:	5b                   	pop    rbx
    aca4:	5d                   	pop    rbp
    aca5:	41 5c                	pop    r12
    aca7:	41 5d                	pop    r13
    aca9:	c3                   	ret    
    acaa:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    Protect_base(luaD_checkstack(L, n));
    acb0:	48 8b 57 30          	mov    rdx,QWORD PTR [rdi+0x30]
    acb4:	48 2b 57 10          	sub    rdx,QWORD PTR [rdi+0x10]
    acb8:	48 89 f5             	mov    rbp,rsi
    acbb:	48 63 f3             	movsxd rsi,ebx
    acbe:	48 c1 fa 04          	sar    rdx,0x4
    acc2:	49 89 fc             	mov    r12,rdi
    acc5:	48 39 f2             	cmp    rdx,rsi
    acc8:	7e 26                	jle    acf0 <raviV_op_vararg+0x110>
    L->top = ra + n;
    acca:	48 63 cb             	movsxd rcx,ebx
    ra = base + a; /* previous call may change the stack */
    accd:	4a 8d 14 28          	lea    rdx,[rax+r13*1]
    L->top = ra + n;
    acd1:	41 89 d9             	mov    r9d,ebx
    b = n;     /* get all var. arguments */
    acd4:	41 89 d8             	mov    r8d,ebx
    L->top = ra + n;
    acd7:	48 c1 e1 04          	shl    rcx,0x4
    acdb:	48 01 d1             	add    rcx,rdx
    acde:	49 89 4c 24 10       	mov    QWORD PTR [r12+0x10],rcx
    ace3:	e9 47 ff ff ff       	jmp    ac2f <raviV_op_vararg+0x4f>
    ace8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    acef:	00 
    Protect_base(luaD_checkstack(L, n));
    acf0:	89 de                	mov    esi,ebx
    acf2:	e8 00 00 00 00       	call   acf7 <raviV_op_vararg+0x117>
    acf7:	48 8b 45 20          	mov    rax,QWORD PTR [rbp+0x20]
    acfb:	eb cd                	jmp    acca <raviV_op_vararg+0xea>
    acfd:	0f 1f 00             	nop    DWORD PTR [rax]

000000000000ad00 <raviV_op_loadnil>:

// This is a cheat for a boring opcode
void raviV_op_loadnil(CallInfo *ci, int a, int b) {
  StkId base;
  base = ci->u.l.base;
  TValue *ra = base + a;
    ad00:	48 63 f6             	movsxd rsi,esi
    ad03:	48 8b 47 20          	mov    rax,QWORD PTR [rdi+0x20]
    ad07:	89 d2                	mov    edx,edx
    ad09:	48 c1 e6 04          	shl    rsi,0x4
    ad0d:	48 83 c2 01          	add    rdx,0x1
    ad11:	48 01 f0             	add    rax,rsi
    ad14:	48 c1 e2 04          	shl    rdx,0x4
    ad18:	48 01 c2             	add    rdx,rax
    ad1b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  do { setnilvalue(ra++); } while (b--);
    ad20:	48 83 c0 10          	add    rax,0x10
    ad24:	31 c9                	xor    ecx,ecx
    ad26:	66 89 48 f8          	mov    WORD PTR [rax-0x8],cx
    ad2a:	48 39 d0             	cmp    rax,rdx
    ad2d:	75 f1                	jne    ad20 <raviV_op_loadnil+0x20>
}
    ad2f:	c3                   	ret    

000000000000ad30 <raviV_op_setupvali>:

void raviV_op_setupvali(lua_State *L, LClosure *cl, TValue *ra, int b) {
    ad30:	55                   	push   rbp
    ad31:	48 89 fd             	mov    rbp,rdi
    ad34:	48 89 d7             	mov    rdi,rdx
    ad37:	53                   	push   rbx
    ad38:	48 89 f3             	mov    rbx,rsi
    ad3b:	48 83 ec 28          	sub    rsp,0x28
    ad3f:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    ad46:	00 00 
    ad48:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    ad4d:	31 c0                	xor    eax,eax
  lua_Integer ia;
  if (tointeger(ra, &ia)) {
    ad4f:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    ad54:	75 3a                	jne    ad90 <raviV_op_setupvali+0x60>
    ad56:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
    UpVal *uv = cl->upvals[b];
    ad59:	48 63 c9             	movsxd rcx,ecx
    setivalue(uv->v, ia);
    ad5c:	48 8b 44 cb 20       	mov    rax,QWORD PTR [rbx+rcx*8+0x20]
    ad61:	48 8b 00             	mov    rax,QWORD PTR [rax]
    ad64:	48 89 10             	mov    QWORD PTR [rax],rdx
    ad67:	ba 13 00 00 00       	mov    edx,0x13
    ad6c:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
    luaC_upvalbarrier(L, uv);
  }
  else
    luaG_runerror(
        L, "upvalue of integer type, cannot be set to non integer value");
}
    ad70:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    ad75:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    ad7c:	00 00 
    ad7e:	75 2f                	jne    adaf <raviV_op_setupvali+0x7f>
    ad80:	48 83 c4 28          	add    rsp,0x28
    ad84:	5b                   	pop    rbx
    ad85:	5d                   	pop    rbp
    ad86:	c3                   	ret    
    ad87:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
    ad8e:	00 00 
  if (tointeger(ra, &ia)) {
    ad90:	31 d2                	xor    edx,edx
    ad92:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    ad97:	89 4c 24 0c          	mov    DWORD PTR [rsp+0xc],ecx
    ad9b:	e8 00 00 00 00       	call   ada0 <raviV_op_setupvali+0x70>
    ada0:	85 c0                	test   eax,eax
    ada2:	74 10                	je     adb4 <raviV_op_setupvali+0x84>
    ada4:	48 8b 54 24 10       	mov    rdx,QWORD PTR [rsp+0x10]
    ada9:	8b 4c 24 0c          	mov    ecx,DWORD PTR [rsp+0xc]
    adad:	eb aa                	jmp    ad59 <raviV_op_setupvali+0x29>
}
    adaf:	e8 00 00 00 00       	call   adb4 <raviV_op_setupvali+0x84>
    luaG_runerror(
    adb4:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # adbb <raviV_op_setupvali+0x8b>
    adbb:	48 89 ef             	mov    rdi,rbp
    adbe:	31 c0                	xor    eax,eax
    adc0:	e8 00 00 00 00       	call   adc5 <raviV_op_setupvali+0x95>
    adc5:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    adcc:	00 00 00 00 

000000000000add0 <raviV_op_setupvalf>:

void raviV_op_setupvalf(lua_State *L, LClosure *cl, TValue *ra, int b) {
    add0:	55                   	push   rbp
    add1:	53                   	push   rbx
    add2:	48 89 f3             	mov    rbx,rsi
    add5:	48 83 ec 28          	sub    rsp,0x28
    add9:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    ade0:	00 00 
    ade2:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    ade7:	31 c0                	xor    eax,eax
  lua_Number na;
  if (tonumber(ra, &na)) {
    ade9:	66 83 7a 08 03       	cmp    WORD PTR [rdx+0x8],0x3
    adee:	75 38                	jne    ae28 <raviV_op_setupvalf+0x58>
    adf0:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    UpVal *uv = cl->upvals[b];
    adf4:	48 63 c9             	movsxd rcx,ecx
    setfltvalue(uv->v, na);
    adf7:	ba 03 00 00 00       	mov    edx,0x3
    adfc:	48 8b 44 cb 20       	mov    rax,QWORD PTR [rbx+rcx*8+0x20]
    ae01:	48 8b 00             	mov    rax,QWORD PTR [rax]
    ae04:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
    ae08:	f2 0f 11 00          	movsd  QWORD PTR [rax],xmm0
    luaC_upvalbarrier(L, uv);
  }
  else
    luaG_runerror(L,
                  "upvalue of number type, cannot be set to non number value");
}
    ae0c:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    ae11:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    ae18:	00 00 
    ae1a:	75 30                	jne    ae4c <raviV_op_setupvalf+0x7c>
    ae1c:	48 83 c4 28          	add    rsp,0x28
    ae20:	5b                   	pop    rbx
    ae21:	5d                   	pop    rbp
    ae22:	c3                   	ret    
    ae23:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    ae28:	48 89 fd             	mov    rbp,rdi
  if (tonumber(ra, &na)) {
    ae2b:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    ae30:	48 89 d7             	mov    rdi,rdx
    ae33:	89 4c 24 0c          	mov    DWORD PTR [rsp+0xc],ecx
    ae37:	e8 00 00 00 00       	call   ae3c <raviV_op_setupvalf+0x6c>
    ae3c:	85 c0                	test   eax,eax
    ae3e:	74 11                	je     ae51 <raviV_op_setupvalf+0x81>
    ae40:	f2 0f 10 44 24 10    	movsd  xmm0,QWORD PTR [rsp+0x10]
    ae46:	8b 4c 24 0c          	mov    ecx,DWORD PTR [rsp+0xc]
    ae4a:	eb a8                	jmp    adf4 <raviV_op_setupvalf+0x24>
}
    ae4c:	e8 00 00 00 00       	call   ae51 <raviV_op_setupvalf+0x81>
    luaG_runerror(L,
    ae51:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # ae58 <raviV_op_setupvalf+0x88>
    ae58:	48 89 ef             	mov    rdi,rbp
    ae5b:	31 c0                	xor    eax,eax
    ae5d:	e8 00 00 00 00       	call   ae62 <raviV_op_setupvalf+0x92>
    ae62:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    ae69:	00 00 00 00 
    ae6d:	0f 1f 00             	nop    DWORD PTR [rax]

000000000000ae70 <raviV_op_setupvalai>:

void raviV_op_setupvalai(lua_State *L, LClosure *cl, TValue *ra, int b) {
  if (!ttisiarray(ra))
    ae70:	66 81 7a 08 15 80    	cmp    WORD PTR [rdx+0x8],0x8015
    ae76:	75 2d                	jne    aea5 <raviV_op_setupvalai+0x35>
    luaG_runerror(
        L, "upvalue of integer[] type, cannot be set to non integer[] value");
  UpVal *uv = cl->upvals[b];
    ae78:	48 63 c9             	movsxd rcx,ecx
  setobj(L, uv->v, ra);
    ae7b:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
  UpVal *uv = cl->upvals[b];
    ae7e:	48 8b 74 ce 20       	mov    rsi,QWORD PTR [rsi+rcx*8+0x20]
  setobj(L, uv->v, ra);
    ae83:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    ae86:	48 89 10             	mov    QWORD PTR [rax],rdx
    ae89:	ba 15 80 ff ff       	mov    edx,0xffff8015
    ae8e:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
  luaC_upvalbarrier(L, uv);
    ae92:	48 8d 56 10          	lea    rdx,[rsi+0x10]
    ae96:	48 39 d0             	cmp    rax,rdx
    ae99:	74 05                	je     aea0 <raviV_op_setupvalai+0x30>
    ae9b:	c3                   	ret    
    ae9c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    aea0:	e9 00 00 00 00       	jmp    aea5 <raviV_op_setupvalai+0x35>
void raviV_op_setupvalai(lua_State *L, LClosure *cl, TValue *ra, int b) {
    aea5:	51                   	push   rcx
    luaG_runerror(
    aea6:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # aead <raviV_op_setupvalai+0x3d>
    aead:	31 c0                	xor    eax,eax
    aeaf:	e8 00 00 00 00       	call   aeb4 <raviV_op_setupvalai+0x44>
    aeb4:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    aebb:	00 00 00 00 
    aebf:	90                   	nop

000000000000aec0 <raviV_op_setupvalaf>:
}

void raviV_op_setupvalaf(lua_State *L, LClosure *cl, TValue *ra, int b) {
  if (!ttisfarray(ra))
    aec0:	66 81 7a 08 25 80    	cmp    WORD PTR [rdx+0x8],0x8025
    aec6:	75 2d                	jne    aef5 <raviV_op_setupvalaf+0x35>
    luaG_runerror(
        L, "upvalue of number[] type, cannot be set to non number[] value");
  UpVal *uv = cl->upvals[b];
    aec8:	48 63 c9             	movsxd rcx,ecx
  setobj(L, uv->v, ra);
    aecb:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
  UpVal *uv = cl->upvals[b];
    aece:	48 8b 74 ce 20       	mov    rsi,QWORD PTR [rsi+rcx*8+0x20]
  setobj(L, uv->v, ra);
    aed3:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    aed6:	48 89 10             	mov    QWORD PTR [rax],rdx
    aed9:	ba 25 80 ff ff       	mov    edx,0xffff8025
    aede:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
  luaC_upvalbarrier(L, uv);
    aee2:	48 8d 56 10          	lea    rdx,[rsi+0x10]
    aee6:	48 39 d0             	cmp    rax,rdx
    aee9:	74 05                	je     aef0 <raviV_op_setupvalaf+0x30>
    aeeb:	c3                   	ret    
    aeec:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    aef0:	e9 00 00 00 00       	jmp    aef5 <raviV_op_setupvalaf+0x35>
void raviV_op_setupvalaf(lua_State *L, LClosure *cl, TValue *ra, int b) {
    aef5:	51                   	push   rcx
    luaG_runerror(
    aef6:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # aefd <raviV_op_setupvalaf+0x3d>
    aefd:	31 c0                	xor    eax,eax
    aeff:	e8 00 00 00 00       	call   af04 <raviV_op_setupvalaf+0x44>
    af04:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    af0b:	00 00 00 00 
    af0f:	90                   	nop

000000000000af10 <raviV_op_setupvalt>:
}

void raviV_op_setupvalt(lua_State *L, LClosure *cl, TValue *ra, int b) {
  if (!ttisLtable(ra))
    af10:	66 81 7a 08 05 80    	cmp    WORD PTR [rdx+0x8],0x8005
    af16:	75 2d                	jne    af45 <raviV_op_setupvalt+0x35>
    luaG_runerror(L, "upvalue of table type, cannot be set to non table value");
  UpVal *uv = cl->upvals[b];
    af18:	48 63 c9             	movsxd rcx,ecx
  setobj(L, uv->v, ra);
    af1b:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
  UpVal *uv = cl->upvals[b];
    af1e:	48 8b 74 ce 20       	mov    rsi,QWORD PTR [rsi+rcx*8+0x20]
  setobj(L, uv->v, ra);
    af23:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    af26:	48 89 10             	mov    QWORD PTR [rax],rdx
    af29:	ba 05 80 ff ff       	mov    edx,0xffff8005
    af2e:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
  luaC_upvalbarrier(L, uv);
    af32:	48 8d 56 10          	lea    rdx,[rsi+0x10]
    af36:	48 39 d0             	cmp    rax,rdx
    af39:	74 05                	je     af40 <raviV_op_setupvalt+0x30>
    af3b:	c3                   	ret    
    af3c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    af40:	e9 00 00 00 00       	jmp    af45 <raviV_op_setupvalt+0x35>
void raviV_op_setupvalt(lua_State *L, LClosure *cl, TValue *ra, int b) {
    af45:	51                   	push   rcx
    luaG_runerror(L, "upvalue of table type, cannot be set to non table value");
    af46:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # af4d <raviV_op_setupvalt+0x3d>
    af4d:	31 c0                	xor    eax,eax
    af4f:	e8 00 00 00 00       	call   af54 <raviV_op_setupvalt+0x44>
    af54:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    af5b:	00 00 00 00 
    af5f:	90                   	nop

000000000000af60 <raviV_op_setupval>:
}

void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b) {
  UpVal *uv = cl->upvals[b];
    af60:	48 63 c9             	movsxd rcx,ecx
    af63:	48 8b 74 ce 20       	mov    rsi,QWORD PTR [rsi+rcx*8+0x20]
  setobj(L, uv->v, ra);
    af68:	48 8b 0a             	mov    rcx,QWORD PTR [rdx]
    af6b:	0f b7 52 08          	movzx  edx,WORD PTR [rdx+0x8]
    af6f:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    af72:	48 89 08             	mov    QWORD PTR [rax],rcx
    af75:	66 89 50 08          	mov    WORD PTR [rax+0x8],dx
  luaC_upvalbarrier(L, uv);
    af79:	66 85 d2             	test   dx,dx
    af7c:	78 02                	js     af80 <raviV_op_setupval+0x20>
}
    af7e:	c3                   	ret    
    af7f:	90                   	nop
  luaC_upvalbarrier(L, uv);
    af80:	48 8d 56 10          	lea    rdx,[rsi+0x10]
    af84:	48 39 d0             	cmp    rax,rdx
    af87:	75 f5                	jne    af7e <raviV_op_setupval+0x1e>
    af89:	e9 00 00 00 00       	jmp    af8e <raviV_op_setupval+0x2e>
    af8e:	66 90                	xchg   ax,ax

000000000000af90 <raviV_op_add>:

void raviV_op_add(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    af90:	41 55                	push   r13
    af92:	49 89 fd             	mov    r13,rdi
    af95:	41 54                	push   r12
    af97:	49 89 f4             	mov    r12,rsi
    af9a:	55                   	push   rbp
    af9b:	48 89 d5             	mov    rbp,rdx
    af9e:	53                   	push   rbx
    af9f:	48 89 cb             	mov    rbx,rcx
    afa2:	48 83 ec 28          	sub    rsp,0x28
    afa6:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    afad:	00 00 
    afaf:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    afb4:	31 c0                	xor    eax,eax
  lua_Number nb;
  lua_Number nc;
  if (ttisinteger(rb) && ttisinteger(rc)) {
    afb6:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    afba:	66 83 f8 13          	cmp    ax,0x13
    afbe:	74 58                	je     b018 <raviV_op_add+0x88>
    lua_Integer ib = ivalue(rb);
    lua_Integer ic = ivalue(rc);
    setivalue(ra, intop(+, ib, ic));
  }
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    afc0:	66 83 f8 03          	cmp    ax,0x3
    afc4:	75 59                	jne    b01f <raviV_op_add+0x8f>
    afc6:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    afca:	f2 0f 11 44 24 08    	movsd  QWORD PTR [rsp+0x8],xmm0
    afd0:	66 83 7b 08 03       	cmp    WORD PTR [rbx+0x8],0x3
    afd5:	0f 85 8d 00 00 00    	jne    b068 <raviV_op_add+0xd8>
    afdb:	f2 0f 10 03          	movsd  xmm0,QWORD PTR [rbx]
    setfltvalue(ra, luai_numadd(L, nb, nc));
    afdf:	f2 0f 58 44 24 08    	addsd  xmm0,QWORD PTR [rsp+0x8]
    afe5:	b8 03 00 00 00       	mov    eax,0x3
    afea:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
    aff0:	f2 41 0f 11 04 24    	movsd  QWORD PTR [r12],xmm0
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_ADD);
  }
}
    aff6:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    affb:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b002:	00 00 
    b004:	75 7e                	jne    b084 <raviV_op_add+0xf4>
    b006:	48 83 c4 28          	add    rsp,0x28
    b00a:	5b                   	pop    rbx
    b00b:	5d                   	pop    rbp
    b00c:	41 5c                	pop    r12
    b00e:	41 5d                	pop    r13
    b010:	c3                   	ret    
    b011:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  if (ttisinteger(rb) && ttisinteger(rc)) {
    b018:	66 83 79 08 13       	cmp    WORD PTR [rcx+0x8],0x13
    b01d:	74 31                	je     b050 <raviV_op_add+0xc0>
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b01f:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b024:	48 89 ef             	mov    rdi,rbp
    b027:	e8 00 00 00 00       	call   b02c <raviV_op_add+0x9c>
    b02c:	85 c0                	test   eax,eax
    b02e:	75 a0                	jne    afd0 <raviV_op_add+0x40>
    luaT_trybinTM(L, rb, rc, ra, TM_ADD);
    b030:	41 b8 06 00 00 00    	mov    r8d,0x6
    b036:	4c 89 e1             	mov    rcx,r12
    b039:	48 89 da             	mov    rdx,rbx
    b03c:	48 89 ee             	mov    rsi,rbp
    b03f:	4c 89 ef             	mov    rdi,r13
    b042:	e8 00 00 00 00       	call   b047 <raviV_op_add+0xb7>
}
    b047:	eb ad                	jmp    aff6 <raviV_op_add+0x66>
    b049:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    setivalue(ra, intop(+, ib, ic));
    b050:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    b053:	48 03 02             	add    rax,QWORD PTR [rdx]
    b056:	ba 13 00 00 00       	mov    edx,0x13
    b05b:	48 89 06             	mov    QWORD PTR [rsi],rax
    b05e:	66 89 56 08          	mov    WORD PTR [rsi+0x8],dx
  if (ttisinteger(rb) && ttisinteger(rc)) {
    b062:	eb 92                	jmp    aff6 <raviV_op_add+0x66>
    b064:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b068:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b06d:	48 89 df             	mov    rdi,rbx
    b070:	e8 00 00 00 00       	call   b075 <raviV_op_add+0xe5>
    b075:	85 c0                	test   eax,eax
    b077:	74 b7                	je     b030 <raviV_op_add+0xa0>
    b079:	f2 0f 10 44 24 10    	movsd  xmm0,QWORD PTR [rsp+0x10]
    b07f:	e9 5b ff ff ff       	jmp    afdf <raviV_op_add+0x4f>
}
    b084:	e8 00 00 00 00       	call   b089 <raviV_op_add+0xf9>
    b089:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

000000000000b090 <raviV_op_sub>:

void raviV_op_sub(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    b090:	41 55                	push   r13
    b092:	49 89 fd             	mov    r13,rdi
    b095:	41 54                	push   r12
    b097:	49 89 f4             	mov    r12,rsi
    b09a:	55                   	push   rbp
    b09b:	48 89 d5             	mov    rbp,rdx
    b09e:	53                   	push   rbx
    b09f:	48 89 cb             	mov    rbx,rcx
    b0a2:	48 83 ec 28          	sub    rsp,0x28
    b0a6:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b0ad:	00 00 
    b0af:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    b0b4:	31 c0                	xor    eax,eax
  lua_Number nb;
  lua_Number nc;
  if (ttisinteger(rb) && ttisinteger(rc)) {
    b0b6:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    b0ba:	66 83 f8 13          	cmp    ax,0x13
    b0be:	74 58                	je     b118 <raviV_op_sub+0x88>
    lua_Integer ib = ivalue(rb);
    lua_Integer ic = ivalue(rc);
    setivalue(ra, intop(-, ib, ic));
  }
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b0c0:	66 83 f8 03          	cmp    ax,0x3
    b0c4:	75 59                	jne    b11f <raviV_op_sub+0x8f>
    b0c6:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    b0ca:	f2 0f 11 44 24 08    	movsd  QWORD PTR [rsp+0x8],xmm0
    b0d0:	66 83 7b 08 03       	cmp    WORD PTR [rbx+0x8],0x3
    b0d5:	0f 85 8d 00 00 00    	jne    b168 <raviV_op_sub+0xd8>
    b0db:	f2 0f 10 0b          	movsd  xmm1,QWORD PTR [rbx]
    setfltvalue(ra, luai_numsub(L, nb, nc));
    b0df:	f2 0f 10 44 24 08    	movsd  xmm0,QWORD PTR [rsp+0x8]
    b0e5:	b8 03 00 00 00       	mov    eax,0x3
    b0ea:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
    b0f0:	f2 0f 5c c1          	subsd  xmm0,xmm1
    b0f4:	f2 41 0f 11 04 24    	movsd  QWORD PTR [r12],xmm0
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_SUB);
  }
}
    b0fa:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    b0ff:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b106:	00 00 
    b108:	75 7a                	jne    b184 <raviV_op_sub+0xf4>
    b10a:	48 83 c4 28          	add    rsp,0x28
    b10e:	5b                   	pop    rbx
    b10f:	5d                   	pop    rbp
    b110:	41 5c                	pop    r12
    b112:	41 5d                	pop    r13
    b114:	c3                   	ret    
    b115:	0f 1f 00             	nop    DWORD PTR [rax]
  if (ttisinteger(rb) && ttisinteger(rc)) {
    b118:	66 83 79 08 13       	cmp    WORD PTR [rcx+0x8],0x13
    b11d:	74 31                	je     b150 <raviV_op_sub+0xc0>
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b11f:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b124:	48 89 ef             	mov    rdi,rbp
    b127:	e8 00 00 00 00       	call   b12c <raviV_op_sub+0x9c>
    b12c:	85 c0                	test   eax,eax
    b12e:	75 a0                	jne    b0d0 <raviV_op_sub+0x40>
    luaT_trybinTM(L, rb, rc, ra, TM_SUB);
    b130:	41 b8 07 00 00 00    	mov    r8d,0x7
    b136:	4c 89 e1             	mov    rcx,r12
    b139:	48 89 da             	mov    rdx,rbx
    b13c:	48 89 ee             	mov    rsi,rbp
    b13f:	4c 89 ef             	mov    rdi,r13
    b142:	e8 00 00 00 00       	call   b147 <raviV_op_sub+0xb7>
}
    b147:	eb b1                	jmp    b0fa <raviV_op_sub+0x6a>
    b149:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    setivalue(ra, intop(-, ib, ic));
    b150:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    b153:	ba 13 00 00 00       	mov    edx,0x13
    b158:	48 2b 01             	sub    rax,QWORD PTR [rcx]
    b15b:	48 89 06             	mov    QWORD PTR [rsi],rax
    b15e:	66 89 56 08          	mov    WORD PTR [rsi+0x8],dx
  if (ttisinteger(rb) && ttisinteger(rc)) {
    b162:	eb 96                	jmp    b0fa <raviV_op_sub+0x6a>
    b164:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b168:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b16d:	48 89 df             	mov    rdi,rbx
    b170:	e8 00 00 00 00       	call   b175 <raviV_op_sub+0xe5>
    b175:	85 c0                	test   eax,eax
    b177:	74 b7                	je     b130 <raviV_op_sub+0xa0>
    b179:	f2 0f 10 4c 24 10    	movsd  xmm1,QWORD PTR [rsp+0x10]
    b17f:	e9 5b ff ff ff       	jmp    b0df <raviV_op_sub+0x4f>
}
    b184:	e8 00 00 00 00       	call   b189 <raviV_op_sub+0xf9>
    b189:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

000000000000b190 <raviV_op_mul>:

void raviV_op_mul(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    b190:	41 55                	push   r13
    b192:	49 89 fd             	mov    r13,rdi
    b195:	41 54                	push   r12
    b197:	49 89 f4             	mov    r12,rsi
    b19a:	55                   	push   rbp
    b19b:	48 89 d5             	mov    rbp,rdx
    b19e:	53                   	push   rbx
    b19f:	48 89 cb             	mov    rbx,rcx
    b1a2:	48 83 ec 28          	sub    rsp,0x28
    b1a6:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b1ad:	00 00 
    b1af:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    b1b4:	31 c0                	xor    eax,eax
  lua_Number nb;
  lua_Number nc;
  if (ttisinteger(rb) && ttisinteger(rc)) {
    b1b6:	0f b7 42 08          	movzx  eax,WORD PTR [rdx+0x8]
    b1ba:	66 83 f8 13          	cmp    ax,0x13
    b1be:	74 58                	je     b218 <raviV_op_mul+0x88>
    lua_Integer ib = ivalue(rb);
    lua_Integer ic = ivalue(rc);
    setivalue(ra, intop(*, ib, ic));
  }
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b1c0:	66 83 f8 03          	cmp    ax,0x3
    b1c4:	75 59                	jne    b21f <raviV_op_mul+0x8f>
    b1c6:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    b1ca:	f2 0f 11 44 24 08    	movsd  QWORD PTR [rsp+0x8],xmm0
    b1d0:	66 83 7b 08 03       	cmp    WORD PTR [rbx+0x8],0x3
    b1d5:	0f 85 8d 00 00 00    	jne    b268 <raviV_op_mul+0xd8>
    b1db:	f2 0f 10 03          	movsd  xmm0,QWORD PTR [rbx]
    setfltvalue(ra, luai_nummul(L, nb, nc));
    b1df:	f2 0f 59 44 24 08    	mulsd  xmm0,QWORD PTR [rsp+0x8]
    b1e5:	b8 03 00 00 00       	mov    eax,0x3
    b1ea:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
    b1f0:	f2 41 0f 11 04 24    	movsd  QWORD PTR [r12],xmm0
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_MUL);
  }
}
    b1f6:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    b1fb:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b202:	00 00 
    b204:	75 7e                	jne    b284 <raviV_op_mul+0xf4>
    b206:	48 83 c4 28          	add    rsp,0x28
    b20a:	5b                   	pop    rbx
    b20b:	5d                   	pop    rbp
    b20c:	41 5c                	pop    r12
    b20e:	41 5d                	pop    r13
    b210:	c3                   	ret    
    b211:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  if (ttisinteger(rb) && ttisinteger(rc)) {
    b218:	66 83 79 08 13       	cmp    WORD PTR [rcx+0x8],0x13
    b21d:	74 31                	je     b250 <raviV_op_mul+0xc0>
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b21f:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b224:	48 89 ef             	mov    rdi,rbp
    b227:	e8 00 00 00 00       	call   b22c <raviV_op_mul+0x9c>
    b22c:	85 c0                	test   eax,eax
    b22e:	75 a0                	jne    b1d0 <raviV_op_mul+0x40>
    luaT_trybinTM(L, rb, rc, ra, TM_MUL);
    b230:	41 b8 08 00 00 00    	mov    r8d,0x8
    b236:	4c 89 e1             	mov    rcx,r12
    b239:	48 89 da             	mov    rdx,rbx
    b23c:	48 89 ee             	mov    rsi,rbp
    b23f:	4c 89 ef             	mov    rdi,r13
    b242:	e8 00 00 00 00       	call   b247 <raviV_op_mul+0xb7>
}
    b247:	eb ad                	jmp    b1f6 <raviV_op_mul+0x66>
    b249:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    setivalue(ra, intop(*, ib, ic));
    b250:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    b253:	48 0f af 01          	imul   rax,QWORD PTR [rcx]
    b257:	ba 13 00 00 00       	mov    edx,0x13
    b25c:	66 89 56 08          	mov    WORD PTR [rsi+0x8],dx
    b260:	48 89 06             	mov    QWORD PTR [rsi],rax
  if (ttisinteger(rb) && ttisinteger(rc)) {
    b263:	eb 91                	jmp    b1f6 <raviV_op_mul+0x66>
    b265:	0f 1f 00             	nop    DWORD PTR [rax]
  else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b268:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b26d:	48 89 df             	mov    rdi,rbx
    b270:	e8 00 00 00 00       	call   b275 <raviV_op_mul+0xe5>
    b275:	85 c0                	test   eax,eax
    b277:	74 b7                	je     b230 <raviV_op_mul+0xa0>
    b279:	f2 0f 10 44 24 10    	movsd  xmm0,QWORD PTR [rsp+0x10]
    b27f:	e9 5b ff ff ff       	jmp    b1df <raviV_op_mul+0x4f>
}
    b284:	e8 00 00 00 00       	call   b289 <raviV_op_mul+0xf9>
    b289:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

000000000000b290 <raviV_op_div>:

void raviV_op_div(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    b290:	41 55                	push   r13
    b292:	49 89 fd             	mov    r13,rdi
    b295:	41 54                	push   r12
    b297:	49 89 f4             	mov    r12,rsi
    b29a:	55                   	push   rbp
    b29b:	48 89 cd             	mov    rbp,rcx
    b29e:	53                   	push   rbx
    b29f:	48 89 d3             	mov    rbx,rdx
    b2a2:	48 83 ec 28          	sub    rsp,0x28
    b2a6:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b2ad:	00 00 
    b2af:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    b2b4:	31 c0                	xor    eax,eax
  lua_Number nb;
  lua_Number nc;
  if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b2b6:	66 83 7a 08 03       	cmp    WORD PTR [rdx+0x8],0x3
    b2bb:	75 53                	jne    b310 <raviV_op_div+0x80>
    b2bd:	f2 0f 10 02          	movsd  xmm0,QWORD PTR [rdx]
    b2c1:	f2 0f 11 44 24 08    	movsd  QWORD PTR [rsp+0x8],xmm0
    b2c7:	66 83 7d 08 03       	cmp    WORD PTR [rbp+0x8],0x3
    b2cc:	75 72                	jne    b340 <raviV_op_div+0xb0>
    b2ce:	f2 0f 10 4d 00       	movsd  xmm1,QWORD PTR [rbp+0x0]
    setfltvalue(ra, luai_numdiv(L, nb, nc));
    b2d3:	f2 0f 10 44 24 08    	movsd  xmm0,QWORD PTR [rsp+0x8]
    b2d9:	b8 03 00 00 00       	mov    eax,0x3
    b2de:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
    b2e4:	f2 0f 5e c1          	divsd  xmm0,xmm1
    b2e8:	f2 41 0f 11 04 24    	movsd  QWORD PTR [r12],xmm0
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_DIV);
  }
}
    b2ee:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    b2f3:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b2fa:	00 00 
    b2fc:	75 5e                	jne    b35c <raviV_op_div+0xcc>
    b2fe:	48 83 c4 28          	add    rsp,0x28
    b302:	5b                   	pop    rbx
    b303:	5d                   	pop    rbp
    b304:	41 5c                	pop    r12
    b306:	41 5d                	pop    r13
    b308:	c3                   	ret    
    b309:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b310:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b315:	48 89 d7             	mov    rdi,rdx
    b318:	e8 00 00 00 00       	call   b31d <raviV_op_div+0x8d>
    b31d:	85 c0                	test   eax,eax
    b31f:	75 a6                	jne    b2c7 <raviV_op_div+0x37>
    luaT_trybinTM(L, rb, rc, ra, TM_DIV);
    b321:	41 b8 0b 00 00 00    	mov    r8d,0xb
    b327:	4c 89 e1             	mov    rcx,r12
    b32a:	48 89 ea             	mov    rdx,rbp
    b32d:	48 89 de             	mov    rsi,rbx
    b330:	4c 89 ef             	mov    rdi,r13
    b333:	e8 00 00 00 00       	call   b338 <raviV_op_div+0xa8>
}
    b338:	eb b4                	jmp    b2ee <raviV_op_div+0x5e>
    b33a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
    b340:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b345:	48 89 ef             	mov    rdi,rbp
    b348:	e8 00 00 00 00       	call   b34d <raviV_op_div+0xbd>
    b34d:	85 c0                	test   eax,eax
    b34f:	74 d0                	je     b321 <raviV_op_div+0x91>
    b351:	f2 0f 10 4c 24 10    	movsd  xmm1,QWORD PTR [rsp+0x10]
    b357:	e9 77 ff ff ff       	jmp    b2d3 <raviV_op_div+0x43>
}
    b35c:	e8 00 00 00 00       	call   b361 <raviV_op_div+0xd1>
    b361:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    b368:	00 00 00 00 
    b36c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

000000000000b370 <raviV_op_shl>:

void raviV_op_shl(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    b370:	41 55                	push   r13
    b372:	49 89 fd             	mov    r13,rdi
    b375:	41 54                	push   r12
    b377:	49 89 f4             	mov    r12,rsi
    b37a:	55                   	push   rbp
    b37b:	48 89 cd             	mov    rbp,rcx
    b37e:	53                   	push   rbx
    b37f:	48 89 d3             	mov    rbx,rdx
    b382:	48 83 ec 28          	sub    rsp,0x28
    b386:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b38d:	00 00 
    b38f:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    b394:	31 c0                	xor    eax,eax
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b396:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    b39b:	75 7b                	jne    b418 <raviV_op_shl+0xa8>
    b39d:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    b3a0:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
    b3a5:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    b3aa:	0f 85 a0 00 00 00    	jne    b450 <raviV_op_shl+0xe0>
    b3b0:	48 8b 4d 00          	mov    rcx,QWORD PTR [rbp+0x0]
    setivalue(ra, luaV_shiftl(ib, ic));
    b3b4:	48 8b 54 24 08       	mov    rdx,QWORD PTR [rsp+0x8]
  if (y < 0) {  /* shift right? */
    b3b9:	48 85 c9             	test   rcx,rcx
    b3bc:	78 42                	js     b400 <raviV_op_shl+0x90>
    else return intop(<<, x, y);
    b3be:	48 d3 e2             	shl    rdx,cl
    b3c1:	b8 00 00 00 00       	mov    eax,0x0
    b3c6:	48 83 f9 40          	cmp    rcx,0x40
    b3ca:	48 0f 4c c2          	cmovl  rax,rdx
    setivalue(ra, luaV_shiftl(ib, ic));
    b3ce:	49 89 04 24          	mov    QWORD PTR [r12],rax
    b3d2:	b8 13 00 00 00       	mov    eax,0x13
    b3d7:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_SHL);
  }
}
    b3dd:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    b3e2:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b3e9:	00 00 
    b3eb:	0f 85 7c 00 00 00    	jne    b46d <raviV_op_shl+0xfd>
    b3f1:	48 83 c4 28          	add    rsp,0x28
    b3f5:	5b                   	pop    rbx
    b3f6:	5d                   	pop    rbp
    b3f7:	41 5c                	pop    r12
    b3f9:	41 5d                	pop    r13
    b3fb:	c3                   	ret    
    b3fc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    if (y <= -NBITS) return 0;
    b400:	31 c0                	xor    eax,eax
    b402:	48 83 f9 c1          	cmp    rcx,0xffffffffffffffc1
    b406:	7c c6                	jl     b3ce <raviV_op_shl+0x5e>
    else return intop(>>, x, -y);
    b408:	f7 d9                	neg    ecx
    b40a:	48 89 d0             	mov    rax,rdx
    b40d:	48 d3 e8             	shr    rax,cl
    b410:	eb bc                	jmp    b3ce <raviV_op_shl+0x5e>
    b412:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b418:	31 d2                	xor    edx,edx
    b41a:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b41f:	48 89 df             	mov    rdi,rbx
    b422:	e8 00 00 00 00       	call   b427 <raviV_op_shl+0xb7>
    b427:	85 c0                	test   eax,eax
    b429:	0f 85 76 ff ff ff    	jne    b3a5 <raviV_op_shl+0x35>
    luaT_trybinTM(L, rb, rc, ra, TM_SHL);
    b42f:	41 b8 10 00 00 00    	mov    r8d,0x10
    b435:	4c 89 e1             	mov    rcx,r12
    b438:	48 89 ea             	mov    rdx,rbp
    b43b:	48 89 de             	mov    rsi,rbx
    b43e:	4c 89 ef             	mov    rdi,r13
    b441:	e8 00 00 00 00       	call   b446 <raviV_op_shl+0xd6>
}
    b446:	eb 95                	jmp    b3dd <raviV_op_shl+0x6d>
    b448:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    b44f:	00 
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b450:	31 d2                	xor    edx,edx
    b452:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b457:	48 89 ef             	mov    rdi,rbp
    b45a:	e8 00 00 00 00       	call   b45f <raviV_op_shl+0xef>
    b45f:	85 c0                	test   eax,eax
    b461:	74 cc                	je     b42f <raviV_op_shl+0xbf>
    b463:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    b468:	e9 47 ff ff ff       	jmp    b3b4 <raviV_op_shl+0x44>
}
    b46d:	e8 00 00 00 00       	call   b472 <raviV_op_shl+0x102>
    b472:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    b479:	00 00 00 00 
    b47d:	0f 1f 00             	nop    DWORD PTR [rax]

000000000000b480 <raviV_op_shr>:

void raviV_op_shr(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    b480:	41 55                	push   r13
    b482:	49 89 fd             	mov    r13,rdi
    b485:	41 54                	push   r12
    b487:	49 89 f4             	mov    r12,rsi
    b48a:	55                   	push   rbp
    b48b:	48 89 cd             	mov    rbp,rcx
    b48e:	53                   	push   rbx
    b48f:	48 89 d3             	mov    rbx,rdx
    b492:	48 83 ec 28          	sub    rsp,0x28
    b496:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b49d:	00 00 
    b49f:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    b4a4:	31 c0                	xor    eax,eax
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b4a6:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    b4ab:	0f 85 87 00 00 00    	jne    b538 <raviV_op_shr+0xb8>
    b4b1:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    b4b4:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
    b4b9:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    b4be:	0f 85 ac 00 00 00    	jne    b570 <raviV_op_shr+0xf0>
    b4c4:	48 8b 4d 00          	mov    rcx,QWORD PTR [rbp+0x0]
    setivalue(ra, luaV_shiftl(ib, -ic));
    b4c8:	48 8b 54 24 08       	mov    rdx,QWORD PTR [rsp+0x8]
  if (y < 0) {  /* shift right? */
    b4cd:	48 85 c9             	test   rcx,rcx
    b4d0:	7f 4e                	jg     b520 <raviV_op_shr+0xa0>
    if (y <= -NBITS) return 0;
    b4d2:	31 c0                	xor    eax,eax
    if (y >= NBITS) return 0;
    b4d4:	48 83 f9 c1          	cmp    rcx,0xffffffffffffffc1
    b4d8:	7d 36                	jge    b510 <raviV_op_shr+0x90>
    setivalue(ra, luaV_shiftl(ib, -ic));
    b4da:	49 89 04 24          	mov    QWORD PTR [r12],rax
    b4de:	b8 13 00 00 00       	mov    eax,0x13
    b4e3:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_SHR);
  }
}
    b4e9:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    b4ee:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b4f5:	00 00 
    b4f7:	0f 85 90 00 00 00    	jne    b58d <raviV_op_shr+0x10d>
    b4fd:	48 83 c4 28          	add    rsp,0x28
    b501:	5b                   	pop    rbx
    b502:	5d                   	pop    rbp
    b503:	41 5c                	pop    r12
    b505:	41 5d                	pop    r13
    b507:	c3                   	ret    
    b508:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    b50f:	00 
    setivalue(ra, luaV_shiftl(ib, -ic));
    b510:	48 f7 d9             	neg    rcx
    else return intop(<<, x, y);
    b513:	48 89 d0             	mov    rax,rdx
    b516:	48 d3 e0             	shl    rax,cl
    b519:	eb bf                	jmp    b4da <raviV_op_shr+0x5a>
    b51b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    else return intop(>>, x, -y);
    b520:	48 d3 ea             	shr    rdx,cl
    b523:	b8 00 00 00 00       	mov    eax,0x0
    b528:	48 83 f9 40          	cmp    rcx,0x40
    b52c:	48 0f 4c c2          	cmovl  rax,rdx
    b530:	eb a8                	jmp    b4da <raviV_op_shr+0x5a>
    b532:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b538:	31 d2                	xor    edx,edx
    b53a:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b53f:	48 89 df             	mov    rdi,rbx
    b542:	e8 00 00 00 00       	call   b547 <raviV_op_shr+0xc7>
    b547:	85 c0                	test   eax,eax
    b549:	0f 85 6a ff ff ff    	jne    b4b9 <raviV_op_shr+0x39>
    luaT_trybinTM(L, rb, rc, ra, TM_SHR);
    b54f:	41 b8 11 00 00 00    	mov    r8d,0x11
    b555:	4c 89 e1             	mov    rcx,r12
    b558:	48 89 ea             	mov    rdx,rbp
    b55b:	48 89 de             	mov    rsi,rbx
    b55e:	4c 89 ef             	mov    rdi,r13
    b561:	e8 00 00 00 00       	call   b566 <raviV_op_shr+0xe6>
}
    b566:	eb 81                	jmp    b4e9 <raviV_op_shr+0x69>
    b568:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    b56f:	00 
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b570:	31 d2                	xor    edx,edx
    b572:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b577:	48 89 ef             	mov    rdi,rbp
    b57a:	e8 00 00 00 00       	call   b57f <raviV_op_shr+0xff>
    b57f:	85 c0                	test   eax,eax
    b581:	74 cc                	je     b54f <raviV_op_shr+0xcf>
    b583:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    b588:	e9 3b ff ff ff       	jmp    b4c8 <raviV_op_shr+0x48>
}
    b58d:	e8 00 00 00 00       	call   b592 <raviV_op_shr+0x112>
    b592:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
    b599:	00 00 00 00 
    b59d:	0f 1f 00             	nop    DWORD PTR [rax]

000000000000b5a0 <raviV_op_band>:

void raviV_op_band(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    b5a0:	41 55                	push   r13
    b5a2:	49 89 fd             	mov    r13,rdi
    b5a5:	41 54                	push   r12
    b5a7:	49 89 f4             	mov    r12,rsi
    b5aa:	55                   	push   rbp
    b5ab:	48 89 cd             	mov    rbp,rcx
    b5ae:	53                   	push   rbx
    b5af:	48 89 d3             	mov    rbx,rdx
    b5b2:	48 83 ec 28          	sub    rsp,0x28
    b5b6:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b5bd:	00 00 
    b5bf:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    b5c4:	31 c0                	xor    eax,eax
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b5c6:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    b5cb:	75 43                	jne    b610 <raviV_op_band+0x70>
    b5cd:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    b5d0:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
    b5d5:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    b5da:	75 64                	jne    b640 <raviV_op_band+0xa0>
    b5dc:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    setivalue(ra, intop(&, ib, ic));
    b5e0:	48 23 44 24 08       	and    rax,QWORD PTR [rsp+0x8]
    b5e5:	49 89 04 24          	mov    QWORD PTR [r12],rax
    b5e9:	b8 13 00 00 00       	mov    eax,0x13
    b5ee:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_BAND);
  }
}
    b5f4:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    b5f9:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b600:	00 00 
    b602:	75 56                	jne    b65a <raviV_op_band+0xba>
    b604:	48 83 c4 28          	add    rsp,0x28
    b608:	5b                   	pop    rbx
    b609:	5d                   	pop    rbp
    b60a:	41 5c                	pop    r12
    b60c:	41 5d                	pop    r13
    b60e:	c3                   	ret    
    b60f:	90                   	nop
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b610:	31 d2                	xor    edx,edx
    b612:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b617:	48 89 df             	mov    rdi,rbx
    b61a:	e8 00 00 00 00       	call   b61f <raviV_op_band+0x7f>
    b61f:	85 c0                	test   eax,eax
    b621:	75 b2                	jne    b5d5 <raviV_op_band+0x35>
    luaT_trybinTM(L, rb, rc, ra, TM_BAND);
    b623:	41 b8 0d 00 00 00    	mov    r8d,0xd
    b629:	4c 89 e1             	mov    rcx,r12
    b62c:	48 89 ea             	mov    rdx,rbp
    b62f:	48 89 de             	mov    rsi,rbx
    b632:	4c 89 ef             	mov    rdi,r13
    b635:	e8 00 00 00 00       	call   b63a <raviV_op_band+0x9a>
}
    b63a:	eb b8                	jmp    b5f4 <raviV_op_band+0x54>
    b63c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b640:	31 d2                	xor    edx,edx
    b642:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b647:	48 89 ef             	mov    rdi,rbp
    b64a:	e8 00 00 00 00       	call   b64f <raviV_op_band+0xaf>
    b64f:	85 c0                	test   eax,eax
    b651:	74 d0                	je     b623 <raviV_op_band+0x83>
    b653:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    b658:	eb 86                	jmp    b5e0 <raviV_op_band+0x40>
}
    b65a:	e8 00 00 00 00       	call   b65f <raviV_op_band+0xbf>
    b65f:	90                   	nop

000000000000b660 <raviV_op_bor>:

void raviV_op_bor(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    b660:	41 55                	push   r13
    b662:	49 89 fd             	mov    r13,rdi
    b665:	41 54                	push   r12
    b667:	49 89 f4             	mov    r12,rsi
    b66a:	55                   	push   rbp
    b66b:	48 89 cd             	mov    rbp,rcx
    b66e:	53                   	push   rbx
    b66f:	48 89 d3             	mov    rbx,rdx
    b672:	48 83 ec 28          	sub    rsp,0x28
    b676:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b67d:	00 00 
    b67f:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    b684:	31 c0                	xor    eax,eax
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b686:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    b68b:	75 43                	jne    b6d0 <raviV_op_bor+0x70>
    b68d:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    b690:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
    b695:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    b69a:	75 64                	jne    b700 <raviV_op_bor+0xa0>
    b69c:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    setivalue(ra, intop(|, ib, ic));
    b6a0:	48 0b 44 24 08       	or     rax,QWORD PTR [rsp+0x8]
    b6a5:	49 89 04 24          	mov    QWORD PTR [r12],rax
    b6a9:	b8 13 00 00 00       	mov    eax,0x13
    b6ae:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_BOR);
  }
}
    b6b4:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    b6b9:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b6c0:	00 00 
    b6c2:	75 56                	jne    b71a <raviV_op_bor+0xba>
    b6c4:	48 83 c4 28          	add    rsp,0x28
    b6c8:	5b                   	pop    rbx
    b6c9:	5d                   	pop    rbp
    b6ca:	41 5c                	pop    r12
    b6cc:	41 5d                	pop    r13
    b6ce:	c3                   	ret    
    b6cf:	90                   	nop
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b6d0:	31 d2                	xor    edx,edx
    b6d2:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b6d7:	48 89 df             	mov    rdi,rbx
    b6da:	e8 00 00 00 00       	call   b6df <raviV_op_bor+0x7f>
    b6df:	85 c0                	test   eax,eax
    b6e1:	75 b2                	jne    b695 <raviV_op_bor+0x35>
    luaT_trybinTM(L, rb, rc, ra, TM_BOR);
    b6e3:	41 b8 0e 00 00 00    	mov    r8d,0xe
    b6e9:	4c 89 e1             	mov    rcx,r12
    b6ec:	48 89 ea             	mov    rdx,rbp
    b6ef:	48 89 de             	mov    rsi,rbx
    b6f2:	4c 89 ef             	mov    rdi,r13
    b6f5:	e8 00 00 00 00       	call   b6fa <raviV_op_bor+0x9a>
}
    b6fa:	eb b8                	jmp    b6b4 <raviV_op_bor+0x54>
    b6fc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b700:	31 d2                	xor    edx,edx
    b702:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b707:	48 89 ef             	mov    rdi,rbp
    b70a:	e8 00 00 00 00       	call   b70f <raviV_op_bor+0xaf>
    b70f:	85 c0                	test   eax,eax
    b711:	74 d0                	je     b6e3 <raviV_op_bor+0x83>
    b713:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    b718:	eb 86                	jmp    b6a0 <raviV_op_bor+0x40>
}
    b71a:	e8 00 00 00 00       	call   b71f <raviV_op_bor+0xbf>
    b71f:	90                   	nop

000000000000b720 <raviV_op_bxor>:

void raviV_op_bxor(lua_State *L, TValue *ra, TValue *rb, TValue *rc) {
    b720:	41 55                	push   r13
    b722:	49 89 fd             	mov    r13,rdi
    b725:	41 54                	push   r12
    b727:	49 89 f4             	mov    r12,rsi
    b72a:	55                   	push   rbp
    b72b:	48 89 cd             	mov    rbp,rcx
    b72e:	53                   	push   rbx
    b72f:	48 89 d3             	mov    rbx,rdx
    b732:	48 83 ec 28          	sub    rsp,0x28
    b736:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b73d:	00 00 
    b73f:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    b744:	31 c0                	xor    eax,eax
  lua_Integer ib;
  lua_Integer ic;
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b746:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    b74b:	75 43                	jne    b790 <raviV_op_bxor+0x70>
    b74d:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    b750:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
    b755:	66 83 7d 08 13       	cmp    WORD PTR [rbp+0x8],0x13
    b75a:	75 64                	jne    b7c0 <raviV_op_bxor+0xa0>
    b75c:	48 8b 45 00          	mov    rax,QWORD PTR [rbp+0x0]
    setivalue(ra, intop (^, ib, ic));
    b760:	48 33 44 24 08       	xor    rax,QWORD PTR [rsp+0x8]
    b765:	49 89 04 24          	mov    QWORD PTR [r12],rax
    b769:	b8 13 00 00 00       	mov    eax,0x13
    b76e:	66 41 89 44 24 08    	mov    WORD PTR [r12+0x8],ax
  }
  else {
    luaT_trybinTM(L, rb, rc, ra, TM_BXOR);
  }
}
    b774:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    b779:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b780:	00 00 
    b782:	75 56                	jne    b7da <raviV_op_bxor+0xba>
    b784:	48 83 c4 28          	add    rsp,0x28
    b788:	5b                   	pop    rbx
    b789:	5d                   	pop    rbp
    b78a:	41 5c                	pop    r12
    b78c:	41 5d                	pop    r13
    b78e:	c3                   	ret    
    b78f:	90                   	nop
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b790:	31 d2                	xor    edx,edx
    b792:	48 8d 74 24 08       	lea    rsi,[rsp+0x8]
    b797:	48 89 df             	mov    rdi,rbx
    b79a:	e8 00 00 00 00       	call   b79f <raviV_op_bxor+0x7f>
    b79f:	85 c0                	test   eax,eax
    b7a1:	75 b2                	jne    b755 <raviV_op_bxor+0x35>
    luaT_trybinTM(L, rb, rc, ra, TM_BXOR);
    b7a3:	41 b8 0f 00 00 00    	mov    r8d,0xf
    b7a9:	4c 89 e1             	mov    rcx,r12
    b7ac:	48 89 ea             	mov    rdx,rbp
    b7af:	48 89 de             	mov    rsi,rbx
    b7b2:	4c 89 ef             	mov    rdi,r13
    b7b5:	e8 00 00 00 00       	call   b7ba <raviV_op_bxor+0x9a>
}
    b7ba:	eb b8                	jmp    b774 <raviV_op_bxor+0x54>
    b7bc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  if (tointeger(rb, &ib) && tointeger(rc, &ic)) {
    b7c0:	31 d2                	xor    edx,edx
    b7c2:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    b7c7:	48 89 ef             	mov    rdi,rbp
    b7ca:	e8 00 00 00 00       	call   b7cf <raviV_op_bxor+0xaf>
    b7cf:	85 c0                	test   eax,eax
    b7d1:	74 d0                	je     b7a3 <raviV_op_bxor+0x83>
    b7d3:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
    b7d8:	eb 86                	jmp    b760 <raviV_op_bxor+0x40>
}
    b7da:	e8 00 00 00 00       	call   b7df <raviV_op_bxor+0xbf>
    b7df:	90                   	nop

000000000000b7e0 <raviV_op_bnot>:

void raviV_op_bnot(lua_State *L, TValue *ra, TValue *rb) {
    b7e0:	41 54                	push   r12
    b7e2:	55                   	push   rbp
    b7e3:	48 89 f5             	mov    rbp,rsi
    b7e6:	53                   	push   rbx
    b7e7:	48 89 d3             	mov    rbx,rdx
    b7ea:	48 83 ec 10          	sub    rsp,0x10
    b7ee:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    b7f5:	00 00 
    b7f7:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
    b7fc:	31 c0                	xor    eax,eax
  lua_Integer ib;
  if (tointeger(rb, &ib)) { setivalue(ra, intop (^, ~l_castS2U(0), ib)); }
    b7fe:	66 83 7a 08 13       	cmp    WORD PTR [rdx+0x8],0x13
    b803:	75 33                	jne    b838 <raviV_op_bnot+0x58>
    b805:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    b808:	48 f7 d0             	not    rax
    b80b:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
    b80f:	b8 13 00 00 00       	mov    eax,0x13
    b814:	66 89 45 08          	mov    WORD PTR [rbp+0x8],ax
  else {
    luaT_trybinTM(L, rb, rb, ra, TM_BNOT);
  }
}
    b818:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
    b81d:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    b824:	00 00 
    b826:	75 49                	jne    b871 <raviV_op_bnot+0x91>
    b828:	48 83 c4 10          	add    rsp,0x10
    b82c:	5b                   	pop    rbx
    b82d:	5d                   	pop    rbp
    b82e:	41 5c                	pop    r12
    b830:	c3                   	ret    
    b831:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    b838:	49 89 fc             	mov    r12,rdi
  if (tointeger(rb, &ib)) { setivalue(ra, intop (^, ~l_castS2U(0), ib)); }
    b83b:	31 d2                	xor    edx,edx
    b83d:	48 89 e6             	mov    rsi,rsp
    b840:	48 89 df             	mov    rdi,rbx
    b843:	e8 00 00 00 00       	call   b848 <raviV_op_bnot+0x68>
    b848:	85 c0                	test   eax,eax
    b84a:	74 0c                	je     b858 <raviV_op_bnot+0x78>
    b84c:	48 8b 04 24          	mov    rax,QWORD PTR [rsp]
    b850:	eb b6                	jmp    b808 <raviV_op_bnot+0x28>
    b852:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    luaT_trybinTM(L, rb, rb, ra, TM_BNOT);
    b858:	41 b8 13 00 00 00    	mov    r8d,0x13
    b85e:	48 89 e9             	mov    rcx,rbp
    b861:	48 89 da             	mov    rdx,rbx
    b864:	48 89 de             	mov    rsi,rbx
    b867:	4c 89 e7             	mov    rdi,r12
    b86a:	e8 00 00 00 00       	call   b86f <raviV_op_bnot+0x8f>
}
    b86f:	eb a7                	jmp    b818 <raviV_op_bnot+0x38>
    b871:	e8 00 00 00 00       	call   b876 <raviV_op_bnot+0x96>
    b876:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
    b87d:	00 00 00 

000000000000b880 <raviV_gettable_sskey>:
** Compute 'val = t[key]'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void raviV_gettable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val) {
  GETTABLE_INLINE_SSKEY(L, t, key, val);
    b880:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    b884:	66 3d 05 80          	cmp    ax,0x8005
    b888:	75 5f                	jne    b8e9 <raviV_gettable_sskey+0x69>
    b88a:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    b88d:	4c 8b 0a             	mov    r9,QWORD PTR [rdx]
  Node *n = hashstr(t, key);
    b890:	45 8b 41 0c          	mov    r8d,DWORD PTR [r9+0xc]
    b894:	44 23 40 50          	and    r8d,DWORD PTR [rax+0x50]
    b898:	49 c1 e0 05          	shl    r8,0x5
    b89c:	4c 03 40 18          	add    r8,QWORD PTR [rax+0x18]
    b8a0:	eb 15                	jmp    b8b7 <raviV_gettable_sskey+0x37>
    b8a2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
      int nx = gnext(n);
    b8a8:	49 63 40 1c          	movsxd rax,DWORD PTR [r8+0x1c]
      if (nx == 0)
    b8ac:	85 c0                	test   eax,eax
    b8ae:	74 30                	je     b8e0 <raviV_gettable_sskey+0x60>
      n += nx;
    b8b0:	48 c1 e0 05          	shl    rax,0x5
    b8b4:	49 01 c0             	add    r8,rax
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
    b8b7:	66 41 81 78 18 04 80 	cmp    WORD PTR [r8+0x18],0x8004
    b8be:	75 e8                	jne    b8a8 <raviV_gettable_sskey+0x28>
    b8c0:	4d 3b 48 10          	cmp    r9,QWORD PTR [r8+0x10]
    b8c4:	75 e2                	jne    b8a8 <raviV_gettable_sskey+0x28>
    b8c6:	41 0f b7 40 08       	movzx  eax,WORD PTR [r8+0x8]
    b8cb:	66 85 c0             	test   ax,ax
    b8ce:	74 2a                	je     b8fa <raviV_gettable_sskey+0x7a>
    b8d0:	49 8b 10             	mov    rdx,QWORD PTR [r8]
    b8d3:	66 89 41 08          	mov    WORD PTR [rcx+0x8],ax
    b8d7:	48 89 11             	mov    QWORD PTR [rcx],rdx
    b8da:	c3                   	ret    
    b8db:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
        return luaO_nilobject;  /* not found */
    b8e0:	4c 8d 05 00 00 00 00 	lea    r8,[rip+0x0]        # b8e7 <raviV_gettable_sskey+0x67>
    b8e7:	eb dd                	jmp    b8c6 <raviV_gettable_sskey+0x46>
    b8e9:	83 e0 0f             	and    eax,0xf
    b8ec:	66 83 f8 05          	cmp    ax,0x5
    b8f0:	74 0d                	je     b8ff <raviV_gettable_sskey+0x7f>
    b8f2:	45 31 c0             	xor    r8d,r8d
    b8f5:	e9 b6 4f ff ff       	jmp    8b0 <luaV_finishget>
    b8fa:	e9 b1 4f ff ff       	jmp    8b0 <luaV_finishget>
void raviV_gettable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val) {
    b8ff:	50                   	push   rax
  GETTABLE_INLINE_SSKEY(L, t, key, val);
    b900:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # b907 <raviV_gettable_sskey+0x87>
    b907:	e8 00 00 00 00       	call   b90c <raviV_gettable_sskey+0x8c>
    b90c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

000000000000b910 <raviV_settable_sskey>:
** Compute 't[key] = val'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void raviV_settable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val) {
  SETTABLE_INLINE_SSKEY(L, t, key, val);
    b910:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    b914:	66 3d 05 80          	cmp    ax,0x8005
    b918:	0f 85 91 00 00 00    	jne    b9af <raviV_settable_sskey+0x9f>
    b91e:	48 8b 06             	mov    rax,QWORD PTR [rsi]
    b921:	4c 8b 0a             	mov    r9,QWORD PTR [rdx]
  Node *n = hashstr(t, key);
    b924:	45 8b 41 0c          	mov    r8d,DWORD PTR [r9+0xc]
    b928:	44 23 40 50          	and    r8d,DWORD PTR [rax+0x50]
    b92c:	49 c1 e0 05          	shl    r8,0x5
    b930:	4c 03 40 18          	add    r8,QWORD PTR [rax+0x18]
    b934:	eb 19                	jmp    b94f <raviV_settable_sskey+0x3f>
    b936:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
    b93d:	00 00 00 
      int nx = gnext(n);
    b940:	49 63 40 1c          	movsxd rax,DWORD PTR [r8+0x1c]
      if (nx == 0)
    b944:	85 c0                	test   eax,eax
    b946:	74 38                	je     b980 <raviV_settable_sskey+0x70>
      n += nx;
    b948:	48 c1 e0 05          	shl    rax,0x5
    b94c:	49 01 c0             	add    r8,rax
    if (ttisshrstring(k) && eqshrstr(tsvalue(k), key))
    b94f:	66 41 81 78 18 04 80 	cmp    WORD PTR [r8+0x18],0x8004
    b956:	75 e8                	jne    b940 <raviV_settable_sskey+0x30>
    b958:	4d 3b 48 10          	cmp    r9,QWORD PTR [r8+0x10]
    b95c:	75 e2                	jne    b940 <raviV_settable_sskey+0x30>
    b95e:	66 41 83 78 08 00    	cmp    WORD PTR [r8+0x8],0x0
    b964:	74 29                	je     b98f <raviV_settable_sskey+0x7f>
    b966:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    b969:	49 89 00             	mov    QWORD PTR [r8],rax
    b96c:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    b970:	66 41 89 40 08       	mov    WORD PTR [r8+0x8],ax
    b975:	66 83 79 08 00       	cmp    WORD PTR [rcx+0x8],0x0
    b97a:	78 1c                	js     b998 <raviV_settable_sskey+0x88>
    b97c:	c3                   	ret    
    b97d:	0f 1f 00             	nop    DWORD PTR [rax]
        return luaO_nilobject;  /* not found */
    b980:	4c 8d 05 00 00 00 00 	lea    r8,[rip+0x0]        # b987 <raviV_settable_sskey+0x77>
    b987:	66 41 83 78 08 00    	cmp    WORD PTR [r8+0x8],0x0
    b98d:	75 d7                	jne    b966 <raviV_settable_sskey+0x56>
    b98f:	e9 7c 50 ff ff       	jmp    a10 <luaV_finishset>
    b994:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    b998:	48 8b 36             	mov    rsi,QWORD PTR [rsi]
    b99b:	f6 46 09 04          	test   BYTE PTR [rsi+0x9],0x4
    b99f:	74 db                	je     b97c <raviV_settable_sskey+0x6c>
    b9a1:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    b9a4:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
    b9a8:	74 d2                	je     b97c <raviV_settable_sskey+0x6c>
    b9aa:	e9 00 00 00 00       	jmp    b9af <raviV_settable_sskey+0x9f>
    b9af:	83 e0 0f             	and    eax,0xf
    b9b2:	66 83 f8 05          	cmp    ax,0x5
    b9b6:	74 08                	je     b9c0 <raviV_settable_sskey+0xb0>
    b9b8:	45 31 c0             	xor    r8d,r8d
    b9bb:	e9 50 50 ff ff       	jmp    a10 <luaV_finishset>
void raviV_settable_sskey(lua_State *L, const TValue *t, TValue *key, StkId val) {
    b9c0:	50                   	push   rax
  SETTABLE_INLINE_SSKEY(L, t, key, val);
    b9c1:	48 8d 15 00 00 00 00 	lea    rdx,[rip+0x0]        # b9c8 <raviV_settable_sskey+0xb8>
    b9c8:	e8 00 00 00 00       	call   b9cd <raviV_settable_sskey+0xbd>
    b9cd:	0f 1f 00             	nop    DWORD PTR [rax]

000000000000b9d0 <raviV_gettable_i>:
** Main function for table access (invoking metamethods if needed).
** Compute 'val = t[key]'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void raviV_gettable_i(lua_State *L, const TValue *t, TValue *key, StkId val) {
    b9d0:	55                   	push   rbp
    b9d1:	48 89 fd             	mov    rbp,rdi
    b9d4:	53                   	push   rbx
    b9d5:	48 89 f3             	mov    rbx,rsi
    b9d8:	48 83 ec 18          	sub    rsp,0x18
  GETTABLE_INLINE_I(L, t, key, val);
    b9dc:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    b9e0:	66 3d 05 80          	cmp    ax,0x8005
    b9e4:	75 3a                	jne    ba20 <raviV_gettable_i+0x50>
    b9e6:	48 8b 3b             	mov    rdi,QWORD PTR [rbx]
    b9e9:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    b9ec:	8b 47 0c             	mov    eax,DWORD PTR [rdi+0xc]
    b9ef:	4c 8d 46 ff          	lea    r8,[rsi-0x1]
    b9f3:	49 39 c0             	cmp    r8,rax
    b9f6:	73 60                	jae    ba58 <raviV_gettable_i+0x88>
    b9f8:	48 8b 47 10          	mov    rax,QWORD PTR [rdi+0x10]
    b9fc:	48 c1 e6 04          	shl    rsi,0x4
    ba00:	4c 8d 44 30 f0       	lea    r8,[rax+rsi*1-0x10]
    ba05:	41 0f b7 40 08       	movzx  eax,WORD PTR [r8+0x8]
    ba0a:	66 85 c0             	test   ax,ax
    ba0d:	74 69                	je     ba78 <raviV_gettable_i+0xa8>
    ba0f:	49 8b 10             	mov    rdx,QWORD PTR [r8]
    ba12:	66 89 41 08          	mov    WORD PTR [rcx+0x8],ax
    ba16:	48 89 11             	mov    QWORD PTR [rcx],rdx
}
    ba19:	48 83 c4 18          	add    rsp,0x18
    ba1d:	5b                   	pop    rbx
    ba1e:	5d                   	pop    rbp
    ba1f:	c3                   	ret    
  GETTABLE_INLINE_I(L, t, key, val);
    ba20:	66 3d 25 80          	cmp    ax,0x8025
    ba24:	74 7a                	je     baa0 <raviV_gettable_i+0xd0>
    ba26:	66 3d 15 80          	cmp    ax,0x8015
    ba2a:	75 64                	jne    ba90 <raviV_gettable_i+0xc0>
    ba2c:	48 8b 36             	mov    rsi,QWORD PTR [rsi]
    ba2f:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    ba32:	48 8b 56 38          	mov    rdx,QWORD PTR [rsi+0x38]
    ba36:	39 46 40             	cmp    DWORD PTR [rsi+0x40],eax
    ba39:	0f 86 89 00 00 00    	jbe    bac8 <raviV_gettable_i+0xf8>
    ba3f:	89 c0                	mov    eax,eax
    ba41:	48 8b 04 c2          	mov    rax,QWORD PTR [rdx+rax*8]
    ba45:	48 89 01             	mov    QWORD PTR [rcx],rax
    ba48:	b8 13 00 00 00       	mov    eax,0x13
    ba4d:	66 89 41 08          	mov    WORD PTR [rcx+0x8],ax
    ba51:	eb c6                	jmp    ba19 <raviV_gettable_i+0x49>
    ba53:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
    ba58:	48 89 4c 24 08       	mov    QWORD PTR [rsp+0x8],rcx
    ba5d:	48 89 14 24          	mov    QWORD PTR [rsp],rdx
    ba61:	e8 00 00 00 00       	call   ba66 <raviV_gettable_i+0x96>
    ba66:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
    ba6b:	48 8b 14 24          	mov    rdx,QWORD PTR [rsp]
    ba6f:	49 89 c0             	mov    r8,rax
    ba72:	eb 91                	jmp    ba05 <raviV_gettable_i+0x35>
    ba74:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
}
    ba78:	48 83 c4 18          	add    rsp,0x18
  GETTABLE_INLINE_I(L, t, key, val);
    ba7c:	48 89 de             	mov    rsi,rbx
    ba7f:	48 89 ef             	mov    rdi,rbp
}
    ba82:	5b                   	pop    rbx
    ba83:	5d                   	pop    rbp
  GETTABLE_INLINE_I(L, t, key, val);
    ba84:	e9 27 4e ff ff       	jmp    8b0 <luaV_finishget>
    ba89:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
}
    ba90:	48 83 c4 18          	add    rsp,0x18
  GETTABLE_INLINE_I(L, t, key, val);
    ba94:	45 31 c0             	xor    r8d,r8d
}
    ba97:	5b                   	pop    rbx
    ba98:	5d                   	pop    rbp
  GETTABLE_INLINE_I(L, t, key, val);
    ba99:	e9 12 4e ff ff       	jmp    8b0 <luaV_finishget>
    ba9e:	66 90                	xchg   ax,ax
    baa0:	48 8b 36             	mov    rsi,QWORD PTR [rsi]
    baa3:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    baa6:	48 8b 56 38          	mov    rdx,QWORD PTR [rsi+0x38]
    baaa:	39 46 40             	cmp    DWORD PTR [rsi+0x40],eax
    baad:	76 19                	jbe    bac8 <raviV_gettable_i+0xf8>
    baaf:	89 c0                	mov    eax,eax
    bab1:	f2 0f 10 04 c2       	movsd  xmm0,QWORD PTR [rdx+rax*8]
    bab6:	ba 03 00 00 00       	mov    edx,0x3
    babb:	66 89 51 08          	mov    WORD PTR [rcx+0x8],dx
    babf:	f2 0f 11 01          	movsd  QWORD PTR [rcx],xmm0
    bac3:	e9 51 ff ff ff       	jmp    ba19 <raviV_gettable_i+0x49>
    bac8:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # bacf <raviV_gettable_i+0xff>
    bacf:	48 89 ef             	mov    rdi,rbp
    bad2:	31 c0                	xor    eax,eax
    bad4:	e8 00 00 00 00       	call   bad9 <raviV_gettable_i+0x109>
    bad9:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

000000000000bae0 <raviV_settable_i>:
** Main function for table assignment (invoking metamethods if needed).
** Compute 't[key] = val'
** In Lua 5.3.2 this function is a macro but we need it to be a function
** so that JIT code can invoke it
*/
void raviV_settable_i(lua_State *L, const TValue *t, TValue *key, StkId val) {
    bae0:	41 55                	push   r13
    bae2:	49 89 fd             	mov    r13,rdi
    bae5:	41 54                	push   r12
    bae7:	49 89 d4             	mov    r12,rdx
    baea:	55                   	push   rbp
    baeb:	53                   	push   rbx
    baec:	48 89 f3             	mov    rbx,rsi
    baef:	48 83 ec 28          	sub    rsp,0x28
    baf3:	64 48 8b 04 25 28 00 	mov    rax,QWORD PTR fs:0x28
    bafa:	00 00 
    bafc:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
    bb01:	31 c0                	xor    eax,eax
  SETTABLE_INLINE_I(L, t, key, val);
    bb03:	0f b7 46 08          	movzx  eax,WORD PTR [rsi+0x8]
    bb07:	66 3d 05 80          	cmp    ax,0x8005
    bb0b:	0f 85 9f 00 00 00    	jne    bbb0 <raviV_settable_i+0xd0>
    bb11:	48 8b 2b             	mov    rbp,QWORD PTR [rbx]
    bb14:	48 8b 32             	mov    rsi,QWORD PTR [rdx]
    bb17:	8b 45 0c             	mov    eax,DWORD PTR [rbp+0xc]
    bb1a:	48 8d 56 ff          	lea    rdx,[rsi-0x1]
    bb1e:	48 39 c2             	cmp    rdx,rax
    bb21:	0f 83 c9 00 00 00    	jae    bbf0 <raviV_settable_i+0x110>
    bb27:	48 8b 45 10          	mov    rax,QWORD PTR [rbp+0x10]
    bb2b:	48 c1 e6 04          	shl    rsi,0x4
    bb2f:	4c 8d 44 30 f0       	lea    r8,[rax+rsi*1-0x10]
    bb34:	66 41 83 78 08 00    	cmp    WORD PTR [r8+0x8],0x0
    bb3a:	74 5c                	je     bb98 <raviV_settable_i+0xb8>
    bb3c:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    bb3f:	49 89 00             	mov    QWORD PTR [r8],rax
    bb42:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    bb46:	66 41 89 40 08       	mov    WORD PTR [r8+0x8],ax
    bb4b:	66 83 79 08 00       	cmp    WORD PTR [rcx+0x8],0x0
    bb50:	78 26                	js     bb78 <raviV_settable_i+0x98>
}
    bb52:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
    bb57:	64 48 33 04 25 28 00 	xor    rax,QWORD PTR fs:0x28
    bb5e:	00 00 
    bb60:	0f 85 de 01 00 00    	jne    bd44 <raviV_settable_i+0x264>
    bb66:	48 83 c4 28          	add    rsp,0x28
    bb6a:	5b                   	pop    rbx
    bb6b:	5d                   	pop    rbp
    bb6c:	41 5c                	pop    r12
    bb6e:	41 5d                	pop    r13
    bb70:	c3                   	ret    
    bb71:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  SETTABLE_INLINE_I(L, t, key, val);
    bb78:	f6 45 09 04          	test   BYTE PTR [rbp+0x9],0x4
    bb7c:	74 d4                	je     bb52 <raviV_settable_i+0x72>
    bb7e:	48 8b 01             	mov    rax,QWORD PTR [rcx]
    bb81:	f6 40 09 03          	test   BYTE PTR [rax+0x9],0x3
    bb85:	74 cb                	je     bb52 <raviV_settable_i+0x72>
    bb87:	48 89 ee             	mov    rsi,rbp
    bb8a:	4c 89 ef             	mov    rdi,r13
    bb8d:	e8 00 00 00 00       	call   bb92 <raviV_settable_i+0xb2>
    bb92:	eb be                	jmp    bb52 <raviV_settable_i+0x72>
    bb94:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    bb98:	4c 89 e2             	mov    rdx,r12
    bb9b:	48 89 de             	mov    rsi,rbx
    bb9e:	4c 89 ef             	mov    rdi,r13
    bba1:	e8 00 00 00 00       	call   bba6 <raviV_settable_i+0xc6>
    bba6:	eb aa                	jmp    bb52 <raviV_settable_i+0x72>
    bba8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    bbaf:	00 
    bbb0:	66 3d 25 80          	cmp    ax,0x8025
    bbb4:	74 6a                	je     bc20 <raviV_settable_i+0x140>
    bbb6:	66 3d 15 80          	cmp    ax,0x8015
    bbba:	75 54                	jne    bc10 <raviV_settable_i+0x130>
    bbbc:	66 83 79 08 13       	cmp    WORD PTR [rcx+0x8],0x13
    bbc1:	48 8b 1e             	mov    rbx,QWORD PTR [rsi]
    bbc4:	0f 85 96 00 00 00    	jne    bc60 <raviV_settable_i+0x180>
    bbca:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    bbcd:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
    bbd0:	89 c2                	mov    edx,eax
    bbd2:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
    bbd5:	0f 86 1e 01 00 00    	jbe    bcf9 <raviV_settable_i+0x219>
    bbdb:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
    bbdf:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    bbe3:	e9 6a ff ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bbe8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
    bbef:	00 
    bbf0:	48 89 ef             	mov    rdi,rbp
    bbf3:	48 89 4c 24 08       	mov    QWORD PTR [rsp+0x8],rcx
    bbf8:	e8 00 00 00 00       	call   bbfd <raviV_settable_i+0x11d>
    bbfd:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
    bc02:	49 89 c0             	mov    r8,rax
    bc05:	e9 2a ff ff ff       	jmp    bb34 <raviV_settable_i+0x54>
    bc0a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    bc10:	45 31 c0             	xor    r8d,r8d
    bc13:	e8 00 00 00 00       	call   bc18 <raviV_settable_i+0x138>
}
    bc18:	e9 35 ff ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bc1d:	0f 1f 00             	nop    DWORD PTR [rax]
  SETTABLE_INLINE_I(L, t, key, val);
    bc20:	0f b7 41 08          	movzx  eax,WORD PTR [rcx+0x8]
    bc24:	48 8b 1e             	mov    rbx,QWORD PTR [rsi]
    bc27:	66 83 f8 03          	cmp    ax,0x3
    bc2b:	74 73                	je     bca0 <raviV_settable_i+0x1c0>
    bc2d:	66 83 f8 13          	cmp    ax,0x13
    bc31:	0f 85 89 00 00 00    	jne    bcc0 <raviV_settable_i+0x1e0>
    bc37:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    bc3a:	66 0f ef c0          	pxor   xmm0,xmm0
    bc3e:	f2 48 0f 2a 01       	cvtsi2sd xmm0,QWORD PTR [rcx]
    bc43:	89 c2                	mov    edx,eax
    bc45:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
    bc48:	0f 86 d9 00 00 00    	jbe    bd27 <raviV_settable_i+0x247>
    bc4e:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
    bc52:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    bc57:	e9 f6 fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bc5c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    bc60:	31 d2                	xor    edx,edx
    bc62:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    bc67:	48 89 cf             	mov    rdi,rcx
  SETTABLE_INLINE_I(L, t, key, val);
    bc6a:	48 c7 44 24 10 00 00 	mov    QWORD PTR [rsp+0x10],0x0
    bc71:	00 00 
  return luaV_tointeger(obj, p, LUA_FLOORN2I);
    bc73:	e8 00 00 00 00       	call   bc78 <raviV_settable_i+0x198>
  SETTABLE_INLINE_I(L, t, key, val);
    bc78:	85 c0                	test   eax,eax
    bc7a:	0f 84 da 00 00 00    	je     bd5a <raviV_settable_i+0x27a>
    bc80:	49 8b 04 24          	mov    rax,QWORD PTR [r12]
    bc84:	48 8b 4c 24 10       	mov    rcx,QWORD PTR [rsp+0x10]
    bc89:	89 c2                	mov    edx,eax
    bc8b:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
    bc8e:	76 76                	jbe    bd06 <raviV_settable_i+0x226>
    bc90:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
    bc94:	48 89 0c d0          	mov    QWORD PTR [rax+rdx*8],rcx
    bc98:	e9 b5 fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bc9d:	0f 1f 00             	nop    DWORD PTR [rax]
    bca0:	48 8b 02             	mov    rax,QWORD PTR [rdx]
    bca3:	89 c2                	mov    edx,eax
    bca5:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
    bca8:	76 6c                	jbe    bd16 <raviV_settable_i+0x236>
    bcaa:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    bcae:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
    bcb2:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    bcb7:	e9 96 fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bcbc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    bcc0:	48 8d 74 24 10       	lea    rsi,[rsp+0x10]
    bcc5:	48 89 cf             	mov    rdi,rcx
    bcc8:	48 c7 44 24 10 00 00 	mov    QWORD PTR [rsp+0x10],0x0
    bccf:	00 00 
    bcd1:	e8 00 00 00 00       	call   bcd6 <raviV_settable_i+0x1f6>
    bcd6:	85 c0                	test   eax,eax
    bcd8:	74 6f                	je     bd49 <raviV_settable_i+0x269>
    bcda:	49 8b 04 24          	mov    rax,QWORD PTR [r12]
    bcde:	f2 0f 10 44 24 10    	movsd  xmm0,QWORD PTR [rsp+0x10]
    bce4:	89 c2                	mov    edx,eax
    bce6:	39 43 40             	cmp    DWORD PTR [rbx+0x40],eax
    bce9:	76 49                	jbe    bd34 <raviV_settable_i+0x254>
    bceb:	48 8b 43 38          	mov    rax,QWORD PTR [rbx+0x38]
    bcef:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
    bcf4:	e9 59 fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bcf9:	48 89 de             	mov    rsi,rbx
    bcfc:	e8 00 00 00 00       	call   bd01 <raviV_settable_i+0x221>
    bd01:	e9 4c fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bd06:	48 89 de             	mov    rsi,rbx
    bd09:	4c 89 ef             	mov    rdi,r13
    bd0c:	e8 00 00 00 00       	call   bd11 <raviV_settable_i+0x231>
    bd11:	e9 3c fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bd16:	f2 0f 10 01          	movsd  xmm0,QWORD PTR [rcx]
    bd1a:	48 89 de             	mov    rsi,rbx
    bd1d:	e8 00 00 00 00       	call   bd22 <raviV_settable_i+0x242>
    bd22:	e9 2b fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bd27:	48 89 de             	mov    rsi,rbx
    bd2a:	e8 00 00 00 00       	call   bd2f <raviV_settable_i+0x24f>
    bd2f:	e9 1e fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
    bd34:	48 89 de             	mov    rsi,rbx
    bd37:	4c 89 ef             	mov    rdi,r13
    bd3a:	e8 00 00 00 00       	call   bd3f <raviV_settable_i+0x25f>
    bd3f:	e9 0e fe ff ff       	jmp    bb52 <raviV_settable_i+0x72>
}
    bd44:	e8 00 00 00 00       	call   bd49 <raviV_settable_i+0x269>
  SETTABLE_INLINE_I(L, t, key, val);
    bd49:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # bd50 <raviV_settable_i+0x270>
    bd50:	4c 89 ef             	mov    rdi,r13
    bd53:	31 c0                	xor    eax,eax
    bd55:	e8 00 00 00 00       	call   bd5a <raviV_settable_i+0x27a>
    bd5a:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # bd61 <raviV_settable_i+0x281>
    bd61:	4c 89 ef             	mov    rdi,r13
    bd64:	31 c0                	xor    eax,eax
    bd66:	e8 00 00 00 00       	call   bd6b <raviV_settable_i+0x28b>
    bd6b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]

000000000000bd70 <raviV_op_totype>:
/**
** Opcode TOTYPE validates that the register A contains a
** type whose metatable is registered by name in constant Bx
*/
void raviV_op_totype(lua_State *L, TValue *ra, TValue *rb) {
  if (ttisnil(ra))
    bd70:	66 83 7e 08 00       	cmp    WORD PTR [rsi+0x8],0x0
    bd75:	74 29                	je     bda0 <raviV_op_totype+0x30>
    bd77:	48 89 d0             	mov    rax,rdx
void raviV_op_totype(lua_State *L, TValue *ra, TValue *rb) {
    bd7a:	48 83 ec 08          	sub    rsp,0x8
    bd7e:	48 89 f2             	mov    rdx,rsi
    return;
  if (!ttisshrstring(rb)) luaG_runerror(L, "type name must be string");
    bd81:	66 81 78 08 04 80    	cmp    WORD PTR [rax+0x8],0x8004
    bd87:	75 28                	jne    bdb1 <raviV_op_totype+0x41>
  TString *key = tsvalue(rb);
    bd89:	4c 8b 08             	mov    r9,QWORD PTR [rax]
  if (!raviV_check_usertype(L, key, ra))
    bd8c:	4c 89 ce             	mov    rsi,r9
    bd8f:	e8 00 00 00 00       	call   bd94 <raviV_op_totype+0x24>
    bd94:	85 c0                	test   eax,eax
    bd96:	74 09                	je     bda1 <raviV_op_totype+0x31>
    luaG_runerror(L, "type mismatch: expected %s", getstr(key));
}
    bd98:	48 83 c4 08          	add    rsp,0x8
    bd9c:	c3                   	ret    
    bd9d:	0f 1f 00             	nop    DWORD PTR [rax]
    bda0:	c3                   	ret    
    luaG_runerror(L, "type mismatch: expected %s", getstr(key));
    bda1:	49 8d 51 18          	lea    rdx,[r9+0x18]
    bda5:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # bdac <raviV_op_totype+0x3c>
    bdac:	e8 00 00 00 00       	call   bdb1 <raviV_op_totype+0x41>
  if (!ttisshrstring(rb)) luaG_runerror(L, "type name must be string");
    bdb1:	48 8d 35 00 00 00 00 	lea    rsi,[rip+0x0]        # bdb8 <raviV_op_totype+0x48>
    bdb8:	31 c0                	xor    eax,eax
    bdba:	e8 00 00 00 00       	call   bdbf <raviV_op_totype+0x4f>
