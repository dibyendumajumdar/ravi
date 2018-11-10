; ModuleID = 'lua_if_else.c'
source_filename = "lua_if_else.c"
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
define void @test1(%struct.lua_State* %L) #0 {
  %L.addr = alloca %struct.lua_State*, align 8
  %ci = alloca %struct.CallInfoLua*, align 8
  %cl = alloca %struct.LClosure*, align 8
  %k = alloca %struct.TValue*, align 8
  %base = alloca %struct.TValue*, align 8
  %cil = alloca %struct.CallInfoL*, align 8
  %ra2 = alloca %struct.TValue*, align 8
  %rb2 = alloca %struct.TValue*, align 8
  %ra3 = alloca %struct.TValue*, align 8
  %b = alloca i32, align 4
  %ra = alloca %struct.TValue*, align 8
  %rb = alloca %struct.TValue*, align 8
  %rc = alloca %struct.TValue*, align 8
  %eq = alloca i32, align 4
  %a = alloca i32, align 4
  %cleanup.dest.slot = alloca i32
  %a33 = alloca i32, align 4
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
  %6 = bitcast %struct.TValue** %ra2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %6) #3
  %7 = bitcast %struct.TValue** %rb2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %7) #3
  %8 = bitcast %struct.TValue** %ra3 to i8*
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
  %21 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %21) #3
  %22 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr = getelementptr inbounds %struct.TValue, %struct.TValue* %22, i64 0
  store %struct.TValue* %add.ptr, %struct.TValue** %ra, align 8, !tbaa !2
  %23 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %23) #3
  %24 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr4 = getelementptr inbounds %struct.TValue, %struct.TValue* %24, i64 0
  store %struct.TValue* %add.ptr4, %struct.TValue** %rb, align 8, !tbaa !2
  %25 = bitcast %struct.TValue** %rc to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %25) #3
  %26 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr5 = getelementptr inbounds %struct.TValue, %struct.TValue* %26, i64 0
  store %struct.TValue* %add.ptr5, %struct.TValue** %rc, align 8, !tbaa !2
  %27 = bitcast i32* %eq to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %27) #3
  %28 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %29 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %30 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call = call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %30, %struct.TValue* %29, %struct.TValue* %28)
  store i32 %call, i32* %eq, align 4, !tbaa !24
  %31 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l6 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %31, i32 0, i32 4
  %base7 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l6, i32 0, i32 0
  %32 = load %struct.TValue*, %struct.TValue** %base7, align 8, !tbaa !14
  store %struct.TValue* %32, %struct.TValue** %base, align 8, !tbaa !2
  %33 = load i32, i32* %eq, align 4, !tbaa !24
  %cmp = icmp eq i32 %33, 0
  br i1 %cmp, label %34, label %44

; <label>:34:                                     ; preds = %0
  %35 = bitcast i32* %a to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %35) #3
  store i32 0, i32* %a, align 4, !tbaa !24
  %36 = load i32, i32* %a, align 4, !tbaa !24
  %cmp8 = icmp sgt i32 %36, 0
  br i1 %cmp8, label %37, label %42

; <label>:37:                                     ; preds = %34
  %38 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l9 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %38, i32 0, i32 4
  %base10 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l9, i32 0, i32 0
  %39 = load %struct.TValue*, %struct.TValue** %base10, align 8, !tbaa !14
  %40 = load i32, i32* %a, align 4, !tbaa !24
  %idx.ext = sext i32 %40 to i64
  %add.ptr11 = getelementptr inbounds %struct.TValue, %struct.TValue* %39, i64 %idx.ext
  %add.ptr12 = getelementptr inbounds %struct.TValue, %struct.TValue* %add.ptr11, i64 -1
  %41 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call13 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %41, %struct.TValue* %add.ptr12)
  br label %42

; <label>:42:                                     ; preds = %37, %34
  store i32 2, i32* %cleanup.dest.slot, align 4
  %43 = bitcast i32* %a to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %43) #3
  %cleanup.dest = load i32, i32* %cleanup.dest.slot, align 4
  switch i32 %cleanup.dest, label %145 [
    i32 2, label %71
  ]

; <label>:44:                                     ; preds = %0
  br label %45

