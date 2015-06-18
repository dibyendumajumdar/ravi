; ModuleID = 'lua_for_num.c'
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
@.str1 = private unnamed_addr constant [29 x i8] c"'for' limit must be a number\00", align 1
@.str2 = private unnamed_addr constant [28 x i8] c"'for' step must be a number\00", align 1
@.str3 = private unnamed_addr constant [37 x i8] c"'for' initial value must be a number\00", align 1
@.str4 = private unnamed_addr constant [13 x i8] c"ptr diff %d\0A\00", align 1

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
define void @test1(%struct.lua_State* %L) #0 {
entry:
  %L.addr = alloca %struct.lua_State*, align 4
  %ci = alloca %struct.CallInfoLua*, align 4
  %cl = alloca %struct.LClosure*, align 4
  %k = alloca %struct.TValue*, align 4
  %base = alloca %struct.TValue*, align 4
  %cil = alloca %struct.CallInfoL*, align 4
  %ra = alloca %struct.TValue*, align 4
  %rb = alloca %struct.TValue*, align 4
  %rc = alloca %struct.TValue*, align 4
  %b = alloca i32, align 4
  %init = alloca %struct.TValue*, align 4
  %plimit = alloca %struct.TValue*, align 4
  %pstep = alloca %struct.TValue*, align 4
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
  %step116 = alloca double, align 8
  %idx119 = alloca double, align 8
  %limit124 = alloca double, align 8
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 4, !tbaa !1
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
  %5 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.start(i64 4, i8* %5) #2
  %6 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.start(i64 4, i8* %6) #2
  %7 = bitcast %struct.TValue** %rc to i8*
  call void @llvm.lifetime.start(i64 4, i8* %7) #2
  %8 = bitcast i32* %b to i8*
  call void @llvm.lifetime.start(i64 4, i8* %8) #2
  %9 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %ci1 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %9, i32 0, i32 6
  %10 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci1, align 4, !tbaa !7
  store %struct.CallInfoLua* %10, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %11 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %11, i32 0, i32 4
  %base2 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l, i32 0, i32 0
  %12 = load %struct.TValue*, %struct.TValue** %base2, align 4, !tbaa !13
  store %struct.TValue* %12, %struct.TValue** %base, align 4, !tbaa !1
  %13 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %func = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %13, i32 0, i32 0
  %14 = load %struct.TValue*, %struct.TValue** %func, align 4, !tbaa !16
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %14, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %15 = load %struct.GCObject*, %struct.GCObject** %gc, align 4, !tbaa !1
  %16 = bitcast %struct.GCObject* %15 to %struct.LClosure*
  store %struct.LClosure* %16, %struct.LClosure** %cl, align 4, !tbaa !1
  %17 = load %struct.LClosure*, %struct.LClosure** %cl, align 4, !tbaa !1
  %p = getelementptr inbounds %struct.LClosure, %struct.LClosure* %17, i32 0, i32 5
  %18 = load %struct.Proto*, %struct.Proto** %p, align 4, !tbaa !17
  %k3 = getelementptr inbounds %struct.Proto, %struct.Proto* %18, i32 0, i32 14
  %19 = load %struct.TValue*, %struct.TValue** %k3, align 4, !tbaa !19
  store %struct.TValue* %19, %struct.TValue** %k, align 4, !tbaa !1
  %20 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr = getelementptr inbounds %struct.TValue, %struct.TValue* %20, i32 0
  store %struct.TValue* %add.ptr, %struct.TValue** %ra, align 4, !tbaa !1
  %21 = load %struct.TValue*, %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr4 = getelementptr inbounds %struct.TValue, %struct.TValue* %21, i32 0
  store %struct.TValue* %add.ptr4, %struct.TValue** %rb, align 4, !tbaa !1
  %22 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %23 = load %struct.TValue*, %struct.TValue** %rb, align 4, !tbaa !1
  %24 = bitcast %struct.TValue* %22 to i8*
  %25 = bitcast %struct.TValue* %23 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %24, i8* %25, i32 16, i32 8, i1 false), !tbaa.struct !22
  %26 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr5 = getelementptr inbounds %struct.TValue, %struct.TValue* %26, i32 1
  store %struct.TValue* %add.ptr5, %struct.TValue** %ra, align 4, !tbaa !1
  %27 = load %struct.TValue*, %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr6 = getelementptr inbounds %struct.TValue, %struct.TValue* %27, i32 1
  store %struct.TValue* %add.ptr6, %struct.TValue** %rb, align 4, !tbaa !1
  %28 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %29 = load %struct.TValue*, %struct.TValue** %rb, align 4, !tbaa !1
  %30 = bitcast %struct.TValue* %28 to i8*
  %31 = bitcast %struct.TValue* %29 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %30, i8* %31, i32 16, i32 8, i1 false), !tbaa.struct !22
  %32 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr7 = getelementptr inbounds %struct.TValue, %struct.TValue* %32, i32 2
  store %struct.TValue* %add.ptr7, %struct.TValue** %ra, align 4, !tbaa !1
  %33 = load %struct.TValue*, %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr8 = getelementptr inbounds %struct.TValue, %struct.TValue* %33, i32 2
  store %struct.TValue* %add.ptr8, %struct.TValue** %rb, align 4, !tbaa !1
  %34 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %35 = load %struct.TValue*, %struct.TValue** %rb, align 4, !tbaa !1
  %36 = bitcast %struct.TValue* %34 to i8*
  %37 = bitcast %struct.TValue* %35 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %36, i8* %37, i32 16, i32 8, i1 false), !tbaa.struct !22
  %38 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr9 = getelementptr inbounds %struct.TValue, %struct.TValue* %38, i32 3
  store %struct.TValue* %add.ptr9, %struct.TValue** %ra, align 4, !tbaa !1
  %39 = load %struct.TValue*, %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr10 = getelementptr inbounds %struct.TValue, %struct.TValue* %39, i32 1
  store %struct.TValue* %add.ptr10, %struct.TValue** %rb, align 4, !tbaa !1
  %40 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %41 = load %struct.TValue*, %struct.TValue** %rb, align 4, !tbaa !1
  %42 = bitcast %struct.TValue* %40 to i8*
  %43 = bitcast %struct.TValue* %41 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %42, i8* %43, i32 16, i32 8, i1 false), !tbaa.struct !22
  br label %label_forprep

