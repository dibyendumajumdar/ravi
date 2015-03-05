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
  %stopnow35 = alloca i32
  %ilimit36 = alloca i64
  %nlimit39 = alloca double
  %ninit40 = alloca double
  %nstep41 = alloca double
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
  %. = select i1 %step.gt.zero, i8* blockaddress(@ravif1, %forloop_ilt13_), i8* blockaddress(@ravif1, %forloop_igt13_)
  br i1 %step.gt.zero, label %forloop_ilt13_, label %forloop_igt13_

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
  %.171 = select i1 %step.gt.zero18, i8* blockaddress(@ravif1, %forloop_flt13_), i8* blockaddress(@ravif1, %forloop_fgt13_)
  br i1 %step.gt.zero18, label %forloop_flt13_, label %forloop_fgt13_

forbody6_:                                        ; preds = %updatef151, %updatei137
  %23 = phi i32 [ 19, %updatei137 ], [ 3, %updatef151 ]
  %24 = phi double [ %.pre, %updatei137 ], [ %init.n154, %updatef151 ]
  %base34 = phi %ravi.TValue* [ %45, %updatei137 ], [ %47, %updatef151 ]
  %iidx38.0 = phi i64 [ %iidx38.11, %updatei137 ], [ %iidx38.14, %updatef151 ]
  %istep.0 = phi i64 [ %istep.3, %updatei137 ], [ %istep.6, %updatef151 ]
  %iidx.0 = phi i64 [ %iidx.3, %updatei137 ], [ %iidx.6, %updatef151 ]
  %brnch.2 = phi i8* [ %brnch.5, %updatei137 ], [ %brnch.8, %updatef151 ]
  %istep37.0 = phi i64 [ %istep37.11, %updatei137 ], [ %istep37.14, %updatef151 ]
  %dest.value = getelementptr inbounds %ravi.TValue* %base34, i32 0, i32 0, i32 0
  store double %24, double* %dest.value, !tbaa !15
  %dest.tt = getelementptr inbounds %ravi.TValue* %base34, i32 0, i32 1
  store i32 %23, i32* %dest.tt, !tbaa !17
  %srcvalue22 = getelementptr inbounds %ravi.TValue* %5, i32 4, i32 0, i32 0
  %destvalue23 = getelementptr inbounds %ravi.TValue* %base34, i32 5, i32 0, i32 0
  %25 = load double* %srcvalue22, !tbaa !15
  store double %25, double* %destvalue23, !tbaa !15
  %srctype24 = getelementptr inbounds %ravi.TValue* %5, i32 4, i32 1
  %desttype25 = getelementptr inbounds %ravi.TValue* %base34, i32 5, i32 1
  %26 = load i32* %srctype24, !tbaa !17
  store i32 %26, i32* %desttype25, !tbaa !17
  %srcvalue26 = getelementptr inbounds %ravi.TValue* %5, i32 5, i32 0, i32 0
  %destvalue27 = getelementptr inbounds %ravi.TValue* %base34, i32 6, i32 0, i32 0
  %27 = load double* %srcvalue26, !tbaa !15
  store double %27, double* %destvalue27, !tbaa !15
  %srctype28 = getelementptr inbounds %ravi.TValue* %5, i32 5, i32 1
  %desttype29 = getelementptr inbounds %ravi.TValue* %base34, i32 6, i32 1
  %28 = load i32* %srctype28, !tbaa !17
  store i32 %28, i32* %desttype29, !tbaa !17
  %destvalue31 = getelementptr inbounds %ravi.TValue* %base34, i32 7, i32 0, i32 0
  %29 = load double* %srcvalue9, !tbaa !15
  store double %29, double* %destvalue31, !tbaa !15
  %desttype33 = getelementptr inbounds %ravi.TValue* %base34, i32 7, i32 1
  %30 = load i32* %srctype11, !tbaa !17
  store i32 %30, i32* %desttype33, !tbaa !17
  %31 = getelementptr inbounds %ravi.TValue* %base34, i32 5
  %32 = getelementptr inbounds %ravi.TValue* %base34, i32 6
  %33 = getelementptr inbounds %ravi.TValue* %base34, i32 7
  %init.tt46 = load i32* %desttype25, !tbaa !17
  %init.is.integer47 = icmp eq i32 %init.tt46, 19
  %step.is.integer50 = icmp eq i32 %30, 19
  %step.i.ptr51 = bitcast %ravi.TValue* %33 to i64*
  %step.i52 = load i64* %step.i.ptr51, !tbaa !15
  %34 = call i32 @luaV_forlimit(%ravi.TValue* %32, i64* %ilimit36, i64 %step.i52, i32* %stopnow35)
  %init.and.step.are.integers53 = and i1 %init.is.integer47, %step.is.integer50
  %35 = icmp ne i32 %34, 0
  %all.integers54 = and i1 %init.and.step.are.integers53, %35
  br i1 %all.integers54, label %if.all.integers55, label %if.not.all.integers68

