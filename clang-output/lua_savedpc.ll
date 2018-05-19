; ModuleID = 'lua_savedpc.c'
source_filename = "lua_savedpc.c"
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8, i16, i8 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i16, i16, i8, i8 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.TValue = type { %union.Value, i16 }
%union.Value = type { %struct.GCObject* }
%struct.UpVal = type { %struct.TValue*, i64, %union.anon.0 }
%union.anon.0 = type { %struct.anon }
%struct.anon = type { %struct.UpVal*, i32 }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.lua_longjmp = type opaque
%struct.CallInfo = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %union.anon, i64, i16, i16, i16, i8, i8 }
%union.anon = type { %struct.CallInfoL }
%struct.lua_Debug = type opaque
%struct.LClosure = type { %struct.GCObject*, i8, i8, i8, %struct.GCObject*, %struct.Proto*, [1 x %struct.UpVal*] }
%struct.Proto = type { %struct.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %struct.TValue*, i32*, %struct.Proto**, i32*, %struct.LocVar*, %struct.Upvaldesc*, %struct.LClosure*, %struct.TString*, %struct.GCObject*, %struct.RaviJITProto }
%struct.LocVar = type { %struct.TString*, %struct.TString*, i32, i32, i8 }
%struct.Upvaldesc = type { %struct.TString*, %struct.TString*, i8, i8, i8 }
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i8, i32, %union.anon.1 }
%union.anon.1 = type { i64 }
%struct.RaviJITProto = type { i8, i8, i16, i8*, i32 (%struct.lua_State*)* }

; Function Attrs: nounwind
define void @test1(%struct.lua_State* %L, i32 %b) #0 {
  %b.addr = alloca i32, align 4
  %L.addr = alloca %struct.lua_State*, align 8
  %ci = alloca %struct.CallInfoLua*, align 8
  %cl = alloca %struct.LClosure*, align 8
  %p = alloca %struct.Proto*, align 8
  store i32 %b, i32* %b.addr, align 4, !tbaa !2
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %1 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %1) #3
  %2 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %2) #3
  %3 = bitcast %struct.Proto** %p to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %3) #3
  %4 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %ci1 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %4, i32 0, i32 6
  %5 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci1, align 8, !tbaa !8
  store %struct.CallInfoLua* %5, %struct.CallInfoLua** %ci, align 8, !tbaa !6
  %6 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !6
  %func = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %6, i32 0, i32 0
  %7 = load %struct.TValue*, %struct.TValue** %func, align 8, !tbaa !13
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %7, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %8 = load %struct.GCObject*, %struct.GCObject** %gc, align 8, !tbaa !16
  %9 = bitcast %struct.GCObject* %8 to %struct.LClosure*
  store %struct.LClosure* %9, %struct.LClosure** %cl, align 8, !tbaa !6
  %10 = load %struct.LClosure*, %struct.LClosure** %cl, align 8, !tbaa !6
  %p2 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %10, i32 0, i32 5
  %11 = load %struct.Proto*, %struct.Proto** %p2, align 8, !tbaa !17
  store %struct.Proto* %11, %struct.Proto** %p, align 8, !tbaa !6
  %12 = load %struct.Proto*, %struct.Proto** %p, align 8, !tbaa !6
  %code = getelementptr inbounds %struct.Proto, %struct.Proto* %12, i32 0, i32 15
  %13 = load i32*, i32** %code, align 8, !tbaa !19
  %14 = load i32, i32* %b.addr, align 4, !tbaa !2
  %idxprom = sext i32 %14 to i64
  %arrayidx = getelementptr inbounds i32, i32* %13, i64 %idxprom
  %15 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !6
  %l = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %15, i32 0, i32 4
  %savedpc = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l, i32 0, i32 1
  store i32* %arrayidx, i32** %savedpc, align 8, !tbaa !22
  %16 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %hookmask = getelementptr inbounds %struct.lua_State, %struct.lua_State* %16, i32 0, i32 22
  %17 = load i8, i8* %hookmask, align 8, !tbaa !23
  %conv = zext i8 %17 to i32
  %and = and i32 %conv, 12
  %tobool = icmp ne i32 %and, 0
  br i1 %tobool, label %18, label %26

