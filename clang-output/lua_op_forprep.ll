; ModuleID = 'lua_op_forprep.c'
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
  %call = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([12 x i8]* @.str, i32 0, i32 0), i32 %conv) #2
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
  %add.ptr = getelementptr inbounds %struct.TValue* %1, i32 1
  %add.ptr5 = getelementptr inbounds %struct.TValue* %1, i32 2
  %add.ptr6 = getelementptr inbounds %struct.TValue* %1, i32 3
  %tt_ = getelementptr inbounds %struct.TValue* %1, i32 1, i32 1
  %2 = load i32* %tt_, align 4, !tbaa !15
  %cmp = icmp eq i32 %2, 19
  %tt_7 = getelementptr inbounds %struct.TValue* %1, i32 3, i32 1
  %3 = load i32* %tt_7, align 4, !tbaa !15
  %cmp8 = icmp eq i32 %3, 19
  %i = getelementptr inbounds %struct.TValue* %add.ptr6, i32 0, i32 0, i32 0
  %4 = load i64* %i, align 8, !tbaa !17
  %call = call i32 @forlimit(%struct.TValue* %add.ptr5, i64* %ilimit, i64 %4, i32* %stopnow) #2
  %or.cond = and i1 %cmp, %cmp8
  %tobool13 = icmp ne i32 %call, 0
  %or.cond73 = and i1 %or.cond, %tobool13
  br i1 %or.cond73, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %5 = load i32* %stopnow, align 4, !tbaa !18
  %tobool14 = icmp eq i32 %5, 0
  %i16 = getelementptr inbounds %struct.TValue* %add.ptr, i32 0, i32 0, i32 0
  br i1 %tobool14, label %cond.false, label %cond.end

cond.false:                                       ; preds = %if.then
  %6 = load i64* %i16, align 8, !tbaa !17
  br label %cond.end

cond.end:                                         ; preds = %if.then, %cond.false
  %cond = phi i64 [ %6, %cond.false ], [ 0, %if.then ]
  %7 = load i64* %ilimit, align 8, !tbaa !17
  %i18 = getelementptr inbounds %struct.TValue* %add.ptr5, i32 0, i32 0, i32 0
  store i64 %7, i64* %i18, align 8, !tbaa !17
  %tt_19 = getelementptr inbounds %struct.TValue* %1, i32 2, i32 1
  store i32 19, i32* %tt_19, align 4, !tbaa !15
  %8 = load i64* %i, align 8, !tbaa !17
  %sub = sub nsw i64 %cond, %8
  store i64 %sub, i64* %i16, align 8, !tbaa !17
  br label %label_forloop

if.else:                                          ; preds = %entry
  %tt_25 = getelementptr inbounds %struct.TValue* %1, i32 2, i32 1
  %9 = load i32* %tt_25, align 4, !tbaa !15
  %cmp26 = icmp eq i32 %9, 3
  br i1 %cmp26, label %if.then28, label %if.else30

if.then28:                                        ; preds = %if.else
  %n = bitcast %struct.TValue* %add.ptr5 to double*
  %10 = load double* %n, align 8, !tbaa !19
  store double %10, double* %nlimit, align 8, !tbaa !19
  br label %if.end35

if.else30:                                        ; preds = %if.else
  %call31 = call i32 @luaV_tonumber_(%struct.TValue* %add.ptr5, double* %nlimit) #2
  %phitmp = icmp eq i32 %call31, 0
  br i1 %phitmp, label %if.then33, label %if.end35

if.then33:                                        ; preds = %if.else30
  %call34 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %L, i8* getelementptr inbounds ([29 x i8]* @.str1, i32 0, i32 0)) #2
  br label %if.end35

if.end35:                                         ; preds = %if.else30, %if.then28, %if.then33
  %11 = load double* %nlimit, align 8, !tbaa !19
  %n37 = bitcast %struct.TValue* %add.ptr5 to double*
  store double %11, double* %n37, align 8, !tbaa !19
  store i32 3, i32* %tt_25, align 4, !tbaa !15
  %12 = load i32* %tt_7, align 4, !tbaa !15
  %cmp40 = icmp eq i32 %12, 3
  br i1 %cmp40, label %if.then42, label %if.else45

if.then42:                                        ; preds = %if.end35
  %n44 = bitcast %struct.TValue* %add.ptr6 to double*
  %13 = load double* %n44, align 8, !tbaa !19
  store double %13, double* %nstep, align 8, !tbaa !19
  br label %if.end51

if.else45:                                        ; preds = %if.end35
  %call46 = call i32 @luaV_tonumber_(%struct.TValue* %add.ptr6, double* %nstep) #2
  %phitmp104 = icmp eq i32 %call46, 0
  br i1 %phitmp104, label %if.then49, label %if.end51

if.then49:                                        ; preds = %if.else45
  %call50 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %L, i8* getelementptr inbounds ([28 x i8]* @.str2, i32 0, i32 0)) #2
  br label %if.end51

if.end51:                                         ; preds = %if.else45, %if.then42, %if.then49
  %14 = load double* %nstep, align 8, !tbaa !19
  %n53 = bitcast %struct.TValue* %add.ptr6 to double*
  store double %14, double* %n53, align 8, !tbaa !19
  store i32 3, i32* %tt_7, align 4, !tbaa !15
  %15 = load i32* %tt_, align 4, !tbaa !15
  %cmp56 = icmp eq i32 %15, 3
  br i1 %cmp56, label %if.then58, label %if.else61

if.then58:                                        ; preds = %if.end51
  %n60 = bitcast %struct.TValue* %add.ptr to double*
  %16 = load double* %n60, align 8, !tbaa !19
  store double %16, double* %ninit, align 8, !tbaa !19
  br label %if.end67

if.else61:                                        ; preds = %if.end51
  %call62 = call i32 @luaV_tonumber_(%struct.TValue* %add.ptr, double* %ninit) #2
  %phitmp105 = icmp eq i32 %call62, 0
  br i1 %phitmp105, label %if.then65, label %if.end67

if.then65:                                        ; preds = %if.else61
  %call66 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %L, i8* getelementptr inbounds ([37 x i8]* @.str3, i32 0, i32 0)) #2
  br label %if.end67

if.end67:                                         ; preds = %if.else61, %if.then58, %if.then65
  %17 = load double* %ninit, align 8, !tbaa !19
  %18 = load double* %nstep, align 8, !tbaa !19
  %sub68 = fsub double %17, %18
  %n70 = bitcast %struct.TValue* %add.ptr to double*
  store double %sub68, double* %n70, align 8, !tbaa !19
  br label %label_forloop

label_forloop:                                    ; preds = %cond.end, %if.end67
  %storemerge = phi i32 [ 3, %if.end67 ], [ 19, %cond.end ]
  store i32 %storemerge, i32* %tt_, align 4, !tbaa !15
  ret void
}

declare i32 @forlimit(%struct.TValue*, i64*, i64, i32*) #1

declare i32 @luaV_tonumber_(%struct.TValue*, double*) #1

declare i32 @luaG_runerror(...) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

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
!15 = metadata !{metadata !16, metadata !11, i64 8}
!16 = metadata !{metadata !"TValue", metadata !4, i64 0, metadata !11, i64 8}
!17 = metadata !{metadata !9, metadata !9, i64 0}
!18 = metadata !{metadata !11, metadata !11, i64 0}
!19 = metadata !{metadata !20, metadata !20, i64 0}
!20 = metadata !{metadata !"double", metadata !4, i64 0}