if.all.integers55:                                ; preds = %forbody6_
  %stopnow.value56 = load i32* %stopnow35, !tbaa !13
  %stopnow.is.zero57 = icmp eq i32 %stopnow.value56, 0
  br i1 %stopnow.is.zero57, label %if.stopnow.iszero59, label %if.stopnow.notzero61

if.stopnow.iszero59:                              ; preds = %if.all.integers55
  %init.i.ptr58 = bitcast %ravi.TValue* %31 to i64*
  %init.i60 = load i64* %init.i.ptr58, !tbaa !15
  br label %if.stopnow.notzero61

if.stopnow.notzero61:                             ; preds = %if.stopnow.iszero59, %if.all.integers55
  %initv62 = phi i64 [ %init.i60, %if.stopnow.iszero59 ], [ 0, %if.all.integers55 ]
  %initv-pstep.i63 = sub nsw i64 %initv62, %step.i52
  %step.gt.zero64 = icmp sgt i64 %step.i52, 0
  %.172 = select i1 %step.gt.zero64, i8* blockaddress(@ravif1, %forloop_ilt12_), i8* blockaddress(@ravif1, %forloop_igt12_)
  br i1 %step.gt.zero64, label %forloop_ilt12_, label %forloop_igt12_

if.not.all.integers68:                            ; preds = %forbody6_
  %limit.tt70 = load i32* %desttype29, !tbaa !17
  %limit.is.float71 = icmp eq i32 %limit.tt70, 3
  br i1 %limit.is.float71, label %if.limit.isfloat72, label %if.limit.notfloat75

if.limit.isfloat72:                               ; preds = %if.not.all.integers68
  %limit.n.ptr73 = getelementptr inbounds %ravi.TValue* %32, i32 0, i32 0, i32 0
  %limit.n74 = load double* %limit.n.ptr73, !tbaa !15
  store volatile double %limit.n74, double* %nlimit39, !tbaa !5
  br label %if.else.step78

if.limit.notfloat75:                              ; preds = %if.not.all.integers68
  %36 = call i32 @luaV_tonumber_(%ravi.TValue* %32, double* %nlimit39)
  %limit.float.ok76 = icmp eq i32 %36, 0
  br i1 %limit.float.ok76, label %if.limit.float.failed77, label %if.else.step78

if.limit.float.failed77:                          ; preds = %if.limit.notfloat75
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([29 x i8]* @0, i32 0, i32 0))
  br label %if.else.step78

if.else.step78:                                   ; preds = %if.limit.float.failed77, %if.limit.notfloat75, %if.limit.isfloat72
  %step.tt79 = load i32* %desttype33, !tbaa !17
  %step.is.float80 = icmp eq i32 %step.tt79, 3
  br i1 %step.is.float80, label %if.step.isfloat81, label %if.step.notfloat84