label_forprep:                                    ; preds = %entry
  %44 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr11 = getelementptr inbounds %struct.TValue, %struct.TValue* %44, i32 1
  store %struct.TValue* %add.ptr11, %struct.TValue** %ra, align 4, !tbaa !1
  %45 = bitcast %struct.TValue** %init to i8*
  call void @llvm.lifetime.start(i64 4, i8* %45) #2
  %46 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  store %struct.TValue* %46, %struct.TValue** %init, align 4, !tbaa !1
  %47 = bitcast %struct.TValue** %plimit to i8*
  call void @llvm.lifetime.start(i64 4, i8* %47) #2
  %48 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %add.ptr12 = getelementptr inbounds %struct.TValue, %struct.TValue* %48, i32 1
  store %struct.TValue* %add.ptr12, %struct.TValue** %plimit, align 4, !tbaa !1
  %49 = bitcast %struct.TValue** %pstep to i8*
  call void @llvm.lifetime.start(i64 4, i8* %49) #2
  %50 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %add.ptr13 = getelementptr inbounds %struct.TValue, %struct.TValue* %50, i32 2
  store %struct.TValue* %add.ptr13, %struct.TValue** %pstep, align 4, !tbaa !1
  %51 = bitcast i64* %ilimit to i8*
  call void @llvm.lifetime.start(i64 8, i8* %51) #2
  %52 = bitcast i32* %stopnow to i8*
  call void @llvm.lifetime.start(i64 4, i8* %52) #2
  %53 = bitcast i32* %init_is_integer to i8*
  call void @llvm.lifetime.start(i64 4, i8* %53) #2
  %54 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %tt_ = getelementptr inbounds %struct.TValue, %struct.TValue* %54, i32 0, i32 1
  %55 = load i32, i32* %tt_, align 4, !tbaa !27
  %cmp = icmp eq i32 %55, 19
  %conv = zext i1 %cmp to i32
  store i32 %conv, i32* %init_is_integer, align 4, !tbaa !23
  %56 = bitcast i32* %pstep_is_integer to i8*
  call void @llvm.lifetime.start(i64 4, i8* %56) #2
  %57 = load %struct.TValue*, %struct.TValue** %pstep, align 4, !tbaa !1
  %tt_14 = getelementptr inbounds %struct.TValue, %struct.TValue* %57, i32 0, i32 1
  %58 = load i32, i32* %tt_14, align 4, !tbaa !27
  %cmp15 = icmp eq i32 %58, 19
  %conv16 = zext i1 %cmp15 to i32
  store i32 %conv16, i32* %pstep_is_integer, align 4, !tbaa !23
  %59 = bitcast i32* %fl to i8*
  call void @llvm.lifetime.start(i64 4, i8* %59) #2
  %60 = load %struct.TValue*, %struct.TValue** %plimit, align 4, !tbaa !1
  %61 = load %struct.TValue*, %struct.TValue** %pstep, align 4, !tbaa !1
  %value_17 = getelementptr inbounds %struct.TValue, %struct.TValue* %61, i32 0, i32 0
  %i = bitcast %union.Value* %value_17 to i64*
  %62 = load i64, i64* %i, align 8, !tbaa !24
  %call = call i32 @forlimit(%struct.TValue* %60, i64* %ilimit, i64 %62, i32* %stopnow)
  store i32 %call, i32* %fl, align 4, !tbaa !23
  %63 = load i32, i32* %init_is_integer, align 4, !tbaa !23
  %tobool = icmp ne i32 %63, 0
  br i1 %tobool, label %land.lhs.true, label %if.else

land.lhs.true:                                    ; preds = %label_forprep
  %64 = load i32, i32* %pstep_is_integer, align 4, !tbaa !23
  %tobool18 = icmp ne i32 %64, 0
  br i1 %tobool18, label %land.lhs.true19, label %if.else

land.lhs.true19:                                  ; preds = %land.lhs.true
  %65 = load i32, i32* %fl, align 4, !tbaa !23
  %tobool20 = icmp ne i32 %65, 0
  br i1 %tobool20, label %if.then, label %if.else

