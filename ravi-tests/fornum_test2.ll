; ModuleID = 'ravi_module_ravif1'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc-elf"

%0 = type { %ravi.TValue*, i32*, i64 }
%ravi.lua_State = type { %ravi.GCObject*, i8, i8, i8, %ravi.TValue*, %ravi.global_State*, %ravi.CallInfo*, i32*, %ravi.TValue*, %ravi.TValue*, %ravi.UpVal*, %ravi.GCObject*, %ravi.lua_State*, %ravi.lua_longjmp*, %ravi.CallInfo, i8* (%ravi.lua_State*, %ravi.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8 }
%ravi.global_State = type opaque
%ravi.TValue = type { %ravi.Value, i32 }
%ravi.Value = type { double }
%ravi.UpVal = type opaque
%ravi.GCObject = type { %ravi.GCObject*, i8, i8 }
%ravi.lua_longjmp = type opaque
%ravi.CallInfo = type { %ravi.TValue*, %ravi.TValue*, %ravi.CallInfo*, %ravi.CallInfo*, %0, i64, i16, i8 }
%ravi.lua_Debug = type opaque
%ravi.LClosure = type { %ravi.GCObject*, i8, i8, i8, %ravi.GCObject*, %ravi.Proto*, [1 x %ravi.UpVal*] }
%ravi.Proto = type { %ravi.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %ravi.TValue*, i32*, %ravi.Proto**, i32*, %ravi.LocVar*, %ravi.Upvaldesc*, %ravi.LClosure*, %ravi.TString*, %ravi.GCObject*, %ravi.RaviJITProto* }
%ravi.LocVar = type { %ravi.TString*, i32, i32, i32 }
%ravi.Upvaldesc = type { %ravi.TString*, i8, i8 }
%ravi.TString = type { %ravi.GCObject*, i8, i8, i8, i32, i64, %ravi.TString* }
%ravi.RaviJITProto = type opaque

@0 = private unnamed_addr constant [29 x i8] c"'for' limit must be a number\00"
@1 = private unnamed_addr constant [28 x i8] c"'for' step must be a number\00"
@2 = private unnamed_addr constant [37 x i8] c"'for' initial value must be a number\00"

define i32 @ravif1(%ravi.lua_State* %L) {
entry:
  %stopnow = alloca i32
  %ilimit = alloca i64
  %nlimit = alloca double
  %ninit = alloca double
  %nstep = alloca double
  %L_ci = getelementptr inbounds %ravi.lua_State* %L, i32 0, i32 6
  %0 = load %ravi.CallInfo** %L_ci, !tbaa !0
  %base = getelementptr inbounds %ravi.CallInfo* %0, i32 0, i32 4, i32 0
  %1 = bitcast %ravi.CallInfo* %0 to %ravi.LClosure***
  %2 = load %ravi.LClosure*** %1, !tbaa !7
  %3 = load %ravi.LClosure** %2, !tbaa !8
  %Proto = getelementptr inbounds %ravi.LClosure* %3, i32 0, i32 5
  %4 = load %ravi.Proto** %Proto, !tbaa !9
  %k = getelementptr inbounds %ravi.Proto* %4, i32 0, i32 14
  %5 = load %ravi.TValue** %k, !tbaa !11
  tail call void @luaV_op_loadnil(%ravi.CallInfo* %0, i32 0, i32 0)
  %6 = load %ravi.TValue** %base, !tbaa !14
  %dest.value = getelementptr inbounds %ravi.TValue* %6, i32 0, i32 0, i32 0
  store double 0.000000e+00, double* %dest.value, !tbaa !15
  %dest.tt = getelementptr inbounds %ravi.TValue* %6, i32 0, i32 1
  store i32 3, i32* %dest.tt, !tbaa !17
  %srcvalue = getelementptr inbounds %ravi.TValue* %5, i32 0, i32 0, i32 0
  %destvalue = getelementptr inbounds %ravi.TValue* %6, i32 1, i32 0, i32 0
  %7 = load double* %srcvalue, !tbaa !15
  store double %7, double* %destvalue, !tbaa !15
  %srctype = getelementptr inbounds %ravi.TValue* %5, i32 0, i32 1
  %desttype = getelementptr inbounds %ravi.TValue* %6, i32 1, i32 1
  %8 = load i32* %srctype, !tbaa !17
  store i32 %8, i32* %desttype, !tbaa !17
  %srcvalue1 = getelementptr inbounds %ravi.TValue* %5, i32 1, i32 0, i32 0
  %destvalue2 = getelementptr inbounds %ravi.TValue* %6, i32 2, i32 0, i32 0
  %9 = load double* %srcvalue1, !tbaa !15
  store double %9, double* %destvalue2, !tbaa !15
  %srctype3 = getelementptr inbounds %ravi.TValue* %5, i32 1, i32 1
  %desttype4 = getelementptr inbounds %ravi.TValue* %6, i32 2, i32 1
  %10 = load i32* %srctype3, !tbaa !17
  store i32 %10, i32* %desttype4, !tbaa !17
  %destvalue6 = getelementptr inbounds %ravi.TValue* %6, i32 3, i32 0, i32 0
  %11 = load double* %srcvalue, !tbaa !15
  store double %11, double* %destvalue6, !tbaa !15
  %desttype8 = getelementptr inbounds %ravi.TValue* %6, i32 3, i32 1
  %12 = load i32* %srctype, !tbaa !17
  store i32 %12, i32* %desttype8, !tbaa !17
  %13 = getelementptr inbounds %ravi.TValue* %6, i32 1
  %14 = getelementptr inbounds %ravi.TValue* %6, i32 2
  %15 = getelementptr inbounds %ravi.TValue* %6, i32 3
  %init.tt = load i32* %desttype, !tbaa !17
  %init.is.integer = icmp eq i32 %init.tt, 19
  %step.is.integer = icmp eq i32 %12, 19
  %step.i.ptr = bitcast %ravi.TValue* %15 to i64*
  %step.i = load i64* %step.i.ptr, !tbaa !15
  %16 = call i32 @luaV_forlimit(%ravi.TValue* %14, i64* %ilimit, i64 %step.i, i32* %stopnow)
  %init.and.step.are.integers = and i1 %init.is.integer, %step.is.integer
  %17 = icmp ne i32 %16, 0
  %all.integers = and i1 %init.and.step.are.integers, %17
  br i1 %all.integers, label %if.all.integers, label %if.not.all.integers

if.all.integers:                                  ; preds = %entry
  %stopnow.value = load i32* %stopnow, !tbaa !13
  %stopnow.is.zero = icmp eq i32 %stopnow.value, 0
  br i1 %stopnow.is.zero, label %if.stopnow.iszero, label %if.stopnow.notzero

if.stopnow.iszero:                                ; preds = %if.all.integers
  %init.i.ptr = bitcast %ravi.TValue* %13 to i64*
  %init.i = load i64* %init.i.ptr, !tbaa !15
  br label %if.stopnow.notzero

if.stopnow.notzero:                               ; preds = %if.stopnow.iszero, %if.all.integers
  %initv = phi i64 [ %init.i, %if.stopnow.iszero ], [ 0, %if.all.integers ]
  %initv-pstep.i = sub nsw i64 %initv, %step.i
  %step.gt.zero = icmp sgt i64 %step.i, 0
  %. = select i1 %step.gt.zero, i8* blockaddress(@ravif1, %forloop_ilt8_), i8* blockaddress(@ravif1, %forloop_igt8_)
  br i1 %step.gt.zero, label %forloop_ilt8_, label %forloop_igt8_

if.not.all.integers:                              ; preds = %entry
  %limit.tt = load i32* %desttype4, !tbaa !17
  %limit.is.float = icmp eq i32 %limit.tt, 3
  br i1 %limit.is.float, label %if.limit.isfloat, label %if.limit.notfloat

if.limit.isfloat:                                 ; preds = %if.not.all.integers
  %limit.n.ptr = getelementptr inbounds %ravi.TValue* %14, i32 0, i32 0, i32 0
  %limit.n = load double* %limit.n.ptr, !tbaa !15
  store volatile double %limit.n, double* %nlimit, !tbaa !5
  br label %if.else.step

if.limit.notfloat:                                ; preds = %if.not.all.integers
  %18 = call i32 @luaV_tonumber_(%ravi.TValue* %14, double* %nlimit)
  %limit.float.ok = icmp eq i32 %18, 0
  br i1 %limit.float.ok, label %if.limit.float.failed, label %if.else.step

if.limit.float.failed:                            ; preds = %if.limit.notfloat
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([29 x i8]* @0, i32 0, i32 0))
  br label %if.else.step

if.else.step:                                     ; preds = %if.limit.float.failed, %if.limit.notfloat, %if.limit.isfloat
  %step.tt10 = load i32* %desttype8, !tbaa !17
  %step.is.float = icmp eq i32 %step.tt10, 3
  br i1 %step.is.float, label %if.step.isfloat, label %if.step.notfloat

if.step.isfloat:                                  ; preds = %if.else.step
  %step.n.ptr = getelementptr inbounds %ravi.TValue* %15, i32 0, i32 0, i32 0
  %step.n = load double* %step.n.ptr, !tbaa !15
  store volatile double %step.n, double* %nstep, !tbaa !5
  br label %if.else.init

if.step.notfloat:                                 ; preds = %if.else.step
  %19 = call i32 @luaV_tonumber_(%ravi.TValue* %15, double* %nstep)
  %step.float.ok = icmp eq i32 %19, 0
  br i1 %step.float.ok, label %if.step.float.failed, label %if.else.init

if.step.float.failed:                             ; preds = %if.step.notfloat
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([28 x i8]* @1, i32 0, i32 0))
  br label %if.else.init

