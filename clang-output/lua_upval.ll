; ModuleID = 'lua_upval.c'
source_filename = "lua_upval.c"
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8, i16 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i8, i8, i16 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.UpVal = type { %struct.GCObject*, i8, i8, %struct.TValue*, %union.anon.0 }
%union.anon.0 = type { %struct.anon }
%struct.anon = type { %struct.UpVal*, %struct.UpVal** }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.lua_longjmp = type opaque
%struct.CallInfo = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %union.anon, i64, i16, i8, i8 }
%union.anon = type { %struct.CallInfoL }
%struct.lua_Debug = type opaque
%struct.LClosure = type { %struct.GCObject*, i8, i8, i8, %struct.GCObject*, %struct.Proto*, [1 x %struct.UpVal*] }
%struct.Proto = type { %struct.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %struct.TValue*, i32*, %struct.Proto**, i32*, %struct.LocVar*, %struct.Upvaldesc*, %struct.LClosure*, %struct.TString*, %struct.GCObject*, i8, %struct.RaviJITProto }
%struct.LocVar = type { %struct.TString*, %struct.TString*, i32, i32, i8 }
%struct.Upvaldesc = type { %struct.TString*, %struct.TString*, i8, i8, i8 }
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i8, i32, %union.anon.1 }
%union.anon.1 = type { i64 }
%struct.RaviJITProto = type { i8, i8*, i32 (%struct.lua_State*)* }
%struct.TValue = type { %union.Value, i8 }
%union.Value = type { %struct.GCObject* }

; Function Attrs: nounwind
define void @luaV_op_call(%struct.lua_State* %L, %struct.LClosure* nocapture readonly %cl, %struct.TValue* nocapture readonly %ra, i32 %b, i32 %c) local_unnamed_addr #0 {
entry:
  %idxprom = sext i32 %b to i64
  %arrayidx = getelementptr inbounds %struct.LClosure, %struct.LClosure* %cl, i64 0, i32 6, i64 %idxprom
  %0 = load %struct.UpVal*, %struct.UpVal** %arrayidx, align 8, !tbaa !1
  %tt_ = getelementptr inbounds %struct.TValue, %struct.TValue* %ra, i64 0, i32 1
  %1 = load i8, i8* %tt_, align 8, !tbaa !5
  %2 = and i8 %1, 64
  %marked = getelementptr inbounds %struct.UpVal, %struct.UpVal* %0, i64 0, i32 2
  %3 = load i8, i8* %marked, align 1, !tbaa !7
  %4 = and i8 %3, 32
  %gc = getelementptr inbounds %struct.TValue, %struct.TValue* %ra, i64 0, i32 0, i32 0
  %5 = load %struct.GCObject*, %struct.GCObject** %gc, align 8, !tbaa !1
  %marked3 = getelementptr inbounds %struct.GCObject, %struct.GCObject* %5, i64 0, i32 2
  %6 = load i8, i8* %marked3, align 1, !tbaa !9
  %7 = and i8 %6, 24
  %tobool = icmp ne i8 %2, 0
  %tobool6 = icmp ne i8 %4, 0
  %or.cond = and i1 %tobool, %tobool6
  %tobool8 = icmp ne i8 %7, 0
  %or.cond9 = and i1 %or.cond, %tobool8
  br i1 %or.cond9, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  tail call void @luaC_upvalbarrier_(%struct.lua_State* %L, %struct.UpVal* %0) #2
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  ret void
}

declare void @luaC_upvalbarrier_(%struct.lua_State*, %struct.UpVal*) local_unnamed_addr #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!1 = !{!2, !2, i64 0}
!2 = !{!"any pointer", !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
!5 = !{!6, !3, i64 8}
!6 = !{!"TValue", !3, i64 0, !3, i64 8}
!7 = !{!8, !3, i64 9}
!8 = !{!"UpVal", !2, i64 0, !3, i64 8, !3, i64 9, !2, i64 16, !3, i64 24}
!9 = !{!10, !3, i64 9}
!10 = !{!"GCObject", !2, i64 0, !3, i64 8, !3, i64 9}