if.step.isfloat81:                                ; preds = %if.else.step78
  %step.n.ptr82 = getelementptr inbounds %ravi.TValue* %33, i32 0, i32 0, i32 0
  %step.n83 = load double* %step.n.ptr82, !tbaa !15
  store volatile double %step.n83, double* %nstep41, !tbaa !5
  br label %if.else.init87

if.step.notfloat84:                               ; preds = %if.else.step78
  %37 = call i32 @luaV_tonumber_(%ravi.TValue* %33, double* %nstep41)
  %step.float.ok85 = icmp eq i32 %37, 0
  br i1 %step.float.ok85, label %if.step.float.failed86, label %if.else.init87

if.step.float.failed86:                           ; preds = %if.step.notfloat84
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([28 x i8]* @1, i32 0, i32 0))
  br label %if.else.init87

if.else.init87:                                   ; preds = %if.step.float.failed86, %if.step.notfloat84, %if.step.isfloat81
  %init.tt88 = load i32* %desttype25, !tbaa !17
  %init.is.float89 = icmp eq i32 %init.tt88, 3
  br i1 %init.is.float89, label %if.init.is.float90, label %if.init.not.float93

if.init.is.float90:                               ; preds = %if.else.init87
  %init.n.ptr91 = getelementptr inbounds %ravi.TValue* %31, i32 0, i32 0, i32 0
  %init.n92 = load double* %init.n.ptr91, !tbaa !15
  store double %init.n92, double* %ninit40, !tbaa !5
  br label %if.else.done96

if.init.not.float93:                              ; preds = %if.else.init87
  %38 = call i32 @luaV_tonumber_(%ravi.TValue* %31, double* %ninit40)
  %init.float.ok94 = icmp eq i32 %38, 0
  br i1 %init.float.ok94, label %if.init.float.failed95, label %if.else.done96

if.init.float.failed95:                           ; preds = %if.init.not.float93
  call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([37 x i8]* @2, i32 0, i32 0))
  br label %if.else.done96

if.else.done96:                                   ; preds = %if.init.float.failed95, %if.init.not.float93, %if.init.is.float90
  %ninit97 = load double* %ninit40, !tbaa !5
  %nstep98 = load double* %nstep41, !tbaa !5
  %ninit-nstep99 = fsub double %ninit97, %nstep98
  store volatile double %ninit-nstep99, double* %ninit40, !tbaa !5
  %step.gt.zero100 = fcmp ogt double %nstep98, 0.000000e+00
  %.173 = select i1 %step.gt.zero100, i8* blockaddress(@ravif1, %forloop_flt12_), i8* blockaddress(@ravif1, %forloop_fgt12_)
  br i1 %step.gt.zero100, label %forloop_flt12_, label %forloop_fgt12_

forbody11_:                                       ; preds = %updatef, %updatei
  %39 = phi i32 [ 19, %updatei ], [ 3, %updatef ]
  %40 = phi double [ %.pre174, %updatei ], [ %init.n126, %updatef ]
  %41 = phi %ravi.TValue* [ %42, %updatei ], [ %44, %updatef ]
  %brnch42.2 = phi i8* [ %brnch42.5, %updatei ], [ %brnch42.8, %updatef ]
  %iidx38.1 = phi i64 [ %iidx38.4, %updatei ], [ %iidx38.7, %updatef ]
  %istep37.1 = phi i64 [ %istep37.4, %updatei ], [ %istep37.7, %updatef ]
  %dest.value105 = getelementptr inbounds %ravi.TValue* %41, i32 0, i32 0, i32 0
  store double %40, double* %dest.value105, !tbaa !15
  %dest.tt107 = getelementptr inbounds %ravi.TValue* %41, i32 0, i32 1
  store i32 %39, i32* %dest.tt107, !tbaa !17
  indirectbr i8* %brnch42.2, [label %forloop_ilt12_, label %forloop_igt12_, label %forloop_flt12_, label %forloop_fgt12_]

