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
  %1 = load %struct.TValue** %base2, align 4, !tbaa !12
  %2 = bitcast %struct.CallInfoLua* %0 to %struct.LClosure***
  %3 = load %struct.LClosure*** %2, align 4, !tbaa !15
  %4 = load %struct.LClosure** %3, align 4, !tbaa !16
  %p = getelementptr inbounds %struct.LClosure* %4, i32 0, i32 5
  %5 = load %struct.Proto** %p, align 4, !tbaa !17
  %k3 = getelementptr inbounds %struct.Proto* %5, i32 0, i32 14
  %6 = load %struct.TValue** %k3, align 4, !tbaa !19
  %call = tail call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %1, %struct.TValue* %6) #2
  %cmp = icmp eq i32 %call, 0
  br i1 %cmp, label %label6, label %if.end13

if.end13:                                         ; preds = %entry
  %7 = load %struct.TValue** %base2, align 4, !tbaa !12
  %add.ptr16 = getelementptr inbounds %struct.TValue* %7, i32 1
  %add.ptr17 = getelementptr inbounds %struct.TValue* %6, i32 1
  %8 = bitcast %struct.TValue* %add.ptr16 to i8*
  %9 = bitcast %struct.TValue* %add.ptr17 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %8, i8* %9, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr20 = getelementptr inbounds %struct.TValue* %7, i32 2
  %top = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr20, %struct.TValue** %top, align 4, !tbaa !26
  %call21 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr16) #2
  %tobool = icmp eq i32 %call21, 0
  br i1 %tobool, label %return, label %if.then22

if.then22:                                        ; preds = %if.end13
  %top23 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %10 = load %struct.TValue** %top23, align 4, !tbaa !27
  store %struct.TValue* %10, %struct.TValue** %top, align 4, !tbaa !26
  br label %return

label6:                                           ; preds = %entry
  %add.ptr28 = getelementptr inbounds %struct.TValue* %6, i32 2
  %call31 = tail call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %1, %struct.TValue* %add.ptr28) #2
  %cmp32 = icmp eq i32 %call31, 0
  br i1 %cmp32, label %label11, label %if.end44

if.end44:                                         ; preds = %label6
  %11 = load %struct.TValue** %base2, align 4, !tbaa !12
  %add.ptr47 = getelementptr inbounds %struct.TValue* %11, i32 1
  %add.ptr48 = getelementptr inbounds %struct.TValue* %6, i32 3
  %12 = bitcast %struct.TValue* %add.ptr47 to i8*
  %13 = bitcast %struct.TValue* %add.ptr48 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %12, i8* %13, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr51 = getelementptr inbounds %struct.TValue* %11, i32 2
  %top52 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr51, %struct.TValue** %top52, align 4, !tbaa !26
  %call53 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr47) #2
  %tobool54 = icmp eq i32 %call53, 0
  br i1 %tobool54, label %return, label %if.then55

if.then55:                                        ; preds = %if.end44
  %top56 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %14 = load %struct.TValue** %top56, align 4, !tbaa !27
  store %struct.TValue* %14, %struct.TValue** %top52, align 4, !tbaa !26
  br label %return

label11:                                          ; preds = %label6
  %add.ptr59 = getelementptr inbounds %struct.TValue* %1, i32 1
  %add.ptr60 = getelementptr inbounds %struct.TValue* %6, i32 4
  %15 = bitcast %struct.TValue* %add.ptr59 to i8*
  %16 = bitcast %struct.TValue* %add.ptr60 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %15, i8* %16, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr63 = getelementptr inbounds %struct.TValue* %1, i32 2
  %top64 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr63, %struct.TValue** %top64, align 4, !tbaa !26
  %call65 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr59) #2
  %tobool66 = icmp eq i32 %call65, 0
  br i1 %tobool66, label %return, label %if.then67

if.then67:                                        ; preds = %label11
  %top68 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %17 = load %struct.TValue** %top68, align 4, !tbaa !27
  store %struct.TValue* %17, %struct.TValue** %top64, align 4, !tbaa !26
  br label %return

return:                                           ; preds = %if.end13, %if.end44, %label11, %if.then67, %if.then55, %if.then22
  ret void
}

declare i32 @luaV_equalobj(...) #1

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
!26 = metadata !{metadata !7, metadata !3, i64 8}
!27 = metadata !{metadata !13, metadata !3, i64 4}
