; ModuleID = 'lua_if_else.c'
target datalayout = "e-m:w-p:32:32-i64:64-f80:32-n8:16:32-S32"
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
  %0 = load %struct.GCObject** %obj.addr, align 4, !tbaa !1
  %tt = getelementptr inbounds %struct.GCObject* %0, i32 0, i32 1
  %1 = load i8* %tt, align 1, !tbaa !5
  %conv = zext i8 %1 to i32
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([12 x i8]* @.str, i32 0, i32 0), i32 %conv)
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
  %ra2 = alloca %struct.TValue*, align 4
  %rb2 = alloca %struct.TValue*, align 4
  %ra3 = alloca %struct.TValue*, align 4
  %b = alloca i32, align 4
  %ra = alloca %struct.TValue*, align 4
  %rb = alloca %struct.TValue*, align 4
  %rc = alloca %struct.TValue*, align 4
  %eq = alloca i32, align 4
  %a = alloca i32, align 4
  %a39 = alloca i32, align 4
  store %struct.lua_State* %L, %struct.lua_State** %L.addr, align 4, !tbaa !1
  %0 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %ci1 = getelementptr inbounds %struct.lua_State* %0, i32 0, i32 6
  %1 = load %struct.CallInfoLua** %ci1, align 4, !tbaa !7
  store %struct.CallInfoLua* %1, %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %2 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l = getelementptr inbounds %struct.CallInfoLua* %2, i32 0, i32 4
  %base2 = getelementptr inbounds %struct.CallInfoL* %l, i32 0, i32 0
  %3 = load %struct.TValue** %base2, align 4, !tbaa !13
  store %struct.TValue* %3, %struct.TValue** %base, align 4, !tbaa !1
  %4 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %func = getelementptr inbounds %struct.CallInfoLua* %4, i32 0, i32 0
  %5 = load %struct.TValue** %func, align 4, !tbaa !16
  %value_ = getelementptr inbounds %struct.TValue* %5, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %6 = load %struct.GCObject** %gc, align 4, !tbaa !1
  %7 = bitcast %struct.GCObject* %6 to %struct.LClosure*
  store %struct.LClosure* %7, %struct.LClosure** %cl, align 4, !tbaa !1
  %8 = load %struct.LClosure** %cl, align 4, !tbaa !1
  %p = getelementptr inbounds %struct.LClosure* %8, i32 0, i32 5
  %9 = load %struct.Proto** %p, align 4, !tbaa !17
  %k3 = getelementptr inbounds %struct.Proto* %9, i32 0, i32 14
  %10 = load %struct.TValue** %k3, align 4, !tbaa !19
  store %struct.TValue* %10, %struct.TValue** %k, align 4, !tbaa !1
  %11 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr = getelementptr inbounds %struct.TValue* %11, i32 0
  store %struct.TValue* %add.ptr, %struct.TValue** %ra, align 4, !tbaa !1
  %12 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr4 = getelementptr inbounds %struct.TValue* %12, i32 0
  store %struct.TValue* %add.ptr4, %struct.TValue** %rb, align 4, !tbaa !1
  %13 = load %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr5 = getelementptr inbounds %struct.TValue* %13, i32 0
  store %struct.TValue* %add.ptr5, %struct.TValue** %rc, align 4, !tbaa !1
  %14 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %15 = load %struct.TValue** %rb, align 4, !tbaa !1
  %16 = load %struct.TValue** %rc, align 4, !tbaa !1
  %call = call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %14, %struct.TValue* %15, %struct.TValue* %16)
  store i32 %call, i32* %eq, align 4, !tbaa !22
  %17 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l6 = getelementptr inbounds %struct.CallInfoLua* %17, i32 0, i32 4
  %base7 = getelementptr inbounds %struct.CallInfoL* %l6, i32 0, i32 0
  %18 = load %struct.TValue** %base7, align 4, !tbaa !13
  store %struct.TValue* %18, %struct.TValue** %base, align 4, !tbaa !1
  %19 = load i32* %eq, align 4, !tbaa !22
  %cmp = icmp eq i32 %19, 0
  br i1 %cmp, label %if.then, label %if.end15

if.then:                                          ; preds = %entry
  store i32 0, i32* %a, align 4, !tbaa !22
  %20 = load i32* %a, align 4, !tbaa !22
  %cmp8 = icmp sgt i32 %20, 0
  br i1 %cmp8, label %if.then9, label %if.end

