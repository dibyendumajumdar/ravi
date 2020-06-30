; ModuleID = 'lua_op_call.c'
source_filename = "lua_op_call.c"
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8, i16, i8 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i16, i16, i8, i8 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.UpVal = type { %struct.TValue*, i64, %union.anon.0 }
%union.anon.0 = type { %struct.anon }
%struct.anon = type { %struct.UpVal*, i32 }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.lua_longjmp = type opaque
%struct.CallInfo = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %union.anon, i64, i16, i16, i16, i8, i8 }
%union.anon = type { %struct.CallInfoL }
%struct.lua_Debug = type opaque
%struct.TValue = type { %union.Value, i16 }
%union.Value = type { %struct.GCObject* }

; Function Attrs: nounwind
define void @luaV_op_call(%struct.lua_State* %L, %struct.CallInfo* %ci, %struct.TValue* %ra, i32 %b, i32 %c) #0 {
  %c.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %ra.addr = alloca %struct.TValue*, align 8
  %ci.addr = alloca %struct.CallInfo*, align 8
  %L.addr = alloca %struct.lua_State*, align 8
  %nresults = alloca i32, align 4
  %c_or_compiled = alloca i32, align 4
  store i32 %c, i32* %c.addr, align 4, !tbaa !2
  store i32 %b, i32* %b.addr, align 4, !tbaa !2
  store %struct.TValue* %ra, %struct.TValue** %ra.addr, align 8, !tbaa !6
  store %struct.CallInfo* %ci, %struct.CallInfo** %ci.addr, align 8, !tbaa !6
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %1 = bitcast i32* %nresults to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %1) #3
  %2 = load i32, i32* %c.addr, align 4, !tbaa !2
  %sub = sub nsw i32 %2, 1
  store i32 %sub, i32* %nresults, align 4, !tbaa !2
  %3 = load i32, i32* %b.addr, align 4, !tbaa !2
  %cmp = icmp ne i32 %3, 0
  br i1 %cmp, label %4, label %8

; <label>:4:                                      ; preds = %0
  %5 = load %struct.TValue*, %struct.TValue** %ra.addr, align 8, !tbaa !6
  %6 = load i32, i32* %b.addr, align 4, !tbaa !2
  %idx.ext = sext i32 %6 to i64
  %add.ptr = getelementptr inbounds %struct.TValue, %struct.TValue* %5, i64 %idx.ext
  %7 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %top = getelementptr inbounds %struct.lua_State, %struct.lua_State* %7, i32 0, i32 4
  store %struct.TValue* %add.ptr, %struct.TValue** %top, align 8, !tbaa !8
  br label %8

; <label>:8:                                      ; preds = %4, %0
  %9 = bitcast i32* %c_or_compiled to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %9) #3
  %10 = load i32, i32* %nresults, align 4, !tbaa !2
  %11 = load %struct.TValue*, %struct.TValue** %ra.addr, align 8, !tbaa !6
  %12 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %call = call i32 @luaD_precall(%struct.lua_State* %12, %struct.TValue* %11, i32 %10, i32 1)
  store i32 %call, i32* %c_or_compiled, align 4, !tbaa !2
  %13 = load i32, i32* %c_or_compiled, align 4, !tbaa !2
  %tobool = icmp ne i32 %13, 0
  br i1 %tobool, label %14, label %23

; <label>:14:                                     ; preds = %8
  %15 = load i32, i32* %c_or_compiled, align 4, !tbaa !2
  %cmp1 = icmp eq i32 %15, 1
  br i1 %cmp1, label %16, label %22

; <label>:16:                                     ; preds = %14
  %17 = load i32, i32* %nresults, align 4, !tbaa !2
  %cmp2 = icmp sge i32 %17, 0
  br i1 %cmp2, label %18, label %22

; <label>:18:                                     ; preds = %16
  %19 = load %struct.CallInfo*, %struct.CallInfo** %ci.addr, align 8, !tbaa !6
  %top3 = getelementptr inbounds %struct.CallInfo, %struct.CallInfo* %19, i32 0, i32 1
  %20 = load %struct.TValue*, %struct.TValue** %top3, align 8, !tbaa !13
  %21 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %top4 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %21, i32 0, i32 4
  store %struct.TValue* %20, %struct.TValue** %top4, align 8, !tbaa !8
  br label %22

; <label>:22:                                     ; preds = %18, %16, %14
  br label %25

; <label>:23:                                     ; preds = %8
  %24 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  call void @luaV_execute(%struct.lua_State* %24)
  br label %25

; <label>:25:                                     ; preds = %23, %22
  %26 = bitcast i32* %c_or_compiled to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %26) #3
  %27 = bitcast i32* %nresults to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %27) #3
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #1

declare i32 @luaD_precall(%struct.lua_State*, %struct.TValue*, i32, i32) #2

declare void @luaV_execute(%struct.lua_State*) #2

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
!8 = !{!9, !7, i64 16}
!9 = !{!"lua_State", !7, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !7, i64 16, !7, i64 24, !7, i64 32, !7, i64 40, !7, i64 48, !7, i64 56, !7, i64 64, !7, i64 72, !7, i64 80, !7, i64 88, !10, i64 96, !7, i64 168, !11, i64 176, !3, i64 184, !3, i64 188, !3, i64 192, !12, i64 196, !12, i64 198, !4, i64 200, !4, i64 201, !12, i64 202, !4, i64 204}
!10 = !{!"CallInfo", !7, i64 0, !7, i64 8, !7, i64 16, !7, i64 24, !4, i64 32, !11, i64 56, !12, i64 64, !12, i64 66, !12, i64 68, !4, i64 70, !4, i64 71}
!11 = !{!"long long", !4, i64 0}
!12 = !{!"short", !4, i64 0}
!13 = !{!10, !7, i64 8}