; <label>:45:                                     ; preds = %44
  %46 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr14 = getelementptr inbounds %struct.TValue, %struct.TValue* %46, i64 1
  store %struct.TValue* %add.ptr14, %struct.TValue** %ra2, align 8, !tbaa !2
  %47 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr15 = getelementptr inbounds %struct.TValue, %struct.TValue* %47, i64 1
  store %struct.TValue* %add.ptr15, %struct.TValue** %rb2, align 8, !tbaa !2
  %48 = load %struct.TValue*, %struct.TValue** %ra2, align 8, !tbaa !2
  %49 = load %struct.TValue*, %struct.TValue** %rb2, align 8, !tbaa !2
  %50 = bitcast %struct.TValue* %48 to i8*
  %51 = bitcast %struct.TValue* %49 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %50, i8* %51, i64 16, i32 8, i1 false), !tbaa.struct !25
  store i32 2, i32* %b, align 4, !tbaa !24
  %52 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr16 = getelementptr inbounds %struct.TValue, %struct.TValue* %52, i64 1
  store %struct.TValue* %add.ptr16, %struct.TValue** %ra3, align 8, !tbaa !2
  %53 = load %struct.TValue*, %struct.TValue** %ra3, align 8, !tbaa !2
  %54 = load i32, i32* %b, align 4, !tbaa !24
  %idx.ext17 = sext i32 %54 to i64
  %add.ptr18 = getelementptr inbounds %struct.TValue, %struct.TValue* %53, i64 %idx.ext17
  %add.ptr19 = getelementptr inbounds %struct.TValue, %struct.TValue* %add.ptr18, i64 -1
  %55 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top = getelementptr inbounds %struct.lua_State, %struct.lua_State* %55, i32 0, i32 4
  store %struct.TValue* %add.ptr19, %struct.TValue** %top, align 8, !tbaa !30
  %56 = load %struct.LClosure*, %struct.LClosure** %cl, align 8, !tbaa !2
  %p20 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %56, i32 0, i32 5
  %57 = load %struct.Proto*, %struct.Proto** %p20, align 8, !tbaa !19
  %sizep = getelementptr inbounds %struct.Proto, %struct.Proto* %57, i32 0, i32 10
  %58 = load i32, i32* %sizep, align 8, !tbaa !31
  %cmp21 = icmp sgt i32 %58, 0
  br i1 %cmp21, label %59, label %62

; <label>:59:                                     ; preds = %45
  %60 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %61 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call22 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %61, %struct.TValue* %60)
  br label %62

; <label>:62:                                     ; preds = %59, %45
  %63 = load %struct.TValue*, %struct.TValue** %ra3, align 8, !tbaa !2
  %64 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call23 = call i32 @luaD_poscall(%struct.lua_State* %64, %struct.TValue* %63)
  store i32 %call23, i32* %b, align 4, !tbaa !24
  %65 = load i32, i32* %b, align 4, !tbaa !24
  %tobool = icmp ne i32 %65, 0
  br i1 %tobool, label %66, label %70

; <label>:66:                                     ; preds = %62
  %67 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %top24 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %67, i32 0, i32 1
  %68 = load %struct.TValue*, %struct.TValue** %top24, align 8, !tbaa !32
  %69 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top25 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %69, i32 0, i32 4
  store %struct.TValue* %68, %struct.TValue** %top25, align 8, !tbaa !30
  br label %70

; <label>:70:                                     ; preds = %66, %62
  store i32 1, i32* %cleanup.dest.slot, align 4
  br label %145

; <label>:71:                                     ; preds = %42
  %72 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr26 = getelementptr inbounds %struct.TValue, %struct.TValue* %72, i64 0
  store %struct.TValue* %add.ptr26, %struct.TValue** %ra, align 8, !tbaa !2
  %73 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr27 = getelementptr inbounds %struct.TValue, %struct.TValue* %73, i64 0
  store %struct.TValue* %add.ptr27, %struct.TValue** %rb, align 8, !tbaa !2
  %74 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr28 = getelementptr inbounds %struct.TValue, %struct.TValue* %74, i64 2
  store %struct.TValue* %add.ptr28, %struct.TValue** %rc, align 8, !tbaa !2
  %75 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %76 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %77 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call29 = call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %77, %struct.TValue* %76, %struct.TValue* %75)
  store i32 %call29, i32* %eq, align 4, !tbaa !24
  %78 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l30 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %78, i32 0, i32 4
  %base31 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l30, i32 0, i32 0
  %79 = load %struct.TValue*, %struct.TValue** %base31, align 8, !tbaa !14
  store %struct.TValue* %79, %struct.TValue** %base, align 8, !tbaa !2
  %80 = load i32, i32* %eq, align 4, !tbaa !24
  %cmp32 = icmp eq i32 %80, 0
  br i1 %cmp32, label %81, label %91

; <label>:81:                                     ; preds = %71
  %82 = bitcast i32* %a33 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %82) #3
  store i32 0, i32* %a33, align 4, !tbaa !24
  %83 = load i32, i32* %a33, align 4, !tbaa !24
  %cmp34 = icmp sgt i32 %83, 0
  br i1 %cmp34, label %84, label %89

