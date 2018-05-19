; ModuleID = 'lua_op_forloop.c'
source_filename = "lua_op_forloop.c"
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

$"\01??_C@_05JLNEMJLN@dummy?$AA@" = comdat any

@"\01??_C@_0M@KDKGEOFG@value?5?$DN?5?$CFd?6?$AA@" = linkonce_odr unnamed_addr constant [12 x i8] c"value = %d\0A\00", comdat, align 1
@"\01??_C@_05JLNEMJLN@dummy?$AA@" = linkonce_odr unnamed_addr constant [6 x i8] c"dummy\00", comdat, align 1

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
  %rd = alloca %struct.TValue*, align 8
  %A = alloca i32, align 4
  %step = alloca i64, align 8
  %idx = alloca i64, align 8
  %limit = alloca i64, align 8
  %cleanup.dest.slot = alloca i32
  %step34 = alloca double, align 8
  %idx36 = alloca double, align 8
  %limit43 = alloca double, align 8
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
  %9 = bitcast %struct.TValue** %rd to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %9) #3
  %10 = bitcast i32* %A to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %10) #3
  %11 = load %struct.lua_State*, %struct.lua_State** %L.addr, align 8, !tbaa !2
  %ci1 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %11, i32 0, i32 6
  %12 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci1, align 8, !tbaa !8
  store %struct.CallInfoLua* %12, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %13 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %l = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %13, i32 0, i32 4
  %base2 = getelementptr inbounds %struct.CallInfoL, %struct.CallInfoL* %l, i32 0, i32 0
  %14 = load %struct.TValue*, %struct.TValue** %base2, align 8, !tbaa !14
  store %struct.TValue* %14, %struct.TValue** %base, align 8, !tbaa !2
  %15 = load %struct.CallInfoLua*, %struct.CallInfoLua** %ci, align 8, !tbaa !2
  %func = getelementptr inbounds %struct.CallInfoLua, %struct.CallInfoLua* %15, i32 0, i32 0
  %16 = load %struct.TValue*, %struct.TValue** %func, align 8, !tbaa !17
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %16, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %17 = load %struct.GCObject*, %struct.GCObject** %gc, align 8, !tbaa !18
  %18 = bitcast %struct.GCObject* %17 to %struct.LClosure*
  store %struct.LClosure* %18, %struct.LClosure** %cl, align 8, !tbaa !2
  %19 = load %struct.LClosure*, %struct.LClosure** %cl, align 8, !tbaa !2
  %p = getelementptr inbounds %struct.LClosure, %struct.LClosure* %19, i32 0, i32 5
  %20 = load %struct.Proto*, %struct.Proto** %p, align 8, !tbaa !19
  %k3 = getelementptr inbounds %struct.Proto, %struct.Proto* %20, i32 0, i32 14
  %21 = load %struct.TValue*, %struct.TValue** %k3, align 8, !tbaa !21
  store %struct.TValue* %21, %struct.TValue** %k, align 8, !tbaa !2
  br label %22

; <label>:22:                                     ; preds = %97, %60, %0
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @"\01??_C@_05JLNEMJLN@dummy?$AA@", i32 0, i32 0))
  br label %23

; <label>:23:                                     ; preds = %22
  store i32 1, i32* %A, align 4, !tbaa !24
  %24 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %25 = load i32, i32* %A, align 4, !tbaa !24
  %idx.ext = sext i32 %25 to i64
  %add.ptr = getelementptr inbounds %struct.TValue, %struct.TValue* %24, i64 %idx.ext
  store %struct.TValue* %add.ptr, %struct.TValue** %ra, align 8, !tbaa !2
  %26 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %tt_ = getelementptr inbounds %struct.TValue, %struct.TValue* %26, i32 0, i32 1
  %27 = load i16, i16* %tt_, align 8, !tbaa !25
  %conv = zext i16 %27 to i32
  %cmp = icmp eq i32 %conv, 19
  br i1 %cmp, label %28, label %65

