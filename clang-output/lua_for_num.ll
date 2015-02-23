; ModuleID = 'lua_for_num.c'
target datalayout = "e-m:w-p:32:32-i64:64-f80:32-n8:16:32-S32"
target triple = "i686-pc-windows-gnu"

%struct.Proto = type { %struct.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %struct.TValue*, i32*, %struct.Proto**, i32*, %struct.LocVar*, %struct.Upvaldesc*, %struct.LClosure*, %struct.TString*, %struct.GCObject* }
%struct.TValue = type { %union.Value, i32 }
%union.Value = type { i64 }
%struct.LocVar = type { %struct.TString*, i32, i32, i32 }
%struct.Upvaldesc = type { %struct.TString*, i8, i8 }
%struct.LClosure = type { %struct.GCObject*, i8, i8, i8, %struct.GCObject*, %struct.Proto*, [1 x %struct.UpVal*] }
%struct.UpVal = type opaque
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i32, i64, %struct.TString* }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i8 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.lua_longjmp = type opaque
%struct.CallInfo = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %union.anon, i64, i16, i8 }
%union.anon = type { %struct.CallInfoC }
%struct.CallInfoC = type { i32 (%struct.lua_State*, i32, i64)*, i64, i64 }
%struct.lua_Debug = type opaque

@.str = private unnamed_addr constant [12 x i8] c"value = %d\0A\00", align 1
@.str1 = private unnamed_addr constant [29 x i8] c"'for' limit must be a number\00", align 1
@.str2 = private unnamed_addr constant [28 x i8] c"'for' step must be a number\00", align 1
@.str3 = private unnamed_addr constant [37 x i8] c"'for' initial value must be a number\00", align 1
@Proto = common global %struct.Proto zeroinitializer, align 4

; Function Attrs: nounwind
define void @testfunc(%struct.GCObject* nocapture readonly %obj) #0 {
entry:
  %tt = getelementptr inbounds %struct.GCObject* %obj, i32 0, i32 1
  %0 = load i8* %tt, align 1, !tbaa !1
  %conv = zext i8 %0 to i32
  %call = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([12 x i8]* @.str, i32 0, i32 0), i32 %conv) #1
  ret void
}

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) #0

; Function Attrs: nounwind
define void @test1(%struct.lua_State* %L) #0 {
entry:
  %ilimit = alloca i64, align 8
  %stopnow = alloca i32, align 4
  %ninit = alloca double, align 8
  %nlimit = alloca double, align 8
  %nstep = alloca double, align 8
  %ci1 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 6
  %0 = load %struct.CallInfoLua** %ci1, align 4, !tbaa !6
  %base2 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 4, i32 0
  %1 = load %struct.TValue** %base2, align 4, !tbaa !12
  %2 = bitcast %struct.CallInfoLua* %0 to %struct.LClosure***
  %3 = load %struct.LClosure*** %2, align 4, !tbaa !15
  %4 = load %struct.LClosure** %3, align 4, !tbaa !16
  %p = getelementptr inbounds %struct.LClosure* %4, i32 0, i32 5
  %5 = load %struct.Proto** %p, align 4, !tbaa !17
  %k3 = getelementptr inbounds %struct.Proto* %5, i32 0, i32 14
  %6 = load %struct.TValue** %k3, align 4, !tbaa !19
  %7 = bitcast %struct.TValue* %1 to i8*
  %8 = bitcast %struct.TValue* %6 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %7, i8* %8, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr5 = getelementptr inbounds %struct.TValue* %1, i32 1
  %add.ptr6 = getelementptr inbounds %struct.TValue* %6, i32 1
  %9 = bitcast %struct.TValue* %add.ptr5 to i8*
  %10 = bitcast %struct.TValue* %add.ptr6 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %9, i8* %10, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr7 = getelementptr inbounds %struct.TValue* %1, i32 2
  %add.ptr8 = getelementptr inbounds %struct.TValue* %6, i32 2
  %11 = bitcast %struct.TValue* %add.ptr7 to i8*
  %12 = bitcast %struct.TValue* %add.ptr8 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %11, i8* %12, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr9 = getelementptr inbounds %struct.TValue* %1, i32 3
  %13 = bitcast %struct.TValue* %add.ptr9 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %13, i8* %10, i32 16, i32 8, i1 false), !tbaa.struct !21
  %tt_ = getelementptr inbounds %struct.TValue* %1, i32 1, i32 1
  %14 = load i32* %tt_, align 4, !tbaa !26
  %cmp = icmp eq i32 %14, 19
  %tt_14 = getelementptr inbounds %struct.TValue* %1, i32 3, i32 1
  %15 = load i32* %tt_14, align 4, !tbaa !26
  %cmp15 = icmp eq i32 %15, 19
  %i = getelementptr inbounds %struct.TValue* %add.ptr9, i32 0, i32 0, i32 0
  %16 = load i64* %i, align 8, !tbaa !23
  %call = call i32 @forlimit(%struct.TValue* %add.ptr7, i64* %ilimit, i64 %16, i32* %stopnow) #1
  %or.cond = and i1 %cmp, %cmp15
  %tobool20 = icmp ne i32 %call, 0
  %or.cond158 = and i1 %or.cond, %tobool20
  br i1 %or.cond158, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %17 = load i32* %stopnow, align 4, !tbaa !22
  %tobool21 = icmp eq i32 %17, 0
  %i23 = getelementptr inbounds %struct.TValue* %add.ptr5, i32 0, i32 0, i32 0
  br i1 %tobool21, label %cond.false, label %cond.end