if.then:                                          ; preds = %land.lhs.true19
  %66 = bitcast i64* %initv to i8*
  call void @llvm.lifetime.start(i64 8, i8* %66) #2
  %67 = load i32, i32* %stopnow, align 4, !tbaa !23
  %tobool21 = icmp ne i32 %67, 0
  br i1 %tobool21, label %cond.true, label %cond.false

cond.true:                                        ; preds = %if.then
  br label %cond.end

cond.false:                                       ; preds = %if.then
  %68 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %value_22 = getelementptr inbounds %struct.TValue, %struct.TValue* %68, i32 0, i32 0
  %i23 = bitcast %union.Value* %value_22 to i64*
  %69 = load i64, i64* %i23, align 8, !tbaa !24
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond = phi i64 [ 0, %cond.true ], [ %69, %cond.false ]
  store i64 %cond, i64* %initv, align 8, !tbaa !24
  %70 = load i64, i64* %ilimit, align 8, !tbaa !24
  %71 = load %struct.TValue*, %struct.TValue** %plimit, align 4, !tbaa !1
  %value_24 = getelementptr inbounds %struct.TValue, %struct.TValue* %71, i32 0, i32 0
  %i25 = bitcast %union.Value* %value_24 to i64*
  store i64 %70, i64* %i25, align 8, !tbaa !24
  %72 = load %struct.TValue*, %struct.TValue** %plimit, align 4, !tbaa !1
  %tt_26 = getelementptr inbounds %struct.TValue, %struct.TValue* %72, i32 0, i32 1
  store i32 19, i32* %tt_26, align 4, !tbaa !27
  %73 = load i64, i64* %initv, align 8, !tbaa !24
  %74 = load %struct.TValue*, %struct.TValue** %pstep, align 4, !tbaa !1
  %value_27 = getelementptr inbounds %struct.TValue, %struct.TValue* %74, i32 0, i32 0
  %i28 = bitcast %union.Value* %value_27 to i64*
  %75 = load i64, i64* %i28, align 8, !tbaa !24
  %sub = sub nsw i64 %73, %75
  %76 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %value_29 = getelementptr inbounds %struct.TValue, %struct.TValue* %76, i32 0, i32 0
  %i30 = bitcast %union.Value* %value_29 to i64*
  store i64 %sub, i64* %i30, align 8, !tbaa !24
  %77 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %tt_31 = getelementptr inbounds %struct.TValue, %struct.TValue* %77, i32 0, i32 1
  store i32 19, i32* %tt_31, align 4, !tbaa !27
  %78 = bitcast i64* %initv to i8*
  call void @llvm.lifetime.end(i64 8, i8* %78) #2
  br label %if.end79

if.else:                                          ; preds = %land.lhs.true19, %land.lhs.true, %label_forprep
  %79 = bitcast double* %ninit to i8*
  call void @llvm.lifetime.start(i64 8, i8* %79) #2
  %80 = bitcast double* %nlimit to i8*
  call void @llvm.lifetime.start(i64 8, i8* %80) #2
  %81 = bitcast double* %nstep to i8*
  call void @llvm.lifetime.start(i64 8, i8* %81) #2
  %82 = bitcast i32* %plimit_is_float to i8*
  call void @llvm.lifetime.start(i64 4, i8* %82) #2
  store i32 0, i32* %plimit_is_float, align 4, !tbaa !23
  %83 = load %struct.TValue*, %struct.TValue** %plimit, align 4, !tbaa !1
  %tt_32 = getelementptr inbounds %struct.TValue, %struct.TValue* %83, i32 0, i32 1
  %84 = load i32, i32* %tt_32, align 4, !tbaa !27
  %cmp33 = icmp eq i32 %84, 3
  br i1 %cmp33, label %if.then35, label %if.else37

if.then35:                                        ; preds = %if.else
  store i32 1, i32* %plimit_is_float, align 4, !tbaa !23
  %85 = load %struct.TValue*, %struct.TValue** %plimit, align 4, !tbaa !1
  %value_36 = getelementptr inbounds %struct.TValue, %struct.TValue* %85, i32 0, i32 0
  %n = bitcast %union.Value* %value_36 to double*
  %86 = load double, double* %n, align 8, !tbaa !25
  store double %86, double* %nlimit, align 8, !tbaa !25
  br label %if.end

if.else37:                                        ; preds = %if.else
  %87 = load %struct.TValue*, %struct.TValue** %plimit, align 4, !tbaa !1
  %call38 = call i32 @luaV_tonumber_(%struct.TValue* %87, double* %nlimit)
  store i32 %call38, i32* %plimit_is_float, align 4, !tbaa !23
  br label %if.end

if.end:                                           ; preds = %if.else37, %if.then35
  %88 = load i32, i32* %plimit_is_float, align 4, !tbaa !23
  %tobool39 = icmp ne i32 %88, 0
  br i1 %tobool39, label %if.end42, label %if.then40

if.then40:                                        ; preds = %if.end
  %89 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %call41 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %89, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str1, i32 0, i32 0))
  br label %if.end42