; <label>:28:                                     ; preds = %23
  %29 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %30 = load i32, i32* %A, align 4, !tbaa !24
  %add = add nsw i32 %30, 2
  %idx.ext5 = sext i32 %add to i64
  %add.ptr6 = getelementptr inbounds %struct.TValue, %struct.TValue* %29, i64 %idx.ext5
  store %struct.TValue* %add.ptr6, %struct.TValue** %rb, align 8, !tbaa !2
  %31 = bitcast i64* %step to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %31) #3
  %32 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %value_7 = getelementptr inbounds %struct.TValue, %struct.TValue* %32, i32 0, i32 0
  %i = bitcast %union.Value* %value_7 to i64*
  %33 = load i64, i64* %i, align 8, !tbaa !18
  store i64 %33, i64* %step, align 8, !tbaa !27
  %34 = bitcast i64* %idx to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %34) #3
  %35 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %value_8 = getelementptr inbounds %struct.TValue, %struct.TValue* %35, i32 0, i32 0
  %i9 = bitcast %union.Value* %value_8 to i64*
  %36 = load i64, i64* %i9, align 8, !tbaa !18
  %37 = load i64, i64* %step, align 8, !tbaa !27
  %add10 = add nsw i64 %36, %37
  store i64 %add10, i64* %idx, align 8, !tbaa !27
  %38 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %39 = load i32, i32* %A, align 4, !tbaa !24
  %add11 = add nsw i32 %39, 1
  %idx.ext12 = sext i32 %add11 to i64
  %add.ptr13 = getelementptr inbounds %struct.TValue, %struct.TValue* %38, i64 %idx.ext12
  store %struct.TValue* %add.ptr13, %struct.TValue** %rc, align 8, !tbaa !2
  %40 = bitcast i64* %limit to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %40) #3
  %41 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %value_14 = getelementptr inbounds %struct.TValue, %struct.TValue* %41, i32 0, i32 0
  %i15 = bitcast %union.Value* %value_14 to i64*
  %42 = load i64, i64* %i15, align 8, !tbaa !18
  store i64 %42, i64* %limit, align 8, !tbaa !27
  %43 = load i64, i64* %step, align 8, !tbaa !27
  %cmp16 = icmp slt i64 0, %43
  br i1 %cmp16, label %44, label %47

; <label>:44:                                     ; preds = %28
  %45 = load i64, i64* %idx, align 8, !tbaa !27
  %46 = load i64, i64* %limit, align 8, !tbaa !27
  %cmp18 = icmp sle i64 %45, %46
  br i1 %cmp18, label %50, label %59

; <label>:47:                                     ; preds = %28
  %48 = load i64, i64* %limit, align 8, !tbaa !27
  %49 = load i64, i64* %idx, align 8, !tbaa !27
  %cmp20 = icmp sle i64 %48, %49
  br i1 %cmp20, label %50, label %59

; <label>:50:                                     ; preds = %47, %44
  %51 = load i64, i64* %idx, align 8, !tbaa !27
  %52 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %value_22 = getelementptr inbounds %struct.TValue, %struct.TValue* %52, i32 0, i32 0
  %i23 = bitcast %union.Value* %value_22 to i64*
  store i64 %51, i64* %i23, align 8, !tbaa !18
  %53 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %tt_24 = getelementptr inbounds %struct.TValue, %struct.TValue* %53, i32 0, i32 1
  store i16 19, i16* %tt_24, align 8, !tbaa !25
  %54 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %55 = load i32, i32* %A, align 4, !tbaa !24
  %add25 = add nsw i32 %55, 3
  %idx.ext26 = sext i32 %add25 to i64
  %add.ptr27 = getelementptr inbounds %struct.TValue, %struct.TValue* %54, i64 %idx.ext26
  store %struct.TValue* %add.ptr27, %struct.TValue** %rd, align 8, !tbaa !2
  %56 = load i64, i64* %idx, align 8, !tbaa !27
  %57 = load %struct.TValue*, %struct.TValue** %rd, align 8, !tbaa !2
  %value_28 = getelementptr inbounds %struct.TValue, %struct.TValue* %57, i32 0, i32 0
  %i29 = bitcast %union.Value* %value_28 to i64*
  store i64 %56, i64* %i29, align 8, !tbaa !18
  %58 = load %struct.TValue*, %struct.TValue** %rd, align 8, !tbaa !2
  %tt_30 = getelementptr inbounds %struct.TValue, %struct.TValue* %58, i32 0, i32 1
  store i16 19, i16* %tt_30, align 8, !tbaa !25
  store i32 2, i32* %cleanup.dest.slot, align 4
  br label %60