if.then9:                                         ; preds = %if.then
  %21 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %22 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l10 = getelementptr inbounds %struct.CallInfoLua* %22, i32 0, i32 4
  %base11 = getelementptr inbounds %struct.CallInfoL* %l10, i32 0, i32 0
  %23 = load %struct.TValue** %base11, align 4, !tbaa !13
  %24 = load i32* %a, align 4, !tbaa !22
  %add.ptr12 = getelementptr inbounds %struct.TValue* %23, i32 %24
  %add.ptr13 = getelementptr inbounds %struct.TValue* %add.ptr12, i32 -1
  %call14 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %21, %struct.TValue* %add.ptr13)
  br label %if.end

if.end:                                           ; preds = %if.then9, %if.then
  br label %label6

if.end15:                                         ; preds = %entry
  br label %label3

label3:                                           ; preds = %if.end15
  %25 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr16 = getelementptr inbounds %struct.TValue* %25, i32 1
  store %struct.TValue* %add.ptr16, %struct.TValue** %ra2, align 4, !tbaa !1
  %26 = load %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr17 = getelementptr inbounds %struct.TValue* %26, i32 1
  store %struct.TValue* %add.ptr17, %struct.TValue** %rb2, align 4, !tbaa !1
  %27 = load %struct.TValue** %ra2, align 4, !tbaa !1
  %28 = load %struct.TValue** %rb2, align 4, !tbaa !1
  %29 = bitcast %struct.TValue* %27 to i8*
  %30 = bitcast %struct.TValue* %28 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %29, i8* %30, i32 16, i32 8, i1 false), !tbaa.struct !23
  store i32 2, i32* %b, align 4, !tbaa !22
  %31 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr18 = getelementptr inbounds %struct.TValue* %31, i32 1
  store %struct.TValue* %add.ptr18, %struct.TValue** %ra3, align 4, !tbaa !1
  %32 = load %struct.TValue** %ra3, align 4, !tbaa !1
  %33 = load i32* %b, align 4, !tbaa !22
  %add.ptr19 = getelementptr inbounds %struct.TValue* %32, i32 %33
  %add.ptr20 = getelementptr inbounds %struct.TValue* %add.ptr19, i32 -1
  %34 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top = getelementptr inbounds %struct.lua_State* %34, i32 0, i32 4
  store %struct.TValue* %add.ptr20, %struct.TValue** %top, align 4, !tbaa !27
  %35 = load %struct.LClosure** %cl, align 4, !tbaa !1
  %p21 = getelementptr inbounds %struct.LClosure* %35, i32 0, i32 5
  %36 = load %struct.Proto** %p21, align 4, !tbaa !17
  %sizep = getelementptr inbounds %struct.Proto* %36, i32 0, i32 10
  %37 = load i32* %sizep, align 4, !tbaa !28
  %cmp22 = icmp sgt i32 %37, 0
  br i1 %cmp22, label %if.then23, label %if.end25

if.then23:                                        ; preds = %label3
  %38 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %39 = load %struct.TValue** %base, align 4, !tbaa !1
  %call24 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %38, %struct.TValue* %39)
  br label %if.end25

if.end25:                                         ; preds = %if.then23, %label3
  %40 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %41 = load %struct.TValue** %ra3, align 4, !tbaa !1
  %call26 = call i32 @luaD_poscall(%struct.lua_State* %40, %struct.TValue* %41)
  store i32 %call26, i32* %b, align 4, !tbaa !22
  %42 = load i32* %b, align 4, !tbaa !22
  %tobool = icmp ne i32 %42, 0
  br i1 %tobool, label %if.then27, label %if.end30

if.then27:                                        ; preds = %if.end25
  %43 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %top28 = getelementptr inbounds %struct.CallInfoLua* %43, i32 0, i32 1
  %44 = load %struct.TValue** %top28, align 4, !tbaa !29
  %45 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top29 = getelementptr inbounds %struct.lua_State* %45, i32 0, i32 4
  store %struct.TValue* %44, %struct.TValue** %top29, align 4, !tbaa !27
  br label %if.end30

if.end30:                                         ; preds = %if.then27, %if.end25
  br label %return