if.else.init:                                     ; preds = %if.step.float.failed, %if.step.notfloat, %if.step.isfloat
  %init.tt11 = load i32* %desttype, !tbaa !17
  %init.is.float = icmp eq i32 %init.tt11, 3
  br i1 %init.is.float, label %if.init.is.float, label %if.init.not.float

if.init.is.float:                                 ; preds = %if.else.init
  %init.n.ptr = getelementptr inbounds %ravi.TValue* %13, i32 0, i32 0, i32 0
  %init.n = load double* %init.n.ptr, !tbaa !15
  store double %init.n, double* %ninit, !tbaa !5
  br label %if.else.done

if.init.not.float:                                ; preds = %if.else.init
  %20 = call i32 @luaV_tonumber_(%ravi.TValue* %13, double* %ninit)
  %init.float.ok = icmp eq i32 %20, 0
  br i1 %init.float.ok, label %if.init.float.failed, label %if.else.done

if.init.float.failed:                             ; preds = %if.init.not.float
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([37 x i8]* @2, i32 0, i32 0))
  br label %if.else.done

if.else.done:                                     ; preds = %if.init.float.failed, %if.init.not.float, %if.init.is.float
  %ninit12 = load double* %ninit, !tbaa !5
  %nstep13 = load double* %nstep, !tbaa !5
  %ninit-nstep = fsub double %ninit12, %nstep13
  store volatile double %ninit-nstep, double* %ninit, !tbaa !5
  %step.gt.zero14 = fcmp ogt double %nstep13, 0.000000e+00
  %.49 = select i1 %step.gt.zero14, i8* blockaddress(@ravif1, %forloop_flt8_), i8* blockaddress(@ravif1, %forloop_fgt8_)
  br i1 %step.gt.zero14, label %forloop_flt8_, label %forloop_fgt8_