; <label>:59:                                     ; preds = %47, %44
  store i32 0, i32* %cleanup.dest.slot, align 4
  br label %60

; <label>:60:                                     ; preds = %59, %50
  %61 = bitcast i64* %limit to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %61) #3
  %62 = bitcast i64* %idx to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %62) #3
  %63 = bitcast i64* %step to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %63) #3
  %cleanup.dest = load i32, i32* %cleanup.dest.slot, align 4
  switch i32 %cleanup.dest, label %113 [
    i32 0, label %64
    i32 2, label %22
  ]

; <label>:64:                                     ; preds = %60
  br label %102

; <label>:65:                                     ; preds = %23
  %66 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %67 = load i32, i32* %A, align 4, !tbaa !24
  %add31 = add nsw i32 %67, 2
  %idx.ext32 = sext i32 %add31 to i64
  %add.ptr33 = getelementptr inbounds %struct.TValue, %struct.TValue* %66, i64 %idx.ext32
  store %struct.TValue* %add.ptr33, %struct.TValue** %rb, align 8, !tbaa !2
  %68 = bitcast double* %step34 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %68) #3
  %69 = load %struct.TValue*, %struct.TValue** %rb, align 8, !tbaa !2
  %value_35 = getelementptr inbounds %struct.TValue, %struct.TValue* %69, i32 0, i32 0
  %n = bitcast %union.Value* %value_35 to double*
  %70 = load double, double* %n, align 8, !tbaa !18
  store double %70, double* %step34, align 8, !tbaa !28
  %71 = bitcast double* %idx36 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %71) #3
  %72 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %value_37 = getelementptr inbounds %struct.TValue, %struct.TValue* %72, i32 0, i32 0
  %n38 = bitcast %union.Value* %value_37 to double*
  %73 = load double, double* %n38, align 8, !tbaa !18
  %74 = load double, double* %step34, align 8, !tbaa !28
  %add39 = fadd double %73, %74
  store double %add39, double* %idx36, align 8, !tbaa !28
  %75 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %76 = load i32, i32* %A, align 4, !tbaa !24
  %add40 = add nsw i32 %76, 1
  %idx.ext41 = sext i32 %add40 to i64
  %add.ptr42 = getelementptr inbounds %struct.TValue, %struct.TValue* %75, i64 %idx.ext41
  store %struct.TValue* %add.ptr42, %struct.TValue** %rc, align 8, !tbaa !2
  %77 = bitcast double* %limit43 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %77) #3
  %78 = load %struct.TValue*, %struct.TValue** %rc, align 8, !tbaa !2
  %value_44 = getelementptr inbounds %struct.TValue, %struct.TValue* %78, i32 0, i32 0
  %n45 = bitcast %union.Value* %value_44 to double*
  %79 = load double, double* %n45, align 8, !tbaa !18
  store double %79, double* %limit43, align 8, !tbaa !28
  %80 = load double, double* %step34, align 8, !tbaa !28
  %cmp46 = fcmp olt double 0.000000e+00, %80
  br i1 %cmp46, label %81, label %84

