; ModuleID = 'lua_op_forprep.c'
source_filename = "lua_op_forprep.c"
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

@"\01??_C@_0M@KDKGEOFG@value?5?$DN?5?$CFd?6?$AA@" = linkonce_odr unnamed_addr constant [12 x i8] c"value = %d\0A\00", comdat, align 1
@"\01??_C@_0BN@GHPMJMPH@?8for?8?5limit?5must?5be?5a?5number?$AA@" = linkonce_odr unnamed_addr constant [29 x i8] c"'for' limit must be a number\00", comdat, align 1
@"\01??_C@_0BM@EGCFKNIJ@?8for?8?5step?5must?5be?5a?5number?$AA@" = linkonce_odr unnamed_addr constant [28 x i8] c"'for' step must be a number\00", comdat, align 1
@"\01??_C@_0CF@JJFDEONC@?8for?8?5initial?5value?5must?5be?5a?5nu@" = linkonce_odr unnamed_addr constant [37 x i8] c"'for' initial value must be a number\00", comdat, align 1

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
  %b = alloca i32, align 4
  %A = alloca i32, align 4
  %A1 = alloca i32, align 4
  %A2 = alloca i32, align 4
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
  %6 = bitcast i32* %b to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %6) #3
  %7 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %ci1 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %7, i32 0, i32 6
  %8 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci1, align 8, !tbaa !8
  store %struct.CallInfoLua* %8, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %9 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %9, i32 0, i32 4
  %base2 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l, i32 0, i32 0
  %10 = load %struct.TValue*, %struct.TValue** %base2, align 8, !tbaa !14
  store %struct.TValue* %10, %struct.TValue** %base, align 8, !tbaa !2
  %11 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %func = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %11, i32 0, i32 0
  %12 = load %struct.TValue*, %struct.TValue** %func, align 8, !tbaa !17
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %12, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %13 = load %struct.GCObject*, %struct.GCObject** %gc, align 8, !tbaa !18
  %14 = bitcast %struct.GCObject* %13 to %struct.LClosure*
  store %struct.LClosure* %14, %struct.LClosure** %cl, align 8, !tbaa !2
  %15 = load %struct.LClosure*, %struct.LClosure** %cl, align 8, !tbaa !2
  %p = getelementptr inbounds %struct.LClosure, %struct.LClosure* %15, i32 0, i32 5
  %16 = load %struct.Proto*, %struct.Proto** %p, align 8, !tbaa !19
  %k3 = getelementptr inbounds %struct.Proto, %struct.Proto* %16, i32 0, i32 14
  %17 = load %struct.TValue*, %struct.TValue** %k3, align 8, !tbaa !21
  store %struct.TValue* %17, %struct.TValue** %k, align 8, !tbaa !2
  %18 = bitcast i32* %A to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %18) #3
  %19 = bitcast i32* %A1 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %19) #3
  %20 = bitcast i32* %A2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %20) #3
  br label %21

