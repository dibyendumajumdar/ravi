; ModuleID = 'lua_upval.c'
source_filename = "lua_upval.c"
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
%struct.LClosure = type { %struct.GCObject*, i8, i8, i8, %struct.GCObject*, %struct.Proto*, [1 x %struct.UpVal*] }
%struct.Proto = type { %struct.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %struct.TValue*, i32*, %struct.Proto**, i32*, %struct.LocVar*, %struct.Upvaldesc*, %struct.LClosure*, %struct.TString*, %struct.GCObject*, %struct.RaviJITProto }
%struct.LocVar = type { %struct.TString*, %struct.TString*, i32, i32, i8 }
%struct.Upvaldesc = type { %struct.TString*, %struct.TString*, i8, i8, i8 }
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i8, i32, %union.anon.1 }
%union.anon.1 = type { i64 }
%struct.RaviJITProto = type { i8, i8, i16, i8*, i32 (%struct.lua_State*)* }
%struct.TValue = type { %union.Value, i16 }
%union.Value = type { %struct.GCObject* }

; Function Attrs: nounwind
define void @luaV_op_call(%struct.lua_State* %L, %struct.LClosure* %cl, %struct.TValue* %ra, i32 %b, i32 %c) #0 {
  %c.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %ra.addr = alloca %struct.TValue*, align 8
  %cl.addr = alloca %struct.LClosure*, align 8
  %L.addr = alloca %struct.lua_State*, align 8
  %uv = alloca %struct.UpVal*, align 8
  %b1 = alloca i32, align 4
  %value8 = alloca %struct.TValue*, align 8
  %b2 = alloca i32, align 4
  store i32 %c, i32* %c.addr, align 4, !tbaa !2
  store i32 %b, i32* %b.addr, align 4, !tbaa !2
  store %struct.TValue* %ra, %struct.TValue** %ra.addr, align 8, !tbaa !6
  store %struct.LClosure* %cl, %struct.LClosure** %cl.addr, align 8, !tbaa !6
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 8, !tbaa !6
  %1 = bitcast %struct.UpVal** %uv to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %1) #3
  %2 = load %struct.LClosure*, %struct.LClosure** %cl.addr, align 8, !tbaa !6
  %upvals = getelementptr inbounds %struct.LClosure, %struct.LClosure* %2, i32 0, i32 6
  %3 = load i32, i32* %b.addr, align 4, !tbaa !2
  %idxprom = sext i32 %3 to i64
  %arrayidx = getelementptr inbounds [1 x %struct.UpVal*], [1 x %struct.UpVal*]* %upvals, i64 0, i64 %idxprom
  %4 = load %struct.UpVal*, %struct.UpVal** %arrayidx, align 8, !tbaa !6
  store %struct.UpVal* %4, %struct.UpVal** %uv, align 8, !tbaa !6
  %5 = load %struct.TValue*, %struct.TValue** %ra.addr, align 8, !tbaa !6
  %tt_ = getelementptr inbounds %struct.TValue, %struct.TValue* %5, i32 0, i32 1
  %6 = load i16, i16* %tt_, align 8, !tbaa !8
  %7 = load %struct.UpVal*, %struct.UpVal** %uv, align 8, !tbaa !6
  %v = getelementptr inbounds %struct.UpVal, %struct.UpVal* %7, i32 0, i32 0
  %8 = load %struct.TValue*, %struct.TValue** %v, align 8, !tbaa !11
  %tt_1 = getelementptr inbounds %struct.TValue, %struct.TValue* %8, i32 0, i32 1
  store i16 %6, i16* %tt_1, align 8, !tbaa !8
  %9 = load %struct.TValue*, %struct.TValue** %ra.addr, align 8, !tbaa !6
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %9, i32 0, i32 0
  %n = bitcast %union.Value* %value_ to double*
  %10 = load double, double* %n, align 8, !tbaa !14
  %11 = load %struct.UpVal*, %struct.UpVal** %uv, align 8, !tbaa !6
  %v2 = getelementptr inbounds %struct.UpVal, %struct.UpVal* %11, i32 0, i32 0
  %12 = load %struct.TValue*, %struct.TValue** %v2, align 8, !tbaa !11
  %value_3 = getelementptr inbounds %struct.TValue, %struct.TValue* %12, i32 0, i32 0
  %n4 = bitcast %union.Value* %value_3 to double*
  store double %10, double* %n4, align 8, !tbaa !14
  %13 = bitcast i32* %b1 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %13) #3
  %14 = load %struct.UpVal*, %struct.UpVal** %uv, align 8, !tbaa !6
  %v5 = getelementptr inbounds %struct.UpVal, %struct.UpVal* %14, i32 0, i32 0
  %15 = load %struct.TValue*, %struct.TValue** %v5, align 8, !tbaa !11
  %tt_6 = getelementptr inbounds %struct.TValue, %struct.TValue* %15, i32 0, i32 1
  %16 = load i16, i16* %tt_6, align 8, !tbaa !8
  %conv = zext i16 %16 to i32
  %and = and i32 %conv, 32768
  store i32 %and, i32* %b1, align 4, !tbaa !2
  %17 = load %struct.UpVal*, %struct.UpVal** %uv, align 8, !tbaa !6
  %u = getelementptr inbounds %struct.UpVal, %struct.UpVal* %17, i32 0, i32 2
  %value = bitcast %union.anon.0* %u to %struct.TValue*
  %tt_7 = getelementptr inbounds %struct.TValue, %struct.TValue* %value, i32 0, i32 1
  store i16 1, i16* %tt_7, align 8, !tbaa !14
  %18 = bitcast %struct.TValue** %value8 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %18) #3
  %19 = load %struct.UpVal*, %struct.UpVal** %uv, align 8, !tbaa !6
  %u9 = getelementptr inbounds %struct.UpVal, %struct.UpVal* %19, i32 0, i32 2
  %value10 = bitcast %union.anon.0* %u9 to %struct.TValue*
  store %struct.TValue* %value10, %struct.TValue** %value8, align 8, !tbaa !6
  %20 = bitcast i32* %b2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %20) #3
  %21 = load %struct.UpVal*, %struct.UpVal** %uv, align 8, !tbaa !6
  %v11 = getelementptr inbounds %struct.UpVal, %struct.UpVal* %21, i32 0, i32 0
  %22 = load %struct.TValue*, %struct.TValue** %v11, align 8, !tbaa !11
  %23 = load %struct.TValue*, %struct.TValue** %value8, align 8, !tbaa !6
  %cmp = icmp ne %struct.TValue* %22, %23
  %conv12 = zext i1 %cmp to i32
  store i32 %conv12, i32* %b2, align 4, !tbaa !2
  %24 = load i32, i32* %b1, align 4, !tbaa !2
  %tobool = icmp ne i32 %24, 0
  br i1 %tobool, label %25, label %30