label6:                                           ; preds = %if.end
  %46 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr31 = getelementptr inbounds %struct.TValue* %46, i32 0
  store %struct.TValue* %add.ptr31, %struct.TValue** %ra, align 4, !tbaa !1
  %47 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr32 = getelementptr inbounds %struct.TValue* %47, i32 0
  store %struct.TValue* %add.ptr32, %struct.TValue** %rb, align 4, !tbaa !1
  %48 = load %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr33 = getelementptr inbounds %struct.TValue* %48, i32 2
  store %struct.TValue* %add.ptr33, %struct.TValue** %rc, align 4, !tbaa !1
  %49 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %50 = load %struct.TValue** %rb, align 4, !tbaa !1
  %51 = load %struct.TValue** %rc, align 4, !tbaa !1
  %call34 = call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %49, %struct.TValue* %50, %struct.TValue* %51)
  store i32 %call34, i32* %eq, align 4, !tbaa !22
  %52 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l35 = getelementptr inbounds %struct.CallInfoLua* %52, i32 0, i32 4
  %base36 = getelementptr inbounds %struct.CallInfoL* %l35, i32 0, i32 0
  %53 = load %struct.TValue** %base36, align 4, !tbaa !13
  store %struct.TValue* %53, %struct.TValue** %base, align 4, !tbaa !1
  %54 = load i32* %eq, align 4, !tbaa !22
  %cmp37 = icmp eq i32 %54, 0
  br i1 %cmp37, label %if.then38, label %if.end48

if.then38:                                        ; preds = %label6
  store i32 0, i32* %a39, align 4, !tbaa !22
  %55 = load i32* %a39, align 4, !tbaa !22
  %cmp40 = icmp sgt i32 %55, 0
  br i1 %cmp40, label %if.then41, label %if.end47

if.then41:                                        ; preds = %if.then38
  %56 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %57 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %l42 = getelementptr inbounds %struct.CallInfoLua* %57, i32 0, i32 4
  %base43 = getelementptr inbounds %struct.CallInfoL* %l42, i32 0, i32 0
  %58 = load %struct.TValue** %base43, align 4, !tbaa !13
  %59 = load i32* %a39, align 4, !tbaa !22
  %add.ptr44 = getelementptr inbounds %struct.TValue* %58, i32 %59
  %add.ptr45 = getelementptr inbounds %struct.TValue* %add.ptr44, i32 -1
  %call46 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %56, %struct.TValue* %add.ptr45)
  br label %if.end47

if.end47:                                         ; preds = %if.then41, %if.then38
  br label %label11

if.end48:                                         ; preds = %label6
  br label %label8

label8:                                           ; preds = %if.end48
  %60 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr49 = getelementptr inbounds %struct.TValue* %60, i32 1
  store %struct.TValue* %add.ptr49, %struct.TValue** %ra2, align 4, !tbaa !1
  %61 = load %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr50 = getelementptr inbounds %struct.TValue* %61, i32 3
  store %struct.TValue* %add.ptr50, %struct.TValue** %rb2, align 4, !tbaa !1
  %62 = load %struct.TValue** %ra2, align 4, !tbaa !1
  %63 = load %struct.TValue** %rb2, align 4, !tbaa !1
  %64 = bitcast %struct.TValue* %62 to i8*
  %65 = bitcast %struct.TValue* %63 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %64, i8* %65, i32 16, i32 8, i1 false), !tbaa.struct !23
  store i32 2, i32* %b, align 4, !tbaa !22
  %66 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr51 = getelementptr inbounds %struct.TValue* %66, i32 1
  store %struct.TValue* %add.ptr51, %struct.TValue** %ra3, align 4, !tbaa !1
  %67 = load %struct.TValue** %ra3, align 4, !tbaa !1
  %68 = load i32* %b, align 4, !tbaa !22
  %add.ptr52 = getelementptr inbounds %struct.TValue* %67, i32 %68
  %add.ptr53 = getelementptr inbounds %struct.TValue* %add.ptr52, i32 -1
  %69 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top54 = getelementptr inbounds %struct.lua_State* %69, i32 0, i32 4
  store %struct.TValue* %add.ptr53, %struct.TValue** %top54, align 4, !tbaa !27
  %70 = load %struct.LClosure** %cl, align 4, !tbaa !1
  %p55 = getelementptr inbounds %struct.LClosure* %70, i32 0, i32 5
  %71 = load %struct.Proto** %p55, align 4, !tbaa !17
  %sizep56 = getelementptr inbounds %struct.Proto* %71, i32 0, i32 10
  %72 = load i32* %sizep56, align 4, !tbaa !28
  %cmp57 = icmp sgt i32 %72, 0
  br i1 %cmp57, label %if.then58, label %if.end60

if.then58:                                        ; preds = %label8
  %73 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %74 = load %struct.TValue** %base, align 4, !tbaa !1
  %call59 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %73, %struct.TValue* %74)
  br label %if.end60

if.end60:                                         ; preds = %if.then58, %label8
  %75 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %76 = load %struct.TValue** %ra3, align 4, !tbaa !1
  %call61 = call i32 @luaD_poscall(%struct.lua_State* %75, %struct.TValue* %76)
  store i32 %call61, i32* %b, align 4, !tbaa !22
  %77 = load i32* %b, align 4, !tbaa !22
  %tobool62 = icmp ne i32 %77, 0
  br i1 %tobool62, label %if.then63, label %if.end66