; <label>:21:                                     ; preds = %0
  store i32 1, i32* %A, align 4, !tbaa !24
  %22 = load i32, i32* %A, align 4, !tbaa !24
  %add = add nsw i32 %22, 1
  store i32 %add, i32* %A1, align 4, !tbaa !24
  %23 = load i32, i32* %A, align 4, !tbaa !24
  %add4 = add nsw i32 %23, 2
  store i32 %add4, i32* %A2, align 4, !tbaa !24
  %24 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %25 = load i32, i32* %A, align 4, !tbaa !24
  %idx.ext = sext i32 %25 to i64
  %add.ptr = getelementptr inbounds %struct.TValue, %struct.TValue* %24, i64 %idx.ext
  store %struct.TValue* %add.ptr, %struct.TValue** %init, align 8, !tbaa !2
  %26 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %27 = load i32, i32* %A1, align 4, !tbaa !24
  %idx.ext5 = sext i32 %27 to i64
  %add.ptr6 = getelementptr inbounds %struct.TValue, %struct.TValue* %26, i64 %idx.ext5
  store %struct.TValue* %add.ptr6, %struct.TValue** %plimit, align 8, !tbaa !2
  %28 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %29 = load i32, i32* %A2, align 4, !tbaa !24
  %idx.ext7 = sext i32 %29 to i64
  %add.ptr8 = getelementptr inbounds %struct.TValue, %struct.TValue* %28, i64 %idx.ext7
  store %struct.TValue* %add.ptr8, %struct.TValue** %pstep, align 8, !tbaa !2
  %30 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %tt_ = getelementptr inbounds %struct.TValue, %struct.TValue* %30, i32 0, i32 1
  %31 = load i16, i16* %tt_, align 8, !tbaa !25
  %conv = zext i16 %31 to i32
  %cmp = icmp eq i32 %conv, 19
  %conv9 = zext i1 %cmp to i32
  store i32 %conv9, i32* %init_is_integer, align 4, !tbaa !24
  %32 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %tt_10 = getelementptr inbounds %struct.TValue, %struct.TValue* %32, i32 0, i32 1
  %33 = load i16, i16* %tt_10, align 8, !tbaa !25
  %conv11 = zext i16 %33 to i32
  %cmp12 = icmp eq i32 %conv11, 19
  %conv13 = zext i1 %cmp12 to i32
  store i32 %conv13, i32* %pstep_is_integer, align 4, !tbaa !24
  %34 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %value_14 = getelementptr inbounds %struct.TValue, %struct.TValue* %34, i32 0, i32 0
  %i = bitcast %union.Value* %value_14 to i64*
  %35 = load i64, i64* %i, align 8, !tbaa !18
  %36 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %call = call i32 @forlimit(%struct.TValue* %36, i64* %ilimit, i64 %35, i32* %stopnow)
  store i32 %call, i32* %fl, align 4, !tbaa !24
  %37 = load i32, i32* %init_is_integer, align 4, !tbaa !24
  %tobool = icmp ne i32 %37, 0
  br i1 %tobool, label %38, label %59

; <label>:38:                                     ; preds = %21
  %39 = load i32, i32* %pstep_is_integer, align 4, !tbaa !24
  %tobool15 = icmp ne i32 %39, 0
  br i1 %tobool15, label %40, label %59

; <label>:40:                                     ; preds = %38
  %41 = load i32, i32* %fl, align 4, !tbaa !24
  %tobool16 = icmp ne i32 %41, 0
  br i1 %tobool16, label %42, label %59

; <label>:42:                                     ; preds = %40
  %43 = bitcast i64* %initv to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %43) #3
  %44 = load i32, i32* %stopnow, align 4, !tbaa !24
  %tobool17 = icmp ne i32 %44, 0
  br i1 %tobool17, label %45, label %46

; <label>:45:                                     ; preds = %42
  br label %49

; <label>:46:                                     ; preds = %42
  %47 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %value_18 = getelementptr inbounds %struct.TValue, %struct.TValue* %47, i32 0, i32 0
  %i19 = bitcast %union.Value* %value_18 to i64*
  %48 = load i64, i64* %i19, align 8, !tbaa !18
  br label %49

; <label>:49:                                     ; preds = %46, %45
  %cond = phi i64 [ 0, %45 ], [ %48, %46 ]
  store i64 %cond, i64* %initv, align 8, !tbaa !27
  %50 = load i64, i64* %ilimit, align 8, !tbaa !27
  %51 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %value_20 = getelementptr inbounds %struct.TValue, %struct.TValue* %51, i32 0, i32 0
  %i21 = bitcast %union.Value* %value_20 to i64*
  store i64 %50, i64* %i21, align 8, !tbaa !18
  %52 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %tt_22 = getelementptr inbounds %struct.TValue, %struct.TValue* %52, i32 0, i32 1
  store i16 19, i16* %tt_22, align 8, !tbaa !25
  %53 = load i64, i64* %initv, align 8, !tbaa !27
  %54 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %value_23 = getelementptr inbounds %struct.TValue, %struct.TValue* %54, i32 0, i32 0
  %i24 = bitcast %union.Value* %value_23 to i64*
  %55 = load i64, i64* %i24, align 8, !tbaa !18
  %sub = sub nsw i64 %53, %55
  %56 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %value_25 = getelementptr inbounds %struct.TValue, %struct.TValue* %56, i32 0, i32 0
  %i26 = bitcast %union.Value* %value_25 to i64*
  store i64 %sub, i64* %i26, align 8, !tbaa !18
  %57 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %tt_27 = getelementptr inbounds %struct.TValue, %struct.TValue* %57, i32 0, i32 1
  store i16 19, i16* %tt_27, align 8, !tbaa !25
  %58 = bitcast i64* %initv to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %58) #3
  br label %118