forbody7_:                                        ; preds = %updatef, %updatei
  %iidx.0 = phi i64 [ %iidx.3, %updatei ], [ %iidx.6, %updatef ]
  %brnch.2 = phi i8* [ %brnch.5, %updatei ], [ %brnch.8, %updatef ]
  %istep.0 = phi i64 [ %istep.3, %updatei ], [ %istep.6, %updatef ]
  %21 = bitcast %ravi.TValue** %base to double**
  %22 = load double** %21, !tbaa !14
  %23 = load double* %22, !tbaa !15
  %24 = fadd double %23, 1.000000e+00
  store double %24, double* %22, !tbaa !15
  indirectbr i8* %brnch.2, [label %forloop_ilt8_, label %forloop_igt8_, label %forloop_flt8_, label %forloop_fgt8_]

forloop_ilt8_:                                    ; preds = %forbody7_, %if.stopnow.notzero
  %iidx.1 = phi i64 [ %initv-pstep.i, %if.stopnow.notzero ], [ %iidx.0, %forbody7_ ]
  %brnch.3 = phi i8* [ %., %if.stopnow.notzero ], [ %brnch.2, %forbody7_ ]
  %istep.1 = phi i64 [ %step.i, %if.stopnow.notzero ], [ %istep.0, %forbody7_ ]
  %next.idx = add nsw i64 %istep.1, %iidx.1
  %limit.i = load i64* %ilimit, !tbaa !5
  %idx.gt.limit = icmp sgt i64 %next.idx, %limit.i
  br i1 %idx.gt.limit, label %exit_iforloop, label %updatei

