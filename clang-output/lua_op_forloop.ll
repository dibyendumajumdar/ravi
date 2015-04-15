; ModuleID = 'lua_op_forloop.c'
target datalayout = "e-m:w-p:32:32-i64:64-f80:32-n8:16:32-S32"
target triple = "i686-pc-windows-gnu"

%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i8, i8 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.TValue = type { %union.Value, i32 }
%union.Value = type { i64 }
%struct.UpVal = type { %struct.TValue*, i64, %union.anon.0 }
%union.anon.0 = type { %struct.TValue }
%struct.lua_longjmp = type opaque
%struct.CallInfo = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %union.anon, i64, i16, i8, i8 }
%union.anon = type { %struct.CallInfoC }
%struct.CallInfoC = type { i32 (%struct.lua_State*, i32, i64)*, i64, i64 }
%struct.lua_Debug = type opaque

@.str = private unnamed_addr constant [12 x i8] c"value = %d\0A\00", align 1
@.str1 = private unnamed_addr constant [6 x i8] c"dummy\00", align 1

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
define void @test1(%struct.lua_State* nocapture readonly %L) #0 {
entry:
  %ci1 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 6
  %0 = load %struct.CallInfoLua** %ci1, align 4, !tbaa !6
  %base2 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 4, i32 0
  %1 = load %struct.TValue** %base2, align 4, !tbaa !12
  %add.ptr = getelementptr inbounds %struct.TValue* %1, i32 1
  %tt_ = getelementptr inbounds %struct.TValue* %1, i32 1, i32 1
  %value_5 = getelementptr inbounds %struct.TValue* %1, i32 3, i32 0
  %i = getelementptr inbounds %union.Value* %value_5, i32 0, i32 0
  %i7 = getelementptr inbounds %struct.TValue* %add.ptr, i32 0, i32 0, i32 0
  %i12 = getelementptr inbounds %struct.TValue* %1, i32 2, i32 0, i32 0
  %i23 = getelementptr inbounds %struct.TValue* %1, i32 4, i32 0, i32 0
  %tt_24 = getelementptr inbounds %struct.TValue* %1, i32 4, i32 1
  %n = bitcast %union.Value* %value_5 to double*
  %n31 = bitcast %struct.TValue* %add.ptr to double*
  %value_36 = getelementptr inbounds %struct.TValue* %1, i32 2, i32 0
  %n37 = bitcast %union.Value* %value_36 to double*
  %add.ptr48 = getelementptr inbounds %struct.TValue* %1, i32 4
  %n50 = bitcast %struct.TValue* %add.ptr48 to double*
  br label %label_loopbody

label_loopbody:                                   ; preds = %label_loopbody.backedge, %entry
  %call = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str1, i32 0, i32 0)) #1
  %2 = load i32* %tt_, align 4, !tbaa !15
  %cmp = icmp eq i32 %2, 19
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %label_loopbody
  %3 = load i64* %i, align 8, !tbaa !17
  %4 = load i64* %i7, align 8, !tbaa !17
  %add8 = add nsw i64 %4, %3
  %5 = load i64* %i12, align 8, !tbaa !17
  %cmp13 = icmp sgt i64 %3, 0
  br i1 %cmp13, label %cond.true, label %cond.false

cond.true:                                        ; preds = %if.then
  %cmp14 = icmp sgt i64 %add8, %5
  br i1 %cmp14, label %if.end53, label %if.then16

cond.false:                                       ; preds = %if.then
  %cmp15 = icmp sgt i64 %5, %add8
  br i1 %cmp15, label %if.end53, label %if.then16

if.then16:                                        ; preds = %cond.true, %cond.false
  store i64 %add8, i64* %i7, align 8, !tbaa !17
  store i32 19, i32* %tt_, align 4, !tbaa !15
  store i64 %add8, i64* %i23, align 8, !tbaa !17
  br label %label_loopbody.backedge

label_loopbody.backedge:                          ; preds = %if.then16, %if.then43
  %storemerge = phi i32 [ 3, %if.then43 ], [ 19, %if.then16 ]
  store i32 %storemerge, i32* %tt_24, align 4, !tbaa !15
  br label %label_loopbody

if.else:                                          ; preds = %label_loopbody
  %6 = load double* %n, align 8, !tbaa !18
  %7 = load double* %n31, align 8, !tbaa !18
  %add32 = fadd double %6, %7
  %8 = load double* %n37, align 8, !tbaa !18
  %cmp38 = fcmp ogt double %6, 0.000000e+00
  br i1 %cmp38, label %cond.true39, label %cond.false41

cond.true39:                                      ; preds = %if.else
  %cmp40 = fcmp ugt double %add32, %8
  br i1 %cmp40, label %if.end53, label %if.then43

cond.false41:                                     ; preds = %if.else
  %cmp42 = fcmp ugt double %8, %add32
  br i1 %cmp42, label %if.end53, label %if.then43

if.then43:                                        ; preds = %cond.true39, %cond.false41
  store double %add32, double* %n31, align 8, !tbaa !18
  store i32 3, i32* %tt_, align 4, !tbaa !15
  store double %add32, double* %n50, align 8, !tbaa !18
  br label %label_loopbody.backedge

if.end53:                                         ; preds = %cond.true, %cond.false, %cond.true39, %cond.false41
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.6.0 (trunk)"}
!1 = metadata !{metadata !2, metadata !4, i64 4}
!2 = metadata !{metadata !"GCObject", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5}
!3 = metadata !{metadata !"any pointer", metadata !4, i64 0}
!4 = metadata !{metadata !"omnipotent char", metadata !5, i64 0}
!5 = metadata !{metadata !"Simple C/C++ TBAA"}
!6 = metadata !{metadata !7, metadata !3, i64 16}
!7 = metadata !{metadata !"lua_State", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, metadata !3, i64 16, metadata !3, i64 20, metadata !3, i64 24, metadata !3, i64 28, metadata !3, i64 32, metadata !3, i64 36, metadata !3, i64 40, metadata !3, i64 44, metadata !8, i64 48, metadata !3, i64 104, metadata !9, i64 112, metadata !11, i64 120, metadata !11, i64 124, metadata !11, i64 128, metadata !10, i64 132, metadata !10, i64 134, metadata !4, i64 136, metadata !4, i64 137}
!8 = metadata !{metadata !"CallInfo", metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, metadata !3, i64 12, metadata !4, i64 16, metadata !9, i64 40, metadata !10, i64 48, metadata !4, i64 50, metadata !4, i64 51}
!9 = metadata !{metadata !"long long", metadata !4, i64 0}
!10 = metadata !{metadata !"short", metadata !4, i64 0}
!11 = metadata !{metadata !"int", metadata !4, i64 0}
!12 = metadata !{metadata !13, metadata !3, i64 16}
!13 = metadata !{metadata !"CallInfoLua", metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, metadata !3, i64 12, metadata !14, i64 16, metadata !9, i64 32, metadata !10, i64 40, metadata !4, i64 42, metadata !4, i64 43}
!14 = metadata !{metadata !"CallInfoL", metadata !3, i64 0, metadata !3, i64 4, metadata !9, i64 8}
!15 = metadata !{metadata !16, metadata !11, i64 8}
!16 = metadata !{metadata !"TValue", metadata !4, i64 0, metadata !11, i64 8}
!17 = metadata !{metadata !9, metadata !9, i64 0}
!18 = metadata !{metadata !19, metadata !19, i64 0}
!19 = metadata !{metadata !"double", metadata !4, i64 0}