if.end42:                                         ; preds = %if.then40, %if.end
  %90 = load double, double* %nlimit, align 8, !tbaa !25
  %91 = load %struct.TValue*, %struct.TValue** %plimit, align 4, !tbaa !1
  %value_43 = getelementptr inbounds %struct.TValue, %struct.TValue* %91, i32 0, i32 0
  %n44 = bitcast %union.Value* %value_43 to double*
  store double %90, double* %n44, align 8, !tbaa !25
  %92 = load %struct.TValue*, %struct.TValue** %plimit, align 4, !tbaa !1
  %tt_45 = getelementptr inbounds %struct.TValue, %struct.TValue* %92, i32 0, i32 1
  store i32 3, i32* %tt_45, align 4, !tbaa !27
  %93 = bitcast i32* %pstep_is_float to i8*
  call void @llvm.lifetime.start(i64 4, i8* %93) #2
  store i32 0, i32* %pstep_is_float, align 4, !tbaa !23
  %94 = load %struct.TValue*, %struct.TValue** %pstep, align 4, !tbaa !1
  %tt_46 = getelementptr inbounds %struct.TValue, %struct.TValue* %94, i32 0, i32 1
  %95 = load i32, i32* %tt_46, align 4, !tbaa !27
  %cmp47 = icmp eq i32 %95, 3
  br i1 %cmp47, label %if.then49, label %if.else52

if.then49:                                        ; preds = %if.end42
  store i32 1, i32* %pstep_is_float, align 4, !tbaa !23
  %96 = load %struct.TValue*, %struct.TValue** %pstep, align 4, !tbaa !1
  %value_50 = getelementptr inbounds %struct.TValue, %struct.TValue* %96, i32 0, i32 0
  %n51 = bitcast %union.Value* %value_50 to double*
  %97 = load double, double* %n51, align 8, !tbaa !25
  store double %97, double* %nstep, align 8, !tbaa !25
  br label %if.end54

if.else52:                                        ; preds = %if.end42
  %98 = load %struct.TValue*, %struct.TValue** %pstep, align 4, !tbaa !1
  %call53 = call i32 @luaV_tonumber_(%struct.TValue* %98, double* %nstep)
  store i32 %call53, i32* %pstep_is_float, align 4, !tbaa !23
  br label %if.end54

if.end54:                                         ; preds = %if.else52, %if.then49
  %99 = load i32, i32* %pstep_is_float, align 4, !tbaa !23
  %tobool55 = icmp ne i32 %99, 0
  br i1 %tobool55, label %if.end58, label %if.then56

if.then56:                                        ; preds = %if.end54
  %100 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %call57 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %100, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @.str2, i32 0, i32 0))
  br label %if.end58

if.end58:                                         ; preds = %if.then56, %if.end54
  %101 = load double, double* %nstep, align 8, !tbaa !25
  %102 = load %struct.TValue*, %struct.TValue** %pstep, align 4, !tbaa !1
  %value_59 = getelementptr inbounds %struct.TValue, %struct.TValue* %102, i32 0, i32 0
  %n60 = bitcast %union.Value* %value_59 to double*
  store double %101, double* %n60, align 8, !tbaa !25
  %103 = load %struct.TValue*, %struct.TValue** %pstep, align 4, !tbaa !1
  %tt_61 = getelementptr inbounds %struct.TValue, %struct.TValue* %103, i32 0, i32 1
  store i32 3, i32* %tt_61, align 4, !tbaa !27
  %104 = bitcast i32* %init_is_float to i8*
  call void @llvm.lifetime.start(i64 4, i8* %104) #2
  store i32 0, i32* %init_is_float, align 4, !tbaa !23
  %105 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %tt_62 = getelementptr inbounds %struct.TValue, %struct.TValue* %105, i32 0, i32 1
  %106 = load i32, i32* %tt_62, align 4, !tbaa !27
  %cmp63 = icmp eq i32 %106, 3
  br i1 %cmp63, label %if.then65, label %if.else68

if.then65:                                        ; preds = %if.end58
  store i32 1, i32* %init_is_float, align 4, !tbaa !23
  %107 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %value_66 = getelementptr inbounds %struct.TValue, %struct.TValue* %107, i32 0, i32 0
  %n67 = bitcast %union.Value* %value_66 to double*
  %108 = load double, double* %n67, align 8, !tbaa !25
  store double %108, double* %ninit, align 8, !tbaa !25
  br label %if.end70

if.else68:                                        ; preds = %if.end58
  %109 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %call69 = call i32 @luaV_tonumber_(%struct.TValue* %109, double* %ninit)
  store i32 %call69, i32* %init_is_float, align 4, !tbaa !23
  br label %if.end70

if.end70:                                         ; preds = %if.else68, %if.then65
  %110 = load i32, i32* %init_is_float, align 4, !tbaa !23
  %tobool71 = icmp ne i32 %110, 0
  br i1 %tobool71, label %if.end74, label %if.then72

if.then72:                                        ; preds = %if.end70
  %111 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %call73 = call i32 bitcast (i32 (...)* @luaG_runerror to i32 (%struct.lua_State*, i8*)*)(%struct.lua_State* %111, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str3, i32 0, i32 0))
  br label %if.end74

