; ModuleID = 'ravi_module_ravif1'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc-elf"

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
  %L_ci = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 6
  %0 = load %struct.CallInfo** %L_ci, !tbaa !0
  %base = getelementptr inbounds %struct.CallInfo* %0, i32 0, i32 4, i32 0
  %1 = bitcast %struct.CallInfo* %0 to %struct.LClosure***
  %2 = load %struct.LClosure*** %1, !tbaa !7
  %3 = load %struct.LClosure** %2, !tbaa !8
  %Proto = getelementptr inbounds %struct.LClosure* %3, i32 0, i32 5
  %4 = load %struct.Proto** %Proto, !tbaa !9
  %k = getelementptr inbounds %struct.Proto* %4, i32 0, i32 14
  %5 = load %struct.TValue** %k, !tbaa !11
  %6 = load %struct.TValue** %base, !tbaa !14
  %value.tt.ptr = getelementptr inbounds %struct.TValue* %6, i32 0, i32 1
  %value.tt = load i32* %value.tt.ptr, !tbaa !15
  %is.not.integer = icmp ne i32 %value.tt, 19
  br i1 %is.not.integer, label %if.not.integer, label %done

if.not.integer:                                   ; preds = %entry
  %7 = call i32 @luaV_tointeger_(%struct.TValue* %6, i64* %i)
  %int.conversion.failed = icmp eq i32 %7, 0
  br i1 %int.conversion.failed, label %if.conversion.failed, label %conversion.ok

if.conversion.failed:                             ; preds = %if.not.integer
  call void (%struct.lua_State*, i8*, ...)* @luaG_runerror(%struct.lua_State* %L, i8* getelementptr inbounds ([17 x i8]* @0, i32 0, i32 0))
  br label %conversion.ok

conversion.ok:                                    ; preds = %if.conversion.failed, %if.not.integer
  %8 = load i64* %i, !tbaa !17
  %9 = bitcast %struct.TValue* %6 to i64*
  store i64 %8, i64* %9, !tbaa !18
  %dest.tt = getelementptr inbounds %struct.TValue* %6, i32 0, i32 1
  store i32 19, i32* %dest.tt, !tbaa !15
  br label %done

done:                                             ; preds = %conversion.ok, %entry
  %sunkaddr = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr100 = add i64 %sunkaddr, 32
  %sunkaddr101 = inttoptr i64 %sunkaddr100 to %struct.TValue**
  %10 = load %struct.TValue** %sunkaddr101, !tbaa !14
  %11 = getelementptr inbounds %struct.TValue* %10, i32 1
  %upvals = getelementptr inbounds %struct.LClosure* %3, i32 0, i32 6, i32 0
  %12 = load %struct.UpVal** %upvals, !tbaa !8
  %v102 = bitcast %struct.UpVal* %12 to %struct.TValue**
  %13 = load %struct.TValue** %v102, !tbaa !19
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %13, %struct.TValue* %5, %struct.TValue* %11)
  %14 = load %struct.TValue** %sunkaddr101, !tbaa !14
  %15 = getelementptr inbounds %struct.TValue* %14, i32 2
  %16 = getelementptr inbounds %struct.TValue* %5, i32 1
  %srcvalue103 = bitcast %struct.TValue* %16 to double*
  %destvalue104 = bitcast %struct.TValue* %15 to double*
  %17 = load double* %srcvalue103, !tbaa !18
  store double %17, double* %destvalue104, !tbaa !18
  %srctype = getelementptr inbounds %struct.TValue* %16, i32 0, i32 1
  %desttype = getelementptr inbounds %struct.TValue* %15, i32 0, i32 1
  %18 = load i32* %srctype, !tbaa !15
  store i32 %18, i32* %desttype, !tbaa !15
  %19 = load %struct.TValue** %sunkaddr101, !tbaa !14
  %20 = getelementptr inbounds %struct.TValue* %19, i32 3
  %srcvalue1105 = bitcast %struct.TValue* %19 to double*
  %destvalue2106 = bitcast %struct.TValue* %20 to double*
  %21 = load double* %srcvalue1105, !tbaa !18
  store double %21, double* %destvalue2106, !tbaa !18
  %srctype3 = getelementptr inbounds %struct.TValue* %19, i32 0, i32 1
  %desttype4 = getelementptr inbounds %struct.TValue* %20, i32 0, i32 1
  %22 = load i32* %srctype3, !tbaa !15
  store i32 %22, i32* %desttype4, !tbaa !15
  %23 = load %struct.TValue** %sunkaddr101, !tbaa !14
  %24 = getelementptr inbounds %struct.TValue* %23, i32 4
  %L.top = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %24, %struct.TValue** %L.top, !tbaa !21
  %25 = getelementptr inbounds %struct.TValue* %23, i32 1
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
  %ci_top = getelementptr inbounds %struct.CallInfo* %0, i32 0, i32 1
  %29 = load %struct.TValue** %ci_top, !tbaa !23
  %L_top = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %29, %struct.TValue** %L_top, !tbaa !21
  br label %op_call.done

