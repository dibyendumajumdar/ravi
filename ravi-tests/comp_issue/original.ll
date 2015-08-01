; ModuleID = 'ravi_module_ravif1'

%0 = type { %struct.TValue*, i32*, i64 }
%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfo*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, i8* (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8 }
%struct.global_State = type opaque
%struct.TValue = type { %union.Value, i32 }
%union.Value = type { double }
%struct.UpVal = type { %struct.TValue*, i64, %struct.TValue }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.lua_longjmp = type opaque
%struct.CallInfo = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %0, i64, i16, i8, i8 }
%struct.lua_Debug = type opaque
%struct.LClosure = type { %struct.GCObject*, i8, i8, i8, %struct.GCObject*, %struct.Proto*, [1 x %struct.UpVal*] }
%struct.Proto = type { %struct.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %struct.TValue*, i32*, %struct.Proto**, i32*, %struct.LocVar*, %struct.Upvaldesc*, %struct.LClosure*, %struct.TString*, %struct.GCObject*, %struct.RaviJITProto }
%struct.LocVar = type { %struct.TString*, i32, i32, i32 }
%struct.Upvaldesc = type { %struct.TString*, i32, i8, i8 }
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i8, i32, i64 }
%struct.RaviJITProto = type { i8, i8, i16, i8*, i32 (%struct.lua_State*)* }
%struct.Table = type { %struct.GCObject*, i8, i8, i8, i8, i32, %struct.TValue*, %struct.Node*, %struct.Node*, %struct.Table*, %struct.GCObject*, %struct.RaviArray }
%struct.Node = type { %struct.TValue, %struct.TKey }
%struct.TKey = type { %union.Value, i32, i32 }
%struct.RaviArray = type { i8*, i32, i32, i8, i8 }

@0 = private unnamed_addr constant [17 x i8] c"integer expected\00"

define i32 @ravif1(%struct.lua_State* %L) {
entry:
  %nb = alloca double
  %nc = alloca double
  %ilimit52 = alloca i64
  %iidx53 = alloca i64
  %ilimit = alloca i64
  %iidx = alloca i64
  %i = alloca i64
  %L_ci = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 6
  %0 = load %struct.CallInfo*, %struct.CallInfo** %L_ci, !tbaa !0
  %base = getelementptr inbounds %struct.CallInfo, %struct.CallInfo* %0, i32 0, i32 4, i32 0
  %1 = bitcast %struct.CallInfo* %0 to %struct.LClosure***
  %2 = load %struct.LClosure**, %struct.LClosure*** %1, !tbaa !7
  %3 = load %struct.LClosure*, %struct.LClosure** %2, !tbaa !8
  %Proto = getelementptr inbounds %struct.LClosure, %struct.LClosure* %3, i32 0, i32 5
  %4 = load %struct.Proto*, %struct.Proto** %Proto, !tbaa !9
  %k = getelementptr inbounds %struct.Proto, %struct.Proto* %4, i32 0, i32 14
  %5 = load %struct.TValue*, %struct.TValue** %k, !tbaa !11
  %6 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %value.tt.ptr = getelementptr inbounds %struct.TValue, %struct.TValue* %6, i32 0, i32 1
  %value.tt = load i32, i32* %value.tt.ptr, !tbaa !15
  %is.not.integer = icmp ne i32 %value.tt, 19
  br i1 %is.not.integer, label %if.not.integer, label %done

if.not.integer:                                   ; preds = %entry
  %7 = call i32 @luaV_tointeger_(%struct.TValue* %6, i64* %i)
  %int.conversion.failed = icmp eq i32 %7, 0
  br i1 %int.conversion.failed, label %if.conversion.failed, label %conversion.ok

if.conversion.failed:                             ; preds = %if.not.integer
  call void (%struct.lua_State*, i8*, ...) @luaG_runerror(%struct.lua_State* %L, i8* getelementptr inbounds ([17 x i8], [17 x i8]* @0, i32 0, i32 0))
  br label %conversion.ok

conversion.ok:                                    ; preds = %if.conversion.failed, %if.not.integer
  %8 = load i64, i64* %i, !tbaa !17
  %9 = bitcast %struct.TValue* %6 to i64*
  store i64 %8, i64* %9, !tbaa !18
  %dest.tt = getelementptr inbounds %struct.TValue, %struct.TValue* %6, i32 0, i32 1
  store i32 19, i32* %dest.tt, !tbaa !15
  br label %done

done:                                             ; preds = %conversion.ok, %entry
  %10 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %11 = getelementptr inbounds %struct.TValue, %struct.TValue* %10, i32 1
  %upvals = getelementptr inbounds %struct.LClosure, %struct.LClosure* %3, i32 0, i32 6, i32 0
  %12 = load %struct.UpVal*, %struct.UpVal** %upvals, !tbaa !8
  %v = getelementptr inbounds %struct.UpVal, %struct.UpVal* %12, i32 0, i32 0
  %13 = load %struct.TValue*, %struct.TValue** %v, !tbaa !19
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %13, %struct.TValue* %5, %struct.TValue* %11)
  %14 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %15 = getelementptr inbounds %struct.TValue, %struct.TValue* %14, i32 2
  %16 = getelementptr inbounds %struct.TValue, %struct.TValue* %5, i32 1
  %srcvalue = getelementptr inbounds %struct.TValue, %struct.TValue* %16, i32 0, i32 0, i32 0
  %destvalue = getelementptr inbounds %struct.TValue, %struct.TValue* %15, i32 0, i32 0, i32 0
  %17 = load double, double* %srcvalue, !tbaa !18
  store double %17, double* %destvalue, !tbaa !18
  %srctype = getelementptr inbounds %struct.TValue, %struct.TValue* %16, i32 0, i32 1
  %desttype = getelementptr inbounds %struct.TValue, %struct.TValue* %15, i32 0, i32 1
  %18 = load i32, i32* %srctype, !tbaa !15
  store i32 %18, i32* %desttype, !tbaa !15
  %19 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %20 = getelementptr inbounds %struct.TValue, %struct.TValue* %19, i32 3
  %srcvalue1 = getelementptr inbounds %struct.TValue, %struct.TValue* %19, i32 0, i32 0, i32 0
  %destvalue2 = getelementptr inbounds %struct.TValue, %struct.TValue* %20, i32 0, i32 0, i32 0
  %21 = load double, double* %srcvalue1, !tbaa !18
  store double %21, double* %destvalue2, !tbaa !18
  %srctype3 = getelementptr inbounds %struct.TValue, %struct.TValue* %19, i32 0, i32 1
  %desttype4 = getelementptr inbounds %struct.TValue, %struct.TValue* %20, i32 0, i32 1
  %22 = load i32, i32* %srctype3, !tbaa !15
  store i32 %22, i32* %desttype4, !tbaa !15
  %23 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %24 = getelementptr inbounds %struct.TValue, %struct.TValue* %23, i32 4
  %L.top = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %24, %struct.TValue** %L.top, !tbaa !21
  %25 = getelementptr inbounds %struct.TValue, %struct.TValue* %23, i32 1
  %26 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %25, i32 0)
  %27 = icmp eq i32 %26, 0
  br i1 %27, label %if.lua.function, label %if.not.lua.function