if.end74:                                         ; preds = %if.then72, %if.end70
  %112 = load double, double* %ninit, align 8, !tbaa !25
  %113 = load double, double* %nstep, align 8, !tbaa !25
  %sub75 = fsub double %112, %113
  %114 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %value_76 = getelementptr inbounds %struct.TValue, %struct.TValue* %114, i32 0, i32 0
  %n77 = bitcast %union.Value* %value_76 to double*
  store double %sub75, double* %n77, align 8, !tbaa !25
  %115 = load %struct.TValue*, %struct.TValue** %init, align 4, !tbaa !1
  %tt_78 = getelementptr inbounds %struct.TValue, %struct.TValue* %115, i32 0, i32 1
  store i32 3, i32* %tt_78, align 4, !tbaa !27
  %116 = bitcast i32* %init_is_float to i8*
  call void @llvm.lifetime.end(i64 4, i8* %116) #2
  %117 = bitcast i32* %pstep_is_float to i8*
  call void @llvm.lifetime.end(i64 4, i8* %117) #2
  %118 = bitcast i32* %plimit_is_float to i8*
  call void @llvm.lifetime.end(i64 4, i8* %118) #2
  %119 = bitcast double* %nstep to i8*
  call void @llvm.lifetime.end(i64 8, i8* %119) #2
  %120 = bitcast double* %nlimit to i8*
  call void @llvm.lifetime.end(i64 8, i8* %120) #2
  %121 = bitcast double* %ninit to i8*
  call void @llvm.lifetime.end(i64 8, i8* %121) #2
  br label %if.end79

if.end79:                                         ; preds = %if.end74, %cond.end
  br label %label_forloop

label_loopbody:                                   ; preds = %cleanup144, %cleanup
  %122 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr80 = getelementptr inbounds %struct.TValue, %struct.TValue* %122, i32 0
  store %struct.TValue* %add.ptr80, %struct.TValue** %ra, align 4, !tbaa !1
  %123 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr81 = getelementptr inbounds %struct.TValue, %struct.TValue* %123, i32 4
  store %struct.TValue* %add.ptr81, %struct.TValue** %rb, align 4, !tbaa !1
  %124 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %125 = load %struct.TValue*, %struct.TValue** %rb, align 4, !tbaa !1
  %126 = bitcast %struct.TValue* %124 to i8*
  %127 = bitcast %struct.TValue* %125 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %126, i8* %127, i32 16, i32 8, i1 false), !tbaa.struct !22
  br label %label_forloop

label_forloop:                                    ; preds = %label_loopbody, %if.end79
  %128 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr82 = getelementptr inbounds %struct.TValue, %struct.TValue* %128, i32 1
  store %struct.TValue* %add.ptr82, %struct.TValue** %ra, align 4, !tbaa !1
  %129 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %tt_83 = getelementptr inbounds %struct.TValue, %struct.TValue* %129, i32 0, i32 1
  %130 = load i32, i32* %tt_83, align 4, !tbaa !27
  %cmp84 = icmp eq i32 %130, 19
  br i1 %cmp84, label %if.then86, label %if.else114

if.then86:                                        ; preds = %label_forloop
  %131 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %add.ptr87 = getelementptr inbounds %struct.TValue, %struct.TValue* %131, i32 2
  store %struct.TValue* %add.ptr87, %struct.TValue** %rb, align 4, !tbaa !1
  %132 = bitcast i64* %step to i8*
  call void @llvm.lifetime.start(i64 8, i8* %132) #2
  %133 = load %struct.TValue*, %struct.TValue** %rb, align 4, !tbaa !1
  %value_88 = getelementptr inbounds %struct.TValue, %struct.TValue* %133, i32 0, i32 0
  %i89 = bitcast %union.Value* %value_88 to i64*
  %134 = load i64, i64* %i89, align 8, !tbaa !24
  store i64 %134, i64* %step, align 8, !tbaa !24
  %135 = bitcast i64* %idx to i8*
  call void @llvm.lifetime.start(i64 8, i8* %135) #2
  %136 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %value_90 = getelementptr inbounds %struct.TValue, %struct.TValue* %136, i32 0, i32 0
  %i91 = bitcast %union.Value* %value_90 to i64*
  %137 = load i64, i64* %i91, align 8, !tbaa !24
  %138 = load i64, i64* %step, align 8, !tbaa !24
  %add = add nsw i64 %137, %138
  store i64 %add, i64* %idx, align 8, !tbaa !24
  %139 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %add.ptr92 = getelementptr inbounds %struct.TValue, %struct.TValue* %139, i32 1
  store %struct.TValue* %add.ptr92, %struct.TValue** %rc, align 4, !tbaa !1
  %140 = bitcast i64* %limit to i8*
  call void @llvm.lifetime.start(i64 8, i8* %140) #2
  %141 = load %struct.TValue*, %struct.TValue** %rc, align 4, !tbaa !1
  %value_93 = getelementptr inbounds %struct.TValue, %struct.TValue* %141, i32 0, i32 0
  %i94 = bitcast %union.Value* %value_93 to i64*
  %142 = load i64, i64* %i94, align 8, !tbaa !24
  store i64 %142, i64* %limit, align 8, !tbaa !24
  %143 = load i64, i64* %step, align 8, !tbaa !24
  %cmp95 = icmp slt i64 0, %143
  br i1 %cmp95, label %cond.true97, label %cond.false100

cond.true97:                                      ; preds = %if.then86
  %144 = load i64, i64* %idx, align 8, !tbaa !24
  %145 = load i64, i64* %limit, align 8, !tbaa !24
  %cmp98 = icmp sle i64 %144, %145
  br i1 %cmp98, label %if.then103, label %if.end111

