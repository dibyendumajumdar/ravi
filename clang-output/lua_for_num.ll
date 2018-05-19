; ModuleID = 'lua_for_num.c'
source_filename = "lua_for_num.c"
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

$"\01??_C@_0BN@GHPMJMPH@?8for?8?5limit?5must?5be?5a?5number?$AA@" = comdat any

$"\01??_C@_0BM@EGCFKNIJ@?8for?8?5step?5must?5be?5a?5number?$AA@" = comdat any

$"\01??_C@_0CF@JJFDEONC@?8for?8?5initial?5value?5must?5be?5a?5nu@" = comdat any

$"\01??_C@_0N@JPNMJCEO@ptr?5diff?5?$CFd?6?$AA@" = comdat any

@"\01??_C@_0M@KDKGEOFG@value?5?$DN?5?$CFd?6?$AA@" = linkonce_odr unnamed_addr constant [12 x i8] c"value = %d\0A\00", comdat, align 1
@"\01??_C@_0BN@GHPMJMPH@?8for?8?5limit?5must?5be?5a?5number?$AA@" = linkonce_odr unnamed_addr constant [29 x i8] c"'for' limit must be a number\00", comdat, align 1
@"\01??_C@_0BM@EGCFKNIJ@?8for?8?5step?5must?5be?5a?5number?$AA@" = linkonce_odr unnamed_addr constant [28 x i8] c"'for' step must be a number\00", comdat, align 1
@"\01??_C@_0CF@JJFDEONC@?8for?8?5initial?5value?5must?5be?5a?5nu@" = linkonce_odr unnamed_addr constant [37 x i8] c"'for' initial value must be a number\00", comdat, align 1
@"\01??_C@_0N@JPNMJCEO@ptr?5diff?5?$CFd?6?$AA@" = linkonce_odr unnamed_addr constant [13 x i8] c"ptr diff %d\0A\00", comdat, align 1

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
define void @test1(%struct.lua_State* %L) #0 {
  %L.addr = alloca %struct.lua_State*, align 8
  %ci = alloca %struct.CallInfoLua*, align 8
  %cl = alloca %struct.LClosure*, align 8
  %k = alloca %struct.TValue*, align 8
  %base = alloca %struct.TValue*, align 8
  %cil = alloca %struct.CallInfoL*, align 8
  %ra = alloca %struct.TValue*, align 8
  %rb = alloca %struct.TValue*, align 8
  %rc = alloca %struct.TValue*, align 8
  %b = alloca i32, align 4
  %init = alloca %struct.TValue*, align 8
  %plimit = alloca %struct.TValue*, align 8
  %pstep = alloca %struct.TValue*, align 8
  %ilimit = alloca i64, align 8
  %stopnow = alloca i32, align 4
  %init_is_integer = alloca i32, align 4
  %pstep_is_integer = alloca i32, align 4
  %fl = alloca i32, align 4
  %initv = alloca i64, align 8
  %ninit = alloca double, align 8
  %nlimit = alloca double, align 8
  %nstep = alloca double, align 8
  %plimit_is_float = alloca i32, align 4
  %pstep_is_float = alloca i32, align 4
  %init_is_float = alloca i32, align 4
  %step = alloca i64, align 8
  %idx = alloca i64, align 8
  %limit = alloca i64, align 8
  %cleanup.dest.slot = alloca i32
  %step98 = alloca double, align 8
  %idx101 = alloca double, align 8
  %limit106 = alloca double, align 8
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
  %6 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %6) #3
  %7 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %7) #3
  %8 = bitcast %struct.TValue** %rc to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %8) #3
  %9 = bitcast i32* %b to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %9) #3
  %10 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %ci1 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %10, i32 0, i32 6
  %11 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci1, align 8, !tbaa !8
  store %struct.CallInfoLua* %11, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %12 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %12, i32 0, i32 4
  %base2 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l, i32 0, i32 0
  %13 = load %struct.TValue*, %struct.TValue** %base2, align 8, !tbaa !14
  store %struct.TValue* %13, %struct.TValue** %base, align 8, !tbaa !2
  %14 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %func = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %14, i32 0, i32 0
  %15 = load %struct.TValue*, %struct.TValue** %func, align 8, !tbaa !17
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %15, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %16 = load %struct.GCObject*, %struct.GCObject** %gc, align 8, !tbaa !18
  %17 = bitcast %struct.GCObject* %16 to %struct.LClosure*
  store %struct.LClosure* %17, %struct.LClosure** %cl, align 8, !tbaa !2
  %18 = load %struct.LClosure*, %struct.LClosure** %cl, align 8, !tbaa !2
  %p = getelementptr inbounds %struct.LClosure, %struct.LClosure* %18, i32 0, i32 5
  %19 = load %struct.Proto*, %struct.Proto** %p, align 8, !tbaa !19
  %k3 = getelementptr inbounds %struct.Proto, %struct.Proto* %19, i32 0, i32 14
  %20 = load %struct.TValue*, %struct.TValue** %k3, align 8, !tbaa !21
  store %struct.TValue* %20, %struct.TValue** %k, align 8, !tbaa !2
  %21 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr = getelementptr inbounds %struct.TValue, %struct.TValue* %21, i64 0
  store %struct.TValue* %add.ptr, %struct.TValue** %ra, align 8, !tbaa !2
  %22 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr4 = getelementptr inbounds %struct.TValue, %struct.TValue* %22, i64 0
  store %struct.TValue* %add.ptr4, %struct.TValue** %rb, align 8, !tbaa !2
  %23 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %24 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %25 = bitcast %struct.TValue* %23 to i8*
  %26 = bitcast %struct.TValue* %24 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %25, i8* %26, i64 16, i32 8, i1 false), !tbaa.struct !24
  %27 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr5 = getelementptr inbounds %struct.TValue, %struct.TValue* %27, i64 1
  store %struct.TValue* %add.ptr5, %struct.TValue** %ra, align 8, !tbaa !2
  %28 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr6 = getelementptr inbounds %struct.TValue, %struct.TValue* %28, i64 1
  store %struct.TValue* %add.ptr6, %struct.TValue** %rb, align 8, !tbaa !2
  %29 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %30 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %31 = bitcast %struct.TValue* %29 to i8*
  %32 = bitcast %struct.TValue* %30 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %31, i8* %32, i64 16, i32 8, i1 false), !tbaa.struct !24
  %33 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr7 = getelementptr inbounds %struct.TValue, %struct.TValue* %33, i64 2
  store %struct.TValue* %add.ptr7, %struct.TValue** %ra, align 8, !tbaa !2
  %34 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr8 = getelementptr inbounds %struct.TValue, %struct.TValue* %34, i64 2
  store %struct.TValue* %add.ptr8, %struct.TValue** %rb, align 8, !tbaa !2
  %35 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %36 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %37 = bitcast %struct.TValue* %35 to i8*
  %38 = bitcast %struct.TValue* %36 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %37, i8* %38, i64 16, i32 8, i1 false), !tbaa.struct !24
  %39 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr9 = getelementptr inbounds %struct.TValue, %struct.TValue* %39, i64 3
  store %struct.TValue* %add.ptr9, %struct.TValue** %ra, align 8, !tbaa !2
  %40 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr10 = getelementptr inbounds %struct.TValue, %struct.TValue* %40, i64 1
  store %struct.TValue* %add.ptr10, %struct.TValue** %rb, align 8, !tbaa !2
  %41 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %42 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %43 = bitcast %struct.TValue* %41 to i8*
  %44 = bitcast %struct.TValue* %42 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %43, i8* %44, i64 16, i32 8, i1 false), !tbaa.struct !24
  br label %45