op_call.done:                                     ; preds = %if.C.function, %if.not.lua.function, %if.lua.function
  %sunkaddr107 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr108 = add i64 %sunkaddr107, 32
  %sunkaddr109 = inttoptr i64 %sunkaddr108 to %struct.TValue**
  %30 = load %struct.TValue** %sunkaddr109, !tbaa !14
  %31 = getelementptr inbounds %struct.TValue* %30, i32 1
  %32 = getelementptr inbounds %struct.TValue* %5, i32 2
  %upvals5 = getelementptr inbounds %struct.LClosure* %3, i32 0, i32 6, i32 0
  %33 = load %struct.UpVal** %upvals5, !tbaa !8
  %v6110 = bitcast %struct.UpVal* %33 to %struct.TValue**
  %34 = load %struct.TValue** %v6110, !tbaa !19
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %34, %struct.TValue* %32, %struct.TValue* %31)
  %35 = load %struct.TValue** %sunkaddr109, !tbaa !14
  %36 = getelementptr inbounds %struct.TValue* %35, i32 1
  %37 = getelementptr inbounds %struct.TValue* %35, i32 1
  %38 = getelementptr inbounds %struct.TValue* %5, i32 3
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %37, %struct.TValue* %38, %struct.TValue* %36)
  %39 = load %struct.TValue** %sunkaddr109, !tbaa !14
  %40 = getelementptr inbounds %struct.TValue* %39, i32 2
  %L.top7 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %40, %struct.TValue** %L.top7, !tbaa !21
  %41 = getelementptr inbounds %struct.TValue* %39, i32 1
  %42 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %41, i32 1)
  %43 = icmp eq i32 %42, 0
  br i1 %43, label %if.lua.function8, label %if.not.lua.function9

if.lua.function8:                                 ; preds = %op_call.done
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done13

if.not.lua.function9:                             ; preds = %op_call.done
  %44 = icmp eq i32 %42, 1
  br i1 %44, label %if.C.function10, label %op_call.done13

if.C.function10:                                  ; preds = %if.not.lua.function9
  %ci_top11 = getelementptr inbounds %struct.CallInfo* %0, i32 0, i32 1
  %45 = load %struct.TValue** %ci_top11, !tbaa !23
  %L_top12 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %45, %struct.TValue** %L_top12, !tbaa !21
  br label %op_call.done13

op_call.done13:                                   ; preds = %if.C.function10, %if.not.lua.function9, %if.lua.function8
  %sunkaddr111 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr112 = add i64 %sunkaddr111, 32
  %sunkaddr113 = inttoptr i64 %sunkaddr112 to %struct.TValue**
  %46 = load %struct.TValue** %sunkaddr113, !tbaa !14
  %47 = getelementptr inbounds %struct.TValue* %46, i32 2
  %upvals14 = getelementptr inbounds %struct.LClosure* %3, i32 0, i32 6, i32 1
  %48 = load %struct.UpVal** %upvals14, !tbaa !8
  %v15114 = bitcast %struct.UpVal* %48 to %struct.TValue**
  %49 = load %struct.TValue** %v15114, !tbaa !19
  %srcvalue16115 = bitcast %struct.TValue* %49 to double*
  %destvalue17116 = bitcast %struct.TValue* %47 to double*
  %50 = load double* %srcvalue16115, !tbaa !18
  store double %50, double* %destvalue17116, !tbaa !18
  %srctype18 = getelementptr inbounds %struct.TValue* %49, i32 0, i32 1
  %desttype19 = getelementptr inbounds %struct.TValue* %47, i32 0, i32 1
  %51 = load i32* %srctype18, !tbaa !15
  store i32 %51, i32* %desttype19, !tbaa !15
  %52 = load %struct.TValue** %sunkaddr113, !tbaa !14
  %53 = getelementptr inbounds %struct.TValue* %52, i32 3
  %srcvalue20117 = bitcast %struct.TValue* %52 to double*
  %destvalue21118 = bitcast %struct.TValue* %53 to double*
  %54 = load double* %srcvalue20117, !tbaa !18
  store double %54, double* %destvalue21118, !tbaa !18
  %srctype22 = getelementptr inbounds %struct.TValue* %52, i32 0, i32 1
  %desttype23 = getelementptr inbounds %struct.TValue* %53, i32 0, i32 1
  %55 = load i32* %srctype22, !tbaa !15
  store i32 %55, i32* %desttype23, !tbaa !15
  %56 = load %struct.TValue** %sunkaddr113, !tbaa !14
  %57 = getelementptr inbounds %struct.TValue* %56, i32 4
  %srcvalue24119 = bitcast %struct.TValue* %56 to double*
  %destvalue25120 = bitcast %struct.TValue* %57 to double*
  %58 = load double* %srcvalue24119, !tbaa !18
  store double %58, double* %destvalue25120, !tbaa !18
  %srctype26 = getelementptr inbounds %struct.TValue* %56, i32 0, i32 1
  %desttype27 = getelementptr inbounds %struct.TValue* %57, i32 0, i32 1
  %59 = load i32* %srctype26, !tbaa !15
  store i32 %59, i32* %desttype27, !tbaa !15
  %60 = load %struct.TValue** %sunkaddr113, !tbaa !14
  %61 = getelementptr inbounds %struct.TValue* %60, i32 5
  %L.top28 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %61, %struct.TValue** %L.top28, !tbaa !21
  %62 = getelementptr inbounds %struct.TValue* %60, i32 2
  %63 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %62, i32 1)
  %64 = icmp eq i32 %63, 0
  br i1 %64, label %if.lua.function29, label %if.not.lua.function30

