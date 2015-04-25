; ModuleID = 'ravi_module_ravif4'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc-elf"

%0 = type { %ravi.TValue*, i32*, i64 }
%ravi.lua_State = type { %ravi.GCObject*, i8, i8, i8, %ravi.TValue*, %ravi.global_State*, %ravi.CallInfo*, i32*, %ravi.TValue*, %ravi.TValue*, %ravi.UpVal*, %ravi.GCObject*, %ravi.lua_State*, %ravi.lua_longjmp*, %ravi.CallInfo, i8* (%ravi.lua_State*, %ravi.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8 }
%ravi.global_State = type opaque
%ravi.TValue = type { %ravi.Value, i32 }
%ravi.Value = type { double }
%ravi.UpVal = type { %ravi.TValue*, i64, %ravi.TValue }
%ravi.GCObject = type { %ravi.GCObject*, i8, i8 }
%ravi.lua_longjmp = type opaque
%ravi.CallInfo = type { %ravi.TValue*, %ravi.TValue*, %ravi.CallInfo*, %ravi.CallInfo*, %0, i64, i16, i8, i8 }
%ravi.lua_Debug = type opaque
%ravi.LClosure = type { %ravi.GCObject*, i8, i8, i8, %ravi.GCObject*, %ravi.Proto*, [1 x %ravi.UpVal*] }
%ravi.Proto = type { %ravi.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %ravi.TValue*, i32*, %ravi.Proto**, i32*, %ravi.LocVar*, %ravi.Upvaldesc*, %ravi.LClosure*, %ravi.TString*, %ravi.GCObject*, %ravi.RaviJITProto }
%ravi.LocVar = type { %ravi.TString*, i32, i32, i32 }
%ravi.Upvaldesc = type { %ravi.TString*, i32, i8, i8 }
%ravi.TString = type { %ravi.GCObject*, i8, i8, i8, i32, i64, %ravi.TString* }
%ravi.RaviJITProto = type { i8, i8, i16, i8*, i32 (%ravi.lua_State*)* }
%ravi.Table = type { %ravi.GCObject*, i8, i8, i8, i8, i32, %ravi.TValue*, %ravi.Node*, %ravi.Node*, %ravi.Table*, %ravi.GCObject*, %ravi.RaviArray }
%ravi.Node = type { %ravi.TValue, %ravi.TKey }
%ravi.TKey = type { %ravi.Value, i32, i32 }
%ravi.RaviArray = type { i8*, i32, i32, i32 }

@0 = private unnamed_addr constant [19 x i8] c"integer[] expected\00"

; Function Attrs: nounwind
define i32 @ravif4(%ravi.lua_State* %L) #0 {
entry:
  %L_ci = getelementptr inbounds %ravi.lua_State* %L, i64 0, i32 6
  %0 = load %ravi.CallInfo** %L_ci, align 8, !tbaa !0
  %base = getelementptr inbounds %ravi.CallInfo* %0, i64 0, i32 4, i32 0
  %1 = bitcast %ravi.CallInfo* %0 to %ravi.LClosure***
  %2 = load %ravi.LClosure*** %1, align 8, !tbaa !7
  %3 = load %ravi.LClosure** %2, align 8, !tbaa !8
  %Proto = getelementptr inbounds %ravi.LClosure* %3, i64 0, i32 5
  %4 = load %ravi.Proto** %Proto, align 8, !tbaa !9
  %k = getelementptr inbounds %ravi.Proto* %4, i64 0, i32 14
  %5 = load %ravi.TValue** %k, align 8, !tbaa !11
  %6 = load %ravi.TValue** %base, align 8, !tbaa !14
  tail call void @raviV_op_newarrayint(%ravi.lua_State* %L, %ravi.CallInfo* %0, %ravi.TValue* %6)
  %7 = load %ravi.TValue** %base, align 8, !tbaa !14
  %srcvalue6 = bitcast %ravi.TValue* %5 to double*
  %destvalue = getelementptr inbounds %ravi.TValue* %7, i64 1, i32 0, i32 0
  %8 = load double* %srcvalue6, align 8, !tbaa !15
  store double %8, double* %destvalue, align 8, !tbaa !15
  %srctype = getelementptr inbounds %ravi.TValue* %5, i64 0, i32 1
  %desttype = getelementptr inbounds %ravi.TValue* %7, i64 1, i32 1
  %9 = load i32* %srctype, align 4, !tbaa !17
  store i32 %9, i32* %desttype, align 4, !tbaa !17
  tail call void @raviV_op_setlist(%ravi.lua_State* %L, %ravi.CallInfo* %0, %ravi.TValue* %7, i32 1, i32 1)
  %10 = load %ravi.TValue** %base, align 8, !tbaa !14
  %11 = getelementptr inbounds %ravi.TValue* %10, i64 1
  %12 = getelementptr inbounds %ravi.TValue* %5, i64 1
  %upvals = getelementptr inbounds %ravi.LClosure* %3, i64 0, i32 6, i64 0
  %13 = load %ravi.UpVal** %upvals, align 8, !tbaa !8
  %v7 = bitcast %ravi.UpVal* %13 to %ravi.TValue**
  %14 = load %ravi.TValue** %v7, align 8, !tbaa !18
  tail call void @luaV_gettable(%ravi.lua_State* %L, %ravi.TValue* %14, %ravi.TValue* %12, %ravi.TValue* %11)
  %15 = load %ravi.TValue** %base, align 8, !tbaa !14
  %16 = getelementptr inbounds %ravi.TValue* %15, i64 2
  tail call void @raviV_op_newtable(%ravi.lua_State* %L, %ravi.CallInfo* %0, %ravi.TValue* %16, i32 0, i32 0)
  %17 = load %ravi.TValue** %base, align 8, !tbaa !14
  %18 = getelementptr inbounds %ravi.TValue* %17, i64 3
  %L.top = getelementptr inbounds %ravi.lua_State* %L, i64 0, i32 4
  store %ravi.TValue* %18, %ravi.TValue** %L.top, align 8, !tbaa !20
  %19 = getelementptr inbounds %ravi.TValue* %17, i64 1
  %20 = tail call i32 @luaD_precall(%ravi.lua_State* %L, %ravi.TValue* %19, i32 1)
  switch i32 %20, label %op_call.done [
    i32 0, label %if.lua.function
    i32 1, label %if.C.function
  ]

if.lua.function:                                  ; preds = %entry
  tail call void @luaV_execute(%ravi.lua_State* %L)
  br label %op_call.done

if.C.function:                                    ; preds = %entry
  %ci_top = getelementptr inbounds %ravi.CallInfo* %0, i64 0, i32 1
  %21 = load %ravi.TValue** %ci_top, align 8, !tbaa !22
  %sunkaddr = ptrtoint %ravi.lua_State* %L to i64
  %sunkaddr8 = add i64 %sunkaddr, 16
  %sunkaddr9 = inttoptr i64 %sunkaddr8 to %ravi.TValue**
  store %ravi.TValue* %21, %ravi.TValue** %sunkaddr9, align 8, !tbaa !20
  br label %op_call.done

op_call.done:                                     ; preds = %entry, %if.C.function, %if.lua.function
  %sunkaddr10 = ptrtoint %ravi.CallInfo* %0 to i64
  %sunkaddr11 = add i64 %sunkaddr10, 32
  %sunkaddr12 = inttoptr i64 %sunkaddr11 to %ravi.TValue**
  %22 = load %ravi.TValue** %sunkaddr12, align 8, !tbaa !14
  %value.tt.ptr = getelementptr inbounds %ravi.TValue* %22, i64 1, i32 1
  %value.tt = load i32* %value.tt.ptr, align 4, !tbaa !17
  %is.not.table = icmp eq i32 %value.tt, 69
  br i1 %is.not.table, label %test.if.array, label %if.not.table

if.not.table:                                     ; preds = %op_call.done, %test.if.array
  tail call void @luaG_runerror(%ravi.lua_State* %L, i8* getelementptr inbounds ([19 x i8]* @0, i64 0, i64 0))
  unreachable

test.if.array:                                    ; preds = %op_call.done
  %23 = getelementptr inbounds %ravi.TValue* %22, i64 1
  %24 = bitcast %ravi.TValue* %23 to %ravi.Table**
  %25 = load %ravi.Table** %24, align 8, !tbaa !23
  %raviarray.type_ptr = getelementptr inbounds %ravi.Table* %25, i64 0, i32 11, i32 1
  %raviarray.type = load i32* %raviarray.type_ptr, align 4, !tbaa !24
  %is.array.type = icmp eq i32 %raviarray.type, 3
  br i1 %is.array.type, label %done, label %if.not.table

done:                                             ; preds = %test.if.array
  %26 = ptrtoint %ravi.Table* %25 to i64
  %27 = bitcast %ravi.TValue* %22 to i64*
  store i64 %26, i64* %27, align 8, !tbaa !15
  %desttype4 = getelementptr inbounds %ravi.TValue* %22, i64 0, i32 1
  store i32 69, i32* %desttype4, align 4, !tbaa !17
  %sunkaddr13 = ptrtoint %ravi.lua_State* %L to i64
  %sunkaddr14 = add i64 %sunkaddr13, 16
  %sunkaddr15 = inttoptr i64 %sunkaddr14 to %ravi.TValue**
  store %ravi.TValue* %22, %ravi.TValue** %sunkaddr15, align 8, !tbaa !20
  %sizep = getelementptr inbounds %ravi.Proto* %4, i64 0, i32 10
  %28 = load i32* %sizep, align 4, !tbaa !27
  %29 = icmp sgt i32 %28, 0
  br i1 %29, label %if.then, label %if.else

if.then:                                          ; preds = %done
  tail call void @luaF_close(%ravi.lua_State* %L, %ravi.TValue* %22)
  br label %if.else

if.else:                                          ; preds = %if.then, %done
  %30 = tail call i32 @luaD_poscall(%ravi.lua_State* %L, %ravi.TValue* %22)
  ret i32 1
}