; <label>:45:                                     ; preds = %0
  %46 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr11 = getelementptr inbounds %struct.TValue, %struct.TValue* %46, i64 1
  store %struct.TValue* %add.ptr11, %struct.TValue** %ra, align 8, !tbaa !2
  %47 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  store %struct.TValue* %47, %struct.TValue** %init, align 8, !tbaa !2
  %48 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %add.ptr12 = getelementptr inbounds %struct.TValue, %struct.TValue* %48, i64 1
  store %struct.TValue* %add.ptr12, %struct.TValue** %plimit, align 8, !tbaa !2
  %49 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %add.ptr13 = getelementptr inbounds %struct.TValue, %struct.TValue* %49, i64 2
  store %struct.TValue* %add.ptr13, %struct.TValue** %pstep, align 8, !tbaa !2
  %50 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %tt_ = getelementptr inbounds %struct.TValue, %struct.TValue* %50, i32 0, i32 1
  %51 = load i16, i16* %tt_, align 8, !tbaa !30
  %conv = zext i16 %51 to i32
  %cmp = icmp eq i32 %conv, 19
  %conv14 = zext i1 %cmp to i32
  store i32 %conv14, i32* %init_is_integer, align 4, !tbaa !25
  %52 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %tt_15 = getelementptr inbounds %struct.TValue, %struct.TValue* %52, i32 0, i32 1
  %53 = load i16, i16* %tt_15, align 8, !tbaa !30
  %conv16 = zext i16 %53 to i32
  %cmp17 = icmp eq i32 %conv16, 19
  %conv18 = zext i1 %cmp17 to i32
  store i32 %conv18, i32* %pstep_is_integer, align 4, !tbaa !25
  %54 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %value_19 = getelementptr inbounds %struct.TValue, %struct.TValue* %54, i32 0, i32 0
  %i = bitcast %union.Value* %value_19 to i64*
  %55 = load i64, i64* %i, align 8, !tbaa !18
  %56 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %call = call i32 @forlimit(%struct.TValue* %56, i64* %ilimit, i64 %55, i32* %stopnow)
  store i32 %call, i32* %fl, align 4, !tbaa !25
  %57 = load i32, i32* %init_is_integer, align 4, !tbaa !25
  %tobool = icmp ne i32 %57, 0
  br i1 %tobool, label %58, label %79

