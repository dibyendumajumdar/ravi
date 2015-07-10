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
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i32, i64, %struct.TString* }
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
define void @test1(%struct.lua_State* %L, i32 %b) #0 {
entry:
  %L.addr = alloca %struct.lua_State*, align 4
  %b.addr = alloca i32, align 4
  %ci = alloca %struct.CallInfoLua*, align 4
  %cl = alloca %struct.LClosure*, align 4
  %k = alloca %struct.TValue*, align 4
  %base = alloca %struct.TValue*, align 4
  %cil = alloca %struct.CallInfoL*, align 4
  %ra = alloca %struct.TValue*, align 4
  %rb = alloca %struct.TValue*, align 4
  %ra2 = alloca %struct.TValue*, align 4
  %rb2 = alloca %struct.TValue*, align 4
  %ra3 = alloca %struct.TValue*, align 4
  %n = alloca i32, align 4
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 4, !tbaa !1
  store i32 %b, i32* %b.addr, align 4, !tbaa !7
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
  %func = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %9, i32 0, i32 0
  %10 = load %struct.TValue*, %struct.TValue** %func, align 4, !tbaa !17
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %10, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %11 = load %struct.GCObject*, %struct.GCObject** %gc, align 4, !tbaa !1
  %12 = bitcast %struct.GCObject* %11 to %struct.LClosure*
  store %struct.LClosure* %12, %struct.LClosure** %cl, align 4, !tbaa !1
  %13 = load %struct.LClosure*, %struct.LClosure** %cl, align 4, !tbaa !1
  %p = getelementptr inbounds %struct.LClosure, %struct.LClosure* %13, i32 0, i32 5
  %14 = load %struct.Proto*, %struct.Proto** %p, align 4, !tbaa !18
  %k3 = getelementptr inbounds %struct.Proto, %struct.Proto* %14, i32 0, i32 14
  %15 = load %struct.TValue*, %struct.TValue** %k3, align 4, !tbaa !20
  store %struct.TValue* %15, %struct.TValue** %k, align 4, !tbaa !1
  %16 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.start(i64 4, i8* %16) #2
  %17 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr = getelementptr inbounds %struct.TValue, %struct.TValue* %17, i32 0
  store %struct.TValue* %add.ptr, %struct.TValue** %ra, align 4, !tbaa !1
  %18 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.start(i64 4, i8* %18) #2
  %19 = load %struct.TValue*, %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr4 = getelementptr inbounds %struct.TValue, %struct.TValue* %19, i32 0
  store %struct.TValue* %add.ptr4, %struct.TValue** %rb, align 4, !tbaa !1
  %20 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %21 = load %struct.TValue*, %struct.TValue** %rb, align 4, !tbaa !1
  %22 = bitcast %struct.TValue* %20 to i8*
  %23 = bitcast %struct.TValue* %21 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %22, i8* %23, i32 16, i32 8, i1 false), !tbaa.struct !23
  %24 = bitcast %struct.TValue** %ra2 to i8*
  call void @llvm.lifetime.start(i64 4, i8* %24) #2
  %25 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr5 = getelementptr inbounds %struct.TValue, %struct.TValue* %25, i32 1
  store %struct.TValue* %add.ptr5, %struct.TValue** %ra2, align 4, !tbaa !1
  %26 = bitcast %struct.TValue** %rb2 to i8*
  call void @llvm.lifetime.start(i64 4, i8* %26) #2
  %27 = load %struct.TValue*, %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr6 = getelementptr inbounds %struct.TValue, %struct.TValue* %27, i32 1
  store %struct.TValue* %add.ptr6, %struct.TValue** %rb2, align 4, !tbaa !1
  %28 = load %struct.TValue*, %struct.TValue** %ra2, align 4, !tbaa !1
  %29 = load %struct.TValue*, %struct.TValue** %rb2, align 4, !tbaa !1
  %30 = bitcast %struct.TValue* %28 to i8*
  %31 = bitcast %struct.TValue* %29 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %30, i8* %31, i32 16, i32 8, i1 false), !tbaa.struct !23
  %32 = bitcast %struct.TValue** %ra3 to i8*
  call void @llvm.lifetime.start(i64 4, i8* %32) #2
  %33 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr7 = getelementptr inbounds %struct.TValue, %struct.TValue* %33, i32 0
  store %struct.TValue* %add.ptr7, %struct.TValue** %ra3, align 4, !tbaa !1
  %34 = load %struct.LClosure*, %struct.LClosure** %cl, align 4, !tbaa !1
  %p8 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %34, i32 0, i32 5
  %35 = load %struct.Proto*, %struct.Proto** %p8, align 4, !tbaa !18
  %sizep = getelementptr inbounds %struct.Proto, %struct.Proto* %35, i32 0, i32 10
  %36 = load i32, i32* %sizep, align 4, !tbaa !27
  %cmp = icmp sgt i32 %36, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %37 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %38 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  call void @luaF_close(%struct.lua_State* %37, %struct.TValue* %38)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %39 = bitcast i32* %n to i8*
  call void @llvm.lifetime.start(i64 4, i8* %39) #2
  %40 = load i32, i32* %b.addr, align 4, !tbaa !7
  %cmp9 = icmp ne i32 %40, 0
  br i1 %cmp9, label %cond.true, label %cond.false