if.lua.function:                                  ; preds = %done
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done

if.not.lua.function:                              ; preds = %done
  %28 = icmp eq i32 %26, 1
  br i1 %28, label %if.C.function, label %op_call.done

if.C.function:                                    ; preds = %if.not.lua.function
  %ci_top = getelementptr inbounds %struct.CallInfo, %struct.CallInfo* %0, i32 0, i32 1
  %29 = load %struct.TValue*, %struct.TValue** %ci_top, !tbaa !23
  %L_top = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %29, %struct.TValue** %L_top, !tbaa !21
  br label %op_call.done

op_call.done:                                     ; preds = %if.C.function, %if.not.lua.function, %if.lua.function
  %30 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %31 = getelementptr inbounds %struct.TValue, %struct.TValue* %30, i32 1
  %32 = getelementptr inbounds %struct.TValue, %struct.TValue* %5, i32 2
  %upvals5 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %3, i32 0, i32 6, i32 0
  %33 = load %struct.UpVal*, %struct.UpVal** %upvals5, !tbaa !8
  %v6 = getelementptr inbounds %struct.UpVal, %struct.UpVal* %33, i32 0, i32 0
  %34 = load %struct.TValue*, %struct.TValue** %v6, !tbaa !19
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %34, %struct.TValue* %32, %struct.TValue* %31)
  %35 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %36 = getelementptr inbounds %struct.TValue, %struct.TValue* %35, i32 1
  %37 = getelementptr inbounds %struct.TValue, %struct.TValue* %35, i32 1
  %38 = getelementptr inbounds %struct.TValue, %struct.TValue* %5, i32 3
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %37, %struct.TValue* %38, %struct.TValue* %36)
  %39 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %40 = getelementptr inbounds %struct.TValue, %struct.TValue* %39, i32 2
  %L.top7 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %40, %struct.TValue** %L.top7, !tbaa !21
  %41 = getelementptr inbounds %struct.TValue, %struct.TValue* %39, i32 1
  %42 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %41, i32 1)
  %43 = icmp eq i32 %42, 0
  br i1 %43, label %if.lua.function8, label %if.not.lua.function.9

if.lua.function8:                                 ; preds = %op_call.done
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done.13

if.not.lua.function.9:                            ; preds = %op_call.done
  %44 = icmp eq i32 %42, 1
  br i1 %44, label %if.C.function10, label %op_call.done.13

if.C.function10:                                  ; preds = %if.not.lua.function.9
  %ci_top11 = getelementptr inbounds %struct.CallInfo, %struct.CallInfo* %0, i32 0, i32 1
  %45 = load %struct.TValue*, %struct.TValue** %ci_top11, !tbaa !23
  %L_top12 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %45, %struct.TValue** %L_top12, !tbaa !21
  br label %op_call.done.13

op_call.done.13:                                  ; preds = %if.C.function10, %if.not.lua.function.9, %if.lua.function8
  %46 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %47 = getelementptr inbounds %struct.TValue, %struct.TValue* %46, i32 2
  %upvals14 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %3, i32 0, i32 6, i32 1
  %48 = load %struct.UpVal*, %struct.UpVal** %upvals14, !tbaa !8
  %v15 = getelementptr inbounds %struct.UpVal, %struct.UpVal* %48, i32 0, i32 0
  %49 = load %struct.TValue*, %struct.TValue** %v15, !tbaa !19
  %srcvalue16 = getelementptr inbounds %struct.TValue, %struct.TValue* %49, i32 0, i32 0, i32 0
  %destvalue17 = getelementptr inbounds %struct.TValue, %struct.TValue* %47, i32 0, i32 0, i32 0
  %50 = load double, double* %srcvalue16, !tbaa !18
  store double %50, double* %destvalue17, !tbaa !18
  %srctype18 = getelementptr inbounds %struct.TValue, %struct.TValue* %49, i32 0, i32 1
  %desttype19 = getelementptr inbounds %struct.TValue, %struct.TValue* %47, i32 0, i32 1
  %51 = load i32, i32* %srctype18, !tbaa !15
  store i32 %51, i32* %desttype19, !tbaa !15
  %52 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %53 = getelementptr inbounds %struct.TValue, %struct.TValue* %52, i32 3
  %srcvalue20 = getelementptr inbounds %struct.TValue, %struct.TValue* %52, i32 0, i32 0, i32 0
  %destvalue21 = getelementptr inbounds %struct.TValue, %struct.TValue* %53, i32 0, i32 0, i32 0
  %54 = load double, double* %srcvalue20, !tbaa !18
  store double %54, double* %destvalue21, !tbaa !18
  %srctype22 = getelementptr inbounds %struct.TValue, %struct.TValue* %52, i32 0, i32 1
  %desttype23 = getelementptr inbounds %struct.TValue, %struct.TValue* %53, i32 0, i32 1
  %55 = load i32, i32* %srctype22, !tbaa !15
  store i32 %55, i32* %desttype23, !tbaa !15
  %56 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %57 = getelementptr inbounds %struct.TValue, %struct.TValue* %56, i32 4
  %srcvalue24 = getelementptr inbounds %struct.TValue, %struct.TValue* %56, i32 0, i32 0, i32 0
  %destvalue25 = getelementptr inbounds %struct.TValue, %struct.TValue* %57, i32 0, i32 0, i32 0
  %58 = load double, double* %srcvalue24, !tbaa !18
  store double %58, double* %destvalue25, !tbaa !18
  %srctype26 = getelementptr inbounds %struct.TValue, %struct.TValue* %56, i32 0, i32 1
  %desttype27 = getelementptr inbounds %struct.TValue, %struct.TValue* %57, i32 0, i32 1
  %59 = load i32, i32* %srctype26, !tbaa !15
  store i32 %59, i32* %desttype27, !tbaa !15
  %60 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %61 = getelementptr inbounds %struct.TValue, %struct.TValue* %60, i32 5
  %L.top28 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %61, %struct.TValue** %L.top28, !tbaa !21
  %62 = getelementptr inbounds %struct.TValue, %struct.TValue* %60, i32 2
  %63 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %62, i32 1)
  %64 = icmp eq i32 %63, 0
  br i1 %64, label %if.lua.function29, label %if.not.lua.function.30