if.lua.function29:                                ; preds = %op_call.done13
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done34

if.not.lua.function30:                            ; preds = %op_call.done13
  %65 = icmp eq i32 %63, 1
  br i1 %65, label %if.C.function31, label %op_call.done34

if.C.function31:                                  ; preds = %if.not.lua.function30
  %ci_top32 = getelementptr inbounds %struct.CallInfo* %0, i32 0, i32 1
  %66 = load %struct.TValue** %ci_top32, !tbaa !23
  %L_top33 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %66, %struct.TValue** %L_top33, !tbaa !21
  br label %op_call.done34

op_call.done34:                                   ; preds = %if.C.function31, %if.not.lua.function30, %if.lua.function29
  %sunkaddr121 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr122 = add i64 %sunkaddr121, 32
  %sunkaddr123 = inttoptr i64 %sunkaddr122 to %struct.TValue**
  %67 = load %struct.TValue** %sunkaddr123, !tbaa !14
  %68 = getelementptr inbounds %struct.TValue* %67, i32 3
  %69 = bitcast %struct.TValue* %67 to i64*
  %70 = load i64* %69, !tbaa !18
  %71 = sitofp i64 %70 to double
  %72 = fdiv double 1.000000e+00, %71
  %73 = bitcast %struct.TValue* %68 to double*
  store double %72, double* %73, !tbaa !18
  %dest.tt35 = getelementptr inbounds %struct.TValue* %68, i32 0, i32 1
  store i32 3, i32* %dest.tt35, !tbaa !15
  %74 = load %struct.TValue** %sunkaddr123, !tbaa !14
  %75 = getelementptr inbounds %struct.TValue* %74, i32 3
  %76 = getelementptr inbounds %struct.TValue* %74, i32 3
  %77 = bitcast %struct.TValue* %76 to double*
  %78 = load double* %77, !tbaa !18
  %79 = bitcast %struct.TValue* %74 to i64*
  %80 = load i64* %79, !tbaa !18
  %81 = sitofp i64 %80 to double
  %82 = fdiv double %78, %81
  %83 = bitcast %struct.TValue* %75 to double*
  store double %82, double* %83, !tbaa !18
  %dest.tt36 = getelementptr inbounds %struct.TValue* %75, i32 0, i32 1
  store i32 3, i32* %dest.tt36, !tbaa !15
  call void @raviV_op_loadnil(%struct.CallInfo* %0, i32 4, i32 0)
  %84 = load %struct.TValue** %sunkaddr123, !tbaa !14
  %85 = getelementptr inbounds %struct.TValue* %84, i32 4
  %86 = bitcast %struct.TValue* %85 to i64*
  store i64 0, i64* %86, !tbaa !18
  %dest.tt37 = getelementptr inbounds %struct.TValue* %85, i32 0, i32 1
  store i32 19, i32* %dest.tt37, !tbaa !15
  %87 = load %struct.TValue** %sunkaddr123, !tbaa !14
  %88 = getelementptr inbounds %struct.TValue* %87, i32 5
  %89 = bitcast %struct.TValue* %88 to i64*
  store i64 1, i64* %89, !tbaa !18
  %dest.tt38 = getelementptr inbounds %struct.TValue* %88, i32 0, i32 1
  store i32 19, i32* %dest.tt38, !tbaa !15
  %90 = load %struct.TValue** %sunkaddr123, !tbaa !14
  %91 = getelementptr inbounds %struct.TValue* %90, i32 6
  %srcvalue39124 = bitcast %struct.TValue* %90 to double*
  %destvalue40125 = bitcast %struct.TValue* %91 to double*
  %92 = load double* %srcvalue39124, !tbaa !18
  store double %92, double* %destvalue40125, !tbaa !18
  %srctype41 = getelementptr inbounds %struct.TValue* %90, i32 0, i32 1
  %desttype42 = getelementptr inbounds %struct.TValue* %91, i32 0, i32 1
  %93 = load i32* %srctype41, !tbaa !15
  store i32 %93, i32* %desttype42, !tbaa !15
  %94 = load %struct.TValue** %sunkaddr123, !tbaa !14
  %95 = getelementptr inbounds %struct.TValue* %94, i32 7
  %96 = bitcast %struct.TValue* %95 to i64*
  store i64 1, i64* %96, !tbaa !18
  %dest.tt43 = getelementptr inbounds %struct.TValue* %95, i32 0, i32 1
  store i32 19, i32* %dest.tt43, !tbaa !15
  %97 = load %struct.TValue** %sunkaddr123, !tbaa !14
  %98 = getelementptr inbounds %struct.TValue* %97, i32 5
  %99 = getelementptr inbounds %struct.TValue* %97, i32 6
  %100 = bitcast %struct.TValue* %99 to i64*
  %101 = load i64* %100, !tbaa !18
  %102 = bitcast %struct.TValue* %98 to i64*
  %103 = load i64* %102, !tbaa !18
  %initv-pstep.i = sub nsw i64 %103, 1
  store i64 %initv-pstep.i, i64* %iidx, !tbaa !17
  store i64 %101, i64* %ilimit, !tbaa !17
  br label %forloop35_

