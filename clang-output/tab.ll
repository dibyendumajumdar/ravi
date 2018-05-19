; ModuleID = 'tab.c'
source_filename = "tab.c"
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%struct.TValue = type { %union.Value, i16 }
%union.Value = type { %struct.GCObject* }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%union.GCUnion = type { %struct.lua_State }
%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8, i16, i8 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i16, i16, i8, i8 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.UpVal = type { %struct.TValue*, i64, %union.anon.0 }
%union.anon.0 = type { %struct.anon }
%struct.anon = type { %struct.UpVal*, i32 }
%struct.lua_longjmp = type opaque
%struct.CallInfo = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %union.anon, i64, i16, i16, i16, i8, i8 }
%union.anon = type { %struct.CallInfoL }
%struct.lua_Debug = type opaque
%struct.Table = type { %struct.GCObject*, i8, i8, i8, i8, i32, %struct.TValue*, %struct.Node*, %struct.Node*, %struct.Table*, %struct.GCObject*, %struct.RaviArray, i32 }
%struct.Node = type { %struct.TValue, %union.TKey }
%union.TKey = type { %struct.anon.2 }
%struct.anon.2 = type { %union.Value, i16, i32 }
%struct.RaviArray = type { i8*, i32, i32, i8, i8 }

; Function Attrs: nounwind
define i32 @tablevalue(%struct.TValue* %v) #0 {
  %v.addr = alloca %struct.TValue*, align 8
  store %struct.TValue* %v, %struct.TValue** %v.addr, align 8, !tbaa !2
  %1 = load %struct.TValue*, %struct.TValue** %v.addr, align 8, !tbaa !2
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %1, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %2 = load %struct.GCObject*, %struct.GCObject** %gc, align 8, !tbaa !6
  %3 = bitcast %struct.GCObject* %2 to %union.GCUnion*
  %h = bitcast %union.GCUnion* %3 to %struct.Table*
  %sizearray = getelementptr inbounds %struct.Table, %struct.Table* %h, i32 0, i32 5
  %4 = load i32, i32* %sizearray, align 4, !tbaa !6
  ret i32 %4
}

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{!"clang version 6.0.0 (tags/RELEASE_600/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"any pointer", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!4, !4, i64 0}