forloop_ilt12_:                                   ; preds = %forbody11_, %if.stopnow.notzero61
  %brnch42.3 = phi i8* [ %.172, %if.stopnow.notzero61 ], [ %brnch42.2, %forbody11_ ]
  %iidx38.2 = phi i64 [ %initv-pstep.i63, %if.stopnow.notzero61 ], [ %iidx38.1, %forbody11_ ]
  %istep37.2 = phi i64 [ %step.i52, %if.stopnow.notzero61 ], [ %istep37.1, %forbody11_ ]
  %next.idx = add nsw i64 %istep37.2, %iidx38.2
  %limit.i = load i64* %ilimit36, !tbaa !5
  %idx.gt.limit = icmp sgt i64 %next.idx, %limit.i
  br i1 %idx.gt.limit, label %exit_iforloop, label %updatei

forloop_igt12_:                                   ; preds = %forbody11_, %if.stopnow.notzero61
  %brnch42.4 = phi i8* [ %brnch42.2, %forbody11_ ], [ %.172, %if.stopnow.notzero61 ]
  %iidx38.3 = phi i64 [ %iidx38.1, %forbody11_ ], [ %initv-pstep.i63, %if.stopnow.notzero61 ]
  %istep37.3 = phi i64 [ %istep37.1, %forbody11_ ], [ %step.i52, %if.stopnow.notzero61 ]
  %next.idx112 = add nsw i64 %istep37.3, %iidx38.3
  %limit.i113 = load i64* %ilimit36, !tbaa !5
  %limit.gt.idx = icmp sgt i64 %limit.i113, %next.idx112
  br i1 %limit.gt.idx, label %exit_iforloop, label %updatei

updatei:                                          ; preds = %forloop_igt12_, %forloop_ilt12_
  %brnch42.5 = phi i8* [ %brnch42.3, %forloop_ilt12_ ], [ %brnch42.4, %forloop_igt12_ ]
  %iidx38.4 = phi i64 [ %next.idx, %forloop_ilt12_ ], [ %next.idx112, %forloop_igt12_ ]
  %istep37.4 = phi i64 [ %istep37.2, %forloop_ilt12_ ], [ %istep37.3, %forloop_igt12_ ]
  %42 = load %ravi.TValue** %base, !tbaa !14
  %43 = getelementptr inbounds %ravi.TValue* %42, i32 8
  %var.tt.ptr = getelementptr inbounds %ravi.TValue* %42, i32 8, i32 1
  %var.i = bitcast %ravi.TValue* %43 to i64*
  store i64 %iidx38.4, i64* %var.i, !tbaa !15
  store i32 19, i32* %var.tt.ptr, !tbaa !17
  %src.value104.phi.trans.insert = getelementptr inbounds %ravi.TValue* %42, i32 8, i32 0, i32 0
  %.pre174 = load double* %src.value104.phi.trans.insert, !tbaa !15
  br label %forbody11_

forloop_flt12_:                                   ; preds = %forbody11_, %if.else.done96
  %brnch42.6 = phi i8* [ %brnch42.2, %forbody11_ ], [ %.173, %if.else.done96 ]
  %iidx38.5 = phi i64 [ %iidx38.1, %forbody11_ ], [ %iidx38.0, %if.else.done96 ]
  %istep37.5 = phi i64 [ %istep37.1, %forbody11_ ], [ %istep37.0, %if.else.done96 ]
  %step.n115 = load double* %nstep41, !tbaa !5
  %init.n116 = load double* %ninit40, !tbaa !5
  %next.idx117 = fadd double %step.n115, %init.n116
  store double %next.idx117, double* %ninit40, !tbaa !5
  %limit.n118 = load double* %nlimit39, !tbaa !5
  %idx.gt.limit119 = fcmp ogt double %next.idx117, %limit.n118
  br i1 %idx.gt.limit119, label %exit_iforloop, label %updatef