forloop_igt8_:                                    ; preds = %forbody7_, %if.stopnow.notzero
  %iidx.2 = phi i64 [ %iidx.0, %forbody7_ ], [ %initv-pstep.i, %if.stopnow.notzero ]
  %brnch.4 = phi i8* [ %brnch.2, %forbody7_ ], [ %., %if.stopnow.notzero ]
  %istep.2 = phi i64 [ %istep.0, %forbody7_ ], [ %step.i, %if.stopnow.notzero ]
  %next.idx22 = add nsw i64 %istep.2, %iidx.2
  %limit.i23 = load i64* %ilimit, !tbaa !5
  %limit.gt.idx = icmp sgt i64 %limit.i23, %next.idx22
  br i1 %limit.gt.idx, label %exit_iforloop, label %updatei

updatei:                                          ; preds = %forloop_igt8_, %forloop_ilt8_
  %iidx.3 = phi i64 [ %next.idx, %forloop_ilt8_ ], [ %next.idx22, %forloop_igt8_ ]
  %brnch.5 = phi i8* [ %brnch.3, %forloop_ilt8_ ], [ %brnch.4, %forloop_igt8_ ]
  %istep.3 = phi i64 [ %istep.1, %forloop_ilt8_ ], [ %istep.2, %forloop_igt8_ ]
  %25 = load %ravi.TValue** %base, !tbaa !14
  %26 = getelementptr inbounds %ravi.TValue* %25, i32 4
  %var.tt.ptr = getelementptr inbounds %ravi.TValue* %25, i32 4, i32 1
  %var.i = bitcast %ravi.TValue* %26 to i64*
  store i64 %iidx.3, i64* %var.i, !tbaa !15
  store i32 19, i32* %var.tt.ptr, !tbaa !17
  br label %forbody7_

forloop_flt8_:                                    ; preds = %forbody7_, %if.else.done
  %iidx.4 = phi i64 [ %iidx.0, %forbody7_ ], [ undef, %if.else.done ]
  %brnch.6 = phi i8* [ %brnch.2, %forbody7_ ], [ %.49, %if.else.done ]
  %istep.4 = phi i64 [ %istep.0, %forbody7_ ], [ undef, %if.else.done ]
  %step.n25 = load double* %nstep, !tbaa !5
  %init.n26 = load double* %ninit, !tbaa !5
  %next.idx27 = fadd double %step.n25, %init.n26
  store double %next.idx27, double* %ninit, !tbaa !5
  %limit.n28 = load double* %nlimit, !tbaa !5
  %idx.gt.limit29 = fcmp ogt double %next.idx27, %limit.n28
  br i1 %idx.gt.limit29, label %exit_iforloop, label %updatef

forloop_fgt8_:                                    ; preds = %forbody7_, %if.else.done
  %iidx.5 = phi i64 [ %iidx.0, %forbody7_ ], [ undef, %if.else.done ]
  %brnch.7 = phi i8* [ %brnch.2, %forbody7_ ], [ %.49, %if.else.done ]
  %istep.5 = phi i64 [ %istep.0, %forbody7_ ], [ undef, %if.else.done ]
  %step.n30 = load double* %nstep, !tbaa !5
  %init.n31 = load double* %ninit, !tbaa !5
  %next.idx32 = fadd double %step.n30, %init.n31
  store double %next.idx32, double* %ninit, !tbaa !5
  %limit.n33 = load double* %nlimit, !tbaa !5
  %limit.gt.idx34 = fcmp ogt double %limit.n33, %next.idx32
  br i1 %limit.gt.idx34, label %exit_iforloop, label %updatef

updatef:                                          ; preds = %forloop_fgt8_, %forloop_flt8_
  %init.n36 = phi double [ %next.idx32, %forloop_fgt8_ ], [ %next.idx27, %forloop_flt8_ ]
  %iidx.6 = phi i64 [ %iidx.5, %forloop_fgt8_ ], [ %iidx.4, %forloop_flt8_ ]
  %brnch.8 = phi i8* [ %brnch.7, %forloop_fgt8_ ], [ %brnch.6, %forloop_flt8_ ]
  %istep.6 = phi i64 [ %istep.5, %forloop_fgt8_ ], [ %istep.4, %forloop_flt8_ ]
  %27 = load %ravi.TValue** %base, !tbaa !14
  %var.tt.ptr35 = getelementptr inbounds %ravi.TValue* %27, i32 4, i32 1
  %var.n = getelementptr inbounds %ravi.TValue* %27, i32 4, i32 0, i32 0
  store double %init.n36, double* %var.n, !tbaa !15
  store i32 3, i32* %var.tt.ptr35, !tbaa !17
  br label %forbody7_