if.lua.function29:                                ; preds = %op_call.done.13
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done.34

if.not.lua.function.30:                           ; preds = %op_call.done.13
  %65 = icmp eq i32 %63, 1
  br i1 %65, label %if.C.function31, label %op_call.done.34

if.C.function31:                                  ; preds = %if.not.lua.function.30
  %ci_top32 = getelementptr inbounds %struct.CallInfo, %struct.CallInfo* %0, i32 0, i32 1
  %66 = load %struct.TValue*, %struct.TValue** %ci_top32, !tbaa !23
  %L_top33 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %66, %struct.TValue** %L_top33, !tbaa !21
  br label %op_call.done.34

op_call.done.34:                                  ; preds = %if.C.function31, %if.not.lua.function.30, %if.lua.function29
  %67 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %68 = getelementptr inbounds %struct.TValue, %struct.TValue* %67, i32 3
  %69 = bitcast %struct.TValue* %67 to i64*
  %70 = load i64, i64* %69, !tbaa !18
  %71 = sitofp i64 %70 to double
  %72 = fdiv double 1.000000e+00, %71
  %73 = bitcast %struct.TValue* %68 to double*
  store double %72, double* %73, !tbaa !18
  %dest.tt35 = getelementptr inbounds %struct.TValue, %struct.TValue* %68, i32 0, i32 1
  store i32 3, i32* %dest.tt35, !tbaa !15
  %74 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %75 = getelementptr inbounds %struct.TValue, %struct.TValue* %74, i32 3
  %76 = getelementptr inbounds %struct.TValue, %struct.TValue* %74, i32 3
  %77 = bitcast %struct.TValue* %76 to double*
  %78 = load double, double* %77, !tbaa !18
  %79 = bitcast %struct.TValue* %74 to i64*
  %80 = load i64, i64* %79, !tbaa !18
  %81 = sitofp i64 %80 to double
  %82 = fdiv double %78, %81
  %83 = bitcast %struct.TValue* %75 to double*
  store double %82, double* %83, !tbaa !18
  %dest.tt36 = getelementptr inbounds %struct.TValue, %struct.TValue* %75, i32 0, i32 1
  store i32 3, i32* %dest.tt36, !tbaa !15
  call void @raviV_op_loadnil(%struct.CallInfo* %0, i32 4, i32 0)
  %84 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %85 = getelementptr inbounds %struct.TValue, %struct.TValue* %84, i32 4
  %86 = bitcast %struct.TValue* %85 to i64*
  store i64 0, i64* %86, !tbaa !18
  %dest.tt37 = getelementptr inbounds %struct.TValue, %struct.TValue* %85, i32 0, i32 1
  store i32 19, i32* %dest.tt37, !tbaa !15
  %87 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %88 = getelementptr inbounds %struct.TValue, %struct.TValue* %87, i32 5
  %89 = bitcast %struct.TValue* %88 to i64*
  store i64 1, i64* %89, !tbaa !18
  %dest.tt38 = getelementptr inbounds %struct.TValue, %struct.TValue* %88, i32 0, i32 1
  store i32 19, i32* %dest.tt38, !tbaa !15
  %90 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %91 = getelementptr inbounds %struct.TValue, %struct.TValue* %90, i32 6
  %srcvalue39 = getelementptr inbounds %struct.TValue, %struct.TValue* %90, i32 0, i32 0, i32 0
  %destvalue40 = getelementptr inbounds %struct.TValue, %struct.TValue* %91, i32 0, i32 0, i32 0
  %92 = load double, double* %srcvalue39, !tbaa !18
  store double %92, double* %destvalue40, !tbaa !18
  %srctype41 = getelementptr inbounds %struct.TValue, %struct.TValue* %90, i32 0, i32 1
  %desttype42 = getelementptr inbounds %struct.TValue, %struct.TValue* %91, i32 0, i32 1
  %93 = load i32, i32* %srctype41, !tbaa !15
  store i32 %93, i32* %desttype42, !tbaa !15
  %94 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %95 = getelementptr inbounds %struct.TValue, %struct.TValue* %94, i32 7
  %96 = bitcast %struct.TValue* %95 to i64*
  store i64 1, i64* %96, !tbaa !18
  %dest.tt43 = getelementptr inbounds %struct.TValue, %struct.TValue* %95, i32 0, i32 1
  store i32 19, i32* %dest.tt43, !tbaa !15
  %97 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %98 = getelementptr inbounds %struct.TValue, %struct.TValue* %97, i32 5
  %99 = getelementptr inbounds %struct.TValue, %struct.TValue* %97, i32 6
  %100 = bitcast %struct.TValue* %99 to i64*
  %101 = load i64, i64* %100, !tbaa !18
  %102 = bitcast %struct.TValue* %98 to i64*
  %103 = load i64, i64* %102, !tbaa !18
  %initv-pstep.i = sub nsw i64 %103, 1
  store i64 %initv-pstep.i, i64* %iidx, !tbaa !17
  store i64 %101, i64* %ilimit, !tbaa !17
  br label %forloop35_