forloop_fgt12_:                                   ; preds = %forbody11_, %if.else.done96
  %brnch42.7 = phi i8* [ %brnch42.2, %forbody11_ ], [ %.173, %if.else.done96 ]
  %iidx38.6 = phi i64 [ %iidx38.1, %forbody11_ ], [ %iidx38.0, %if.else.done96 ]
  %istep37.6 = phi i64 [ %istep37.1, %forbody11_ ], [ %istep37.0, %if.else.done96 ]
  %step.n120 = load double* %nstep41, !tbaa !5
  %init.n121 = load double* %ninit40, !tbaa !5
  %next.idx122 = fadd double %step.n120, %init.n121
  store double %next.idx122, double* %ninit40, !tbaa !5
  %limit.n123 = load double* %nlimit39, !tbaa !5
  %limit.gt.idx124 = fcmp ogt double %limit.n123, %next.idx122
  br i1 %limit.gt.idx124, label %exit_iforloop, label %updatef

updatef:                                          ; preds = %forloop_fgt12_, %forloop_flt12_
  %init.n126 = phi double [ %next.idx122, %forloop_fgt12_ ], [ %next.idx117, %forloop_flt12_ ]
  %brnch42.8 = phi i8* [ %brnch42.7, %forloop_fgt12_ ], [ %brnch42.6, %forloop_flt12_ ]
  %iidx38.7 = phi i64 [ %iidx38.6, %forloop_fgt12_ ], [ %iidx38.5, %forloop_flt12_ ]
  %istep37.7 = phi i64 [ %istep37.6, %forloop_fgt12_ ], [ %istep37.5, %forloop_flt12_ ]
  %44 = load %ravi.TValue** %base, !tbaa !14
  %var.tt.ptr125 = getelementptr inbounds %ravi.TValue* %44, i32 8, i32 1
  %var.n = getelementptr inbounds %ravi.TValue* %44, i32 8, i32 0, i32 0
  store double %init.n126, double* %var.n, !tbaa !15
  store i32 3, i32* %var.tt.ptr125, !tbaa !17
  br label %forbody11_

exit_iforloop:                                    ; preds = %forloop_fgt12_, %forloop_flt12_, %forloop_igt12_, %forloop_ilt12_
  %iidx38.8 = phi i64 [ %next.idx, %forloop_ilt12_ ], [ %iidx38.6, %forloop_fgt12_ ], [ %iidx38.5, %forloop_flt12_ ], [ %next.idx112, %forloop_igt12_ ]
  %istep37.8 = phi i64 [ %istep37.2, %forloop_ilt12_ ], [ %istep37.6, %forloop_fgt12_ ], [ %istep37.5, %forloop_flt12_ ], [ %istep37.3, %forloop_igt12_ ]
  indirectbr i8* %brnch.2, [label %forloop_ilt13_, label %forloop_igt13_, label %forloop_flt13_, label %forloop_fgt13_]

forloop_ilt13_:                                   ; preds = %exit_iforloop, %if.stopnow.notzero
  %iidx38.9 = phi i64 [ undef, %if.stopnow.notzero ], [ %iidx38.8, %exit_iforloop ]
  %istep.1 = phi i64 [ %step.i, %if.stopnow.notzero ], [ %istep.0, %exit_iforloop ]
  %iidx.1 = phi i64 [ %initv-pstep.i, %if.stopnow.notzero ], [ %iidx.0, %exit_iforloop ]
  %brnch.3 = phi i8* [ %., %if.stopnow.notzero ], [ %brnch.2, %exit_iforloop ]
  %istep37.9 = phi i64 [ undef, %if.stopnow.notzero ], [ %istep37.8, %exit_iforloop ]
  %next.idx129 = add nsw i64 %iidx.1, %istep.1
  %limit.i130 = load i64* %ilimit, !tbaa !5
  %idx.gt.limit131 = icmp sgt i64 %next.idx129, %limit.i130
  br i1 %idx.gt.limit131, label %exit_iforloop155, label %updatei137