; Function Attrs: nounwind
declare i32 @luaD_poscall(%ravi.lua_State*, %ravi.TValue*) #0

; Function Attrs: nounwind
declare i32 @luaD_precall(%ravi.lua_State*, %ravi.TValue*, i32) #0

; Function Attrs: nounwind
declare void @luaF_close(%ravi.lua_State*, %ravi.TValue*) #0

; Function Attrs: noreturn nounwind
declare void @luaG_runerror(%ravi.lua_State*, i8*) #1

; Function Attrs: nounwind
declare void @luaV_execute(%ravi.lua_State*) #0

; Function Attrs: nounwind
declare void @luaV_gettable(%ravi.lua_State*, %ravi.TValue*, %ravi.TValue*, %ravi.TValue*) #0

; Function Attrs: nounwind
declare void @raviV_op_newarrayint(%ravi.lua_State*, %ravi.CallInfo*, %ravi.TValue*) #0

; Function Attrs: nounwind
declare void @raviV_op_newtable(%ravi.lua_State*, %ravi.CallInfo*, %ravi.TValue*, i32, i32) #0

; Function Attrs: nounwind
declare void @raviV_op_setlist(%ravi.lua_State*, %ravi.CallInfo*, %ravi.TValue*, i32, i32) #0

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
!15 = !{!16, !5, i64 0}
!16 = !{!"TValue", !5, i64 0, !13, i64 8}
!17 = !{!16, !13, i64 8}
!18 = !{!19, !1, i64 0}
!19 = !{!"UpVal", !1, i64 0, !5, i64 8, !16, i64 16}
!20 = !{!21, !1, i64 8}
!21 = !{!"lua_State", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !1, i64 8, !1, i64 12, !1, i64 16, !1, i64 20, !1, i64 24, !1, i64 28, !1, i64 32, !1, i64 36, !1, i64 40, !1, i64 44, !0, i64 48, !1, i64 92, !5, i64 96, !13, i64 104, !13, i64 108, !13, i64 112, !6, i64 114, !6, i64 116, !2, i64 118, !2, i64 119}
!22 = !{!0, !1, i64 4}
!23 = !{!16, !1, i64 0}
!24 = !{!25, !2, i64 36}
!25 = !{!"Table", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !2, i64 7, !13, i64 8, !1, i64 12, !1, i64 16, !1, i64 20, !1, i64 24, !1, i64 28, !26, i64 32}
!26 = !{!"RaviArray", !1, i64 0, !2, i64 4, !13, i64 8, !13, i64 12}
!27 = !{!12, !13, i64 28}