; <label>:59:                                     ; preds = %40, %38, %21
  %60 = bitcast double* %ninit to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %60) #3
  %61 = bitcast double* %nlimit to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %61) #3
  %62 = bitcast double* %nstep to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %62) #3
  %63 = bitcast i32* %plimit_is_float to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %63) #3
  store i32 0, i32* %plimit_is_float, align 4, !tbaa !24
  %64 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %tt_28 = getelementptr inbounds %struct.TValue, %struct.TValue* %64, i32 0, i32 1
  %65 = load i16, i16* %tt_28, align 8, !tbaa !25
  %conv29 = zext i16 %65 to i32
  %cmp30 = icmp eq i32 %conv29, 3
  br i1 %cmp30, label %66, label %69

; <label>:66:                                     ; preds = %59
  store i32 1, i32* %plimit_is_float, align 4, !tbaa !24
  %67 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %value_32 = getelementptr inbounds %struct.TValue, %struct.TValue* %67, i32 0, i32 0
  %n = bitcast %union.Value* %value_32 to double*
  %68 = load double, double* %n, align 8, !tbaa !18
  store double %68, double* %nlimit, align 8, !tbaa !28
  br label %71

; <label>:69:                                     ; preds = %59
  %70 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %call33 = call i32 @luaV_tonumber_(%struct.TValue* %70, double* %nlimit)
  store i32 %call33, i32* %plimit_is_float, align 4, !tbaa !24
  br label %71

; <label>:71:                                     ; preds = %69, %66
  %72 = load i32, i32* %plimit_is_float, align 4, !tbaa !24
  %tobool34 = icmp ne i32 %72, 0
  br i1 %tobool34, label %75, label %73

; <label>:73:                                     ; preds = %71
  %74 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call35 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %74, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @"\01??_C@_0BN@GHPMJMPH@?8for?8?5limit?5must?5be?5a?5number?$AA@", i32 0, i32 0))
  br label %75

; <label>:75:                                     ; preds = %73, %71
  %76 = load double, double* %nlimit, align 8, !tbaa !28
  %77 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %value_36 = getelementptr inbounds %struct.TValue, %struct.TValue* %77, i32 0, i32 0
  %n37 = bitcast %union.Value* %value_36 to double*
  store double %76, double* %n37, align 8, !tbaa !18
  %78 = load %struct.TValue*, %struct.TValue** %plimit, align 8, !tbaa !2
  %tt_38 = getelementptr inbounds %struct.TValue, %struct.TValue* %78, i32 0, i32 1
  store i16 3, i16* %tt_38, align 8, !tbaa !25
  %79 = bitcast i32* %pstep_is_float to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %79) #3
  store i32 0, i32* %pstep_is_float, align 4, !tbaa !24
  %80 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %tt_39 = getelementptr inbounds %struct.TValue, %struct.TValue* %80, i32 0, i32 1
  %81 = load i16, i16* %tt_39, align 8, !tbaa !25
  %conv40 = zext i16 %81 to i32
  %cmp41 = icmp eq i32 %conv40, 3
  br i1 %cmp41, label %82, label %85

; <label>:82:                                     ; preds = %75
  store i32 1, i32* %pstep_is_float, align 4, !tbaa !24
  %83 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %value_43 = getelementptr inbounds %struct.TValue, %struct.TValue* %83, i32 0, i32 0
  %n44 = bitcast %union.Value* %value_43 to double*
  %84 = load double, double* %n44, align 8, !tbaa !18
  store double %84, double* %nstep, align 8, !tbaa !28
  br label %87

; <label>:85:                                     ; preds = %75
  %86 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %call45 = call i32 @luaV_tonumber_(%struct.TValue* %86, double* %nstep)
  store i32 %call45, i32* %pstep_is_float, align 4, !tbaa !24
  br label %87

; <label>:87:                                     ; preds = %85, %82
  %88 = load i32, i32* %pstep_is_float, align 4, !tbaa !24
  %tobool46 = icmp ne i32 %88, 0
  br i1 %tobool46, label %91, label %89

; <label>:89:                                     ; preds = %87
  %90 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call47 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %90, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @"\01??_C@_0BM@EGCFKNIJ@?8for?8?5step?5must?5be?5a?5number?$AA@", i32 0, i32 0))
  br label %91