forloop_igt13_:                                   ; preds = %exit_iforloop, %if.stopnow.notzero
  %iidx38.10 = phi i64 [ %iidx38.8, %exit_iforloop ], [ undef, %if.stopnow.notzero ]
  %istep.2 = phi i64 [ %istep.0, %exit_iforloop ], [ %step.i, %if.stopnow.notzero ]
  %iidx.2 = phi i64 [ %iidx.0, %exit_iforloop ], [ %initv-pstep.i, %if.stopnow.notzero ]
  %brnch.4 = phi i8* [ %brnch.2, %exit_iforloop ], [ %., %if.stopnow.notzero ]
  %istep37.10 = phi i64 [ %istep37.8, %exit_iforloop ], [ undef, %if.stopnow.notzero ]
  %next.idx134 = add nsw i64 %iidx.2, %istep.2
  %limit.i135 = load i64* %ilimit, !tbaa !5
  %limit.gt.idx136 = icmp sgt i64 %limit.i135, %next.idx134
  br i1 %limit.gt.idx136, label %exit_iforloop155, label %updatei137

updatei137:                                       ; preds = %forloop_igt13_, %forloop_ilt13_
  %iidx38.11 = phi i64 [ %iidx38.9, %forloop_ilt13_ ], [ %iidx38.10, %forloop_igt13_ ]
  %istep.3 = phi i64 [ %istep.1, %forloop_ilt13_ ], [ %istep.2, %forloop_igt13_ ]
  %iidx.3 = phi i64 [ %next.idx129, %forloop_ilt13_ ], [ %next.idx134, %forloop_igt13_ ]
  %brnch.5 = phi i8* [ %brnch.3, %forloop_ilt13_ ], [ %brnch.4, %forloop_igt13_ ]
  %istep37.11 = phi i64 [ %istep37.9, %forloop_ilt13_ ], [ %istep37.10, %forloop_igt13_ ]
  %45 = load %ravi.TValue** %base, !tbaa !14
  %46 = getelementptr inbounds %ravi.TValue* %45, i32 4
  %var.tt.ptr138 = getelementptr inbounds %ravi.TValue* %45, i32 4, i32 1
  %var.i139 = bitcast %ravi.TValue* %46 to i64*
  store i64 %iidx.3, i64* %var.i139, !tbaa !15
  store i32 19, i32* %var.tt.ptr138, !tbaa !17
  %src.value.phi.trans.insert = getelementptr inbounds %ravi.TValue* %45, i32 4, i32 0, i32 0
  %.pre = load double* %src.value.phi.trans.insert, !tbaa !15
  br label %forbody6_

forloop_flt13_:                                   ; preds = %exit_iforloop, %if.else.done
  %iidx38.12 = phi i64 [ %iidx38.8, %exit_iforloop ], [ undef, %if.else.done ]
  %istep.4 = phi i64 [ %istep.0, %exit_iforloop ], [ undef, %if.else.done ]
  %iidx.4 = phi i64 [ %iidx.0, %exit_iforloop ], [ undef, %if.else.done ]
  %brnch.6 = phi i8* [ %brnch.2, %exit_iforloop ], [ %.171, %if.else.done ]
  %istep37.12 = phi i64 [ %istep37.8, %exit_iforloop ], [ undef, %if.else.done ]
  %step.n141 = load double* %nstep, !tbaa !5
  %init.n142 = load double* %ninit, !tbaa !5
  %next.idx143 = fadd double %step.n141, %init.n142
  store double %next.idx143, double* %ninit, !tbaa !5
  %limit.n144 = load double* %nlimit, !tbaa !5
  %idx.gt.limit145 = fcmp ogt double %next.idx143, %limit.n144
  br i1 %idx.gt.limit145, label %exit_iforloop155, label %updatef151