; <label>:18:                                     ; preds = %0
  %19 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %hookcount = getelementptr inbounds %struct.lua_State, %struct.lua_State* %19, i32 0, i32 19
  %20 = load i32, i32* %hookcount, align 8, !tbaa !24
  %dec = add nsw i32 %20, -1
  store i32 %dec, i32* %hookcount, align 8, !tbaa !24
  %cmp = icmp eq i32 %dec, 0
  br i1 %cmp, label %24, label %21

; <label>:21:                                     ; preds = %18
  %22 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %hookmask4 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %22, i32 0, i32 22
  %23 = load i8, i8* %hookmask4, align 8, !tbaa !23
  %conv5 = zext i8 %23 to i32
  %and6 = and i32 %conv5, 4
  %tobool7 = icmp ne i32 %and6, 0
  br i1 %tobool7, label %24, label %26

; <label>:24:                                     ; preds = %21, %18
  %25 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  call void @luaG_traceexec(%struct.lua_State* %25)
  br label %26

; <label>:26:                                     ; preds = %24, %21, %0
  %27 = bitcast %struct.Proto** %p to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %27) #3
  %28 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %28) #3
  %29 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %29) #3
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #1

declare void @luaG_traceexec(%struct.lua_State*) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{!"clang version 6.0.0 (tags/RELEASE_600/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"int", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"any pointer", !4, i64 0}
!8 = !{!9, !7, i64 32}
!9 = !{!"lua_State", !7, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !7, i64 16, !7, i64 24, !7, i64 32, !7, i64 40, !7, i64 48, !7, i64 56, !7, i64 64, !7, i64 72, !7, i64 80, !7, i64 88, !10, i64 96, !7, i64 168, !11, i64 176, !3, i64 184, !3, i64 188, !3, i64 192, !12, i64 196, !12, i64 198, !4, i64 200, !4, i64 201, !12, i64 202, !4, i64 204}
!10 = !{!"CallInfo", !7, i64 0, !7, i64 8, !7, i64 16, !7, i64 24, !4, i64 32, !11, i64 56, !12, i64 64, !12, i64 66, !12, i64 68, !4, i64 70, !4, i64 71}
!11 = !{!"long long", !4, i64 0}
!12 = !{!"short", !4, i64 0}
!13 = !{!14, !7, i64 0}
!14 = !{!"CallInfoLua", !7, i64 0, !7, i64 8, !7, i64 16, !7, i64 24, !15, i64 32, !11, i64 56, !12, i64 64, !12, i64 66, !12, i64 68, !4, i64 70, !4, i64 71}
!15 = !{!"CallInfoL", !7, i64 0, !7, i64 8, !11, i64 16}
!16 = !{!4, !4, i64 0}
!17 = !{!18, !7, i64 24}
!18 = !{!"LClosure", !7, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !7, i64 16, !7, i64 24, !4, i64 32}
!19 = !{!20, !7, i64 56}
!20 = !{!"Proto", !7, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !4, i64 11, !4, i64 12, !3, i64 16, !3, i64 20, !3, i64 24, !3, i64 28, !3, i64 32, !3, i64 36, !3, i64 40, !3, i64 44, !7, i64 48, !7, i64 56, !7, i64 64, !7, i64 72, !7, i64 80, !7, i64 88, !7, i64 96, !7, i64 104, !7, i64 112, !21, i64 120}
!21 = !{!"RaviJITProto", !4, i64 0, !4, i64 1, !12, i64 2, !7, i64 8, !7, i64 16}
!22 = !{!14, !7, i64 40}
!23 = !{!9, !4, i64 200}
!24 = !{!9, !3, i64 192}