forbody21_:                                       ; preds = %forloop35_
  %sunkaddr137 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr138 = add i64 %sunkaddr137, 32
  %sunkaddr139 = inttoptr i64 %sunkaddr138 to %struct.TValue**
  %104 = load %struct.TValue** %sunkaddr139, !tbaa !14
  %105 = getelementptr inbounds %struct.TValue* %104, i32 8
  %106 = load i64* %iidx, !tbaa !17
  %107 = bitcast %struct.TValue* %105 to i64*
  store i64 %106, i64* %107, !tbaa !18
  %dest.tt65 = getelementptr inbounds %struct.TValue* %105, i32 0, i32 1
  store i32 19, i32* %dest.tt65, !tbaa !15
  %sunkaddr126 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr127 = add i64 %sunkaddr126, 32
  %sunkaddr128 = inttoptr i64 %sunkaddr127 to %struct.TValue**
  %108 = load %struct.TValue** %sunkaddr128, !tbaa !14
  %109 = getelementptr inbounds %struct.TValue* %108, i32 9
  %110 = getelementptr inbounds %struct.TValue* %108, i32 8
  %111 = bitcast %struct.TValue* %110 to i64*
  %112 = load i64* %111, !tbaa !18
  %113 = sub nsw i64 %112, 1
  %114 = bitcast %struct.TValue* %109 to i64*
  store i64 %113, i64* %114, !tbaa !18
  %dest.tt44 = getelementptr inbounds %struct.TValue* %109, i32 0, i32 1
  store i32 19, i32* %dest.tt44, !tbaa !15
  %115 = load %struct.TValue** %sunkaddr128, !tbaa !14
  %116 = getelementptr inbounds %struct.TValue* %115, i32 4
  %117 = getelementptr inbounds %struct.TValue* %115, i32 9
  %118 = bitcast %struct.TValue* %117 to i64*
  %119 = load i64* %118, !tbaa !18
  %120 = bitcast %struct.TValue* %115 to i64*
  %121 = load i64* %120, !tbaa !18
  %122 = mul nsw i64 %119, %121
  %123 = bitcast %struct.TValue* %116 to i64*
  store i64 %122, i64* %123, !tbaa !18
  %dest.tt45 = getelementptr inbounds %struct.TValue* %116, i32 0, i32 1
  store i32 19, i32* %dest.tt45, !tbaa !15
  %124 = load %struct.TValue** %sunkaddr128, !tbaa !14
  %125 = getelementptr inbounds %struct.TValue* %124, i32 9
  %126 = bitcast %struct.TValue* %125 to i64*
  store i64 1, i64* %126, !tbaa !18
  %dest.tt46 = getelementptr inbounds %struct.TValue* %125, i32 0, i32 1
  store i32 19, i32* %dest.tt46, !tbaa !15
  %127 = load %struct.TValue** %sunkaddr128, !tbaa !14
  %128 = getelementptr inbounds %struct.TValue* %127, i32 10
  %srcvalue47129 = bitcast %struct.TValue* %127 to double*
  %destvalue48130 = bitcast %struct.TValue* %128 to double*
  %129 = load double* %srcvalue47129, !tbaa !18
  store double %129, double* %destvalue48130, !tbaa !18
  %srctype49 = getelementptr inbounds %struct.TValue* %127, i32 0, i32 1
  %desttype50 = getelementptr inbounds %struct.TValue* %128, i32 0, i32 1
  %130 = load i32* %srctype49, !tbaa !15
  store i32 %130, i32* %desttype50, !tbaa !15
  %131 = load %struct.TValue** %sunkaddr128, !tbaa !14
  %132 = getelementptr inbounds %struct.TValue* %131, i32 11
  %133 = bitcast %struct.TValue* %132 to i64*
  store i64 1, i64* %133, !tbaa !18
  %dest.tt51 = getelementptr inbounds %struct.TValue* %132, i32 0, i32 1
  store i32 19, i32* %dest.tt51, !tbaa !15
  %134 = load %struct.TValue** %sunkaddr128, !tbaa !14
  %135 = getelementptr inbounds %struct.TValue* %134, i32 9
  %136 = getelementptr inbounds %struct.TValue* %134, i32 10
  %137 = bitcast %struct.TValue* %136 to i64*
  %138 = load i64* %137, !tbaa !18
  %139 = bitcast %struct.TValue* %135 to i64*
  %140 = load i64* %139, !tbaa !18
  %initv-pstep.i54 = sub nsw i64 %140, 1
  store i64 %initv-pstep.i54, i64* %iidx53, !tbaa !17
  store i64 %138, i64* %ilimit52, !tbaa !17
  br label %forloop34_