forbody21_:                                       ; preds = %updatei.64
  %104 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %105 = getelementptr inbounds %struct.TValue, %struct.TValue* %104, i32 9
  %106 = getelementptr inbounds %struct.TValue, %struct.TValue* %104, i32 8
  %107 = bitcast %struct.TValue* %106 to i64*
  %108 = load i64, i64* %107, !tbaa !18
  %109 = sub nsw i64 %108, 1
  %110 = bitcast %struct.TValue* %105 to i64*
  store i64 %109, i64* %110, !tbaa !18
  %dest.tt44 = getelementptr inbounds %struct.TValue, %struct.TValue* %105, i32 0, i32 1
  store i32 19, i32* %dest.tt44, !tbaa !15
  %111 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %112 = getelementptr inbounds %struct.TValue, %struct.TValue* %111, i32 4
  %113 = getelementptr inbounds %struct.TValue, %struct.TValue* %111, i32 9
  %114 = bitcast %struct.TValue* %113 to i64*
  %115 = load i64, i64* %114, !tbaa !18
  %116 = bitcast %struct.TValue* %111 to i64*
  %117 = load i64, i64* %116, !tbaa !18
  %118 = mul nsw i64 %115, %117
  %119 = bitcast %struct.TValue* %112 to i64*
  store i64 %118, i64* %119, !tbaa !18
  %dest.tt45 = getelementptr inbounds %struct.TValue, %struct.TValue* %112, i32 0, i32 1
  store i32 19, i32* %dest.tt45, !tbaa !15
  %120 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %121 = getelementptr inbounds %struct.TValue, %struct.TValue* %120, i32 9
  %122 = bitcast %struct.TValue* %121 to i64*
  store i64 1, i64* %122, !tbaa !18
  %dest.tt46 = getelementptr inbounds %struct.TValue, %struct.TValue* %121, i32 0, i32 1
  store i32 19, i32* %dest.tt46, !tbaa !15
  %123 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %124 = getelementptr inbounds %struct.TValue, %struct.TValue* %123, i32 10
  %srcvalue47 = getelementptr inbounds %struct.TValue, %struct.TValue* %123, i32 0, i32 0, i32 0
  %destvalue48 = getelementptr inbounds %struct.TValue, %struct.TValue* %124, i32 0, i32 0, i32 0
  %125 = load double, double* %srcvalue47, !tbaa !18
  store double %125, double* %destvalue48, !tbaa !18
  %srctype49 = getelementptr inbounds %struct.TValue, %struct.TValue* %123, i32 0, i32 1
  %desttype50 = getelementptr inbounds %struct.TValue, %struct.TValue* %124, i32 0, i32 1
  %126 = load i32, i32* %srctype49, !tbaa !15
  store i32 %126, i32* %desttype50, !tbaa !15
  %127 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %128 = getelementptr inbounds %struct.TValue, %struct.TValue* %127, i32 11
  %129 = bitcast %struct.TValue* %128 to i64*
  store i64 1, i64* %129, !tbaa !18
  %dest.tt51 = getelementptr inbounds %struct.TValue, %struct.TValue* %128, i32 0, i32 1
  store i32 19, i32* %dest.tt51, !tbaa !15
  %130 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %131 = getelementptr inbounds %struct.TValue, %struct.TValue* %130, i32 9
  %132 = getelementptr inbounds %struct.TValue, %struct.TValue* %130, i32 10
  %133 = bitcast %struct.TValue* %132 to i64*
  %134 = load i64, i64* %133, !tbaa !18
  %135 = bitcast %struct.TValue* %131 to i64*
  %136 = load i64, i64* %135, !tbaa !18
  %initv-pstep.i54 = sub nsw i64 %136, 1
  store i64 %initv-pstep.i54, i64* %iidx53, !tbaa !17
  store i64 %134, i64* %ilimit52, !tbaa !17
  br label %forloop34_