cond.false:                                       ; preds = %if.then
  %18 = load i64* %i23, align 8, !tbaa !23
  br label %cond.end

cond.end:                                         ; preds = %if.then, %cond.false
  %cond = phi i64 [ %18, %cond.false ], [ 0, %if.then ]
  %19 = load i64* %ilimit, align 8, !tbaa !23
  %i25 = getelementptr inbounds %struct.TValue* %add.ptr7, i32 0, i32 0, i32 0
  store i64 %19, i64* %i25, align 8, !tbaa !23
  %tt_26 = getelementptr inbounds %struct.TValue* %1, i32 2, i32 1
  store i32 19, i32* %tt_26, align 4, !tbaa !26
  %20 = load i64* %i, align 8, !tbaa !23
  %sub = sub nsw i64 %cond, %20
  store i64 %sub, i64* %i23, align 8, !tbaa !23
  %21 = bitcast i64 %sub to double
  %n116.pre = bitcast %struct.TValue* %add.ptr9 to double*
  %n119.pre = bitcast %struct.TValue* %add.ptr5 to double*
  %n124.pre = bitcast %struct.TValue* %add.ptr7 to double*
  br label %label_forloop.preheader

if.else:                                          ; preds = %entry
  %tt_32 = getelementptr inbounds %struct.TValue* %1, i32 2, i32 1
  %22 = load i32* %tt_32, align 4, !tbaa !26
  %cmp33 = icmp eq i32 %22, 3
  br i1 %cmp33, label %if.then35, label %if.else37

if.then35:                                        ; preds = %if.else
  %n = bitcast %struct.TValue* %add.ptr7 to double*
  %23 = load double* %n, align 8, !tbaa !24
  store double %23, double* %nlimit, align 8, !tbaa !24
  br label %if.end42

if.else37:                                        ; preds = %if.else
  %call38 = call i32 @luaV_tonumber_(%struct.TValue* %add.ptr7, double* %nlimit) #1
  %phitmp = icmp eq i32 %call38, 0
  br i1 %phitmp, label %if.then40, label %if.end42

if.then40:                                        ; preds = %if.else37
  %call41 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %L, i8* getelementptr inbounds ([29 x i8]* @.str1, i32 0, i32 0)) #1
  br label %if.end42

if.end42:                                         ; preds = %if.else37, %if.then35, %if.then40
  %24 = load double* %nlimit, align 8, !tbaa !24
  %n44 = bitcast %struct.TValue* %add.ptr7 to double*
  store double %24, double* %n44, align 8, !tbaa !24
  store i32 3, i32* %tt_32, align 4, !tbaa !26
  %25 = load i32* %tt_14, align 4, !tbaa !26
  %cmp47 = icmp eq i32 %25, 3
  br i1 %cmp47, label %if.then49, label %if.else52