; <label>:58:                                     ; preds = %45
  %59 = load i32, i32* %pstep_is_integer, align 4, !tbaa !25
  %tobool20 = icmp ne i32 %59, 0
  br i1 %tobool20, label %60, label %79

; <label>:60:                                     ; preds = %58
  %61 = load i32, i32* %fl, align 4, !tbaa !25
  %tobool21 = icmp ne i32 %61, 0
  br i1 %tobool21, label %62, label %79

; <label>:62:                                     ; preds = %60
  %63 = bitcast i64* %initv to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %63) #3
  %64 = load i32, i32* %stopnow, align 4, !tbaa !25
  %tobool22 = icmp ne i32 %64, 0
  br i1 %tobool22, label %65, label %66

; <label>:65:                                     ; preds = %62
  br label %69

; <label>:66:                                     ; preds = %62
  %67 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %value_23 = getelementptr inbounds %struct.TValue, %struct.TValue* %67, i32 0, i32 0
  %i24 = bitcast %union.Value* %value_23 to i64*
  %68 = load i64, i64* %i24, align 8, !tbaa !18
  br label %69

; <label>:69:                                     ; preds = %66, %65
  %cond = phi i64 [ 0, %65 ], [ %68, %66 ]
  store i64 %cond, i64* %initv, align 8, !tbaa !26
  %70 = load i64, i64* %ilimit, align 8, !tbaa !26
  %71 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %value_25 = getelementptr inbounds %struct.TValue, %struct.TValue* %71, i32 0, i32 0
  %i26 = bitcast %union.Value* %value_25 to i64*
  store i64 %70, i64* %i26, align 8, !tbaa !18
  %72 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %tt_27 = getelementptr inbounds %struct.TValue, %struct.TValue* %72, i32 0, i32 1
  store i16 19, i16* %tt_27, align 8, !tbaa !30
  %73 = load i64, i64* %initv, align 8, !tbaa !26
  %74 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %value_28 = getelementptr inbounds %struct.TValue, %struct.TValue* %74, i32 0, i32 0
  %i29 = bitcast %union.Value* %value_28 to i64*
  %75 = load i64, i64* %i29, align 8, !tbaa !18
  %sub = sub nsw i64 %73, %75
  %76 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %value_30 = getelementptr inbounds %struct.TValue, %struct.TValue* %76, i32 0, i32 0
  %i31 = bitcast %union.Value* %value_30 to i64*
  store i64 %sub, i64* %i31, align 8, !tbaa !18
  %77 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %tt_32 = getelementptr inbounds %struct.TValue, %struct.TValue* %77, i32 0, i32 1
  store i16 19, i16* %tt_32, align 8, !tbaa !30
  %78 = bitcast i64* %initv to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %78) #3
  br label %138

; <label>:79:                                     ; preds = %60, %58, %45
  %80 = bitcast double* %ninit to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %80) #3
  %81 = bitcast double* %nlimit to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %81) #3
  %82 = bitcast double* %nstep to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %82) #3
  %83 = bitcast i32* %plimit_is_float to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %83) #3
  store i32 0, i32* %plimit_is_float, align 4, !tbaa !25
  %84 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %tt_33 = getelementptr inbounds %struct.TValue, %struct.TValue* %84, i32 0, i32 1
  %85 = load i16, i16* %tt_33, align 8, !tbaa !30
  %conv34 = zext i16 %85 to i32
  %cmp35 = icmp eq i32 %conv34, 3
  br i1 %cmp35, label %86, label %89

; <label>:86:                                     ; preds = %79
  store i32 1, i32* %plimit_is_float, align 4, !tbaa !25
  %87 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %value_37 = getelementptr inbounds %struct.TValue, %struct.TValue* %87, i32 0, i32 0
  %n = bitcast %union.Value* %value_37 to double*
  %88 = load double, double* %n, align 8, !tbaa !18
  store double %88, double* %nlimit, align 8, !tbaa !27
  br label %91

; <label>:89:                                     ; preds = %79
  %90 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %call38 = call i32 @luaV_tonumber_(%struct.TValue* %90, double* %nlimit)
  store i32 %call38, i32* %plimit_is_float, align 4, !tbaa !25
  br label %91

