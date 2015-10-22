; ModuleID = 'lua_savedpc.c'
target datalayout = "e-m:x-p:32:32-i64:64-f80:32-n8:16:32-S32"
target triple = "i686-pc-windows-gnu"

%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i8, i8 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.TValue = type { %union.Value, i32 }
%union.Value = type { i64 }
%struct.UpVal = type { %struct.TValue*, i64, %union.anon.0 }
%union.anon.0 = type { %struct.TValue }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
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

; Function Attrs: nounwind
define void @test1(%struct.lua_State* %L, i32 %b) #0 {
entry:
  %L.addr = alloca %struct.lua_State*, align 4
  %b.addr = alloca i32, align 4
  %ci = alloca %struct.CallInfoLua*, align 4
  %cl = alloca %struct.LClosure*, align 4
  %p = alloca %struct.Proto*, align 4
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 4, !tbaa !1
  store i32 %b, i32* %b.addr, align 4, !tbaa !5
  %0 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.start(i64 4, i8* %0) #1
  %1 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.start(i64 4, i8* %1) #1
  %2 = bitcast %struct.Proto** %p to i8*
  call void @llvm.lifetime.start(i64 4, i8* %2) #1
  %3 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %ci1 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %3, i32 0, i32 6
  %4 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci1, align 4, !tbaa !7
  store %struct.CallInfoLua* %4, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %5 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %func = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %5, i32 0, i32 0
  %6 = load %struct.TValue*, %struct.TValue** %func, align 4, !tbaa !12
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %6, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %7 = load %struct.GCObject*, %struct.GCObject** %gc, align 4, !tbaa !1
  %8 = bitcast %struct.GCObject* %7 to %struct.LClosure*
  store %struct.LClosure* %8, %struct.LClosure** %cl, align 4, !tbaa !1
  %9 = load %struct.LClosure*, %struct.LClosure** %cl, align 4, !tbaa !1
  %p2 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %9, i32 0, i32 5
  %10 = load %struct.Proto*, %struct.Proto** %p2, align 4, !tbaa !15
  store %struct.Proto* %10, %struct.Proto** %p, align 4, !tbaa !1
  %11 = load i32, i32* %b.addr, align 4, !tbaa !5
  %12 = load %struct.Proto*, %struct.Proto** %p, align 4, !tbaa !1
  %code = getelementptr inbounds %struct.Proto, %struct.Proto* %12, i32 0, i32 15
  %13 = load i32*, i32** %code, align 4, !tbaa !17
  %arrayidx = getelementptr inbounds i32, i32* %13, i32 %11
  %14 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %14, i32 0, i32 4
  %savedpc = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l, i32 0, i32 1
  store i32* %arrayidx, i32** %savedpc, align 4, !tbaa !20
  %15 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %hookmask = getelementptr inbounds %struct.lua_State, %struct.lua_State* %15, i32 0, i32 22
  %16 = load i8, i8* %hookmask, align 1, !tbaa !21
  %conv = zext i8 %16 to i32
  %and = and i32 %conv, 12
  %tobool = icmp ne i32 %and, 0
  br i1 %tobool, label %land.lhs.true, label %if.end

land.lhs.true:                                    ; preds = %entry
  %17 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %hookcount = getelementptr inbounds %struct.lua_State, %struct.lua_State* %17, i32 0, i32 19
  %18 = load i32, i32* %hookcount, align 4, !tbaa !22
  %dec = add nsw i32 %18, -1
  store i32 %dec, i32* %hookcount, align 4, !tbaa !22
  %cmp = icmp eq i32 %dec, 0
  br i1 %cmp, label %if.then, label %lor.lhs.false

lor.lhs.false:                                    ; preds = %land.lhs.true
  %19 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %hookmask4 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %19, i32 0, i32 22
  %20 = load i8, i8* %hookmask4, align 1, !tbaa !21
  %conv5 = zext i8 %20 to i32
  %and6 = and i32 %conv5, 4
  %tobool7 = icmp ne i32 %and6, 0
  br i1 %tobool7, label %if.then, label %if.end

if.then:                                          ; preds = %lor.lhs.false, %land.lhs.true
  %21 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  call void @luaG_traceexec(%struct.lua_State* %21)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.lhs.false, %entry
  %22 = bitcast %struct.Proto** %p to i8*
  call void @llvm.lifetime.end(i64 4, i8* %22) #1
  %23 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end(i64 4, i8* %23) #1
  %24 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end(i64 4, i8* %24) #1
  ret void
}

; Function Attrs: nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #1

declare void @luaG_traceexec(%struct.lua_State*) #2

; Function Attrs: nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }
attributes #2 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.0 (trunk)"}
!1 = !{!2, !2, i64 0}
!2 = !{!"any pointer", !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !3, i64 0}
!7 = !{!8, !2, i64 16}
!8 = !{!"lua_State", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !2, i64 8, !2, i64 12, !2, i64 16, !2, i64 20, !2, i64 24, !2, i64 28, !2, i64 32, !2, i64 36, !2, i64 40, !2, i64 44, !9, i64 48, !2, i64 104, !10, i64 112, !6, i64 120, !6, i64 124, !6, i64 128, !11, i64 132, !11, i64 134, !3, i64 136, !3, i64 137}
!9 = !{!"CallInfo", !2, i64 0, !2, i64 4, !2, i64 8, !2, i64 12, !3, i64 16, !10, i64 40, !11, i64 48, !3, i64 50, !3, i64 51}
!10 = !{!"long long", !3, i64 0}
!11 = !{!"short", !3, i64 0}
!12 = !{!13, !2, i64 0}
!13 = !{!"CallInfoLua", !2, i64 0, !2, i64 4, !2, i64 8, !2, i64 12, !14, i64 16, !10, i64 32, !11, i64 40, !3, i64 42, !3, i64 43}
!14 = !{!"CallInfoL", !2, i64 0, !2, i64 4, !10, i64 8}
!15 = !{!16, !2, i64 12}
!16 = !{!"LClosure", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !2, i64 8, !2, i64 12, !3, i64 16}
!17 = !{!18, !2, i64 48}
!18 = !{!"Proto", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !3, i64 7, !3, i64 8, !6, i64 12, !6, i64 16, !6, i64 20, !6, i64 24, !6, i64 28, !6, i64 32, !6, i64 36, !6, i64 40, !2, i64 44, !2, i64 48, !2, i64 52, !2, i64 56, !2, i64 60, !2, i64 64, !2, i64 68, !2, i64 72, !2, i64 76, !19, i64 80}
!19 = !{!"RaviJITProto", !3, i64 0, !2, i64 4, !2, i64 8}
!20 = !{!13, !2, i64 20}
!21 = !{!8, !3, i64 136}
!22 = !{!8, !6, i64 128}