forloop_fgt13_:                                   ; preds = %exit_iforloop, %if.else.done
  %iidx38.13 = phi i64 [ %iidx38.8, %exit_iforloop ], [ undef, %if.else.done ]
  %istep.5 = phi i64 [ %istep.0, %exit_iforloop ], [ undef, %if.else.done ]
  %iidx.5 = phi i64 [ %iidx.0, %exit_iforloop ], [ undef, %if.else.done ]
  %brnch.7 = phi i8* [ %brnch.2, %exit_iforloop ], [ %.171, %if.else.done ]
  %istep37.13 = phi i64 [ %istep37.8, %exit_iforloop ], [ undef, %if.else.done ]
  %step.n146 = load double* %nstep, !tbaa !5
  %init.n147 = load double* %ninit, !tbaa !5
  %next.idx148 = fadd double %step.n146, %init.n147
  store double %next.idx148, double* %ninit, !tbaa !5
  %limit.n149 = load double* %nlimit, !tbaa !5
  %limit.gt.idx150 = fcmp ogt double %limit.n149, %next.idx148
  br i1 %limit.gt.idx150, label %exit_iforloop155, label %updatef151

updatef151:                                       ; preds = %forloop_fgt13_, %forloop_flt13_
  %init.n154 = phi double [ %next.idx148, %forloop_fgt13_ ], [ %next.idx143, %forloop_flt13_ ]
  %iidx38.14 = phi i64 [ %iidx38.13, %forloop_fgt13_ ], [ %iidx38.12, %forloop_flt13_ ]
  %istep.6 = phi i64 [ %istep.5, %forloop_fgt13_ ], [ %istep.4, %forloop_flt13_ ]
  %iidx.6 = phi i64 [ %iidx.5, %forloop_fgt13_ ], [ %iidx.4, %forloop_flt13_ ]
  %brnch.8 = phi i8* [ %brnch.7, %forloop_fgt13_ ], [ %brnch.6, %forloop_flt13_ ]
  %istep37.14 = phi i64 [ %istep37.13, %forloop_fgt13_ ], [ %istep37.12, %forloop_flt13_ ]
  %47 = load %ravi.TValue** %base, !tbaa !14
  %var.tt.ptr152 = getelementptr inbounds %ravi.TValue* %47, i32 4, i32 1
  %var.n153 = getelementptr inbounds %ravi.TValue* %47, i32 4, i32 0, i32 0
  store double %init.n154, double* %var.n153, !tbaa !15
  store i32 3, i32* %var.tt.ptr152, !tbaa !17
  br label %forbody6_

exit_iforloop155:                                 ; preds = %forloop_fgt13_, %forloop_flt13_, %forloop_igt13_, %forloop_ilt13_
  %48 = load %ravi.TValue** %base, !tbaa !14
  %49 = getelementptr inbounds %ravi.TValue* %48, i32 1
  %L_top = getelementptr inbounds %ravi.lua_State* %L, i32 0, i32 4
  store %ravi.TValue* %49, %ravi.TValue** %L_top, !tbaa !18
  %sizep = getelementptr inbounds %ravi.Proto* %4, i32 0, i32 10
  %50 = load i32* %sizep
  %51 = icmp sgt i32 %50, 0
  br i1 %51, label %if.then, label %if.else

if.then:                                          ; preds = %exit_iforloop155
  call void @luaF_close(%ravi.lua_State* %L, %ravi.TValue* %48)
  br label %if.else

if.else:                                          ; preds = %if.then, %exit_iforloop155
  %52 = call i32 @luaD_poscall(%ravi.lua_State* %L, %ravi.TValue* %48)
  %53 = icmp eq i32 %52, 0
  br i1 %53, label %if.else157, label %if.then156

if.then156:                                       ; preds = %if.else
  %ci_top = getelementptr inbounds %ravi.CallInfo* %0, i32 0, i32 1
  %54 = load %ravi.TValue** %ci_top
  store %ravi.TValue* %54, %ravi.TValue** %L_top
  br label %if.else157

if.else157:                                       ; preds = %if.else, %if.then156
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