forbody27_:                                       ; preds = %forloop34_
  %sunkaddr134 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr135 = add i64 %sunkaddr134, 32
  %sunkaddr136 = inttoptr i64 %sunkaddr135 to %struct.TValue**
  %141 = load %struct.TValue** %sunkaddr136, !tbaa !14
  %142 = getelementptr inbounds %struct.TValue* %141, i32 12
  %143 = load i64* %iidx53, !tbaa !17
  %144 = bitcast %struct.TValue* %142 to i64*
  store i64 %143, i64* %144, !tbaa !18
  %dest.tt61 = getelementptr inbounds %struct.TValue* %142, i32 0, i32 1
  store i32 19, i32* %dest.tt61, !tbaa !15
  %sunkaddr131 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr132 = add i64 %sunkaddr131, 32
  %sunkaddr133 = inttoptr i64 %sunkaddr132 to %struct.TValue**
  %145 = load %struct.TValue** %sunkaddr133, !tbaa !14
  %146 = getelementptr inbounds %struct.TValue* %145, i32 13
  %147 = getelementptr inbounds %struct.TValue* %145, i32 4
  %148 = bitcast %struct.TValue* %147 to i64*
  %149 = load i64* %148, !tbaa !18
  %150 = getelementptr inbounds %struct.TValue* %145, i32 12
  %151 = bitcast %struct.TValue* %150 to i64*
  %152 = load i64* %151, !tbaa !18
  %153 = add nsw i64 %149, %152
  %154 = bitcast %struct.TValue* %146 to i64*
  store i64 %153, i64* %154, !tbaa !18
  %dest.tt55 = getelementptr inbounds %struct.TValue* %146, i32 0, i32 1
  store i32 19, i32* %dest.tt55, !tbaa !15
  %155 = load %struct.TValue** %sunkaddr133, !tbaa !14
  %156 = getelementptr inbounds %struct.TValue* %155, i32 14
  %157 = getelementptr inbounds %struct.TValue* %155, i32 8
  %158 = bitcast %struct.TValue* %157 to i64*
  %159 = load i64* %158, !tbaa !18
  %160 = getelementptr inbounds %struct.TValue* %155, i32 12
  %161 = bitcast %struct.TValue* %160 to i64*
  %162 = load i64* %161, !tbaa !18
  %163 = sub nsw i64 %159, %162
  %164 = bitcast %struct.TValue* %156 to i64*
  store i64 %163, i64* %164, !tbaa !18
  %dest.tt56 = getelementptr inbounds %struct.TValue* %156, i32 0, i32 1
  store i32 19, i32* %dest.tt56, !tbaa !15
  %165 = load %struct.TValue** %sunkaddr133, !tbaa !14
  %166 = getelementptr inbounds %struct.TValue* %165, i32 14
  %167 = getelementptr inbounds %struct.TValue* %165, i32 3
  %168 = bitcast %struct.TValue* %167 to double*
  %169 = load double* %168, !tbaa !18
  %170 = getelementptr inbounds %struct.TValue* %165, i32 14
  %171 = bitcast %struct.TValue* %170 to i64*
  %172 = load i64* %171, !tbaa !18
  %173 = sitofp i64 %172 to double
  %174 = fmul double %169, %173
  %175 = bitcast %struct.TValue* %166 to double*
  store double %174, double* %175, !tbaa !18
  %dest.tt57 = getelementptr inbounds %struct.TValue* %166, i32 0, i32 1
  store i32 3, i32* %dest.tt57, !tbaa !15
  %176 = load %struct.TValue** %sunkaddr133, !tbaa !14
  %177 = getelementptr inbounds %struct.TValue* %176, i32 15
  %178 = getelementptr inbounds %struct.TValue* %176, i32 8
  %179 = bitcast %struct.TValue* %178 to i64*
  %180 = load i64* %179, !tbaa !18
  %181 = getelementptr inbounds %struct.TValue* %176, i32 12
  %182 = bitcast %struct.TValue* %181 to i64*
  %183 = load i64* %182, !tbaa !18
  %184 = add nsw i64 %180, %183
  %185 = bitcast %struct.TValue* %177 to i64*
  store i64 %184, i64* %185, !tbaa !18
  %dest.tt58 = getelementptr inbounds %struct.TValue* %177, i32 0, i32 1
  store i32 19, i32* %dest.tt58, !tbaa !15
  %186 = load %struct.TValue** %sunkaddr133, !tbaa !14
  %187 = getelementptr inbounds %struct.TValue* %186, i32 15
  %188 = getelementptr inbounds %struct.TValue* %186, i32 15
  %189 = bitcast %struct.TValue* %188 to i64*
  %190 = load i64* %189, !tbaa !18
  %191 = sub nsw i64 %190, 2
  %192 = bitcast %struct.TValue* %187 to i64*
  store i64 %191, i64* %192, !tbaa !18
  %dest.tt59 = getelementptr inbounds %struct.TValue* %187, i32 0, i32 1
  store i32 19, i32* %dest.tt59, !tbaa !15
  %193 = load %struct.TValue** %sunkaddr133, !tbaa !14
  %194 = getelementptr inbounds %struct.TValue* %193, i32 14
  %195 = getelementptr inbounds %struct.TValue* %193, i32 14
  %196 = bitcast %struct.TValue* %195 to double*
  %197 = load double* %196, !tbaa !18
  %198 = getelementptr inbounds %struct.TValue* %193, i32 15
  %199 = bitcast %struct.TValue* %198 to i64*
  %200 = load i64* %199, !tbaa !18
  %201 = sitofp i64 %200 to double
  %202 = fmul double %197, %201
  %203 = bitcast %struct.TValue* %194 to double*
  store double %202, double* %203, !tbaa !18
  %dest.tt60 = getelementptr inbounds %struct.TValue* %194, i32 0, i32 1
  store i32 3, i32* %dest.tt60, !tbaa !15
  %204 = load %struct.TValue** %sunkaddr133, !tbaa !14
  %205 = getelementptr inbounds %struct.TValue* %204, i32 2
  %206 = getelementptr inbounds %struct.TValue* %204, i32 13
  %207 = getelementptr inbounds %struct.TValue* %204, i32 14
  call void @luaV_settable(%struct.lua_State* %L, %struct.TValue* %205, %struct.TValue* %206, %struct.TValue* %207)
  br label %forloop34_