forbody27_:                                       ; preds = %updatei
  %137 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %138 = getelementptr inbounds %struct.TValue, %struct.TValue* %137, i32 13
  %139 = getelementptr inbounds %struct.TValue, %struct.TValue* %137, i32 4
  %140 = bitcast %struct.TValue* %139 to i64*
  %141 = load i64, i64* %140, !tbaa !18
  %142 = getelementptr inbounds %struct.TValue, %struct.TValue* %137, i32 12
  %143 = bitcast %struct.TValue* %142 to i64*
  %144 = load i64, i64* %143, !tbaa !18
  %145 = add nsw i64 %141, %144
  %146 = bitcast %struct.TValue* %138 to i64*
  store i64 %145, i64* %146, !tbaa !18
  %dest.tt55 = getelementptr inbounds %struct.TValue, %struct.TValue* %138, i32 0, i32 1
  store i32 19, i32* %dest.tt55, !tbaa !15
  %147 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %148 = getelementptr inbounds %struct.TValue, %struct.TValue* %147, i32 14
  %149 = getelementptr inbounds %struct.TValue, %struct.TValue* %147, i32 8
  %150 = bitcast %struct.TValue* %149 to i64*
  %151 = load i64, i64* %150, !tbaa !18
  %152 = getelementptr inbounds %struct.TValue, %struct.TValue* %147, i32 12
  %153 = bitcast %struct.TValue* %152 to i64*
  %154 = load i64, i64* %153, !tbaa !18
  %155 = sub nsw i64 %151, %154
  %156 = bitcast %struct.TValue* %148 to i64*
  store i64 %155, i64* %156, !tbaa !18
  %dest.tt56 = getelementptr inbounds %struct.TValue, %struct.TValue* %148, i32 0, i32 1
  store i32 19, i32* %dest.tt56, !tbaa !15
  %157 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %158 = getelementptr inbounds %struct.TValue, %struct.TValue* %157, i32 14
  %159 = getelementptr inbounds %struct.TValue, %struct.TValue* %157, i32 3
  %160 = bitcast %struct.TValue* %159 to double*
  %161 = load double, double* %160, !tbaa !18
  %162 = getelementptr inbounds %struct.TValue, %struct.TValue* %157, i32 14
  %163 = bitcast %struct.TValue* %162 to i64*
  %164 = load i64, i64* %163, !tbaa !18
  %165 = sitofp i64 %164 to double
  %166 = fmul double %161, %165
  %167 = bitcast %struct.TValue* %158 to double*
  store double %166, double* %167, !tbaa !18
  %dest.tt57 = getelementptr inbounds %struct.TValue, %struct.TValue* %158, i32 0, i32 1
  store i32 3, i32* %dest.tt57, !tbaa !15
  %168 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %169 = getelementptr inbounds %struct.TValue, %struct.TValue* %168, i32 15
  %170 = getelementptr inbounds %struct.TValue, %struct.TValue* %168, i32 8
  %171 = bitcast %struct.TValue* %170 to i64*
  %172 = load i64, i64* %171, !tbaa !18
  %173 = getelementptr inbounds %struct.TValue, %struct.TValue* %168, i32 12
  %174 = bitcast %struct.TValue* %173 to i64*
  %175 = load i64, i64* %174, !tbaa !18
  %176 = add nsw i64 %172, %175
  %177 = bitcast %struct.TValue* %169 to i64*
  store i64 %176, i64* %177, !tbaa !18
  %dest.tt58 = getelementptr inbounds %struct.TValue, %struct.TValue* %169, i32 0, i32 1
  store i32 19, i32* %dest.tt58, !tbaa !15
  %178 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %179 = getelementptr inbounds %struct.TValue, %struct.TValue* %178, i32 15
  %180 = getelementptr inbounds %struct.TValue, %struct.TValue* %178, i32 15
  %181 = bitcast %struct.TValue* %180 to i64*
  %182 = load i64, i64* %181, !tbaa !18
  %183 = sub nsw i64 %182, 2
  %184 = bitcast %struct.TValue* %179 to i64*
  store i64 %183, i64* %184, !tbaa !18
  %dest.tt59 = getelementptr inbounds %struct.TValue, %struct.TValue* %179, i32 0, i32 1
  store i32 19, i32* %dest.tt59, !tbaa !15
  %185 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %186 = getelementptr inbounds %struct.TValue, %struct.TValue* %185, i32 14
  %187 = getelementptr inbounds %struct.TValue, %struct.TValue* %185, i32 14
  %188 = bitcast %struct.TValue* %187 to double*
  %189 = load double, double* %188, !tbaa !18
  %190 = getelementptr inbounds %struct.TValue, %struct.TValue* %185, i32 15
  %191 = bitcast %struct.TValue* %190 to i64*
  %192 = load i64, i64* %191, !tbaa !18
  %193 = sitofp i64 %192 to double
  %194 = fmul double %189, %193
  %195 = bitcast %struct.TValue* %186 to double*
  store double %194, double* %195, !tbaa !18
  %dest.tt60 = getelementptr inbounds %struct.TValue, %struct.TValue* %186, i32 0, i32 1
  store i32 3, i32* %dest.tt60, !tbaa !15
  %196 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %197 = getelementptr inbounds %struct.TValue, %struct.TValue* %196, i32 2
  %198 = getelementptr inbounds %struct.TValue, %struct.TValue* %196, i32 13
  %199 = getelementptr inbounds %struct.TValue, %struct.TValue* %196, i32 14
  call void @luaV_settable(%struct.lua_State* %L, %struct.TValue* %197, %struct.TValue* %198, %struct.TValue* %199)
  br label %forloop34_

forloop34_:                                       ; preds = %forbody27_, %forbody21_
  %200 = load i64, i64* %iidx53, !tbaa !17
  %next.idx = add nsw i64 1, %200
  store i64 %next.idx, i64* %iidx53, !tbaa !17
  %201 = load i64, i64* %ilimit52, !tbaa !17
  %idx.gt.limit = icmp sgt i64 %next.idx, %201
  br i1 %idx.gt.limit, label %exit_iforloop, label %updatei

updatei:                                          ; preds = %forloop34_
  %202 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %203 = getelementptr inbounds %struct.TValue, %struct.TValue* %202, i32 12
  %204 = load i64, i64* %iidx53, !tbaa !17
  %205 = bitcast %struct.TValue* %203 to i64*
  store i64 %204, i64* %205, !tbaa !18
  %dest.tt61 = getelementptr inbounds %struct.TValue, %struct.TValue* %203, i32 0, i32 1
  store i32 19, i32* %dest.tt61, !tbaa !15
  br label %forbody27_

exit_iforloop:                                    ; preds = %forloop34_
  br label %forloop35_

forloop35_:                                       ; preds = %exit_iforloop, %op_call.done.34
  %206 = load i64, i64* %iidx, !tbaa !17
  %next.idx62 = add nsw i64 1, %206
  store i64 %next.idx62, i64* %iidx, !tbaa !17
  %207 = load i64, i64* %ilimit, !tbaa !17
  %idx.gt.limit63 = icmp sgt i64 %next.idx62, %207
  br i1 %idx.gt.limit63, label %exit_iforloop.66, label %updatei.64

