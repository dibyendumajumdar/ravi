; ModuleID = 'lua_if_else.c'
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
  %ci1 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 6
  %0 = load %struct.CallInfoLua** %ci1, align 4, !tbaa !6
  %base2 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 4, i32 0
  %1 = bitcast %struct.CallInfoLua* %0 to %struct.LClosure***
  %2 = load %struct.LClosure*** %1, align 4, !tbaa !12
  %3 = load %struct.LClosure** %2, align 4, !tbaa !15
  %p = getelementptr inbounds %struct.LClosure* %3, i32 0, i32 5
  %4 = load %struct.Proto** %p, align 4, !tbaa !16
  %k3 = getelementptr inbounds %struct.Proto* %4, i32 0, i32 14
  %5 = load %struct.TValue** %base2, align 4, !tbaa !18
  %6 = load %struct.TValue** %k3, align 4, !tbaa !19
  %call = tail call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %5, %struct.TValue* %6) #2
  %cmp = icmp eq i32 %call, 0
  br i1 %cmp, label %label6, label %label3

label3:                                           ; preds = %entry
  %add.ptr18 = getelementptr inbounds %struct.TValue* %5, i32 1
  %add.ptr19 = getelementptr inbounds %struct.TValue* %6, i32 1
  %7 = bitcast %struct.TValue* %add.ptr18 to i8*
  %8 = bitcast %struct.TValue* %add.ptr19 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %7, i8* %8, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr22 = getelementptr inbounds %struct.TValue* %5, i32 2
  %top = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr22, %struct.TValue** %top, align 4, !tbaa !26
  %9 = load %struct.Proto** %p, align 4, !tbaa !16
  %sizep = getelementptr inbounds %struct.Proto* %9, i32 0, i32 10
  %10 = load i32* %sizep, align 4, !tbaa !27
  %cmp24 = icmp sgt i32 %10, 0
  br i1 %cmp24, label %if.then25, label %if.end27

if.then25:                                        ; preds = %label3
  %call26 = tail call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %5) #2
  br label %if.end27

if.end27:                                         ; preds = %if.then25, %label3
  %call28 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr18) #2
  %tobool = icmp eq i32 %call28, 0
  br i1 %tobool, label %return, label %if.then29

if.then29:                                        ; preds = %if.end27
  %top30 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %11 = load %struct.TValue** %top30, align 4, !tbaa !28
  store %struct.TValue* %11, %struct.TValue** %top, align 4, !tbaa !26
  br label %return

label6:                                           ; preds = %entry
  %12 = load %struct.TValue** %base2, align 4, !tbaa !18
  %13 = load %struct.Proto** %p, align 4, !tbaa !16
  %k36 = getelementptr inbounds %struct.Proto* %13, i32 0, i32 14
  %14 = load %struct.TValue** %k36, align 4, !tbaa !19
  %add.ptr39 = getelementptr inbounds %struct.TValue* %14, i32 2
  %call40 = tail call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %12, %struct.TValue* %add.ptr39) #2
  %cmp41 = icmp eq i32 %call40, 0
  %add.ptr71 = getelementptr inbounds %struct.TValue* %12, i32 1
  br i1 %cmp41, label %label11, label %label8

label8:                                           ; preds = %label6
  %add.ptr54 = getelementptr inbounds %struct.TValue* %14, i32 3
  %15 = bitcast %struct.TValue* %add.ptr71 to i8*
  %16 = bitcast %struct.TValue* %add.ptr54 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %15, i8* %16, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr57 = getelementptr inbounds %struct.TValue* %12, i32 2
  %top58 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr57, %struct.TValue** %top58, align 4, !tbaa !26
  %17 = load %struct.Proto** %p, align 4, !tbaa !16
  %sizep60 = getelementptr inbounds %struct.Proto* %17, i32 0, i32 10
  %18 = load i32* %sizep60, align 4, !tbaa !27
  %cmp61 = icmp sgt i32 %18, 0
  br i1 %cmp61, label %if.then62, label %if.end64

if.then62:                                        ; preds = %label8
  %call63 = tail call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %12) #2
  br label %if.end64

if.end64:                                         ; preds = %if.then62, %label8
  %call65 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr71) #2
  %tobool66 = icmp eq i32 %call65, 0
  br i1 %tobool66, label %return, label %if.then67