exit_iforloop:                                    ; preds = %forloop_fgt8_, %forloop_flt8_, %forloop_igt8_, %forloop_ilt8_
  %28 = load %ravi.TValue** %base, !tbaa !14
  %29 = getelementptr inbounds %ravi.TValue* %28, i32 1
  %L_top = getelementptr inbounds %ravi.lua_State* %L, i32 0, i32 4
  store %ravi.TValue* %29, %ravi.TValue** %L_top, !tbaa !18
  %sizep = getelementptr inbounds %ravi.Proto* %4, i32 0, i32 10
  %30 = load i32* %sizep
  %31 = icmp sgt i32 %30, 0
  br i1 %31, label %if.then, label %if.else

if.then:                                          ; preds = %exit_iforloop
  call void @luaF_close(%ravi.lua_State* %L, %ravi.TValue* %28)
  br label %if.else

if.else:                                          ; preds = %if.then, %exit_iforloop
  %32 = call i32 @luaD_poscall(%ravi.lua_State* %L, %ravi.TValue* %28)
  %33 = icmp eq i32 %32, 0
  br i1 %33, label %if.else38, label %if.then37

if.then37:                                        ; preds = %if.else
  %ci_top = getelementptr inbounds %ravi.CallInfo* %0, i32 0, i32 1
  %34 = load %ravi.TValue** %ci_top
  store %ravi.TValue* %34, %ravi.TValue** %L_top
  br label %if.else38

if.else38:                                        ; preds = %if.else, %if.then37
  ret i32 1
}

declare i32 @luaD_poscall(%ravi.lua_State*, %ravi.TValue*)

declare void @luaF_close(%ravi.lua_State*, %ravi.TValue*)

declare void @luaG_runerror(%ravi.lua_State*, i8*)

declare i32 @luaV_forlimit(%ravi.TValue*, i64*, i64, i32*)

declare i32 @luaV_tonumber_(%ravi.TValue*, double*)

declare void @luaV_op_loadnil(%ravi.CallInfo*, i32, i32)

!0 = !{!"CallInfo", !1, i64 0, !1, i64 4, !1, i64 8, !1, i64 12, !4, i64 16, !5, i64 32, !6, i64 40, !2, i64 42}
!1 = !{!"any pointer", !2, i64 0}
!2 = !{!"omnipotent char", !3, i64 0}
!3 = !{!"Simple C / C++ TBAA"}
!4 = !{!"CallInfo_l", !1, i64 0, !1, i64 4, !5, i64 8}
!5 = !{!"long long", !2, i64 0}
!6 = !{!"short", !2, i64 0}
!7 = !{!0, !1, i64 0}
!8 = !{!1, !1, i64 0}
!9 = !{!10, !1, i64 12}
!10 = !{!"LClosure", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !1, i64 8, !1, i64 12, !2, i64 16}
!11 = !{!12, !1, i64 44}
!12 = !{!"Proto", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !2, i64 7, !2, i64 8, !13, i64 12, !13, i64 16, !13, i64 20, !13, i64 24, !13, i64 28, !13, i64 32, !13, i64 36, !13, i64 40, !1, i64 44, !1, i64 48, !1, i64 52, !1, i64 56, !1, i64 60, !1, i64 64, !1, i64 68, !1, i64 72, !1, i64 76}
!13 = !{!"int", !2, i64 0}
!14 = !{!0, !1, i64 16}
!15 = !{!16, !5, i64 0}
!16 = !{!"TValue", !5, i64 0, !13, i64 8}
!17 = !{!16, !13, i64 8}
!18 = !{!19, !1, i64 8}
!19 = !{!"lua_State", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !1, i64 8, !1, i64 12, !1, i64 16, !1, i64 20, !1, i64 24, !1, i64 28, !1, i64 32, !1, i64 36, !1, i64 40, !1, i64 44, !0, i64 48, !1, i64 92, !5, i64 96, !13, i64 104, !13, i64 108, !13, i64 112, !6, i64 114, !6, i64 116, !2, i64 118, !2, i64 119}
