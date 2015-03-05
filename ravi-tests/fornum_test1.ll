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
  %6 = load %ravi.TValue** %base, !tbaa !14
  %srcvalue = getelementptr inbounds %ravi.TValue* %5, i32 0, i32 0, i32 0
  %destvalue = getelementptr inbounds %ravi.TValue* %6, i32 0, i32 0, i32 0
  %7 = load double* %srcvalue, !tbaa !15
  store double %7, double* %destvalue, !tbaa !15
  %srctype = getelementptr inbounds %ravi.TValue* %5, i32 0, i32 1
  %desttype = getelementptr inbounds %ravi.TValue* %6, i32 0, i32 1
  %8 = load i32* %srctype, !tbaa !17
  store i32 %8, i32* %desttype, !tbaa !17
  %srcvalue1 = getelementptr inbounds %ravi.TValue* %5, i32 1, i32 0, i32 0
  %destvalue2 = getelementptr inbounds %ravi.TValue* %6, i32 1, i32 0, i32 0
  %9 = load double* %srcvalue1, !tbaa !15
  store double %9, double* %destvalue2, !tbaa !15
  %srctype3 = getelementptr inbounds %ravi.TValue* %5, i32 1, i32 1
  %desttype4 = getelementptr inbounds %ravi.TValue* %6, i32 1, i32 1
  %10 = load i32* %srctype3, !tbaa !17
  store i32 %10, i32* %desttype4, !tbaa !17
  %srcvalue5 = getelementptr inbounds %ravi.TValue* %5, i32 2, i32 0, i32 0
  %destvalue6 = getelementptr inbounds %ravi.TValue* %6, i32 2, i32 0, i32 0
  %11 = load double* %srcvalue5, !tbaa !15
  store double %11, double* %destvalue6, !tbaa !15
  %srctype7 = getelementptr inbounds %ravi.TValue* %5, i32 2, i32 1
  %desttype8 = getelementptr inbounds %ravi.TValue* %6, i32 2, i32 1
  %12 = load i32* %srctype7, !tbaa !17
  store i32 %12, i32* %desttype8, !tbaa !17
  %srcvalue9 = getelementptr inbounds %ravi.TValue* %5, i32 3, i32 0, i32 0
  %destvalue10 = getelementptr inbounds %ravi.TValue* %6, i32 3, i32 0, i32 0
  %13 = load double* %srcvalue9, !tbaa !15
  store double %13, double* %destvalue10, !tbaa !15
  %srctype11 = getelementptr inbounds %ravi.TValue* %5, i32 3, i32 1
  %desttype12 = getelementptr inbounds %ravi.TValue* %6, i32 3, i32 1
  %14 = load i32* %srctype11, !tbaa !17
  store i32 %14, i32* %desttype12, !tbaa !17
  %15 = getelementptr inbounds %ravi.TValue* %6, i32 1
  %16 = getelementptr inbounds %ravi.TValue* %6, i32 2
  %17 = getelementptr inbounds %ravi.TValue* %6, i32 3
  %init.tt = load i32* %desttype4, !tbaa !17
  %init.is.integer = icmp eq i32 %init.tt, 19
  %step.is.integer = icmp eq i32 %14, 19
  %step.i.ptr = bitcast %ravi.TValue* %17 to i64*
  %step.i = load i64* %step.i.ptr, !tbaa !15
  %18 = call i32 @luaV_forlimit(%ravi.TValue* %16, i64* %ilimit, i64 %step.i, i32* %stopnow)
  %init.and.step.are.integers = and i1 %init.is.integer, %step.is.integer
  %19 = icmp ne i32 %18, 0
  %all.integers = and i1 %init.and.step.are.integers, %19
  br i1 %all.integers, label %if.all.integers, label %if.not.all.integers

if.all.integers:                                  ; preds = %entry
  %stopnow.value = load i32* %stopnow, !tbaa !13
  %stopnow.is.zero = icmp eq i32 %stopnow.value, 0
  br i1 %stopnow.is.zero, label %if.stopnow.iszero, label %if.stopnow.notzero

if.stopnow.iszero:                                ; preds = %if.all.integers
  %init.i.ptr = bitcast %ravi.TValue* %15 to i64*
  %init.i = load i64* %init.i.ptr, !tbaa !15
  br label %if.stopnow.notzero