forloop34_:                                       ; preds = %forbody27_, %forbody21_
  %208 = load i64* %iidx53, !tbaa !17
  %next.idx = add nsw i64 1, %208
  store i64 %next.idx, i64* %iidx53, !tbaa !17
  %209 = load i64* %ilimit52, !tbaa !17
  %idx.gt.limit = icmp sgt i64 %next.idx, %209
  br i1 %idx.gt.limit, label %forloop35_, label %forbody27_

forloop35_:                                       ; preds = %forloop34_, %op_call.done34
  %210 = load i64* %iidx, !tbaa !17
  %next.idx62 = add nsw i64 1, %210
  store i64 %next.idx62, i64* %iidx, !tbaa !17
  %211 = load i64* %ilimit, !tbaa !17
  %idx.gt.limit63 = icmp sgt i64 %next.idx62, %211
  br i1 %idx.gt.limit63, label %exit_iforloop66, label %forbody21_

exit_iforloop66:                                  ; preds = %forloop35_
  %sunkaddr140 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr141 = add i64 %sunkaddr140, 32
  %sunkaddr142 = inttoptr i64 %sunkaddr141 to %struct.TValue**
  %212 = load %struct.TValue** %sunkaddr142, !tbaa !14
  %213 = getelementptr inbounds %struct.TValue* %212, i32 5
  %214 = getelementptr inbounds %struct.TValue* %5, i32 2
  %upvals67 = getelementptr inbounds %struct.LClosure* %3, i32 0, i32 6, i32 0
  %215 = load %struct.UpVal** %upvals67, !tbaa !8
  %v68143 = bitcast %struct.UpVal* %215 to %struct.TValue**
  %216 = load %struct.TValue** %v68143, !tbaa !19
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %216, %struct.TValue* %214, %struct.TValue* %213)
  %217 = load %struct.TValue** %sunkaddr142, !tbaa !14
  %218 = getelementptr inbounds %struct.TValue* %217, i32 5
  %219 = getelementptr inbounds %struct.TValue* %217, i32 5
  %220 = getelementptr inbounds %struct.TValue* %5, i32 3
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %219, %struct.TValue* %220, %struct.TValue* %218)
  %221 = load %struct.TValue** %sunkaddr142, !tbaa !14
  %222 = getelementptr inbounds %struct.TValue* %221, i32 6
  %L.top69 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %222, %struct.TValue** %L.top69, !tbaa !21
  %223 = getelementptr inbounds %struct.TValue* %221, i32 5
  %224 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %223, i32 1)
  %225 = icmp eq i32 %224, 0
  br i1 %225, label %if.lua.function70, label %if.not.lua.function71