updatei.64:                                       ; preds = %forloop35_
  %208 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %209 = getelementptr inbounds %struct.TValue, %struct.TValue* %208, i32 8
  %210 = load i64, i64* %iidx, !tbaa !17
  %211 = bitcast %struct.TValue* %209 to i64*
  store i64 %210, i64* %211, !tbaa !18
  %dest.tt65 = getelementptr inbounds %struct.TValue, %struct.TValue* %209, i32 0, i32 1
  store i32 19, i32* %dest.tt65, !tbaa !15
  br label %forbody21_

exit_iforloop.66:                                 ; preds = %forloop35_
  %212 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %213 = getelementptr inbounds %struct.TValue, %struct.TValue* %212, i32 5
  %214 = getelementptr inbounds %struct.TValue, %struct.TValue* %5, i32 2
  %upvals67 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %3, i32 0, i32 6, i32 0
  %215 = load %struct.UpVal*, %struct.UpVal** %upvals67, !tbaa !8
  %v68 = getelementptr inbounds %struct.UpVal, %struct.UpVal* %215, i32 0, i32 0
  %216 = load %struct.TValue*, %struct.TValue** %v68, !tbaa !19
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %216, %struct.TValue* %214, %struct.TValue* %213)
  %217 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %218 = getelementptr inbounds %struct.TValue, %struct.TValue* %217, i32 5
  %219 = getelementptr inbounds %struct.TValue, %struct.TValue* %217, i32 5
  %220 = getelementptr inbounds %struct.TValue, %struct.TValue* %5, i32 3
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %219, %struct.TValue* %220, %struct.TValue* %218)
  %221 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %222 = getelementptr inbounds %struct.TValue, %struct.TValue* %221, i32 6
  %L.top69 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %222, %struct.TValue** %L.top69, !tbaa !21
  %223 = getelementptr inbounds %struct.TValue, %struct.TValue* %221, i32 5
  %224 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %223, i32 1)
  %225 = icmp eq i32 %224, 0
  br i1 %225, label %if.lua.function70, label %if.not.lua.function.71

if.lua.function70:                                ; preds = %exit_iforloop.66
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done.75

if.not.lua.function.71:                           ; preds = %exit_iforloop.66
  %226 = icmp eq i32 %224, 1
  br i1 %226, label %if.C.function72, label %op_call.done.75

if.C.function72:                                  ; preds = %if.not.lua.function.71
  %ci_top73 = getelementptr inbounds %struct.CallInfo, %struct.CallInfo* %0, i32 0, i32 1
  %227 = load %struct.TValue*, %struct.TValue** %ci_top73, !tbaa !23
  %L_top74 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %227, %struct.TValue** %L_top74, !tbaa !21
  br label %op_call.done.75

op_call.done.75:                                  ; preds = %if.C.function72, %if.not.lua.function.71, %if.lua.function70
  %228 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %229 = getelementptr inbounds %struct.TValue, %struct.TValue* %228, i32 6
  %upvals76 = getelementptr inbounds %struct.LClosure, %struct.LClosure* %3, i32 0, i32 6, i32 0
  %230 = load %struct.UpVal*, %struct.UpVal** %upvals76, !tbaa !8
  %v77 = getelementptr inbounds %struct.UpVal, %struct.UpVal* %230, i32 0, i32 0
  %231 = load %struct.TValue*, %struct.TValue** %v77, !tbaa !19
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %231, %struct.TValue* %5, %struct.TValue* %229)
  %232 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %233 = getelementptr inbounds %struct.TValue, %struct.TValue* %232, i32 7
  %234 = getelementptr inbounds %struct.TValue, %struct.TValue* %5, i32 7
  %srcvalue78 = getelementptr inbounds %struct.TValue, %struct.TValue* %234, i32 0, i32 0, i32 0
  %destvalue79 = getelementptr inbounds %struct.TValue, %struct.TValue* %233, i32 0, i32 0, i32 0
  %235 = load double, double* %srcvalue78, !tbaa !18
  store double %235, double* %destvalue79, !tbaa !18
  %srctype80 = getelementptr inbounds %struct.TValue, %struct.TValue* %234, i32 0, i32 1
  %desttype81 = getelementptr inbounds %struct.TValue, %struct.TValue* %233, i32 0, i32 1
  %236 = load i32, i32* %srctype80, !tbaa !15
  store i32 %236, i32* %desttype81, !tbaa !15
  %237 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %238 = getelementptr inbounds %struct.TValue, %struct.TValue* %237, i32 8
  %239 = getelementptr inbounds %struct.TValue, %struct.TValue* %237, i32 5
  %240 = getelementptr inbounds %struct.TValue, %struct.TValue* %237, i32 1
  %value.tt.ptr82 = getelementptr inbounds %struct.TValue, %struct.TValue* %239, i32 0, i32 1
  %value.tt83 = load i32, i32* %value.tt.ptr82, !tbaa !15
  %value.tt.ptr84 = getelementptr inbounds %struct.TValue, %struct.TValue* %240, i32 0, i32 1
  %value.tt85 = load i32, i32* %value.tt.ptr84, !tbaa !15
  %rb.is.integer = icmp eq i32 %value.tt83, 19
  %rc.is.integer = icmp eq i32 %value.tt85, 19
  %241 = and i1 %rb.is.integer, %rc.is.integer
  br i1 %241, label %if.integer, label %if.not.integer.87

if.integer:                                       ; preds = %op_call.done.75
  %242 = bitcast %struct.TValue* %239 to i64*
  %243 = load i64, i64* %242, !tbaa !18
  %244 = bitcast %struct.TValue* %240 to i64*
  %245 = load i64, i64* %244, !tbaa !18
  %246 = sub nsw i64 %243, %245
  %247 = bitcast %struct.TValue* %238 to i64*
  store i64 %246, i64* %247, !tbaa !18
  %dest.tt86 = getelementptr inbounds %struct.TValue, %struct.TValue* %238, i32 0, i32 1
  store i32 19, i32* %dest.tt86, !tbaa !15
  br label %done.89