if.stopnow.notzero:                               ; preds = %if.stopnow.iszero, %if.all.integers
  %initv = phi i64 [ %init.i, %if.stopnow.iszero ], [ 0, %if.all.integers ]
  %initv-pstep.i = sub nsw i64 %initv, %step.i
  %step.gt.zero = icmp sgt i64 %step.i, 0
  %. = select i1 %step.gt.zero, i8* blockaddress(@ravif1, %forloop_ilt7_), i8* blockaddress(@ravif1, %forloop_igt7_)
  br i1 %step.gt.zero, label %forloop_ilt7_, label %forloop_igt7_

if.not.all.integers:                              ; preds = %entry
  %limit.tt = load i32* %desttype8, !tbaa !17
  %limit.is.float = icmp eq i32 %limit.tt, 3
  br i1 %limit.is.float, label %if.limit.isfloat, label %if.limit.notfloat

if.limit.isfloat:                                 ; preds = %if.not.all.integers
  %limit.n.ptr = getelementptr inbounds %ravi.TValue* %16, i32 0, i32 0, i32 0
  %limit.n = load double* %limit.n.ptr, !tbaa !15
  store volatile double %limit.n, double* %nlimit, !tbaa !5
  br label %if.else.step

if.limit.notfloat:                                ; preds = %if.not.all.integers
  %20 = call i32 @luaV_tonumber_(%ravi.TValue* %16, double* %nlimit)
  %limit.float.ok = icmp eq i32 %20, 0
  br i1 %limit.float.ok, label %if.limit.float.failed, label %if.else.step

if.limit.float.failed:                            ; preds = %if.limit.notfloat
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([29 x i8]* @0, i32 0, i32 0))
  br label %if.else.step

if.else.step:                                     ; preds = %if.limit.float.failed, %if.limit.notfloat, %if.limit.isfloat
  %step.tt14 = load i32* %desttype12, !tbaa !17
  %step.is.float = icmp eq i32 %step.tt14, 3
  br i1 %step.is.float, label %if.step.isfloat, label %if.step.notfloat

if.step.isfloat:                                  ; preds = %if.else.step
  %step.n.ptr = getelementptr inbounds %ravi.TValue* %17, i32 0, i32 0, i32 0
  %step.n = load double* %step.n.ptr, !tbaa !15
  store volatile double %step.n, double* %nstep, !tbaa !5
  br label %if.else.init

if.step.notfloat:                                 ; preds = %if.else.step
  %21 = call i32 @luaV_tonumber_(%ravi.TValue* %17, double* %nstep)
  %step.float.ok = icmp eq i32 %21, 0
  br i1 %step.float.ok, label %if.step.float.failed, label %if.else.init

if.step.float.failed:                             ; preds = %if.step.notfloat
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([28 x i8]* @1, i32 0, i32 0))
  br label %if.else.init

if.else.init:                                     ; preds = %if.step.float.failed, %if.step.notfloat, %if.step.isfloat
  %init.tt15 = load i32* %desttype4, !tbaa !17
  %init.is.float = icmp eq i32 %init.tt15, 3
  br i1 %init.is.float, label %if.init.is.float, label %if.init.not.float

if.init.is.float:                                 ; preds = %if.else.init
  %init.n.ptr = getelementptr inbounds %ravi.TValue* %15, i32 0, i32 0, i32 0
  %init.n = load double* %init.n.ptr, !tbaa !15
  store double %init.n, double* %ninit, !tbaa !5
  br label %if.else.done

if.init.not.float:                                ; preds = %if.else.init
  %22 = call i32 @luaV_tonumber_(%ravi.TValue* %15, double* %ninit)
  %init.float.ok = icmp eq i32 %22, 0
  br i1 %init.float.ok, label %if.init.float.failed, label %if.else.done

if.init.float.failed:                             ; preds = %if.init.not.float
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([37 x i8]* @2, i32 0, i32 0))
  br label %if.else.done

if.else.done:                                     ; preds = %if.init.float.failed, %if.init.not.float, %if.init.is.float
  %ninit16 = load double* %ninit, !tbaa !5
  %nstep17 = load double* %nstep, !tbaa !5
  %ninit-nstep = fsub double %ninit16, %nstep17
  store volatile double %ninit-nstep, double* %ninit, !tbaa !5
  %step.gt.zero18 = fcmp ogt double %nstep17, 0.000000e+00
  %.53 = select i1 %step.gt.zero18, i8* blockaddress(@ravif1, %forloop_flt7_), i8* blockaddress(@ravif1, %forloop_fgt7_)
  br i1 %step.gt.zero18, label %forloop_flt7_, label %forloop_fgt7_