; <label>:91:                                     ; preds = %89, %86
  %92 = load i32, i32* %plimit_is_float, align 4, !tbaa !25
  %tobool39 = icmp ne i32 %92, 0
  br i1 %tobool39, label %95, label %93

; <label>:93:                                     ; preds = %91
  %94 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call40 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %94, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @"\01??_C@_0BN@GHPMJMPH@?8for?8?5limit?5must?5be?5a?5number?$AA@", i32 0, i32 0))
  br label %95

; <label>:95:                                     ; preds = %93, %91
  %96 = load double, double* %nlimit, align 8, !tbaa !27
  %97 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %value_41 = getelementptr inbounds %struct.TValue, %struct.TValue* %97, i32 0, i32 0
  %n42 = bitcast %union.Value* %value_41 to double*
  store double %96, double* %n42, align 8, !tbaa !18
  %98 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %tt_43 = getelementptr inbounds %struct.TValue, %struct.TValue* %98, i32 0, i32 1
  store i16 3, i16* %tt_43, align 8, !tbaa !30
  %99 = bitcast i32* %pstep_is_float to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %99) #3
  store i32 0, i32* %pstep_is_float, align 4, !tbaa !25
  %100 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %tt_44 = getelementptr inbounds %struct.TValue, %struct.TValue* %100, i32 0, i32 1
  %101 = load i16, i16* %tt_44, align 8, !tbaa !30
  %conv45 = zext i16 %101 to i32
  %cmp46 = icmp eq i32 %conv45, 3
  br i1 %cmp46, label %102, label %105

; <label>:102:                                    ; preds = %95
  store i32 1, i32* %pstep_is_float, align 4, !tbaa !25
  %103 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %value_48 = getelementptr inbounds %struct.TValue, %struct.TValue* %103, i32 0, i32 0
  %n49 = bitcast %union.Value* %value_48 to double*
  %104 = load double, double* %n49, align 8, !tbaa !18
  store double %104, double* %nstep, align 8, !tbaa !27
  br label %107

; <label>:105:                                    ; preds = %95
  %106 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %call50 = call i32 @luaV_tonumber_(%struct.TValue* %106, double* %nstep)
  store i32 %call50, i32* %pstep_is_float, align 4, !tbaa !25
  br label %107

; <label>:107:                                    ; preds = %105, %102
  %108 = load i32, i32* %pstep_is_float, align 4, !tbaa !25
  %tobool51 = icmp ne i32 %108, 0
  br i1 %tobool51, label %111, label %109

; <label>:109:                                    ; preds = %107
  %110 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call52 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %110, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @"\01??_C@_0BM@EGCFKNIJ@?8for?8?5step?5must?5be?5a?5number?$AA@", i32 0, i32 0))
  br label %111

; <label>:111:                                    ; preds = %109, %107
  %112 = load double, double* %nstep, align 8, !tbaa !27
  %113 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %value_53 = getelementptr inbounds %struct.TValue, %struct.TValue* %113, i32 0, i32 0
  %n54 = bitcast %union.Value* %value_53 to double*
  store double %112, double* %n54, align 8, !tbaa !18
  %114 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %tt_55 = getelementptr inbounds %struct.TValue, %struct.TValue* %114, i32 0, i32 1
  store i16 3, i16* %tt_55, align 8, !tbaa !30
  %115 = bitcast i32* %init_is_float to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %115) #3
  store i32 0, i32* %init_is_float, align 4, !tbaa !25
  %116 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %tt_56 = getelementptr inbounds %struct.TValue, %struct.TValue* %116, i32 0, i32 1
  %117 = load i16, i16* %tt_56, align 8, !tbaa !30
  %conv57 = zext i16 %117 to i32
  %cmp58 = icmp eq i32 %conv57, 3
  br i1 %cmp58, label %118, label %121

; <label>:118:                                    ; preds = %111
  store i32 1, i32* %init_is_float, align 4, !tbaa !25
  %119 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %value_60 = getelementptr inbounds %struct.TValue, %struct.TValue* %119, i32 0, i32 0
  %n61 = bitcast %union.Value* %value_60 to double*
  %120 = load double, double* %n61, align 8, !tbaa !18
  store double %120, double* %ninit, align 8, !tbaa !27
  br label %123

; <label>:121:                                    ; preds = %111
  %122 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %call62 = call i32 @luaV_tonumber_(%struct.TValue* %122, double* %ninit)
  store i32 %call62, i32* %init_is_float, align 4, !tbaa !25
  br label %123