if.not.integer.87:                                ; preds = %op_call.done.75
  %rb.is.float = icmp eq i32 %value.tt83, 3
  br i1 %rb.is.float, label %load.rb, label %convert.rb

convert.rb:                                       ; preds = %if.not.integer.87
  %248 = call i32 @luaV_tonumber_(%struct.TValue* %239, double* %nb)
  %rb.float.ok = icmp eq i32 %248, 1
  br i1 %rb.float.ok, label %test.rc, label %try_meta

load.rb:                                          ; preds = %if.not.integer.87
  %249 = bitcast %struct.TValue* %239 to double*
  %250 = load double, double* %249, !tbaa !18
  store double %250, double* %nb, !tbaa !17
  br label %test.rc

test.rc:                                          ; preds = %load.rb, %convert.rb
  %rc.is.float = icmp eq i32 %value.tt85, 3
  br i1 %rc.is.float, label %load.rc, label %convert.rc

convert.rc:                                       ; preds = %test.rc
  %251 = call i32 @luaV_tonumber_(%struct.TValue* %240, double* %nc)
  %rc.float.ok = icmp eq i32 %251, 1
  br i1 %rc.float.ok, label %float.op, label %try_meta

load.rc:                                          ; preds = %test.rc
  %252 = bitcast %struct.TValue* %240 to double*
  %253 = load double, double* %252, !tbaa !18
  store double %253, double* %nc, !tbaa !17
  br label %float.op

float.op:                                         ; preds = %load.rc, %convert.rc
  %254 = load double, double* %nb, !tbaa !17
  %255 = load double, double* %nc, !tbaa !17
  %256 = fsub double %254, %255
  %257 = bitcast %struct.TValue* %238 to double*
  store double %256, double* %257, !tbaa !18
  %dest.tt88 = getelementptr inbounds %struct.TValue, %struct.TValue* %238, i32 0, i32 1
  store i32 3, i32* %dest.tt88, !tbaa !15
  br label %done.89

try_meta:                                         ; preds = %convert.rc, %convert.rb
  call void @luaT_trybinTM(%struct.lua_State* %L, %struct.TValue* %239, %struct.TValue* %240, %struct.TValue* %238, i32 7)
  br label %done.89

done.89:                                          ; preds = %try_meta, %float.op, %if.integer
  %258 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %259 = getelementptr inbounds %struct.TValue, %struct.TValue* %258, i32 9
  %L.top90 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %259, %struct.TValue** %L.top90, !tbaa !21
  %260 = getelementptr inbounds %struct.TValue, %struct.TValue* %258, i32 6
  %261 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %260, i32 0)
  %262 = icmp eq i32 %261, 0
  br i1 %262, label %if.lua.function91, label %if.not.lua.function.92

if.lua.function91:                                ; preds = %done.89
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done.96

if.not.lua.function.92:                           ; preds = %done.89
  %263 = icmp eq i32 %261, 1
  br i1 %263, label %if.C.function93, label %op_call.done.96

if.C.function93:                                  ; preds = %if.not.lua.function.92
  %ci_top94 = getelementptr inbounds %struct.CallInfo, %struct.CallInfo* %0, i32 0, i32 1
  %264 = load %struct.TValue*, %struct.TValue** %ci_top94, !tbaa !23
  %L_top95 = getelementptr inbounds %struct.lua_State, %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %264, %struct.TValue** %L_top95, !tbaa !21
  br label %op_call.done.96

op_call.done.96:                                  ; preds = %if.C.function93, %if.not.lua.function.92, %if.lua.function91
  %265 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %266 = getelementptr inbounds %struct.TValue, %struct.TValue* %265, i32 2
  %sizep = getelementptr inbounds %struct.Proto, %struct.Proto* %4, i32 0, i32 10
  %267 = load i32, i32* %sizep, !tbaa !24
  %268 = icmp sgt i32 %267, 0
  br i1 %268, label %if.then, label %if.else

if.then:                                          ; preds = %op_call.done.96
  call void @luaF_close(%struct.lua_State* %L, %struct.TValue* %265)
  br label %if.else

if.else:                                          ; preds = %if.then, %op_call.done.96
  %269 = call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %266, i32 1)
  ret i32 1

return:                                           ; No predecessors!
  %270 = load %struct.TValue*, %struct.TValue** %base, !tbaa !14
  %sizep97 = getelementptr inbounds %struct.Proto, %struct.Proto* %4, i32 0, i32 10
  %271 = load i32, i32* %sizep97, !tbaa !24
  %272 = icmp sgt i32 %271, 0
  br i1 %272, label %if.then98, label %if.else.99

if.then98:                                        ; preds = %return
  call void @luaF_close(%struct.lua_State* %L, %struct.TValue* %270)
  br label %if.else.99

if.else.99:                                       ; preds = %if.then98, %return
  %273 = call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %270, i32 0)
  ret i32 1
}

; Function Attrs: nounwind
declare void @luaT_trybinTM(%struct.lua_State*, %struct.TValue*, %struct.TValue*, %struct.TValue*, i32) #0

; Function Attrs: nounwind
declare void @luaD_call(%struct.lua_State*, %struct.TValue*, i32, i32) #0

; Function Attrs: nounwind
declare i32 @luaD_poscall(%struct.lua_State*, %struct.TValue*, i32) #0

; Function Attrs: nounwind
declare i32 @luaD_precall(%struct.lua_State*, %struct.TValue*, i32) #0

; Function Attrs: nounwind
declare void @luaF_close(%struct.lua_State*, %struct.TValue*) #0

; Function Attrs: noreturn nounwind
declare void @luaG_runerror(%struct.lua_State*, i8*, ...) #1

; Function Attrs: nounwind
declare i32 @luaV_equalobj(%struct.lua_State*, %struct.TValue*, %struct.TValue*) #0

