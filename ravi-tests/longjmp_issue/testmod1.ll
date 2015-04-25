; ModuleID = 'ravi_module_ravif2'
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

; Function Attrs: nounwind
define i32 @ravif2(%ravi.lua_State* %L) #0 {
entry:
  %L_ci = getelementptr inbounds %ravi.lua_State* %L, i64 0, i32 6
  %0 = load %ravi.CallInfo** %L_ci, align 8, !tbaa !0
  %base = getelementptr inbounds %ravi.CallInfo* %0, i64 0, i32 4, i32 0
  %1 = bitcast %ravi.CallInfo* %0 to %ravi.LClosure***
  %2 = load %ravi.LClosure*** %1, align 8, !tbaa !7
  %3 = load %ravi.LClosure** %2, align 8, !tbaa !8
  %Proto = getelementptr inbounds %ravi.LClosure* %3, i64 0, i32 5
  %4 = load %ravi.Proto** %Proto, align 8, !tbaa !9
  %5 = load %ravi.TValue** %base, align 8, !tbaa !11
  %6 = getelementptr inbounds %ravi.TValue* %5, i64 1
  %L.top = getelementptr inbounds %ravi.lua_State* %L, i64 0, i32 4
  store %ravi.TValue* %6, %ravi.TValue** %L.top, align 8, !tbaa !12
  %sizep = getelementptr inbounds %ravi.Proto* %4, i64 0, i32 10
  %7 = load i32* %sizep, align 4, !tbaa !15
  %8 = icmp sgt i32 %7, 0
  br i1 %8, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  tail call void @luaF_close(%ravi.lua_State* %L, %ravi.TValue* %5)
  br label %if.else

if.else:                                          ; preds = %if.then, %entry
  %9 = tail call i32 @luaD_poscall(%ravi.lua_State* %L, %ravi.TValue* %5)
  ret i32 1
}

; Function Attrs: nounwind
declare i32 @luaD_poscall(%ravi.lua_State*, %ravi.TValue*) #0

; Function Attrs: nounwind
declare void @luaF_close(%ravi.lua_State*, %ravi.TValue*) #0

attributes #0 = { nounwind }

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
!11 = !{!0, !1, i64 16}
!12 = !{!13, !1, i64 8}
!13 = !{!"lua_State", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !1, i64 8, !1, i64 12, !1, i64 16, !1, i64 20, !1, i64 24, !1, i64 28, !1, i64 32, !1, i64 36, !1, i64 40, !1, i64 44, !0, i64 48, !1, i64 92, !5, i64 96, !14, i64 104, !14, i64 108, !14, i64 112, !6, i64 114, !6, i64 116, !2, i64 118, !2, i64 119}
!14 = !{!"int", !2, i64 0}
!15 = !{!16, !14, i64 28}
!16 = !{!"Proto", !1, i64 0, !2, i64 4, !2, i64 5, !2, i64 6, !2, i64 7, !2, i64 8, !14, i64 12, !14, i64 16, !14, i64 20, !14, i64 24, !14, i64 28, !14, i64 32, !14, i64 36, !14, i64 40, !1, i64 44, !1, i64 48, !1, i64 52, !1, i64 56, !1, i64 60, !1, i64 64, !1, i64 68, !1, i64 72, !1, i64 76, !2, i64 80}
