; ModuleID = 'lua_op_loadk_return.c'
target datalayout = "e-m:x-p:32:32-i64:64-f80:32-n8:16:32-S32"
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
%struct.LClosure = type { %struct.GCObject*, i8, i8, i8, %struct.GCObject*, %struct.Proto*, [1 x %struct.UpVal*] }
%struct.Proto = type { %struct.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %struct.TValue*, i32*, %struct.Proto**, i32*, %struct.LocVar*, %struct.Upvaldesc*, %struct.LClosure*, %struct.TString*, %struct.GCObject*, %struct.RaviJITProto }
%struct.LocVar = type { %struct.TString*, i32, i32, i32 }
%struct.Upvaldesc = type { %struct.TString*, i32, i8, i8 }
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i8, i32, %union.anon.1 }
%union.anon.1 = type { i64 }
%struct.RaviJITProto = type { i8, i8*, i32 (%struct.lua_State*)* }

@.str = private unnamed_addr constant [12 x i8] c"value = %d\0A\00", align 1

; Function Attrs: nounwind
define void @testfunc(%struct.GCObject* %obj) #0 {
entry:
  %obj.addr = alloca %struct.GCObject*, align 4
  store %struct.GCObject* %obj, %struct.GCObject** %obj.addr, align 4, !tbaa !1
  %0 = load %struct.GCObject*, %struct.GCObject** %obj.addr, align 4, !tbaa !1
  %tt = getelementptr inbounds %struct.GCObject, %struct.GCObject* %0, i32 0, i32 1
  %1 = load i8, i8* %tt, align 1, !tbaa !5
  %conv = zext i8 %1 to i32
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i32 0, i32 0), i32 %conv)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: nounwind
define void @test1(%struct.lua_State* %L, i32 %b, i32 %x) #0 {
entry:
  %L.addr = alloca %struct.lua_State*, align 4
  %b.addr = alloca i32, align 4
  %x.addr = alloca i32, align 4
  %ci = alloca %struct.CallInfoLua*, align 4
  %cl = alloca %struct.LClosure*, align 4
  %k = alloca %struct.TValue*, align 4
  %base = alloca %struct.TValue*, align 4
  %cil = alloca %struct.CallInfoL*, align 4
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 4, !tbaa !1
  store i32 %b, i32* %b.addr, align 4, !tbaa !7
  store i32 %x, i32* %x.addr, align 4, !tbaa !7
  %0 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.start(i64 4, i8* %0) #2
  %1 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.start(i64 4, i8* %1) #2
  %2 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.start(i64 4, i8* %2) #2
  %3 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.start(i64 4, i8* %3) #2
  %4 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.start(i64 4, i8* %4) #2
  %5 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %ci1 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %5, i32 0, i32 6
  %6 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci1, align 4, !tbaa !9
  store %struct.CallInfoLua* %6, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %7 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %7, i32 0, i32 4
  %base2 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l, i32 0, i32 0
  %8 = load %struct.TValue*, %struct.TValue** %base2, align 4, !tbaa !14
  store %struct.TValue* %8, %struct.TValue** %base, align 4, !tbaa !1
  %9 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l3 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %9, i32 0, i32 4
  %savedpc = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l3, i32 0, i32 1
  store i32* %x.addr, i32** %savedpc, align 4, !tbaa !17
  %10 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.end(i64 4, i8* %10) #2
  %11 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.end(i64 4, i8* %11) #2
  %12 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end(i64 4, i8* %12) #2
  %13 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end(i64 4, i8* %13) #2
  %14 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end(i64 4, i8* %14) #2
  ret void
}

; Function Attrs: nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #2

; Function Attrs: nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #2

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.0 (trunk)"}
!1 = !{!2, !2, i64 0}
!2 = !{!"any pointer", !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
!5 = !{!6, !3, i64 4}
!6 = !{!"GCObject", !2, i64 0, !3, i64 4, !3, i64 5}
!7 = !{!8, !8, i64 0}
!8 = !{!"int", !3, i64 0}
!9 = !{!10, !2, i64 16}
!10 = !{!"lua_State", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !2, i64 8, !2, i64 12, !2, i64 16, !2, i64 20, !2, i64 24, !2, i64 28, !2, i64 32, !2, i64 36, !2, i64 40, !2, i64 44, !11, i64 48, !2, i64 104, !12, i64 112, !8, i64 120, !8, i64 124, !8, i64 128, !13, i64 132, !13, i64 134, !3, i64 136, !3, i64 137}
!11 = !{!"CallInfo", !2, i64 0, !2, i64 4, !2, i64 8, !2, i64 12, !3, i64 16, !12, i64 40, !13, i64 48, !3, i64 50, !3, i64 51}
!12 = !{!"long long", !3, i64 0}
!13 = !{!"short", !3, i64 0}
!14 = !{!15, !2, i64 16}
!15 = !{!"CallInfoLua", !2, i64 0, !2, i64 4, !2, i64 8, !2, i64 12, !16, i64 16, !12, i64 32, !13, i64 40, !3, i64 42, !3, i64 43}
!16 = !{!"CallInfoL", !2, i64 0, !2, i64 4, !12, i64 8}
!17 = !{!15, !2, i64 20}