; <label>:123:                                    ; preds = %121, %118
  %124 = load i32, i32* %init_is_float, align 4, !tbaa !25
  %tobool63 = icmp ne i32 %124, 0
  br i1 %tobool63, label %127, label %125

; <label>:125:                                    ; preds = %123
  %126 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call64 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %126, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @"\01??_C@_0CF@JJFDEONC@?8for?8?5initial?5value?5must?5be?5a?5nu@", i32 0, i32 0))
  br label %127

; <label>:127:                                    ; preds = %125, %123
  %128 = load double, double* %ninit, align 8, !tbaa !27
  %129 = load double, double* %nstep, align 8, !tbaa !27
  %sub65 = fsub double %128, %129
  %130 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %value_66 = getelementptr inbounds %struct.TValue, %struct.TValue* %130, i32 0, i32 0
  %n67 = bitcast %union.Value* %value_66 to double*
  store double %sub65, double* %n67, align 8, !tbaa !18
  %131 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %tt_68 = getelementptr inbounds %struct.TValue, %struct.TValue* %131, i32 0, i32 1
  store i16 3, i16* %tt_68, align 8, !tbaa !30
  %132 = bitcast i32* %init_is_float to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %132) #3
  %133 = bitcast i32* %pstep_is_float to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %133) #3
  %134 = bitcast i32* %plimit_is_float to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %134) #3
  %135 = bitcast double* %nstep to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %135) #3
  %136 = bitcast double* %nlimit to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %136) #3
  %137 = bitcast double* %ninit to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %137) #3
  br label %138

; <label>:138:                                    ; preds = %127, %69
  br label %146

; <label>:139:                                    ; preds = %213, %179
  %140 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr69 = getelementptr inbounds %struct.TValue, %struct.TValue* %140, i64 0
  store %struct.TValue* %add.ptr69, %struct.TValue** %ra, align 8, !tbaa !2
  %141 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr70 = getelementptr inbounds %struct.TValue, %struct.TValue* %141, i64 4
  store %struct.TValue* %add.ptr70, %struct.TValue** %rb, align 8, !tbaa !2
  %142 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %143 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %144 = bitcast %struct.TValue* %142 to i8*
  %145 = bitcast %struct.TValue* %143 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %144, i8* %145, i64 16, i32 8, i1 false), !tbaa.struct !24
  br label %146

; <label>:146:                                    ; preds = %139, %138
  %147 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr71 = getelementptr inbounds %struct.TValue, %struct.TValue* %147, i64 1
  store %struct.TValue* %add.ptr71, %struct.TValue** %ra, align 8, !tbaa !2
  %148 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %tt_72 = getelementptr inbounds %struct.TValue, %struct.TValue* %148, i32 0, i32 1
  %149 = load i16, i16* %tt_72, align 8, !tbaa !30
  %conv73 = zext i16 %149 to i32
  %cmp74 = icmp eq i32 %conv73, 19
  br i1 %cmp74, label %150, label %184

; <label>:150:                                    ; preds = %146
  %151 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %add.ptr76 = getelementptr inbounds %struct.TValue, %struct.TValue* %151, i64 2
  store %struct.TValue* %add.ptr76, %struct.TValue** %rb, align 8, !tbaa !2
  %152 = bitcast i64* %step to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %152) #3
  %153 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %value_77 = getelementptr inbounds %struct.TValue, %struct.TValue* %153, i32 0, i32 0
  %i78 = bitcast %union.Value* %value_77 to i64*
  %154 = load i64, i64* %i78, align 8, !tbaa !18
  store i64 %154, i64* %step, align 8, !tbaa !26
  %155 = bitcast i64* %idx to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %155) #3
  %156 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %value_79 = getelementptr inbounds %struct.TValue, %struct.TValue* %156, i32 0, i32 0
  %i80 = bitcast %union.Value* %value_79 to i64*
  %157 = load i64, i64* %i80, align 8, !tbaa !18
  %158 = load i64, i64* %step, align 8, !tbaa !26
  %add = add nsw i64 %157, %158
  store i64 %add, i64* %idx, align 8, !tbaa !26
  %159 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %add.ptr81 = getelementptr inbounds %struct.TValue, %struct.TValue* %159, i64 1
  store %struct.TValue* %add.ptr81, %struct.TValue** %rc, align 8, !tbaa !2
  %160 = bitcast i64* %limit to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %160) #3
  %161 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %value_82 = getelementptr inbounds %struct.TValue, %struct.TValue* %161, i32 0, i32 0
  %i83 = bitcast %union.Value* %value_82 to i64*
  %162 = load i64, i64* %i83, align 8, !tbaa !18
  store i64 %162, i64* %limit, align 8, !tbaa !26
  %163 = load i64, i64* %step, align 8, !tbaa !26
  %cmp84 = icmp slt i64 0, %163
  br i1 %cmp84, label %164, label %167