if.then49:                                        ; preds = %if.end42
  %n51 = bitcast %struct.TValue* %add.ptr9 to double*
  %26 = load double* %n51, align 8, !tbaa !24
  store double %26, double* %nstep, align 8, !tbaa !24
  br label %if.end58

if.else52:                                        ; preds = %if.end42
  %call53 = call i32 @luaV_tonumber_(%struct.TValue* %add.ptr9, double* %nstep) #1
  %phitmp245 = icmp eq i32 %call53, 0
  br i1 %phitmp245, label %if.then56, label %if.end58

if.then56:                                        ; preds = %if.else52
  %call57 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %L, i8* getelementptr inbounds ([28 x i8]* @.str2, i32 0, i32 0)) #1
  br label %if.end58

if.end58:                                         ; preds = %if.else52, %if.then49, %if.then56
  %27 = load double* %nstep, align 8, !tbaa !24
  %n60 = bitcast %struct.TValue* %add.ptr9 to double*
  store double %27, double* %n60, align 8, !tbaa !24
  store i32 3, i32* %tt_14, align 4, !tbaa !26
  %28 = load i32* %tt_, align 4, !tbaa !26
  %cmp63 = icmp eq i32 %28, 3
  br i1 %cmp63, label %if.then65, label %if.else68

if.then65:                                        ; preds = %if.end58
  %n67 = bitcast %struct.TValue* %add.ptr5 to double*
  %29 = load double* %n67, align 8, !tbaa !24
  store double %29, double* %ninit, align 8, !tbaa !24
  br label %if.end74

if.else68:                                        ; preds = %if.end58
  %call69 = call i32 @luaV_tonumber_(%struct.TValue* %add.ptr5, double* %ninit) #1
  %phitmp246 = icmp eq i32 %call69, 0
  br i1 %phitmp246, label %if.then72, label %if.end74

if.then72:                                        ; preds = %if.else68
  %call73 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %L, i8* getelementptr inbounds ([37 x i8]* @.str3, i32 0, i32 0)) #1
  br label %if.end74

if.end74:                                         ; preds = %if.else68, %if.then65, %if.then72
  %30 = load double* %ninit, align 8, !tbaa !24
  %31 = load double* %nstep, align 8, !tbaa !24
  %sub75 = fsub double %30, %31
  %n77 = bitcast %struct.TValue* %add.ptr5 to double*
  store double %sub75, double* %n77, align 8, !tbaa !24
  %32 = bitcast double %sub75 to i64
  %i91.pre = getelementptr inbounds %struct.TValue* %add.ptr5, i32 0, i32 0, i32 0
  %i94.pre = getelementptr inbounds %struct.TValue* %add.ptr7, i32 0, i32 0, i32 0
  br label %label_forloop.preheader

label_forloop.preheader:                          ; preds = %if.end74, %cond.end
  %n124.pre-phi = phi double* [ %n44, %if.end74 ], [ %n124.pre, %cond.end ]
  %n119.pre-phi = phi double* [ %n77, %if.end74 ], [ %n119.pre, %cond.end ]
  %n116.pre-phi = phi double* [ %n60, %if.end74 ], [ %n116.pre, %cond.end ]
  %i94.pre-phi = phi i64* [ %i94.pre, %if.end74 ], [ %i25, %cond.end ]
  %i91.pre-phi = phi i64* [ %i91.pre, %if.end74 ], [ %i23, %cond.end ]
  %33 = phi double [ %sub75, %if.end74 ], [ %21, %cond.end ]
  %34 = phi i64 [ %32, %if.end74 ], [ %sub, %cond.end ]
  %storemerge = phi i32 [ 3, %if.end74 ], [ 19, %cond.end ]
  store i32 %storemerge, i32* %tt_, align 4, !tbaa !26
  %i109 = getelementptr inbounds %struct.TValue* %1, i32 4, i32 0, i32 0
  %tt_110 = getelementptr inbounds %struct.TValue* %1, i32 4, i32 1
  %add.ptr81 = getelementptr inbounds %struct.TValue* %1, i32 4
  %35 = bitcast %struct.TValue* %add.ptr81 to i8*
  %n139 = bitcast %struct.TValue* %add.ptr81 to double*
  br label %label_forloop

