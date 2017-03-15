; ModuleID = 'tab.c'
source_filename = "tab.c"
target datalayout = "e-m:x-p:32:32-i64:64-f80:32-n8:16:32-a:0:32-S32"
target triple = "i686-pc-windows-msvc"

%struct.TValue = type { %union.Value, i32 }
%union.Value = type { i64 }
%struct.Table = type { %struct.GCObject*, i8, i8, i8, i8, i32, %struct.TValue*, %struct.Node*, %struct.Node*, %struct.Table*, %struct.GCObject*, %struct.RaviArray, i32 }
%struct.Node = type { %struct.TValue, %union.TKey }
%union.TKey = type { %struct.anon.2 }
%struct.anon.2 = type { %union.Value, i32, i32 }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.RaviArray = type { i8*, i32, i32, i32 }

; Function Attrs: norecurse nounwind readonly
define i32 @tablevalue(%struct.TValue* nocapture readonly %v) local_unnamed_addr #0 {
entry:
  %0 = bitcast %struct.TValue* %v to %struct.Table**
  %1 = load %struct.Table*, %struct.Table** %0, align 8, !tbaa !1
  %sizearray = getelementptr inbounds %struct.Table, %struct.Table* %1, i32 0, i32 5
  %2 = load i32, i32* %sizearray, align 8, !tbaa !5
  ret i32 %2
}

attributes #0 = { norecurse nounwind readonly "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-features"="+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.9.0 (trunk)"}
!1 = !{!2, !2, i64 0}
!2 = !{!"any pointer", !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
!5 = !{!6, !7, i64 8}
!6 = !{!"Table", !2, i64 0, !3, i64 4, !3, i64 5, !3, i64 6, !3, i64 7, !7, i64 8, !2, i64 12, !2, i64 16, !2, i64 20, !2, i64 24, !2, i64 28, !8, i64 32, !7, i64 48}
!7 = !{!"int", !3, i64 0}
!8 = !{!"RaviArray", !2, i64 0, !3, i64 4, !7, i64 8, !7, i64 12}
