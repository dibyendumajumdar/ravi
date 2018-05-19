; ModuleID = 'lua_op_loadk_return.c'
source_filename = "lua_op_loadk_return.c"
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8, i16, i8 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i16, i16, i8, i8 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.TValue = type { %union.Value, i16 }
%union.Value = type { %struct.GCObject* }
%struct.UpVal = type { %struct.TValue*, i64, %union.anon.0 }
%union.anon.0 = type { %struct.anon }
%struct.anon = type { %struct.UpVal*, i32 }
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

$"\01??_C@_0M@KDKGEOFG@value?5?$DN?5?$CFd?6?$AA@" = comdat any

@"\01??_C@_0M@KDKGEOFG@value?5?$DN?5?$CFd?6?$AA@" = linkonce_odr unnamed_addr constant [12 x i8] c"value = %d\0A\00", comdat, align 1

; Function Attrs: nounwind
define void @testfunc(%struct.GCObject* %obj) #0 {
  %obj.addr = alloca %struct.GCObject*, align 8
  store %struct.GCObject* %obj, %struct.GCObject** %obj.addr, align 8, !tbaa !2
  %1 = load %struct.GCObject*, %struct.GCObject** %obj.addr, align 8, !tbaa !2
  %tt = getelementptr inbounds %struct.GCObject, %struct.GCObject* %1, i32 0, i32 1
  %2 = load i8, i8* %tt, align 8, !tbaa !6
  %conv = zext i8 %2 to i32
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"\01??_C@_0M@KDKGEOFG@value?5?$DN?5?$CFd?6?$AA@", i32 0, i32 0), i32 %conv)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: nounwind
define void @test1(%struct.lua_State* %L, i32 %b, i32 %x) #0 {
  %x.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %L.addr = alloca %struct.lua_State*, align 8
  %ci = alloca %struct.CallInfoLua*, align 8
  %cl = alloca %struct.LClosure*, align 8
  %k = alloca %struct.TValue*, align 8
  %base = alloca %struct.TValue*, align 8
  %cil = alloca %struct.CallInfoL*, align 8
  store i32 %x, i32* %x.addr, align 4, !tbaa !8
  store i32 %b, i32* %b.addr, align 4, !tbaa !8
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %1 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %1) #3
  %2 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %2) #3
  %3 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %3) #3
  %4 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %4) #3
  %5 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %5) #3
  %6 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %ci1 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %6, i32 0, i32 6
  %7 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci1, align 8, !tbaa !10
  store %struct.CallInfoLua* %7, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %8 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %8, i32 0, i32 4
  %base2 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l, i32 0, i32 0
  %9 = load %struct.TValue*, %struct.TValue** %base2, align 8, !tbaa !15
  store %struct.TValue* %9, %struct.TValue** %base, align 8, !tbaa !2
  %10 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l3 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %10, i32 0, i32 4
  %savedpc = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l3, i32 0, i32 1
  store i32* %x.addr, i32** %savedpc, align 8, !tbaa !18
  %11 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %11) #3
  %12 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %12) #3
  %13 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %13) #3
  %14 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %14) #3
  %15 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %15) #3
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #2

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{!"clang version 6.0.0 (tags/RELEASE_600/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"any pointer", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !4, i64 8}
!7 = !{!"GCObject", !3, i64 0, !4, i64 8, !4, i64 9}
!8 = !{!9, !9, i64 0}
!9 = !{!"int", !4, i64 0}
!10 = !{!11, !3, i64 32}
!11 = !{!"lua_State", !3, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !3, i64 16, !3, i64 24, !3, i64 32, !3, i64 40, !3, i64 48, !3, i64 56, !3, i64 64, !3, i64 72, !3, i64 80, !3, i64 88, !12, i64 96, !3, i64 168, !13, i64 176, !9, i64 184, !9, i64 188, !9, i64 192, !14, i64 196, !14, i64 198, !4, i64 200, !4, i64 201, !14, i64 202, !4, i64 204}
!12 = !{!"CallInfo", !3, i64 0, !3, i64 8, !3, i64 16, !3, i64 24, !4, i64 32, !13, i64 56, !14, i64 64, !14, i64 66, !14, i64 68, !4, i64 70, !4, i64 71}
!13 = !{!"long long", !4, i64 0}
!14 = !{!"short", !4, i64 0}
!15 = !{!16, !3, i64 32}
!16 = !{!"CallInfoLua", !3, i64 0, !3, i64 8, !3, i64 16, !3, i64 24, !17, i64 32, !13, i64 56, !14, i64 64, !14, i64 66, !14, i64 68, !4, i64 70, !4, i64 71}
!17 = !{!"CallInfoL", !3, i64 0, !3, i64 8, !13, i64 16}
!18 = !{!16, !3, i64 40}