cond.false100:                                    ; preds = %if.then86
  %146 = load i64, i64* %limit, align 8, !tbaa !24
  %147 = load i64, i64* %idx, align 8, !tbaa !24
  %cmp101 = icmp sle i64 %146, %147
  br i1 %cmp101, label %if.then103, label %if.end111

if.then103:                                       ; preds = %cond.false100, %cond.true97
  %148 = load i64, i64* %idx, align 8, !tbaa !24
  %149 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %value_104 = getelementptr inbounds %struct.TValue, %struct.TValue* %149, i32 0, i32 0
  %i105 = bitcast %union.Value* %value_104 to i64*
  store i64 %148, i64* %i105, align 8, !tbaa !24
  %150 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %tt_106 = getelementptr inbounds %struct.TValue, %struct.TValue* %150, i32 0, i32 1
  store i32 19, i32* %tt_106, align 4, !tbaa !27
  %151 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %add.ptr107 = getelementptr inbounds %struct.TValue, %struct.TValue* %151, i32 3
  store %struct.TValue* %add.ptr107, %struct.TValue** %rc, align 4, !tbaa !1
  %152 = load i64, i64* %idx, align 8, !tbaa !24
  %153 = load %struct.TValue*, %struct.TValue** %rc, align 4, !tbaa !1
  %value_108 = getelementptr inbounds %struct.TValue, %struct.TValue* %153, i32 0, i32 0
  %i109 = bitcast %union.Value* %value_108 to i64*
  store i64 %152, i64* %i109, align 8, !tbaa !24
  %154 = load %struct.TValue*, %struct.TValue** %rc, align 4, !tbaa !1
  %tt_110 = getelementptr inbounds %struct.TValue, %struct.TValue* %154, i32 0, i32 1
  store i32 19, i32* %tt_110, align 4, !tbaa !27
  store i32 4, i32* %cleanup.dest.slot
  br label %cleanup

if.end111:                                        ; preds = %cond.false100, %cond.true97
  store i32 0, i32* %cleanup.dest.slot
  br label %cleanup

cleanup:                                          ; preds = %if.end111, %if.then103
  %155 = bitcast i64* %limit to i8*
  call void @llvm.lifetime.end(i64 8, i8* %155) #2
  %156 = bitcast i64* %idx to i8*
  call void @llvm.lifetime.end(i64 8, i8* %156) #2
  %157 = bitcast i64* %step to i8*
  call void @llvm.lifetime.end(i64 8, i8* %157) #2
  %cleanup.dest = load i32, i32* %cleanup.dest.slot
  switch i32 %cleanup.dest, label %unreachable [
    i32 0, label %cleanup.cont
    i32 4, label %label_loopbody
  ]

cleanup.cont:                                     ; preds = %cleanup
  br label %if.end149

if.else114:                                       ; preds = %label_forloop
  %158 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %add.ptr115 = getelementptr inbounds %struct.TValue, %struct.TValue* %158, i32 2
  store %struct.TValue* %add.ptr115, %struct.TValue** %rb, align 4, !tbaa !1
  %159 = bitcast double* %step116 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %159) #2
  %160 = load %struct.TValue*, %struct.TValue** %rb, align 4, !tbaa !1
  %value_117 = getelementptr inbounds %struct.TValue, %struct.TValue* %160, i32 0, i32 0
  %n118 = bitcast %union.Value* %value_117 to double*
  %161 = load double, double* %n118, align 8, !tbaa !25
  store double %161, double* %step116, align 8, !tbaa !25
  %162 = bitcast double* %idx119 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %162) #2
  %163 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %value_120 = getelementptr inbounds %struct.TValue, %struct.TValue* %163, i32 0, i32 0
  %n121 = bitcast %union.Value* %value_120 to double*
  %164 = load double, double* %n121, align 8, !tbaa !25
  %165 = load double, double* %step116, align 8, !tbaa !25
  %add122 = fadd double %164, %165
  store double %add122, double* %idx119, align 8, !tbaa !25
  %166 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %add.ptr123 = getelementptr inbounds %struct.TValue, %struct.TValue* %166, i32 1
  store %struct.TValue* %add.ptr123, %struct.TValue** %rc, align 4, !tbaa !1
  %167 = bitcast double* %limit124 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %167) #2
  %168 = load %struct.TValue*, %struct.TValue** %rc, align 4, !tbaa !1
  %value_125 = getelementptr inbounds %struct.TValue, %struct.TValue* %168, i32 0, i32 0
  %n126 = bitcast %union.Value* %value_125 to double*
  %169 = load double, double* %n126, align 8, !tbaa !25
  store double %169, double* %limit124, align 8, !tbaa !25
  %170 = load double, double* %step116, align 8, !tbaa !25
  %cmp127 = fcmp olt double 0.000000e+00, %170
  br i1 %cmp127, label %cond.true129, label %cond.false132

cond.true129:                                     ; preds = %if.else114
  %171 = load double, double* %idx119, align 8, !tbaa !25
  %172 = load double, double* %limit124, align 8, !tbaa !25
  %cmp130 = fcmp ole double %171, %172
  br i1 %cmp130, label %if.then135, label %if.end143

cond.false132:                                    ; preds = %if.else114
  %173 = load double, double* %limit124, align 8, !tbaa !25
  %174 = load double, double* %idx119, align 8, !tbaa !25
  %cmp133 = fcmp ole double %173, %174
  br i1 %cmp133, label %if.then135, label %if.end143