; <label>:81:                                     ; preds = %65
  %82 = load double, double* %idx36, align 8, !tbaa !28
  %83 = load double, double* %limit43, align 8, !tbaa !28
  %cmp48 = fcmp ole double %82, %83
  br i1 %cmp48, label %87, label %96

; <label>:84:                                     ; preds = %65
  %85 = load double, double* %limit43, align 8, !tbaa !28
  %86 = load double, double* %idx36, align 8, !tbaa !28
  %cmp50 = fcmp ole double %85, %86
  br i1 %cmp50, label %87, label %96

; <label>:87:                                     ; preds = %84, %81
  %88 = load double, double* %idx36, align 8, !tbaa !28
  %89 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %value_52 = getelementptr inbounds %struct.TValue, %struct.TValue* %89, i32 0, i32 0
  %n53 = bitcast %union.Value* %value_52 to double*
  store double %88, double* %n53, align 8, !tbaa !18
  %90 = load %struct.TValue*, %struct.TValue** %ra, align 8, !tbaa !2
  %tt_54 = getelementptr inbounds %struct.TValue, %struct.TValue* %90, i32 0, i32 1
  store i16 3, i16* %tt_54, align 8, !tbaa !25
  %91 = load %struct.TValue*, %struct.TValue** %base, align 8, !tbaa !2
  %92 = load i32, i32* %A, align 4, !tbaa !24
  %add55 = add nsw i32 %92, 3
  %idx.ext56 = sext i32 %add55 to i64
  %add.ptr57 = getelementptr inbounds %struct.TValue, %struct.TValue* %91, i64 %idx.ext56
  store %struct.TValue* %add.ptr57, %struct.TValue** %rd, align 8, !tbaa !2
  %93 = load double, double* %idx36, align 8, !tbaa !28
  %94 = load %struct.TValue*, %struct.TValue** %rd, align 8, !tbaa !2
  %value_58 = getelementptr inbounds %struct.TValue, %struct.TValue* %94, i32 0, i32 0
  %n59 = bitcast %union.Value* %value_58 to double*
  store double %93, double* %n59, align 8, !tbaa !18
  %95 = load %struct.TValue*, %struct.TValue** %rd, align 8, !tbaa !2
  %tt_60 = getelementptr inbounds %struct.TValue, %struct.TValue* %95, i32 0, i32 1
  store i16 3, i16* %tt_60, align 8, !tbaa !25
  store i32 2, i32* %cleanup.dest.slot, align 4
  br label %97

; <label>:96:                                     ; preds = %84, %81
  store i32 0, i32* %cleanup.dest.slot, align 4
  br label %97

; <label>:97:                                     ; preds = %96, %87
  %98 = bitcast double* %limit43 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %98) #3
  %99 = bitcast double* %idx36 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %99) #3
  %100 = bitcast double* %step34 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %100) #3
  %cleanup.dest61 = load i32, i32* %cleanup.dest.slot, align 4
  switch i32 %cleanup.dest61, label %113 [
    i32 0, label %101
    i32 2, label %22
  ]

; <label>:101:                                    ; preds = %97
  br label %102

; <label>:102:                                    ; preds = %101, %64
  %103 = bitcast i32* %A to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %103) #3
  %104 = bitcast %struct.TValue** %rd to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %104) #3
  %105 = bitcast %struct.TValue** %rc to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %105) #3
  %106 = bitcast %struct.TValue** %rb to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %106) #3
  %107 = bitcast %struct.TValue** %ra to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %107) #3
  %108 = bitcast %struct.CallInfoL** %cil to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %108) #3
  %109 = bitcast %struct.TValue** %base to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %109) #3
  %110 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %110) #3
  %111 = bitcast %struct.LClosure** %cl to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %111) #3
  %112 = bitcast %struct.CallInfoLua** %ci to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %112) #3
  ret void

; <label>:113:                                    ; preds = %97, %60
  unreachable
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