if.lua.function70:                                ; preds = %exit_iforloop66
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done75

if.not.lua.function71:                            ; preds = %exit_iforloop66
  %226 = icmp eq i32 %224, 1
  br i1 %226, label %if.C.function72, label %op_call.done75

if.C.function72:                                  ; preds = %if.not.lua.function71
  %ci_top73 = getelementptr inbounds %struct.CallInfo* %0, i32 0, i32 1
  %227 = load %struct.TValue** %ci_top73, !tbaa !23
  %L_top74 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %227, %struct.TValue** %L_top74, !tbaa !21
  br label %op_call.done75

op_call.done75:                                   ; preds = %if.C.function72, %if.not.lua.function71, %if.lua.function70
  %sunkaddr144 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr145 = add i64 %sunkaddr144, 32
  %sunkaddr146 = inttoptr i64 %sunkaddr145 to %struct.TValue**
  %228 = load %struct.TValue** %sunkaddr146, !tbaa !14
  %229 = getelementptr inbounds %struct.TValue* %228, i32 6
  %upvals76 = getelementptr inbounds %struct.LClosure* %3, i32 0, i32 6, i32 0
  %230 = load %struct.UpVal** %upvals76, !tbaa !8
  %v77147 = bitcast %struct.UpVal* %230 to %struct.TValue**
  %231 = load %struct.TValue** %v77147, !tbaa !19
  call void @luaV_gettable(%struct.lua_State* %L, %struct.TValue* %231, %struct.TValue* %5, %struct.TValue* %229)
  %232 = load %struct.TValue** %sunkaddr146, !tbaa !14
  %233 = getelementptr inbounds %struct.TValue* %232, i32 7
  %234 = getelementptr inbounds %struct.TValue* %5, i32 7
  %srcvalue78148 = bitcast %struct.TValue* %234 to double*
  %destvalue79149 = bitcast %struct.TValue* %233 to double*
  %235 = load double* %srcvalue78148, !tbaa !18
  store double %235, double* %destvalue79149, !tbaa !18
  %srctype80 = getelementptr inbounds %struct.TValue* %234, i32 0, i32 1
  %desttype81 = getelementptr inbounds %struct.TValue* %233, i32 0, i32 1
  %236 = load i32* %srctype80, !tbaa !15
  store i32 %236, i32* %desttype81, !tbaa !15
  %237 = load %struct.TValue** %sunkaddr146, !tbaa !14
  %238 = getelementptr inbounds %struct.TValue* %237, i32 8
  %239 = getelementptr inbounds %struct.TValue* %237, i32 5
  %240 = getelementptr inbounds %struct.TValue* %237, i32 1
  %value.tt.ptr82 = getelementptr inbounds %struct.TValue* %239, i32 0, i32 1
  %value.tt83 = load i32* %value.tt.ptr82, !tbaa !15
  %value.tt.ptr84 = getelementptr inbounds %struct.TValue* %240, i32 0, i32 1
  %value.tt85 = load i32* %value.tt.ptr84, !tbaa !15
  %rb.is.integer = icmp eq i32 %value.tt83, 19
  %rc.is.integer = icmp eq i32 %value.tt85, 19
  %241 = and i1 %rb.is.integer, %rc.is.integer
  br i1 %241, label %if.integer, label %if.not.integer87