if.then135:                                       ; preds = %cond.false132, %cond.true129
  %175 = load double, double* %idx119, align 8, !tbaa !25
  %176 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %value_136 = getelementptr inbounds %struct.TValue, %struct.TValue* %176, i32 0, i32 0
  %n137 = bitcast %union.Value* %value_136 to double*
  store double %175, double* %n137, align 8, !tbaa !25
  %177 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %tt_138 = getelementptr inbounds %struct.TValue, %struct.TValue* %177, i32 0, i32 1
  store i32 3, i32* %tt_138, align 4, !tbaa !27
  %178 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %add.ptr139 = getelementptr inbounds %struct.TValue, %struct.TValue* %178, i32 3
  store %struct.TValue* %add.ptr139, %struct.TValue** %rc, align 4, !tbaa !1
  %179 = load double, double* %idx119, align 8, !tbaa !25
  %180 = load %struct.TValue*, %struct.TValue** %rc, align 4, !tbaa !1
  %value_140 = getelementptr inbounds %struct.TValue, %struct.TValue* %180, i32 0, i32 0
  %n141 = bitcast %union.Value* %value_140 to double*
  store double %179, double* %n141, align 8, !tbaa !25
  %181 = load %struct.TValue*, %struct.TValue** %rc, align 4, !tbaa !1
  %tt_142 = getelementptr inbounds %struct.TValue, %struct.TValue* %181, i32 0, i32 1
  store i32 3, i32* %tt_142, align 4, !tbaa !27
  store i32 4, i32* %cleanup.dest.slot
  br label %cleanup144

if.end143:                                        ; preds = %cond.false132, %cond.true129
  store i32 0, i32* %cleanup.dest.slot
  br label %cleanup144

cleanup144:                                       ; preds = %if.end143, %if.then135
  %182 = bitcast double* %limit124 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %182) #2
  %183 = bitcast double* %idx119 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %183) #2
  %184 = bitcast double* %step116 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %184) #2
  %cleanup.dest147 = load i32, i32* %cleanup.dest.slot
  switch i32 %cleanup.dest147, label %unreachable [
    i32 0, label %cleanup.cont148
    i32 4, label %label_loopbody
  ]

cleanup.cont148:                                  ; preds = %cleanup144
  br label %if.end149

if.end149:                                        ; preds = %cleanup.cont148, %cleanup.cont
  store i32 2, i32* %b, align 4, !tbaa !23
  %185 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr150 = getelementptr inbounds %struct.TValue, %struct.TValue* %185, i32 0
  store %struct.TValue* %add.ptr150, %struct.TValue** %ra, align 4, !tbaa !1
  %186 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %187 = load i32, i32* %b, align 4, !tbaa !23
  %add.ptr151 = getelementptr inbounds %struct.TValue, %struct.TValue* %186, i32 %187
  %add.ptr152 = getelementptr inbounds %struct.TValue, %struct.TValue* %add.ptr151, i32 -1
  %188 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top = getelementptr inbounds %struct.lua_State, %struct.lua_State* %188, i32 0, i32 4
  store %struct.TValue* %add.ptr152, %struct.TValue** %top, align 4, !tbaa !29
  %189 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %190 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top153 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %190, i32 0, i32 4
  %191 = load %struct.TValue*, %struct.TValue** %top153, align 4, !tbaa !29
  %sub.ptr.lhs.cast = ptrtoint %struct.TValue* %189 to i32
  %sub.ptr.rhs.cast = ptrtoint %struct.TValue* %191 to i32
  %sub.ptr.sub = sub i32 %sub.ptr.lhs.cast, %sub.ptr.rhs.cast
  %sub.ptr.div = sdiv exact i32 %sub.ptr.sub, 16
  %call154 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str4, i32 0, i32 0), i32 %sub.ptr.div)
  %192 = load %struct.LClosure*, %struct.LClosure** %cl, align 4, !tbaa !1
  %p155 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %192, i32 0, i32 5
  %193 = load %struct.Proto*, %struct.Proto** %p155, align 4, !tbaa !17
  %sizep = getelementptr inbounds %struct.Proto, %struct.Proto* %193, i32 0, i32 10
  %194 = load i32, i32* %sizep, align 4, !tbaa !30
  %cmp156 = icmp sgt i32 %194, 0
  br i1 %cmp156, label %if.then158, label %if.end160

if.then158:                                       ; preds = %if.end149
  %195 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %196 = load %struct.TValue*, %struct.TValue** %base, align 4, !tbaa !1
  %call159 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %195, %struct.TValue* %196)
  br label %if.end160

if.end160:                                        ; preds = %if.then158, %if.end149
  %197 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %198 = load %struct.TValue*, %struct.TValue** %ra, align 4, !tbaa !1
  %call161 = call i32 @luaD_poscall(%struct.lua_State* %197, %struct.TValue* %198)
  store i32 %call161, i32* %b, align 4, !tbaa !23
  %199 = load i32, i32* %b, align 4, !tbaa !23
  %tobool162 = icmp ne i32 %199, 0
  br i1 %tobool162, label %if.then163, label %if.end166