label_loopbody:                                   ; preds = %if.then133, %if.then103
  %36 = phi double [ %add120, %if.then133 ], [ %43, %if.then103 ]
  %.pr = phi i32 [ 3, %if.then133 ], [ 19, %if.then103 ]
  %37 = phi i64 [ %46, %if.then133 ], [ %add, %if.then103 ]
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %7, i8* %35, i32 16, i32 8, i1 false), !tbaa.struct !21
  br label %label_forloop

label_forloop:                                    ; preds = %label_forloop.preheader, %label_loopbody
  %38 = phi double [ %36, %label_loopbody ], [ %33, %label_forloop.preheader ]
  %39 = phi i64 [ %37, %label_loopbody ], [ %34, %label_forloop.preheader ]
  %40 = phi i32 [ %.pr, %label_loopbody ], [ %storemerge, %label_forloop.preheader ]
  %cmp84 = icmp eq i32 %40, 19
  br i1 %cmp84, label %if.then86, label %if.else112

if.then86:                                        ; preds = %label_forloop
  %41 = load i64* %i, align 8, !tbaa !23
  %add = add nsw i64 %39, %41
  %42 = load i64* %i94.pre-phi, align 8, !tbaa !23
  %cmp95 = icmp sgt i64 %41, 0
  br i1 %cmp95, label %cond.true97, label %cond.false100

cond.true97:                                      ; preds = %if.then86
  %cmp98 = icmp sgt i64 %add, %42
  br i1 %cmp98, label %if.end142, label %if.then103

cond.false100:                                    ; preds = %if.then86
  %cmp101 = icmp sgt i64 %42, %add
  br i1 %cmp101, label %if.end142, label %if.then103

if.then103:                                       ; preds = %cond.true97, %cond.false100
  store i64 %add, i64* %i91.pre-phi, align 8, !tbaa !23
  store i32 19, i32* %tt_, align 4, !tbaa !26
  store i64 %add, i64* %i109, align 8, !tbaa !23
  store i32 19, i32* %tt_110, align 4, !tbaa !26
  %43 = bitcast i64 %add to double
  br label %label_loopbody

if.else112:                                       ; preds = %label_forloop
  %44 = load double* %n116.pre-phi, align 8, !tbaa !24
  %add120 = fadd double %44, %38
  %45 = load double* %n124.pre-phi, align 8, !tbaa !24
  %cmp125 = fcmp ogt double %44, 0.000000e+00
  br i1 %cmp125, label %cond.true127, label %cond.false130

cond.true127:                                     ; preds = %if.else112
  %cmp128 = fcmp ugt double %add120, %45
  br i1 %cmp128, label %if.end142, label %if.then133

cond.false130:                                    ; preds = %if.else112
  %cmp131 = fcmp ugt double %45, %add120
  br i1 %cmp131, label %if.end142, label %if.then133

if.then133:                                       ; preds = %cond.true127, %cond.false130
  store double %add120, double* %n119.pre-phi, align 8, !tbaa !24
  store i32 3, i32* %tt_, align 4, !tbaa !26
  store double %add120, double* %n139, align 8, !tbaa !24
  store i32 3, i32* %tt_110, align 4, !tbaa !26
  %46 = bitcast double %add120 to i64
  br label %label_loopbody

if.end142:                                        ; preds = %cond.true97, %cond.false100, %cond.true127, %cond.false130
  %top = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr5, %struct.TValue** %top, align 4, !tbaa !28
  %47 = load %struct.Proto** %p, align 4, !tbaa !17
  %sizep = getelementptr inbounds %struct.Proto* %47, i32 0, i32 10
  %48 = load i32* %sizep, align 4, !tbaa !29
  %cmp147 = icmp sgt i32 %48, 0
  br i1 %cmp147, label %if.then149, label %if.end151

if.then149:                                       ; preds = %if.end142
  %call150 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %1) #1
  br label %if.end151

if.end151:                                        ; preds = %if.then149, %if.end142
  %call152 = call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %1) #1
  %tobool153 = icmp eq i32 %call152, 0
  br i1 %tobool153, label %if.end157, label %if.then154

if.then154:                                       ; preds = %if.end151
  %top155 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %49 = load %struct.TValue** %top155, align 4, !tbaa !30
  store %struct.TValue* %49, %struct.TValue** %top, align 4, !tbaa !28
  br label %if.end157