; <label>:164:                                    ; preds = %150
  %165 = load i64, i64* %idx, align 8, !tbaa !26
  %166 = load i64, i64* %limit, align 8, !tbaa !26
  %cmp86 = icmp sle i64 %165, %166
  br i1 %cmp86, label %170, label %178

; <label>:167:                                    ; preds = %150
  %168 = load i64, i64* %limit, align 8, !tbaa !26
  %169 = load i64, i64* %idx, align 8, !tbaa !26
  %cmp88 = icmp sle i64 %168, %169
  br i1 %cmp88, label %170, label %178

; <label>:170:                                    ; preds = %167, %164
  %171 = load i64, i64* %idx, align 8, !tbaa !26
  %172 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %value_90 = getelementptr inbounds %struct.TValue, %struct.TValue* %172, i32 0, i32 0
  %i91 = bitcast %union.Value* %value_90 to i64*
  store i64 %171, i64* %i91, align 8, !tbaa !18
  %173 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %tt_92 = getelementptr inbounds %struct.TValue, %struct.TValue* %173, i32 0, i32 1
  store i16 19, i16* %tt_92, align 8, !tbaa !30
  %174 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %add.ptr93 = getelementptr inbounds %struct.TValue, %struct.TValue* %174, i64 3
  store %struct.TValue* %add.ptr93, %struct.TValue** %rc, align 8, !tbaa !2
  %175 = load i64, i64* %idx, align 8, !tbaa !26
  %176 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %value_94 = getelementptr inbounds %struct.TValue, %struct.TValue* %176, i32 0, i32 0
  %i95 = bitcast %union.Value* %value_94 to i64*
  store i64 %175, i64* %i95, align 8, !tbaa !18
  %177 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %tt_96 = getelementptr inbounds %struct.TValue, %struct.TValue* %177, i32 0, i32 1
  store i16 19, i16* %tt_96, align 8, !tbaa !30
  store i32 4, i32* %cleanup.dest.slot, align 4
  br label %179

; <label>:178:                                    ; preds = %167, %164
  store i32 0, i32* %cleanup.dest.slot, align 4
  br label %179

; <label>:179:                                    ; preds = %178, %170
  %180 = bitcast i64* %limit to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %180) #3
  %181 = bitcast i64* %idx to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %181) #3
  %182 = bitcast i64* %step to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %182) #3
  %cleanup.dest = load i32, i32* %cleanup.dest.slot, align 4
  switch i32 %cleanup.dest, label %250 [
    i32 0, label %183
    i32 4, label %139
  ]

; <label>:183:                                    ; preds = %179
  br label %218

; <label>:184:                                    ; preds = %146
  %185 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %add.ptr97 = getelementptr inbounds %struct.TValue, %struct.TValue* %185, i64 2
  store %struct.TValue* %add.ptr97, %struct.TValue** %rb, align 8, !tbaa !2
  %186 = bitcast double* %step98 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %186) #3
  %187 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %value_99 = getelementptr inbounds %struct.TValue, %struct.TValue* %187, i32 0, i32 0
  %n100 = bitcast %union.Value* %value_99 to double*
  %188 = load double, double* %n100, align 8, !tbaa !18
  store double %188, double* %step98, align 8, !tbaa !27
  %189 = bitcast double* %idx101 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %189) #3
  %190 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %value_102 = getelementptr inbounds %struct.TValue, %struct.TValue* %190, i32 0, i32 0
  %n103 = bitcast %union.Value* %value_102 to double*
  %191 = load double, double* %n103, align 8, !tbaa !18
  %192 = load double, double* %step98, align 8, !tbaa !27
  %add104 = fadd double %191, %192
  store double %add104, double* %idx101, align 8, !tbaa !27
  %193 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %add.ptr105 = getelementptr inbounds %struct.TValue, %struct.TValue* %193, i64 1
  store %struct.TValue* %add.ptr105, %struct.TValue** %rc, align 8, !tbaa !2
  %194 = bitcast double* %limit106 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %194) #3
  %195 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %value_107 = getelementptr inbounds %struct.TValue, %struct.TValue* %195, i32 0, i32 0
  %n108 = bitcast %union.Value* %value_107 to double*
  %196 = load double, double* %n108, align 8, !tbaa !18
  store double %196, double* %limit106, align 8, !tbaa !27
  %197 = load double, double* %step98, align 8, !tbaa !27
  %cmp109 = fcmp olt double 0.000000e+00, %197
  br i1 %cmp109, label %198, label %201

