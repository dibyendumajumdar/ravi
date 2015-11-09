; ModuleID = 'inline_getstr.c'
target datalayout = "e-m:x-p:32:32-i64:64-f80:32-n8:16:32-S32"
target triple = "i686-pc-windows-gnu"

%struct.TValue = type { %union.Value, i32 }
%union.Value = type { i64 }
%struct.Table = type { %struct.GCObject*, i8, i8, i8, i8, i32, %struct.TValue*, %struct.Node*, %struct.Node*, %struct.Table*, %struct.GCObject*, %struct.RaviArray }
%struct.Node = type { %struct.TValue, %union.TKey }
%union.TKey = type { %struct.anon.1 }
%struct.anon.1 = type { %union.Value, i32, i32 }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.RaviArray = type { i8*, i32, i32, i32 }
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i8, i32, %union.anon.2 }
%union.anon.2 = type { i64 }

; Function Attrs: nounwind
define %struct.TValue* @ravi_getstr(%struct.Table* %t, %struct.TString* %key) #0 {
entry:
  %hash = getelementptr inbounds %struct.TString, %struct.TString* %key, i32 0, i32 5
  %0 = load i32, i32* %hash, align 4, !tbaa !1
  %lsizenode = getelementptr inbounds %struct.Table, %struct.Table* %t, i32 0, i32 4
  %1 = load i8, i8* %lsizenode, align 1, !tbaa !7
  %conv = zext i8 %1 to i32
  %shl = shl i32 1, %conv
  %sub = add nsw i32 %shl, -1
  %and = and i32 %sub, %0
  %node = getelementptr inbounds %struct.Table, %struct.Table* %t, i32 0, i32 7
  %2 = load %struct.Node*, %struct.Node** %node, align 4, !tbaa !10
  %3 = getelementptr inbounds %struct.Node, %struct.Node* %2, i32 %and, i32 1, i32 0, i32 1
  %4 = load i32, i32* %3, align 4, !tbaa !11
  %cmp = icmp eq i32 %4, 68
  br i1 %cmp, label %land.lhs.true, label %if.end

land.lhs.true:                                    ; preds = %entry
  %value_ = getelementptr inbounds %struct.Node, %struct.Node* %2, i32 %and, i32 1, i32 0, i32 0
  %5 = bitcast %union.Value* %value_ to %struct.TString**
  %6 = load %struct.TString*, %struct.TString** %5, align 4, !tbaa !13
  %cmp2 = icmp eq %struct.TString* %6, %key
  br i1 %cmp2, label %if.then, label %if.end

if.then:                                          ; preds = %land.lhs.true
  %i_val = getelementptr inbounds %struct.Node, %struct.Node* %2, i32 %and, i32 0
  br label %cleanup

if.end:                                           ; preds = %land.lhs.true, %entry
  %call = tail call %struct.TValue* @luaH_getstr(%struct.Table* %t, %struct.TString* %key) #2
  br label %cleanup

cleanup:                                          ; preds = %if.end, %if.then
  %retval.0 = phi %struct.TValue* [ %i_val, %if.then ], [ %call, %if.end ]
  ret %struct.TValue* %retval.0
}

declare %struct.TValue* @luaH_getstr(%struct.Table*, %struct.TString*) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.0 (trunk)"}
!1 = !{!2, !6, i64 8}
!2 = !{!"TString", !3, i64 0, !4, i64 4, !4, i64 5, !4, i64 6, !4, i64 7, !6, i64 8, !4, i64 16}
!3 = !{!"any pointer", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!"int", !4, i64 0}
!7 = !{!8, !4, i64 7}
!8 = !{!"Table", !3, i64 0, !4, i64 4, !4, i64 5, !4, i64 6, !4, i64 7, !6, i64 8, !3, i64 12, !3, i64 16, !3, i64 20, !3, i64 24, !3, i64 28, !9, i64 32}
!9 = !{!"RaviArray", !3, i64 0, !4, i64 4, !6, i64 8, !6, i64 12}
!10 = !{!8, !3, i64 16}
!11 = !{!12, !6, i64 8}
!12 = !{!"TValue", !4, i64 0, !6, i64 8}
!13 = !{!3, !3, i64 0}