if.then63:                                        ; preds = %if.end60
  %78 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %top64 = getelementptr inbounds %struct.CallInfoLua* %78, i32 0, i32 1
  %79 = load %struct.TValue** %top64, align 4, !tbaa !29
  %80 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top65 = getelementptr inbounds %struct.lua_State* %80, i32 0, i32 4
  store %struct.TValue* %79, %struct.TValue** %top65, align 4, !tbaa !27
  br label %if.end66

if.end66:                                         ; preds = %if.then63, %if.end60
  br label %return

label11:                                          ; preds = %if.end47
  %81 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr67 = getelementptr inbounds %struct.TValue* %81, i32 1
  store %struct.TValue* %add.ptr67, %struct.TValue** %ra2, align 4, !tbaa !1
  %82 = load %struct.TValue** %k, align 4, !tbaa !1
  %add.ptr68 = getelementptr inbounds %struct.TValue* %82, i32 4
  store %struct.TValue* %add.ptr68, %struct.TValue** %rb2, align 4, !tbaa !1
  %83 = load %struct.TValue** %ra2, align 4, !tbaa !1
  %84 = load %struct.TValue** %rb2, align 4, !tbaa !1
  %85 = bitcast %struct.TValue* %83 to i8*
  %86 = bitcast %struct.TValue* %84 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %85, i8* %86, i32 16, i32 8, i1 false), !tbaa.struct !23
  store i32 2, i32* %b, align 4, !tbaa !22
  %87 = load %struct.TValue** %base, align 4, !tbaa !1
  %add.ptr69 = getelementptr inbounds %struct.TValue* %87, i32 1
  store %struct.TValue* %add.ptr69, %struct.TValue** %ra3, align 4, !tbaa !1
  %88 = load %struct.TValue** %ra3, align 4, !tbaa !1
  %89 = load i32* %b, align 4, !tbaa !22
  %add.ptr70 = getelementptr inbounds %struct.TValue* %88, i32 %89
  %add.ptr71 = getelementptr inbounds %struct.TValue* %add.ptr70, i32 -1
  %90 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top72 = getelementptr inbounds %struct.lua_State* %90, i32 0, i32 4
  store %struct.TValue* %add.ptr71, %struct.TValue** %top72, align 4, !tbaa !27
  %91 = load %struct.LClosure** %cl, align 4, !tbaa !1
  %p73 = getelementptr inbounds %struct.LClosure* %91, i32 0, i32 5
  %92 = load %struct.Proto** %p73, align 4, !tbaa !17
  %sizep74 = getelementptr inbounds %struct.Proto* %92, i32 0, i32 10
  %93 = load i32* %sizep74, align 4, !tbaa !28
  %cmp75 = icmp sgt i32 %93, 0
  br i1 %cmp75, label %if.then76, label %if.end78

if.then76:                                        ; preds = %label11
  %94 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %95 = load %struct.TValue** %base, align 4, !tbaa !1
  %call77 = call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %94, %struct.TValue* %95)
  br label %if.end78

if.end78:                                         ; preds = %if.then76, %label11
  %96 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %97 = load %struct.TValue** %ra3, align 4, !tbaa !1
  %call79 = call i32 @luaD_poscall(%struct.lua_State* %96, %struct.TValue* %97)
  store i32 %call79, i32* %b, align 4, !tbaa !22
  %98 = load i32* %b, align 4, !tbaa !22
  %tobool80 = icmp ne i32 %98, 0
  br i1 %tobool80, label %if.then81, label %if.end84

if.then81:                                        ; preds = %if.end78
  %99 = load %struct.CallInfoLua** %ci, align 4, !tbaa !1
  %top82 = getelementptr inbounds %struct.CallInfoLua* %99, i32 0, i32 1
  %100 = load %struct.TValue** %top82, align 4, !tbaa !29
  %101 = load %struct.lua_State** %L.addr, align 4, !tbaa !1
  %top83 = getelementptr inbounds %struct.lua_State* %101, i32 0, i32 4
  store %struct.TValue* %100, %struct.TValue** %top83, align 4, !tbaa !27
  br label %if.end84

if.end84:                                         ; preds = %if.then81, %if.end78
  br label %return

label_return:                                     ; No predecessors!
  br label %return

return:                                           ; preds = %label_return, %if.end84, %if.end66, %if.end30
  ret void
}

declare i32 @luaV_equalobj(...) #1