if.then67:                                        ; preds = %if.end64
  %top68 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %19 = load %struct.TValue** %top68, align 4, !tbaa !28
  store %struct.TValue* %19, %struct.TValue** %top58, align 4, !tbaa !26
  br label %return

label11:                                          ; preds = %label6
  %add.ptr72 = getelementptr inbounds %struct.TValue* %14, i32 4
  %20 = bitcast %struct.TValue* %add.ptr71 to i8*
  %21 = bitcast %struct.TValue* %add.ptr72 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %20, i8* %21, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr75 = getelementptr inbounds %struct.TValue* %12, i32 2
  %top76 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr75, %struct.TValue** %top76, align 4, !tbaa !26
  %22 = load %struct.Proto** %p, align 4, !tbaa !16
  %sizep78 = getelementptr inbounds %struct.Proto* %22, i32 0, i32 10
  %23 = load i32* %sizep78, align 4, !tbaa !27
  %cmp79 = icmp sgt i32 %23, 0
  br i1 %cmp79, label %if.then80, label %if.end82

if.then80:                                        ; preds = %label11
  %call81 = tail call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %12) #2
  br label %if.end82

if.end82:                                         ; preds = %if.then80, %label11
  %call83 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr71) #2
  %tobool84 = icmp eq i32 %call83, 0
  br i1 %tobool84, label %return, label %if.then85

if.then85:                                        ; preds = %if.end82
  %top86 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %24 = load %struct.TValue** %top86, align 4, !tbaa !28
  store %struct.TValue* %24, %struct.TValue** %top76, align 4, !tbaa !26
  br label %return

return:                                           ; preds = %if.end82, %if.end64, %if.end27, %if.then85, %if.then67, %if.then29
  ret void
}

declare i32 @luaV_equalobj(...) #1

declare i32 @luaF_close(...) #1

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i32(i8* nocapture, i8* nocapture readonly, i32, i32, i1) #2

declare i32 @luaD_poscall(%struct.lua_State*, %struct.TValue*) #1

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
!12 = metadata !{metadata !13, metadata !3, i64 0}
!13 = metadata !{metadata !"CallInfoLua", metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, metadata !3, i64 12, metadata !14, i64 16, metadata !9, i64 32, metadata !10, i64 40, metadata !4, i64 42}
!14 = metadata !{metadata !"CallInfoL", metadata !3, i64 0, metadata !3, i64 4, metadata !9, i64 8}
!15 = metadata !{metadata !3, metadata !3, i64 0}
!16 = metadata !{metadata !17, metadata !3, i64 12}
!17 = metadata !{metadata !"LClosure", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, metadata !4, i64 16}
!18 = metadata !{metadata !13, metadata !3, i64 16}
!19 = metadata !{metadata !20, metadata !3, i64 44}
!20 = metadata !{metadata !"Proto", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !4, i64 7, metadata !4, i64 8, metadata !11, i64 12, metadata !11, i64 16, metadata !11, i64 20, metadata !11, i64 24, metadata !11, i64 28, metadata !11, i64 32, metadata !11, i64 36, metadata !11, i64 40, metadata !3, i64 44, metadata !3, i64 48, metadata !3, i64 52, metadata !3, i64 56, metadata !3, i64 60, metadata !3, i64 64, metadata !3, i64 68, metadata !3, i64 72, metadata !3, i64 76}
!21 = metadata !{i64 0, i64 4, metadata !15, i64 0, i64 4, metadata !15, i64 0, i64 4, metadata !22, i64 0, i64 4, metadata !15, i64 0, i64 8, metadata !23, i64 0, i64 8, metadata !24, i64 8, i64 4, metadata !22}
!22 = metadata !{metadata !11, metadata !11, i64 0}
!23 = metadata !{metadata !9, metadata !9, i64 0}
!24 = metadata !{metadata !25, metadata !25, i64 0}
!25 = metadata !{metadata !"double", metadata !4, i64 0}
!26 = metadata !{metadata !7, metadata !3, i64 8}
!27 = metadata !{metadata !20, metadata !11, i64 28}
!28 = metadata !{metadata !13, metadata !3, i64 4}