if.end157:                                        ; preds = %if.end151, %if.then154
  ret void
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i32(i8* nocapture, i8* nocapture readonly, i32, i32, i1) #1

declare i32 @forlimit(%struct.TValue*, i64*, i64, i32*) #2

declare i32 @luaV_tonumber_(%struct.TValue*, double*) #2

declare i32 @luaG_runerror(...) #2

declare i32 @luaF_close(...) #2

declare i32 @luaD_poscall(%struct.lua_State*, %struct.TValue*) #2

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }
attributes #2 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.6.0 (trunk)"}
!1 = metadata !{metadata !2, metadata !4, i64 4}
!2 = metadata !{metadata !"GCObject", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5}
!3 = metadata !{metadata !"any pointer", metadata !4, i64 0}
!4 = metadata !{metadata !"omnipotent char", metadata !5, i64 0}
!5 = metadata !{metadata !"Simple C/C++ TBAA"}
!6 = metadata !{metadata !7, metadata !3, i64 16}
!7 = metadata !{metadata !"lua_State", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, metadata !3, i64 16, metadata !3, i64 20, metadata !3, i64 24, metadata !3, i64 28, metadata !3, i64 32, metadata !3, i64 36, metadata !3, i64 40, metadata !3, i64 44, metadata !8, i64 48, metadata !3, i64 104, metadata !9, i64 112, metadata !11, i64 120, metadata !11, i64 124, metadata !11, i64 128, metadata !10, i64 132, metadata !10, i64 134, metadata !4, i64 136, metadata !4, i64 137}
!8 = metadata !{metadata !"CallInfo", metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, metadata !3, i64 12, metadata !4, i64 16, metadata !9, i64 40, metadata !10, i64 48, metadata !4, i64 50}
!9 = metadata !{metadata !"long long", metadata !4, i64 0}
!10 = metadata !{metadata !"short", metadata !4, i64 0}
!11 = metadata !{metadata !"int", metadata !4, i64 0}
!12 = metadata !{metadata !13, metadata !3, i64 16}
!13 = metadata !{metadata !"CallInfoLua", metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, metadata !3, i64 12, metadata !14, i64 16, metadata !9, i64 32, metadata !10, i64 40, metadata !4, i64 42}
!14 = metadata !{metadata !"CallInfoL", metadata !3, i64 0, metadata !3, i64 4, metadata !9, i64 8}
!15 = metadata !{metadata !13, metadata !3, i64 0}
!16 = metadata !{metadata !3, metadata !3, i64 0}
!17 = metadata !{metadata !18, metadata !3, i64 12}
!18 = metadata !{metadata !"LClosure", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, metadata !4, i64 16}
!19 = metadata !{metadata !20, metadata !3, i64 44}
!20 = metadata !{metadata !"Proto", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !4, i64 7, metadata !4, i64 8, metadata !11, i64 12, metadata !11, i64 16, metadata !11, i64 20, metadata !11, i64 24, metadata !11, i64 28, metadata !11, i64 32, metadata !11, i64 36, metadata !11, i64 40, metadata !3, i64 44, metadata !3, i64 48, metadata !3, i64 52, metadata !3, i64 56, metadata !3, i64 60, metadata !3, i64 64, metadata !3, i64 68, metadata !3, i64 72, metadata !3, i64 76}
!21 = metadata !{i64 0, i64 4, metadata !16, i64 0, i64 4, metadata !16, i64 0, i64 4, metadata !22, i64 0, i64 4, metadata !16, i64 0, i64 8, metadata !23, i64 0, i64 8, metadata !24, i64 8, i64 4, metadata !22}
!22 = metadata !{metadata !11, metadata !11, i64 0}
!23 = metadata !{metadata !9, metadata !9, i64 0}
!24 = metadata !{metadata !25, metadata !25, i64 0}
!25 = metadata !{metadata !"double", metadata !4, i64 0}
!26 = metadata !{metadata !27, metadata !11, i64 8}
!27 = metadata !{metadata !"TValue", metadata !4, i64 0, metadata !11, i64 8}
!28 = metadata !{metadata !7, metadata !3, i64 8}
!29 = metadata !{metadata !20, metadata !11, i64 28}
!30 = metadata !{metadata !13, metadata !3, i64 4}