declare i32 @luaF_close(...) #1

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i32(i8* nocapture, i8* nocapture readonly, i32, i32, i1) #2

declare i32 @luaD_poscall(%struct.lua_State*, %struct.TValue*) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.6.0 (trunk)"}
!1 = metadata !{metadata !2, metadata !2, i64 0}
!2 = metadata !{metadata !"any pointer", metadata !3, i64 0}
!3 = metadata !{metadata !"omnipotent char", metadata !4, i64 0}
!4 = metadata !{metadata !"Simple C/C++ TBAA"}
!5 = metadata !{metadata !6, metadata !3, i64 4}
!6 = metadata !{metadata !"GCObject", metadata !2, i64 0, metadata !3, i64 4, metadata !3, i64 5}
!7 = metadata !{metadata !8, metadata !2, i64 16}
!8 = metadata !{metadata !"lua_State", metadata !2, i64 0, metadata !3, i64 4, metadata !3, i64 5, metadata !3, i64 6, metadata !2, i64 8, metadata !2, i64 12, metadata !2, i64 16, metadata !2, i64 20, metadata !2, i64 24, metadata !2, i64 28, metadata !2, i64 32, metadata !2, i64 36, metadata !2, i64 40, metadata !2, i64 44, metadata !9, i64 48, metadata !2, i64 104, metadata !10, i64 112, metadata !12, i64 120, metadata !12, i64 124, metadata !12, i64 128, metadata !11, i64 132, metadata !11, i64 134, metadata !3, i64 136, metadata !3, i64 137}
!9 = metadata !{metadata !"CallInfo", metadata !2, i64 0, metadata !2, i64 4, metadata !2, i64 8, metadata !2, i64 12, metadata !3, i64 16, metadata !10, i64 40, metadata !11, i64 48, metadata !3, i64 50, metadata !3, i64 51}
!10 = metadata !{metadata !"long long", metadata !3, i64 0}
!11 = metadata !{metadata !"short", metadata !3, i64 0}
!12 = metadata !{metadata !"int", metadata !3, i64 0}
!13 = metadata !{metadata !14, metadata !2, i64 16}
!14 = metadata !{metadata !"CallInfoLua", metadata !2, i64 0, metadata !2, i64 4, metadata !2, i64 8, metadata !2, i64 12, metadata !15, i64 16, metadata !10, i64 32, metadata !11, i64 40, metadata !3, i64 42, metadata !3, i64 43}
!15 = metadata !{metadata !"CallInfoL", metadata !2, i64 0, metadata !2, i64 4, metadata !10, i64 8}
!16 = metadata !{metadata !14, metadata !2, i64 0}
!17 = metadata !{metadata !18, metadata !2, i64 12}
!18 = metadata !{metadata !"LClosure", metadata !2, i64 0, metadata !3, i64 4, metadata !3, i64 5, metadata !3, i64 6, metadata !2, i64 8, metadata !2, i64 12, metadata !3, i64 16}
!19 = metadata !{metadata !20, metadata !2, i64 44}
!20 = metadata !{metadata !"Proto", metadata !2, i64 0, metadata !3, i64 4, metadata !3, i64 5, metadata !3, i64 6, metadata !3, i64 7, metadata !3, i64 8, metadata !12, i64 12, metadata !12, i64 16, metadata !12, i64 20, metadata !12, i64 24, metadata !12, i64 28, metadata !12, i64 32, metadata !12, i64 36, metadata !12, i64 40, metadata !2, i64 44, metadata !2, i64 48, metadata !2, i64 52, metadata !2, i64 56, metadata !2, i64 60, metadata !2, i64 64, metadata !2, i64 68, metadata !2, i64 72, metadata !2, i64 76, metadata !21, i64 80}
!21 = metadata !{metadata !"RaviJITProto", metadata !3, i64 0, metadata !2, i64 4, metadata !2, i64 8}
!22 = metadata !{metadata !12, metadata !12, i64 0}
!23 = metadata !{i64 0, i64 4, metadata !1, i64 0, i64 4, metadata !1, i64 0, i64 4, metadata !22, i64 0, i64 4, metadata !1, i64 0, i64 8, metadata !24, i64 0, i64 8, metadata !25, i64 8, i64 4, metadata !22}
!24 = metadata !{metadata !10, metadata !10, i64 0}
!25 = metadata !{metadata !26, metadata !26, i64 0}
!26 = metadata !{metadata !"double", metadata !3, i64 0}
!27 = metadata !{metadata !8, metadata !2, i64 8}
!28 = metadata !{metadata !20, metadata !12, i64 28}
!29 = metadata !{metadata !14, metadata !2, i64 4}