; <label>:25:                                     ; preds = %0
  %26 = load i32, i32* %b2, align 4, !tbaa !2
  %tobool13 = icmp ne i32 %26, 0
  br i1 %tobool13, label %30, label %27

; <label>:27:                                     ; preds = %25
  %28 = load %struct.UpVal*, %struct.UpVal** %uv, align 8, !tbaa !6
  %29 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !6
  call void @luaC_upvalbarrier_(%struct.lua_State* %29, %struct.UpVal* %28)
  br label %30

; <label>:30:                                     ; preds = %27, %25, %0
  %31 = bitcast i32* %b2 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %31) #3
  %32 = bitcast %struct.TValue** %value8 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %32) #3
  %33 = bitcast i32* %b1 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %33) #3
  %34 = bitcast %struct.UpVal** %uv to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %34) #3
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #1

declare void @luaC_upvalbarrier_(%struct.lua_State*, %struct.UpVal*) #2

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
!8 = !{!9, !10, i64 8}
!9 = !{!"TValue", !4, i64 0, !10, i64 8}
!10 = !{!"short", !4, i64 0}
!11 = !{!12, !7, i64 0}
!12 = !{!"UpVal", !7, i64 0, !13, i64 8, !4, i64 16}
!13 = !{!"long long", !4, i64 0}
!14 = !{!4, !4, i64 0}