; <label>:84:                                     ; preds = %81
  %85 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l35 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %85, i32 0, i32 4
  %base36 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l35, i32 0, i32 0
  %86 = load %struct.TValue*, %struct.TValue** %base36, align 8, !tbaa !14
  %87 = load i32, i32* %a33, align 4, !tbaa !24
  %idx.ext37 = sext i32 %87 to i64
  %add.ptr38 = getelementptr inbounds %struct.TValue, %struct.TValue* %86, i64 %idx.ext37
  %add.ptr39 = getelementptr inbounds %struct.TValue, %struct.TValue* %add.ptr38, i64 -1
  %88 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call40 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %88, %struct.TValue* %add.ptr39)
  br label %89

; <label>:89:                                     ; preds = %84, %81
  store i32 5, i32* %cleanup.dest.slot, align 4
  %90 = bitcast i32* %a33 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %90) #3
  %cleanup.dest56 = load i32, i32* %cleanup.dest.slot, align 4
  switch i32 %cleanup.dest56, label %145 [
    i32 5, label %118
  ]

; <label>:91:                                     ; preds = %71
  br label %92

; <label>:92:                                     ; preds = %91
  %93 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr41 = getelementptr inbounds %struct.TValue, %struct.TValue* %93, i64 1
  store %struct.TValue* %add.ptr41, %struct.TValue** %ra2, align 8, !tbaa !2
  %94 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr42 = getelementptr inbounds %struct.TValue, %struct.TValue* %94, i64 3
  store %struct.TValue* %add.ptr42, %struct.TValue** %rb2, align 8, !tbaa !2
  %95 = load %struct.TValue*, %struct.TValue** %ra2, align 8, !tbaa !2
  %96 = load %struct.TValue*, %struct.TValue** %rb2, align 8, !tbaa !2
  %97 = bitcast %struct.TValue* %95 to i8*
  %98 = bitcast %struct.TValue* %96 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %97, i8* %98, i64 16, i32 8, i1 false), !tbaa.struct !25
  store i32 2, i32* %b, align 4, !tbaa !24
  %99 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr43 = getelementptr inbounds %struct.TValue, %struct.TValue* %99, i64 1
  store %struct.TValue* %add.ptr43, %struct.TValue** %ra3, align 8, !tbaa !2
  %100 = load %struct.TValue*, %struct.TValue** %ra3, align 8, !tbaa !2
  %101 = load i32, i32* %b, align 4, !tbaa !24
  %idx.ext44 = sext i32 %101 to i64
  %add.ptr45 = getelementptr inbounds %struct.TValue, %struct.TValue* %100, i64 %idx.ext44
  %add.ptr46 = getelementptr inbounds %struct.TValue, %struct.TValue* %add.ptr45, i64 -1
  %102 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top47 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %102, i32 0, i32 4
  store %struct.TValue* %add.ptr46, %struct.TValue** %top47, align 8, !tbaa !30
  %103 = load %struct.LClosure*, %struct.LClosure** %cl, align 8, !tbaa !2
  %p48 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %103, i32 0, i32 5
  %104 = load %struct.Proto*, %struct.Proto** %p48, align 8, !tbaa !19
  %sizep49 = getelementptr inbounds %struct.Proto, %struct.Proto* %104, i32 0, i32 10
  %105 = load i32, i32* %sizep49, align 8, !tbaa !31
  %cmp50 = icmp sgt i32 %105, 0
  br i1 %cmp50, label %106, label %109

; <label>:106:                                    ; preds = %92
  %107 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %108 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call51 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %108, %struct.TValue* %107)
  br label %109

; <label>:109:                                    ; preds = %106, %92
  %110 = load %struct.TValue*, %struct.TValue** %ra3, align 8, !tbaa !2
  %111 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call52 = call i32 @luaD_poscall(%struct.lua_State* %111, %struct.TValue* %110)
  store i32 %call52, i32* %b, align 4, !tbaa !24
  %112 = load i32, i32* %b, align 4, !tbaa !24
  %tobool53 = icmp ne i32 %112, 0
  br i1 %tobool53, label %113, label %117

; <label>:113:                                    ; preds = %109
  %114 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %top54 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %114, i32 0, i32 1
  %115 = load %struct.TValue*, %struct.TValue** %top54, align 8, !tbaa !32
  %116 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top55 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %116, i32 0, i32 4
  store %struct.TValue* %115, %struct.TValue** %top55, align 8, !tbaa !30
  br label %117

; <label>:117:                                    ; preds = %113, %109
  store i32 1, i32* %cleanup.dest.slot, align 4
  br label %145