; <label>:198:                                    ; preds = %184
  %199 = load double, double* %idx101, align 8, !tbaa !27
  %200 = load double, double* %limit106, align 8, !tbaa !27
  %cmp111 = fcmp ole double %199, %200
  br i1 %cmp111, label %204, label %212

; <label>:201:                                    ; preds = %184
  %202 = load double, double* %limit106, align 8, !tbaa !27
  %203 = load double, double* %idx101, align 8, !tbaa !27
  %cmp113 = fcmp ole double %202, %203
  br i1 %cmp113, label %204, label %212

; <label>:204:                                    ; preds = %201, %198
  %205 = load double, double* %idx101, align 8, !tbaa !27
  %206 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %value_115 = getelementptr inbounds %struct.TValue, %struct.TValue* %206, i32 0, i32 0
  %n116 = bitcast %union.Value* %value_115 to double*
  store double %205, double* %n116, align 8, !tbaa !18
  %207 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %tt_117 = getelementptr inbounds %struct.TValue, %struct.TValue* %207, i32 0, i32 1
  store i16 3, i16* %tt_117, align 8, !tbaa !30
  %208 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %add.ptr118 = getelementptr inbounds %struct.TValue, %struct.TValue* %208, i64 3
  store %struct.TValue* %add.ptr118, %struct.TValue** %rc, align 8, !tbaa !2
  %209 = load double, double* %idx101, align 8, !tbaa !27
  %210 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %value_119 = getelementptr inbounds %struct.TValue, %struct.TValue* %210, i32 0, i32 0
  %n120 = bitcast %union.Value* %value_119 to double*
  store double %209, double* %n120, align 8, !tbaa !18
  %211 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %tt_121 = getelementptr inbounds %struct.TValue, %struct.TValue* %211, i32 0, i32 1
  store i16 3, i16* %tt_121, align 8, !tbaa !30
  store i32 4, i32* %cleanup.dest.slot, align 4
  br label %213

; <label>:212:                                    ; preds = %201, %198
  store i32 0, i32* %cleanup.dest.slot, align 4
  br label %213

; <label>:213:                                    ; preds = %212, %204
  %214 = bitcast double* %limit106 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %214) #3
  %215 = bitcast double* %idx101 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %215) #3
  %216 = bitcast double* %step98 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %216) #3
  %cleanup.dest122 = load i32, i32* %cleanup.dest.slot, align 4
  switch i32 %cleanup.dest122, label %250 [
    i32 0, label %217
    i32 4, label %139
  ]

; <label>:217:                                    ; preds = %213
  br label %218

; <label>:218:                                    ; preds = %217, %183
  store i32 2, i32* %b, align 4, !tbaa !25
  %219 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr123 = getelementptr inbounds %struct.TValue, %struct.TValue* %219, i64 0
  store %struct.TValue* %add.ptr123, %struct.TValue** %ra, align 8, !tbaa !2
  %220 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %221 = load i32, i32* %b, align 4, !tbaa !25
  %idx.ext = sext i32 %221 to i64
  %add.ptr124 = getelementptr inbounds %struct.TValue, %struct.TValue* %220, i64 %idx.ext
  %add.ptr125 = getelementptr inbounds %struct.TValue, %struct.TValue* %add.ptr124, i64 -1
  %222 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top = getelementptr inbounds %struct.lua_State, %struct.lua_State* %222, i32 0, i32 4
  store %struct.TValue* %add.ptr125, %struct.TValue** %top, align 8, !tbaa !32
  %223 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %224 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top126 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %224, i32 0, i32 4
  %225 = load %struct.TValue*, %struct.TValue** %top126, align 8, !tbaa !32
  %sub.ptr.lhs.cast = ptrtoint %struct.TValue* %223 to i64
  %sub.ptr.rhs.cast = ptrtoint %struct.TValue* %225 to i64
  %sub.ptr.sub = sub i64 %sub.ptr.lhs.cast, %sub.ptr.rhs.cast
  %sub.ptr.div = sdiv exact i64 %sub.ptr.sub, 16
  %conv127 = trunc i64 %sub.ptr.div to i32
  %call128 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @"\01??_C@_0N@JPNMJCEO@ptr?5diff?5?$CFd?6?$AA@", i32 0, i32 0), i32 %conv127)
  %226 = load %struct.LClosure*, %struct.LClosure** %cl, align 8, !tbaa !2
  %p129 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %226, i32 0, i32 5
  %227 = load %struct.Proto*, %struct.Proto** %p129, align 8, !tbaa !19
  %sizep = getelementptr inbounds %struct.Proto, %struct.Proto* %227, i32 0, i32 10
  %228 = load i32, i32* %sizep, align 8, !tbaa !33
  %cmp130 = icmp sgt i32 %228, 0
  br i1 %cmp130, label %229, label %232