forbody6_:                                        ; preds = %updatef, %updatei
  %23 = phi i32 [ 19, %updatei ], [ 3, %updatef ]
  %24 = phi double [ %.pre, %updatei ], [ %init.n40, %updatef ]
  %25 = phi %ravi.TValue* [ %26, %updatei ], [ %28, %updatef ]
  %iidx.0 = phi i64 [ %iidx.3, %updatei ], [ %iidx.6, %updatef ]
  %brnch.2 = phi i8* [ %brnch.5, %updatei ], [ %brnch.8, %updatef ]
  %istep.0 = phi i64 [ %istep.3, %updatei ], [ %istep.6, %updatef ]
  %dest.value = getelementptr inbounds %ravi.TValue* %25, i32 0, i32 0, i32 0
  store double %24, double* %dest.value, !tbaa !15
  %dest.tt = getelementptr inbounds %ravi.TValue* %25, i32 0, i32 1
  store i32 %23, i32* %dest.tt, !tbaa !17
  indirectbr i8* %brnch.2, [label %forloop_ilt7_, label %forloop_igt7_, label %forloop_flt7_, label %forloop_fgt7_]

forloop_ilt7_:                                    ; preds = %forbody6_, %if.stopnow.notzero
  %iidx.1 = phi i64 [ %initv-pstep.i, %if.stopnow.notzero ], [ %iidx.0, %forbody6_ ]
  %brnch.3 = phi i8* [ %., %if.stopnow.notzero ], [ %brnch.2, %forbody6_ ]
  %istep.1 = phi i64 [ %step.i, %if.stopnow.notzero ], [ %istep.0, %forbody6_ ]
  %next.idx = add nsw i64 %istep.1, %iidx.1
  %limit.i = load i64* %ilimit, !tbaa !5
  %idx.gt.limit = icmp sgt i64 %next.idx, %limit.i
  br i1 %idx.gt.limit, label %exit_iforloop, label %updatei

forloop_igt7_:                                    ; preds = %forbody6_, %if.stopnow.notzero
  %iidx.2 = phi i64 [ %iidx.0, %forbody6_ ], [ %initv-pstep.i, %if.stopnow.notzero ]
  %brnch.4 = phi i8* [ %brnch.2, %forbody6_ ], [ %., %if.stopnow.notzero ]
  %istep.2 = phi i64 [ %istep.0, %forbody6_ ], [ %step.i, %if.stopnow.notzero ]
  %next.idx26 = add nsw i64 %istep.2, %iidx.2
  %limit.i27 = load i64* %ilimit, !tbaa !5
  %limit.gt.idx = icmp sgt i64 %limit.i27, %next.idx26
  br i1 %limit.gt.idx, label %exit_iforloop, label %updatei

updatei:                                          ; preds = %forloop_igt7_, %forloop_ilt7_
  %iidx.3 = phi i64 [ %next.idx, %forloop_ilt7_ ], [ %next.idx26, %forloop_igt7_ ]
  %brnch.5 = phi i8* [ %brnch.3, %forloop_ilt7_ ], [ %brnch.4, %forloop_igt7_ ]
  %istep.3 = phi i64 [ %istep.1, %forloop_ilt7_ ], [ %istep.2, %forloop_igt7_ ]
  %26 = load %ravi.TValue** %base, !tbaa !14
  %27 = getelementptr inbounds %ravi.TValue* %26, i32 4
  %var.tt.ptr = getelementptr inbounds %ravi.TValue* %26, i32 4, i32 1
  %var.i = bitcast %ravi.TValue* %27 to i64*
  store i64 %iidx.3, i64* %var.i, !tbaa !15
  store i32 19, i32* %var.tt.ptr, !tbaa !17
  %src.value.phi.trans.insert = getelementptr inbounds %ravi.TValue* %26, i32 4, i32 0, i32 0
  %.pre = load double* %src.value.phi.trans.insert, !tbaa !15
  br label %forbody6_