cond.true:                                        ; preds = %if.end
  %41 = load i32, i32* %b.addr, align 4, !tbaa !7
  %sub = sub nsw i32 %41, 1
  br label %cond.end

cond.false:                                       ; preds = %if.end
  %42 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top = getelementptr inbounds %struct.lua_State, %struct.lua_State* %42, i32 0, i32 4
  %43 = load %struct.TValue*, %struct.TValue** %top, align 4, !tbaa !28
  %44 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %sub.ptr.lhs.cast = ptrtoint %struct.TValue* %43 to i32
  %sub.ptr.rhs.cast = ptrtoint %struct.TValue* %44 to i32
  %sub.ptr.sub = sub i32 %sub.ptr.lhs.cast, %sub.ptr.rhs.cast
  %sub.ptr.div = sdiv exact i32 %sub.ptr.sub, 16
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond = phi i32 [ %sub, %cond.true ], [ %sub.ptr.div, %cond.false ]
  store i32 %cond, i32* %n, align 4, !tbaa !7
  %45 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %46 = load %struct.TValue*, %struct.TValue** %ra3, align 4, !tbaa !1
  %47 = load i32, i32* %n, align 4, !tbaa !7
  %call = call i32 @luaD_poscall(%struct.lua_State* %45, %struct.TValue* %46, i32 %47)
  store i32 %call, i32* %b.addr, align 4, !tbaa !7
  %48 = load i32, i32* %b.addr, align 4, !tbaa !7
  %tobool = icmp ne i32 %48, 0
  br i1 %tobool, label %if.then10, label %if.end13

if.then10:                                        ; preds = %cond.end
  %49 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %top11 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %49, i32 0, i32 1
  %50 = load %struct.TValue*, %struct.TValue** %top11, align 4, !tbaa !29
  %51 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top12 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %51, i32 0, i32 4
  store %struct.TValue* %50, %struct.TValue** %top12, align 4, !tbaa !28
  br label %if.end13

if.end13:                                         ; preds = %if.then10, %cond.end
  %52 = bitcast i32* %n to i8*
  call void @llvm.lifetime.end(i64 4, i8* %52) #2
  %53 = bitcast %struct.TValue** %ra3 to i8*
  call void @llvm.lifetime.end(i64 4, i8* %53) #2
  %54 = bitcast %struct.TValue** %rb2 to i8*
  call void @llvm.lifetime.end(i64 4, i8* %54) #2
  %55 = bitcast %struct.TValue** %ra2 to i8*
  call void @llvm.lifetime.end(i64 4, i8* %55) #2
  %56 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.end(i64 4, i8* %56) #2
  %57 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.end(i64 4, i8* %57) #2
  %58 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.end(i64 4, i8* %58) #2
  %59 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.end(i64 4, i8* %59) #2
  %60 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end(i64 4, i8* %60) #2
  %61 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end(i64 4, i8* %61) #2
  %62 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end(i64 4, i8* %62) #2
  ret void
}

; Function Attrs: nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #2

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i32(i8* nocapture, i8* nocapture readonly, i32, i32, i1) #2

declare void @luaF_close(%struct.lua_State*, %struct.TValue*) #1

declare i32 @luaD_poscall(%struct.lua_State*, %struct.TValue*, i32) #1

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
!17 = !{!15, !2, i64 0}
!18 = !{!19, !2, i64 12}
!19 = !{!"LClosure", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !2, i64 8, !2, i64 12, !3, i64 16}
!20 = !{!21, !2, i64 44}
!21 = !{!"Proto", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !3, i64 7, !3, i64 8, !8, i64 12, !8, i64 16, !8, i64 20, !8, i64 24, !8, i64 28, !8, i64 32, !8, i64 36, !8, i64 40, !2, i64 44, !2, i64 48, !2, i64 52, !2, i64 56, !2, i64 60, !2, i64 64, !2, i64 68, !2, i64 72, !2, i64 76, !22, i64 80}
!22 = !{!"RaviJITProto", !3, i64 0, !2, i64 4, !2, i64 8}
!23 = !{i64 0, i64 4, !1, i64 0, i64 4, !1, i64 0, i64 4, !7, i64 0, i64 4, !1, i64 0, i64 8, !24, i64 0, i64 8, !25, i64 8, i64 4, !7}
!24 = !{!12, !12, i64 0}
!25 = !{!26, !26, i64 0}
!26 = !{!"double", !3, i64 0}
!27 = !{!21, !8, i64 28}
!28 = !{!10, !2, i64 8}
!29 = !{!15, !2, i64 4}