; Function Attrs: nounwind
declare i32 @luaV_lessthan(%struct.lua_State*, %struct.TValue*, %struct.TValue*) #0

; Function Attrs: nounwind
declare i32 @luaV_lessequal(%struct.lua_State*, %struct.TValue*, %struct.TValue*) #0

; Function Attrs: nounwind
declare i32 @luaV_forlimit(%struct.TValue*, i64*, i64, i32*) #0

; Function Attrs: nounwind
declare i32 @luaV_tonumber_(%struct.TValue*, double*) #0

; Function Attrs: nounwind
declare i32 @luaV_tointeger_(%struct.TValue*, i64*) #0

; Function Attrs: nounwind
declare void @luaV_execute(%struct.lua_State*) #0

; Function Attrs: nounwind
declare void @luaV_settable(%struct.lua_State*, %struct.TValue*, %struct.TValue*, %struct.TValue*) #0

; Function Attrs: nounwind
declare void @luaV_gettable(%struct.lua_State*, %struct.TValue*, %struct.TValue*, %struct.TValue*) #0

; Function Attrs: nounwind
declare void @raviV_op_loadnil(%struct.CallInfo*, i32, i32) #0

; Function Attrs: nounwind
declare void @raviV_op_newarrayint(%struct.lua_State*, %struct.CallInfo*, %struct.TValue*) #0

; Function Attrs: nounwind
declare void @raviV_op_newarrayfloat(%struct.lua_State*, %struct.CallInfo*, %struct.TValue*) #0

; Function Attrs: nounwind
declare void @raviV_op_newtable(%struct.lua_State*, %struct.CallInfo*, %struct.TValue*, i32, i32) #0

; Function Attrs: nounwind
declare void @raviV_op_setlist(%struct.lua_State*, %struct.CallInfo*, %struct.TValue*, i32, i32) #0

; Function Attrs: nounwind
declare i64 @luaV_mod(%struct.lua_State*, i64, i64) #0

; Function Attrs: nounwind
declare i64 @luaV_div(%struct.lua_State*, i64, i64) #0

; Function Attrs: nounwind
declare void @luaV_objlen(%struct.lua_State*, %struct.TValue*, %struct.TValue*) #0

; Function Attrs: nounwind
declare void @luaC_upvalbarrier_(%struct.lua_State*, %struct.UpVal*) #0

; Function Attrs: nounwind
declare void @raviV_op_concat(%struct.lua_State*, %struct.CallInfo*, i32, i32, i32) #0

; Function Attrs: nounwind
declare void @raviV_op_closure(%struct.lua_State*, %struct.CallInfo*, %struct.LClosure*, i32, i32) #0

; Function Attrs: nounwind
declare void @raviV_op_vararg(%struct.lua_State*, %struct.CallInfo*, %struct.LClosure*, i32, i32) #0

; Function Attrs: nounwind
declare void @raviH_set_int(%struct.lua_State*, %struct.Table*, i64, i64) #0

; Function Attrs: nounwind
declare void @raviH_set_float(%struct.lua_State*, %struct.Table*, i64, double) #0

; Function Attrs: nounwind
declare void @raviV_op_shl(%struct.lua_State*, %struct.TValue*, %struct.TValue*, %struct.TValue*) #0

; Function Attrs: nounwind
declare void @raviV_op_shr(%struct.lua_State*, %struct.TValue*, %struct.TValue*, %struct.TValue*) #0

declare i32 @printf(i8*, ...)

declare double @fmod(double, double)

declare double @pow(double, double)

declare double @floor(double)

attributes #0 = { nounwind }
attributes #1 = { noreturn nounwind }

!0 = !{!"CallInfo", !1, i64 0, !1, i64 4, !1, i64 8, !1, i64 12, !4, i64 16, !5, i64 32, !6, i64 40, !2, i64 42, !2, i64 43}
!1 = !{!"any pointer", !2, i64 0}
!2 = !{!"omnipotent char", !3, i64 0}
!3 = !{!"Simple C / C++ TBAA"}
!4 = !{!"CallInfo_l", !1, i64 0, !1, i64 4, !5, i64 8}
!5 = !{!"long long", !2, i64 0}
!6 = !{!"short", !2, i64 0}
!7 = !{!0, !1, i64 0}
!8 = !{!1, !1, i64 0}
!9 = !{!10, !1, i64 12}
!10 = !{!"LClosure", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !1, i64 8, !1, i64 12, !1, i64 16}
!11 = !{!12, !1, i64 44}
!12 = !{!"Proto", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !2, i64 7, !2, i64 8, !13, i64 12, !13, i64 16, !13, i64 20, !13, i64 24, !13, i64 28, !13, i64 32, !13, i64 36, !13, i64 40, !1, i64 44, !1, i64 48, !1, i64 52, !1, i64 56, !1, i64 60, !1, i64 64, !1, i64 68, !1, i64 72, !1, i64 76, !2, i64 80}
!13 = !{!"int", !2, i64 0}
!14 = !{!0, !1, i64 16}
!15 = !{!16, !13, i64 8}
!16 = !{!"TValue", !5, i64 0, !13, i64 8}
!17 = !{!5, !5, i64 0}
!18 = !{!16, !5, i64 0}
!19 = !{!20, !1, i64 0}
!20 = !{!"UpVal", !1, i64 0, !5, i64 8, !16, i64 16}
!21 = !{!22, !1, i64 8}
!22 = !{!"lua_State", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !1, i64 8, !1, i64 12, !1, i64 16, !1, i64 20, !1, i64 24, !1, i64 28, !1, i64 32, !1, i64 36, !1, i64 40, !1, i64 44, !0, i64 48, !1, i64 92, !5, i64 96, !13, i64 104, !13, i64 108, !13, i64 112, !6, i64 114, !6, i64 116, !2, i64 118, !2, i64 119}
!23 = !{!0, !1, i64 4}
!24 = !{!12, !13, i64 28}