; <label>:229:                                    ; preds = %218
  %230 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %231 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call132 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %231, %struct.TValue* %230)
  br label %232

; <label>:232:                                    ; preds = %229, %218
  %233 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %234 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call133 = call i32 @luaD_poscall(%struct.lua_State* %234, %struct.TValue* %233)
  store i32 %call133, i32* %b, align 4, !tbaa !25
  %235 = load i32, i32* %b, align 4, !tbaa !25
  %tobool134 = icmp ne i32 %235, 0
  br i1 %tobool134, label %236, label %240

; <label>:236:                                    ; preds = %232
  %237 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %top135 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %237, i32 0, i32 1
  %238 = load %struct.TValue*, %struct.TValue** %top135, align 8, !tbaa !34
  %239 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top136 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %239, i32 0, i32 4
  store %struct.TValue* %238, %struct.TValue** %top136, align 8, !tbaa !32
  br label %240

; <label>:240:                                    ; preds = %236, %232
  store i32 1, i32* %cleanup.dest.slot, align 4
  %241 = bitcast i32* %b to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %241) #3
  %242 = bitcast %struct.TValue** %rc to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %242) #3
  %243 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %243) #3
  %244 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %244) #3
  %245 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %245) #3
  %246 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %246) #3
  %247 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %247) #3
  %248 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %248) #3
  %249 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %249) #3
  ret void

; <label>:250:                                    ; preds = %213, %179
  unreachable
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

declare i32 @forlimit(%struct.TValue*, i64*, i64, i32*) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #2

declare i32 @luaV_tonumber_(%struct.TValue*, double*) #1

declare i32 @luaG_runerror(...) #1

declare i32 @luaF_close(...) #1

declare i32 @luaD_poscall(%struct.lua_State*, %struct.TValue*) #1

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
!8 = !{!9, !3, i64 32}
!9 = !{!"lua_State", !3, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !3, i64 16, !3, i64 24, !3, i64 32, !3, i64 40, !3, i64 48, !3, i64 56, !3, i64 64, !3, i64 72, !3, i64 80, !3, i64 88, !10, i64 96, !3, i64 168, !11, i64 176, !13, i64 184, !13, i64 188, !13, i64 192, !12, i64 196, !12, i64 198, !4, i64 200, !4, i64 201, !12, i64 202, !4, i64 204}
!10 = !{!"CallInfo", !3, i64 0, !3, i64 8, !3, i64 16, !3, i64 24, !4, i64 32, !11, i64 56, !12, i64 64, !12, i64 66, !12, i64 68, !4, i64 70, !4, i64 71}
!11 = !{!"long long", !4, i64 0}
!12 = !{!"short", !4, i64 0}
!13 = !{!"int", !4, i64 0}
!14 = !{!15, !3, i64 32}
!15 = !{!"CallInfoLua", !3, i64 0, !3, i64 8, !3, i64 16, !3, i64 24, !16, i64 32, !11, i64 56, !12, i64 64, !12, i64 66, !12, i64 68, !4, i64 70, !4, i64 71}
!16 = !{!"CallInfoL", !3, i64 0, !3, i64 8, !11, i64 16}
!17 = !{!15, !3, i64 0}
!18 = !{!4, !4, i64 0}
!19 = !{!20, !3, i64 24}
!20 = !{!"LClosure", !3, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !3, i64 16, !3, i64 24, !4, i64 32}
!21 = !{!22, !3, i64 48}
!22 = !{!"Proto", !3, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !4, i64 11, !4, i64 12, !13, i64 16, !13, i64 20, !13, i64 24, !13, i64 28, !13, i64 32, !13, i64 36, !13, i64 40, !13, i64 44, !3, i64 48, !3, i64 56, !3, i64 64, !3, i64 72, !3, i64 80, !3, i64 88, !3, i64 96, !3, i64 104, !3, i64 112, !23, i64 120}
!23 = !{!"RaviJITProto", !4, i64 0, !4, i64 1, !12, i64 2, !3, i64 8, !3, i64 16}
!24 = !{i64 0, i64 8, !2, i64 0, i64 8, !2, i64 0, i64 4, !25, i64 0, i64 8, !2, i64 0, i64 8, !26, i64 0, i64 8, !27, i64 8, i64 2, !29}
!25 = !{!13, !13, i64 0}
!26 = !{!11, !11, i64 0}
!27 = !{!28, !28, i64 0}
!28 = !{!"double", !4, i64 0}
!29 = !{!12, !12, i64 0}
!30 = !{!31, !12, i64 8}
!31 = !{!"TValue", !4, i64 0, !12, i64 8}
!32 = !{!9, !3, i64 16}
!33 = !{!22, !13, i64 32}
!34 = !{!15, !3, i64 8}
