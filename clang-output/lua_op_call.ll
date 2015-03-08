; ModuleID = 'lua_op_call.c'
target datalayout = "e-m:w-p:32:32-i64:64-f80:32-n8:16:32-S32"
target triple = "i686-pc-windows-gnu"

%struct.Proto = type { %struct.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, %struct.TValue*, i32*, %struct.Proto**, i32*, %struct.LocVar*, %struct.Upvaldesc*, %struct.LClosure*, %struct.TString*, %struct.GCObject* }
%struct.TValue = type { %union.Value, i32 }
%union.Value = type { i64 }
%struct.LocVar = type { %struct.TString*, i32, i32, i32 }
%struct.Upvaldesc = type { %struct.TString*, i8, i8 }
%struct.LClosure = type { %struct.GCObject*, i8, i8, i8, %struct.GCObject*, %struct.Proto*, [1 x %struct.UpVal*] }
%struct.UpVal = type opaque
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i32, i64, %struct.TString* }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.lua_State = type { %struct.GCObject*, i8, i8, i8, %struct.TValue*, %struct.global_State*, %struct.CallInfoLua*, i32*, %struct.TValue*, %struct.TValue*, %struct.UpVal*, %struct.GCObject*, %struct.lua_State*, %struct.lua_longjmp*, %struct.CallInfo, void (%struct.lua_State*, %struct.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8 }
%struct.global_State = type opaque
%struct.CallInfoLua = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %struct.CallInfoL, i64, i16, i8 }
%struct.CallInfoL = type { %struct.TValue*, i32*, i64 }
%struct.lua_longjmp = type opaque
%struct.CallInfo = type { %struct.TValue*, %struct.TValue*, %struct.CallInfo*, %struct.CallInfo*, %union.anon, i64, i16, i8 }
%union.anon = type { %struct.CallInfoC }
%struct.CallInfoC = type { i32 (%struct.lua_State*, i32, i64)*, i64, i64 }
%struct.lua_Debug = type opaque

@Proto = common global %struct.Proto zeroinitializer, align 4

; Function Attrs: nounwind
define void @luaV_op_call(%struct.lua_State* %L, %struct.CallInfo* nocapture readonly %ci, %struct.TValue* %ra, i32 %b, i32 %c) #0 {
entry:
  %sub = add nsw i32 %c, -1
  %cmp = icmp eq i32 %b, 0
  br i1 %cmp, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  %add.ptr = getelementptr inbounds %struct.TValue* %ra, i32 %b
  %top = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr, %struct.TValue** %top, align 4, !tbaa !1
  br label %if.end

if.end:                                           ; preds = %entry, %if.then
  %call = tail call i32 @luaD_precall(%struct.lua_State* %L, %struct.TValue* %ra, i32 %sub, i32 1) #2
  %tobool = icmp eq i32 %call, 0
  br i1 %tobool, label %if.else, label %if.then1

if.then1:                                         ; preds = %if.end
  %cmp2 = icmp sgt i32 %c, 0
  br i1 %cmp2, label %if.then3, label %if.end7

if.then3:                                         ; preds = %if.then1
  %top4 = getelementptr inbounds %struct.CallInfo* %ci, i32 0, i32 1
  %0 = load %struct.TValue** %top4, align 4, !tbaa !10
  %top5 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %0, %struct.TValue** %top5, align 4, !tbaa !1
  br label %if.end7

if.else:                                          ; preds = %if.end
  tail call void @luaV_execute(%struct.lua_State* %L) #2
  br label %if.end7

if.end7:                                          ; preds = %if.then1, %if.then3, %if.else
  ret void
}

declare i32 @luaD_precall(%struct.lua_State*, %struct.TValue*, i32, i32) #1

declare void @luaV_execute(%struct.lua_State*) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.6.0 (trunk)"}
!1 = metadata !{metadata !2, metadata !3, i64 8}
!2 = metadata !{metadata !"lua_State", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, metadata !3, i64 16, metadata !3, i64 20, metadata !3, i64 24, metadata !3, i64 28, metadata !3, i64 32, metadata !3, i64 36, metadata !3, i64 40, metadata !3, i64 44, metadata !6, i64 48, metadata !3, i64 104, metadata !7, i64 112, metadata !9, i64 120, metadata !9, i64 124, metadata !9, i64 128, metadata !8, i64 132, metadata !8, i64 134, metadata !4, i64 136, metadata !4, i64 137}
!3 = metadata !{metadata !"any pointer", metadata !4, i64 0}
!4 = metadata !{metadata !"omnipotent char", metadata !5, i64 0}
!5 = metadata !{metadata !"Simple C/C++ TBAA"}
!6 = metadata !{metadata !"CallInfo", metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, metadata !3, i64 12, metadata !4, i64 16, metadata !7, i64 40, metadata !8, i64 48, metadata !4, i64 50}
!7 = metadata !{metadata !"long long", metadata !4, i64 0}
!8 = metadata !{metadata !"short", metadata !4, i64 0}
!9 = metadata !{metadata !"int", metadata !4, i64 0}
!10 = metadata !{metadata !6, metadata !3, i64 4}