if.then163:                                       ; preds = %if.end160
  %200 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %top164 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %200, i32 0, i32 1
  %201 = load %struct.TValue*, %struct.TValue** %top164, align 4, !tbaa !31
  %202 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top165 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %202, i32 0, i32 4
  store %struct.TValue* %201, %struct.TValue** %top165, align 4, !tbaa !29
  br label %if.end166

if.end166:                                        ; preds = %if.then163, %if.end160
  store i32 1, i32* %cleanup.dest.slot
  %203 = bitcast i32* %fl to i8*
  call void @llvm.lifetime.end(i64 4, i8* %203) #2
  %204 = bitcast i32* %pstep_is_integer to i8*
  call void @llvm.lifetime.end(i64 4, i8* %204) #2
  %205 = bitcast i32* %init_is_integer to i8*
  call void @llvm.lifetime.end(i64 4, i8* %205) #2
  %206 = bitcast i32* %stopnow to i8*
  call void @llvm.lifetime.end(i64 4, i8* %206) #2
  %207 = bitcast i64* %ilimit to i8*
  call void @llvm.lifetime.end(i64 8, i8* %207) #2
  %208 = bitcast %struct.TValue** %pstep to i8*
  call void @llvm.lifetime.end(i64 4, i8* %208) #2
  %209 = bitcast %struct.TValue** %plimit to i8*
  call void @llvm.lifetime.end(i64 4, i8* %209) #2
  %210 = bitcast %struct.TValue** %init to i8*
  call void @llvm.lifetime.end(i64 4, i8* %210) #2
  %211 = bitcast i32* %b to i8*
  call void @llvm.lifetime.end(i64 4, i8* %211) #2
  %212 = bitcast %struct.TValue** %rc to i8*
  call void @llvm.lifetime.end(i64 4, i8* %212) #2
  %213 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.end(i64 4, i8* %213) #2
  %214 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.end(i64 4, i8* %214) #2
  %215 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.end(i64 4, i8* %215) #2
  %216 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.end(i64 4, i8* %216) #2
  %217 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end(i64 4, i8* %217) #2
  %218 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end(i64 4, i8* %218) #2
  %219 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end(i64 4, i8* %219) #2
  ret void

unreachable:                                      ; preds = %cleanup144, %cleanup
  unreachable
}

; Function Attrs: nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #2

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i32(i8* nocapture, i8* nocapture readonly, i32, i32, i1) #2

declare i32 @forlimit(%struct.TValue*, i64*, i64, i32*) #1

; Function Attrs: nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #2

declare i32 @luaV_tonumber_(%struct.TValue*, double*) #1

declare i32 @luaG_runerror(...) #1

declare i32 @luaF_close(...) #1

declare i32 @luaD_poscall(%struct.lua_State*, %struct.TValue*) #1

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
!7 = !{!8, !2, i64 16}
!8 = !{!"lua_State", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !2, i64 8, !2, i64 12, !2, i64 16, !2, i64 20, !2, i64 24, !2, i64 28, !2, i64 32, !2, i64 36, !2, i64 40, !2, i64 44, !9, i64 48, !2, i64 104, !10, i64 112, !12, i64 120, !12, i64 124, !12, i64 128, !11, i64 132, !11, i64 134, !3, i64 136, !3, i64 137}
!9 = !{!"CallInfo", !2, i64 0, !2, i64 4, !2, i64 8, !2, i64 12, !3, i64 16, !10, i64 40, !11, i64 48, !3, i64 50, !3, i64 51}
!10 = !{!"long long", !3, i64 0}
!11 = !{!"short", !3, i64 0}
!12 = !{!"int", !3, i64 0}
!13 = !{!14, !2, i64 16}
!14 = !{!"CallInfoLua", !2, i64 0, !2, i64 4, !2, i64 8, !2, i64 12, !15, i64 16, !10, i64 32, !11, i64 40, !3, i64 42, !3, i64 43}
!15 = !{!"CallInfoL", !2, i64 0, !2, i64 4, !10, i64 8}
!16 = !{!14, !2, i64 0}
!17 = !{!18, !2, i64 12}
!18 = !{!"LClosure", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !2, i64 8, !2, i64 12, !3, i64 16}
!19 = !{!20, !2, i64 44}
!20 = !{!"Proto", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !3, i64 7, !3, i64 8, !12, i64 12, !12, i64 16, !12, i64 20, !12, i64 24, !12, i64 28, !12, i64 32, !12, i64 36, !12, i64 40, !2, i64 44, !2, i64 48, !2, i64 52, !2, i64 56, !2, i64 60, !2, i64 64, !2, i64 68, !2, i64 72, !2, i64 76, !21, i64 80}
!21 = !{!"RaviJITProto", !3, i64 0, !2, i64 4, !2, i64 8}
!22 = !{i64 0, i64 4, !1, i64 0, i64 4, !1, i64 0, i64 4, !23, i64 0, i64 4, !1, i64 0, i64 8, !24, i64 0, i64 8, !25, i64 8, i64 4, !23}
!23 = !{!12, !12, i64 0}
!24 = !{!10, !10, i64 0}
!25 = !{!26, !26, i64 0}
!26 = !{!"double", !3, i64 0}
!27 = !{!28, !12, i64 8}
!28 = !{!"TValue", !3, i64 0, !12, i64 8}
!29 = !{!8, !2, i64 8}
!30 = !{!20, !12, i64 28}
!31 = !{!14, !2, i64 4}