if.integer:                                       ; preds = %op_call.done75
  %242 = bitcast %struct.TValue* %239 to i64*
  %243 = load i64* %242, !tbaa !18
  %244 = bitcast %struct.TValue* %240 to i64*
  %245 = load i64* %244, !tbaa !18
  %246 = sub nsw i64 %243, %245
  %247 = bitcast %struct.TValue* %238 to i64*
  store i64 %246, i64* %247, !tbaa !18
  %dest.tt86 = getelementptr inbounds %struct.TValue* %238, i32 0, i32 1
  store i32 19, i32* %dest.tt86, !tbaa !15
  br label %done89

if.not.integer87:                                 ; preds = %op_call.done75
  %rb.is.float = icmp eq i32 %value.tt83, 3
  br i1 %rb.is.float, label %load.rb, label %convert.rb

convert.rb:                                       ; preds = %if.not.integer87
  %248 = call i32 @luaV_tonumber_(%struct.TValue* %239, double* %nb)
  %rb.float.ok = icmp eq i32 %248, 1
  br i1 %rb.float.ok, label %test.rc, label %try_meta

load.rb:                                          ; preds = %if.not.integer87
  %249 = bitcast %struct.TValue* %239 to double*
  %250 = load double* %249, !tbaa !18
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
  %253 = load double* %252, !tbaa !18
  store double %253, double* %nc, !tbaa !17
  br label %float.op

float.op:                                         ; preds = %load.rc, %convert.rc
  %254 = load double* %nb, !tbaa !17
  %255 = load double* %nc, !tbaa !17
  %256 = fsub double %254, %255
  %257 = bitcast %struct.TValue* %238 to double*
  store double %256, double* %257, !tbaa !18
  %dest.tt88 = getelementptr inbounds %struct.TValue* %238, i32 0, i32 1
  store i32 3, i32* %dest.tt88, !tbaa !15
  br label %done89

try_meta:                                         ; preds = %convert.rc, %convert.rb
  call void @luaT_trybinTM(%struct.lua_State* %L, %struct.TValue* %239, %struct.TValue* %240, %struct.TValue* %238, i32 7)
  br label %done89

done89:                                           ; preds = %try_meta, %float.op, %if.integer
  %sunkaddr150 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr151 = add i64 %sunkaddr150, 32
  %sunkaddr152 = inttoptr i64 %sunkaddr151 to %struct.TValue**
  %258 = load %struct.TValue** %sunkaddr152, !tbaa !14
  %259 = getelementptr inbounds %struct.TValue* %258, i32 9
  %L.top90 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %259, %struct.TValue** %L.top90, !tbaa !21
  %260 = getelementptr inbounds %struct.TValue* %258, i32 6
  %261 = call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %260, i32 0)
  %262 = icmp eq i32 %261, 0
  br i1 %262, label %if.lua.function91, label %if.not.lua.function92

if.lua.function91:                                ; preds = %done89
  call void @luaV_execute(%struct.lua_State* %L)
  br label %op_call.done96

if.not.lua.function92:                            ; preds = %done89
  %263 = icmp eq i32 %261, 1
  br i1 %263, label %if.C.function93, label %op_call.done96

if.C.function93:                                  ; preds = %if.not.lua.function92
  %ci_top94 = getelementptr inbounds %struct.CallInfo* %0, i32 0, i32 1
  %264 = load %struct.TValue** %ci_top94, !tbaa !23
  %L_top95 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %264, %struct.TValue** %L_top95, !tbaa !21
  br label %op_call.done96

op_call.done96:                                   ; preds = %if.C.function93, %if.not.lua.function92, %if.lua.function91
  %sunkaddr153 = ptrtoint %struct.CallInfo* %0 to i64
  %sunkaddr154 = add i64 %sunkaddr153, 32
  %sunkaddr155 = inttoptr i64 %sunkaddr154 to %struct.TValue**
  %265 = load %struct.TValue** %sunkaddr155, !tbaa !14
  %266 = getelementptr inbounds %struct.TValue* %265, i32 2
  %sizep = getelementptr inbounds %struct.Proto* %4, i32 0, i32 10
  %267 = load i32* %sizep, !tbaa !24
  %268 = icmp sgt i32 %267, 0
  br i1 %268, label %if.then, label %if.else

if.then:                                          ; preds = %op_call.done96
  call void @luaF_close(%struct.lua_State* %L, %struct.TValue* %265)
  br label %if.else

if.else:                                          ; preds = %if.then, %op_call.done96
  %269 = call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %266, i32 1)
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