; <label>:91:                                     ; preds = %89, %87
  %92 = load double, double* %nstep, align 8, !tbaa !28
  %93 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %value_48 = getelementptr inbounds %struct.TValue, %struct.TValue* %93, i32 0, i32 0
  %n49 = bitcast %union.Value* %value_48 to double*
  store double %92, double* %n49, align 8, !tbaa !18
  %94 = load %struct.TValue*, %struct.TValue** %pstep, align 8, !tbaa !2
  %tt_50 = getelementptr inbounds %struct.TValue, %struct.TValue* %94, i32 0, i32 1
  store i16 3, i16* %tt_50, align 8, !tbaa !25
  %95 = bitcast i32* %init_is_float to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %95) #3
  store i32 0, i32* %init_is_float, align 4, !tbaa !24
  %96 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %tt_51 = getelementptr inbounds %struct.TValue, %struct.TValue* %96, i32 0, i32 1
  %97 = load i16, i16* %tt_51, align 8, !tbaa !25
  %conv52 = zext i16 %97 to i32
  %cmp53 = icmp eq i32 %conv52, 3
  br i1 %cmp53, label %98, label %101

; <label>:98:                                     ; preds = %91
  store i32 1, i32* %init_is_float, align 4, !tbaa !24
  %99 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %value_55 = getelementptr inbounds %struct.TValue, %struct.TValue* %99, i32 0, i32 0
  %n56 = bitcast %union.Value* %value_55 to double*
  %100 = load double, double* %n56, align 8, !tbaa !18
  store double %100, double* %ninit, align 8, !tbaa !28
  br label %103

; <label>:101:                                    ; preds = %91
  %102 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %call57 = call i32 @luaV_tonumber_(%struct.TValue* %102, double* %ninit)
  store i32 %call57, i32* %init_is_float, align 4, !tbaa !24
  br label %103

; <label>:103:                                    ; preds = %101, %98
  %104 = load i32, i32* %init_is_float, align 4, !tbaa !24
  %tobool58 = icmp ne i32 %104, 0
  br i1 %tobool58, label %107, label %105

; <label>:105:                                    ; preds = %103
  %106 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call59 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %106, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @"\01??_C@_0CF@JJFDEONC@?8for?8?5initial?5value?5must?5be?5a?5nu@", i32 0, i32 0))
  br label %107

; <label>:107:                                    ; preds = %105, %103
  %108 = load double, double* %ninit, align 8, !tbaa !28
  %109 = load double, double* %nstep, align 8, !tbaa !28
  %sub60 = fsub double %108, %109
  %110 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %value_61 = getelementptr inbounds %struct.TValue, %struct.TValue* %110, i32 0, i32 0
  %n62 = bitcast %union.Value* %value_61 to double*
  store double %sub60, double* %n62, align 8, !tbaa !18
  %111 = load %struct.TValue*, %struct.TValue** %init, align 8, !tbaa !2
  %tt_63 = getelementptr inbounds %struct.TValue, %struct.TValue* %111, i32 0, i32 1
  store i16 3, i16* %tt_63, align 8, !tbaa !25
  %112 = bitcast i32* %init_is_float to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %112) #3
  %113 = bitcast i32* %pstep_is_float to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %113) #3
  %114 = bitcast i32* %plimit_is_float to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %114) #3
  %115 = bitcast double* %nstep to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %115) #3
  %116 = bitcast double* %nlimit to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %116) #3
  %117 = bitcast double* %ninit to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %117) #3
  br label %118

; <label>:118:                                    ; preds = %107, %49
  br label %119

; <label>:119:                                    ; preds = %118
  %120 = bitcast i32* %A2 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %120) #3
  %121 = bitcast i32* %A1 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %121) #3
  %122 = bitcast i32* %A to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %122) #3
  %123 = bitcast i32* %b to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %123) #3
  %124 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %124) #3
  %125 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %125) #3
  %126 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %126) #3
  %127 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %127) #3
  %128 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %128) #3
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #2

declare i32 @forlimit(%struct.TValue*, i64*, i64, i32*) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #2

declare i32 @luaV_tonumber_(%struct.TValue*, double*) #1

declare i32 @luaG_runerror(...) #1

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
!24 = !{!13, !13, i64 0}
!25 = !{!26, !12, i64 8}
!26 = !{!"TValue", !4, i64 0, !12, i64 8}
!27 = !{!11, !11, i64 0}
!28 = !{!29, !29, i64 0}
!29 = !{!"double", !4, i64 0}