; <label>:118:                                    ; preds = %89
  %119 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr57 = getelementptr inbounds %struct.TValue, %struct.TValue* %119, i64 1
  store %struct.TValue* %add.ptr57, %struct.TValue** %ra2, align 8, !tbaa !2
  %120 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %add.ptr58 = getelementptr inbounds %struct.TValue, %struct.TValue* %120, i64 4
  store %struct.TValue* %add.ptr58, %struct.TValue** %rb2, align 8, !tbaa !2
  %121 = load %struct.TValue*, %struct.TValue** %ra2, align 8, !tbaa !2
  %122 = load %struct.TValue*, %struct.TValue** %rb2, align 8, !tbaa !2
  %123 = bitcast %struct.TValue* %121 to i8*
  %124 = bitcast %struct.TValue* %122 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %123, i8* %124, i64 16, i32 8, i1 false), !tbaa.struct !25
  store i32 2, i32* %b, align 4, !tbaa !24
  %125 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %add.ptr59 = getelementptr inbounds %struct.TValue, %struct.TValue* %125, i64 1
  store %struct.TValue* %add.ptr59, %struct.TValue** %ra3, align 8, !tbaa !2
  %126 = load %struct.TValue*, %struct.TValue** %ra3, align 8, !tbaa !2
  %127 = load i32, i32* %b, align 4, !tbaa !24
  %idx.ext60 = sext i32 %127 to i64
  %add.ptr61 = getelementptr inbounds %struct.TValue, %struct.TValue* %126, i64 %idx.ext60
  %add.ptr62 = getelementptr inbounds %struct.TValue, %struct.TValue* %add.ptr61, i64 -1
  %128 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top63 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %128, i32 0, i32 4
  store %struct.TValue* %add.ptr62, %struct.TValue** %top63, align 8, !tbaa !30
  %129 = load %struct.LClosure*, %struct.LClosure** %cl, align 8, !tbaa !2
  %p64 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %129, i32 0, i32 5
  %130 = load %struct.Proto*, %struct.Proto** %p64, align 8, !tbaa !19
  %sizep65 = getelementptr inbounds %struct.Proto, %struct.Proto* %130, i32 0, i32 10
  %131 = load i32, i32* %sizep65, align 8, !tbaa !31
  %cmp66 = icmp sgt i32 %131, 0
  br i1 %cmp66, label %132, label %135

; <label>:132:                                    ; preds = %118
  %133 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %134 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call67 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %134, %struct.TValue* %133)
  br label %135

; <label>:135:                                    ; preds = %132, %118
  %136 = load %struct.TValue*, %struct.TValue** %ra3, align 8, !tbaa !2
  %137 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %call68 = call i32 @luaD_poscall(%struct.lua_State* %137, %struct.TValue* %136)
  store i32 %call68, i32* %b, align 4, !tbaa !24
  %138 = load i32, i32* %b, align 4, !tbaa !24
  %tobool69 = icmp ne i32 %138, 0
  br i1 %tobool69, label %139, label %143

; <label>:139:                                    ; preds = %135
  %140 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %top70 = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %140, i32 0, i32 1
  %141 = load %struct.TValue*, %struct.TValue** %top70, align 8, !tbaa !32
  %142 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %top71 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %142, i32 0, i32 4
  store %struct.TValue* %141, %struct.TValue** %top71, align 8, !tbaa !30
  br label %143

; <label>:143:                                    ; preds = %139, %135
  store i32 1, i32* %cleanup.dest.slot, align 4
  br label %145
                                                  ; No predecessors!
  store i32 1, i32* %cleanup.dest.slot, align 4
  br label %145

; <label>:145:                                    ; preds = %144, %143, %89, %117, %42, %70
  %146 = bitcast i32* %eq to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %146) #3
  %147 = bitcast %struct.TValue** %rc to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %147) #3
  %148 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %148) #3
  %149 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %149) #3
  %150 = bitcast i32* %b to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %150) #3
  %151 = bitcast %struct.TValue** %ra3 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %151) #3
  %152 = bitcast %struct.TValue** %rb2 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %152) #3
  %153 = bitcast %struct.TValue** %ra2 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %153) #3
  %154 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %154) #3
  %155 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %155) #3
  %156 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %156) #3
  %157 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %157) #3
  %158 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %158) #3
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #2

declare i32 @luaV_equalobj(...) #1

declare i32 @luaF_close(...) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

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
!24 = !{!13, !13, i64 0}
!25 = !{i64 0, i64 8, !2, i64 0, i64 8, !2, i64 0, i64 4, !24, i64 0, i64 8, !2, i64 0, i64 8, !26, i64 0, i64 8, !27, i64 8, i64 2, !29}
!26 = !{!11, !11, i64 0}
!27 = !{!28, !28, i64 0}
!28 = !{!"double", !4, i64 0}
!29 = !{!12, !12, i64 0}
!30 = !{!9, !3, i64 16}
!31 = !{!22, !13, i64 32}
!32 = !{!15, !3, i64 8}