forloop_flt7_:                                    ; preds = %forbody6_, %if.else.done
  %iidx.4 = phi i64 [ %iidx.0, %forbody6_ ], [ undef, %if.else.done ]
  %brnch.6 = phi i8* [ %brnch.2, %forbody6_ ], [ %.53, %if.else.done ]
  %istep.4 = phi i64 [ %istep.0, %forbody6_ ], [ undef, %if.else.done ]
  %step.n29 = load double* %nstep, !tbaa !5
  %init.n30 = load double* %ninit, !tbaa !5
  %next.idx31 = fadd double %step.n29, %init.n30
  store double %next.idx31, double* %ninit, !tbaa !5
  %limit.n32 = load double* %nlimit, !tbaa !5
  %idx.gt.limit33 = fcmp ogt double %next.idx31, %limit.n32
  br i1 %idx.gt.limit33, label %exit_iforloop, label %updatef

forloop_fgt7_:                                    ; preds = %forbody6_, %if.else.done
  %iidx.5 = phi i64 [ %iidx.0, %forbody6_ ], [ undef, %if.else.done ]
  %brnch.7 = phi i8* [ %brnch.2, %forbody6_ ], [ %.53, %if.else.done ]
  %istep.5 = phi i64 [ %istep.0, %forbody6_ ], [ undef, %if.else.done ]
  %step.n34 = load double* %nstep, !tbaa !5
  %init.n35 = load double* %ninit, !tbaa !5
  %next.idx36 = fadd double %step.n34, %init.n35
  store double %next.idx36, double* %ninit, !tbaa !5
  %limit.n37 = load double* %nlimit, !tbaa !5
  %limit.gt.idx38 = fcmp ogt double %limit.n37, %next.idx36
  br i1 %limit.gt.idx38, label %exit_iforloop, label %updatef

updatef:                                          ; preds = %forloop_fgt7_, %forloop_flt7_
  %init.n40 = phi double [ %next.idx36, %forloop_fgt7_ ], [ %next.idx31, %forloop_flt7_ ]
  %iidx.6 = phi i64 [ %iidx.5, %forloop_fgt7_ ], [ %iidx.4, %forloop_flt7_ ]
  %brnch.8 = phi i8* [ %brnch.7, %forloop_fgt7_ ], [ %brnch.6, %forloop_flt7_ ]
  %istep.6 = phi i64 [ %istep.5, %forloop_fgt7_ ], [ %istep.4, %forloop_flt7_ ]
  %28 = load %ravi.TValue** %base, !tbaa !14
  %var.tt.ptr39 = getelementptr inbounds %ravi.TValue* %28, i32 4, i32 1
  %var.n = getelementptr inbounds %ravi.TValue* %28, i32 4, i32 0, i32 0
  store double %init.n40, double* %var.n, !tbaa !15
  store i32 3, i32* %var.tt.ptr39, !tbaa !17
  br label %forbody6_

exit_iforloop:                                    ; preds = %forloop_fgt7_, %forloop_flt7_, %forloop_igt7_, %forloop_ilt7_
  %29 = load %ravi.TValue** %base, !tbaa !14
  %30 = getelementptr inbounds %ravi.TValue* %29, i32 1
  %L_top = getelementptr inbounds %ravi.lua_State* %L, i32 0, i32 4
  store %ravi.TValue* %30, %ravi.TValue** %L_top, !tbaa !18
  %sizep = getelementptr inbounds %ravi.Proto* %4, i32 0, i32 10
  %31 = load i32* %sizep
  %32 = icmp sgt i32 %31, 0
  br i1 %32, label %if.then, label %if.else

if.then:                                          ; preds = %exit_iforloop
  call void @luaF_close(%ravi.lua_State* %L, %ravi.TValue* %29)
  br label %if.else

if.else:                                          ; preds = %if.then, %exit_iforloop
  %33 = call i32 @luaD_poscall(%ravi.lua_State* %L, %ravi.TValue* %29)
  %34 = icmp eq i32 %33, 0
  br i1 %34, label %if.else42, label %if.then41

if.then41:                                        ; preds = %if.else
  %ci_top = getelementptr inbounds %ravi.CallInfo* %0, i32 0, i32 1
  %35 = load %ravi.TValue** %ci_top
  store %ravi.TValue* %35, %ravi.TValue** %L_top
  br label %if.else42

if.else42:                                        ; preds = %if.else, %if.then41
  ret i32 1
}

declare i32 @luaD_poscall(%ravi.lua_State*, %ravi.TValue*)

declare void @luaF_close(%ravi.lua_State*, %ravi.TValue*)

declare void @luaG_runerror(%ravi.lua_State*, i8*)

declare i32 @luaV_forlimit(%ravi.TValue*, i64*, i64, i32*)

declare i32 @luaV_tonumber_(%ravi.TValue*, double*)

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
