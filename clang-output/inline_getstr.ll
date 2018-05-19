; ModuleID = 'inline_getstr.c'
source_filename = "inline_getstr.c"
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%struct.TValue = type { %union.Value, i16 }
%union.Value = type { %struct.GCObject* }
%struct.GCObject = type { %struct.GCObject*, i8, i8 }
%struct.Table = type { %struct.GCObject*, i8, i8, i8, i8, i32, %struct.TValue*, %struct.Node*, %struct.Node*, %struct.Table*, %struct.GCObject*, %struct.RaviArray, i32 }
%struct.Node = type { %struct.TValue, %union.TKey }
%union.TKey = type { %struct.anon.1 }
%struct.anon.1 = type { %union.Value, i16, i32 }
%struct.RaviArray = type { i8*, i32, i32, i8, i8 }
%struct.TString = type { %struct.GCObject*, i8, i8, i8, i8, i32, %union.anon.2 }
%union.anon.2 = type { i64 }

; Function Attrs: nounwind
define %struct.TValue* @ravi_getstr(%struct.Table* %t, %struct.TString* %key) #0 {
  %retval = alloca %struct.TValue*, align 8
  %key.addr = alloca %struct.TString*, align 8
  %t.addr = alloca %struct.Table*, align 8
  %n = alloca %struct.Node*, align 8
  %k = alloca %struct.TValue*, align 8
  %cleanup.dest.slot = alloca i32
  store %struct.TString* %key, %struct.TString** %key.addr, align 8, !tbaa !2
  store %struct.Table* %t, %struct.Table** %t.addr, align 8, !tbaa !2
  %1 = bitcast %struct.Node** %n to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %1) #3
  %2 = load %struct.Table*, %struct.Table** %t.addr, align 8, !tbaa !2
  %node = getelementptr inbounds %struct.Table, %struct.Table* %2, i32 0, i32 7
  %3 = load %struct.Node*, %struct.Node** %node, align 8, !tbaa !6
  %4 = load %struct.TString*, %struct.TString** %key.addr, align 8, !tbaa !2
  %hash = getelementptr inbounds %struct.TString, %struct.TString* %4, i32 0, i32 5
  %5 = load i32, i32* %hash, align 4, !tbaa !10
  %6 = load %struct.Table*, %struct.Table** %t.addr, align 8, !tbaa !2
  %hmask = getelementptr inbounds %struct.Table, %struct.Table* %6, i32 0, i32 12
  %7 = load i32, i32* %hmask, align 8, !tbaa !12
  %and = and i32 %5, %7
  %idxprom = zext i32 %and to i64
  %arrayidx = getelementptr inbounds %struct.Node, %struct.Node* %3, i64 %idxprom
  store %struct.Node* %arrayidx, %struct.Node** %n, align 8, !tbaa !2
  %8 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %8) #3
  %9 = load %struct.Node*, %struct.Node** %n, align 8, !tbaa !2
  %i_key = getelementptr inbounds %struct.Node, %struct.Node* %9, i32 0, i32 1
  %tvk = bitcast %union.TKey* %i_key to %struct.TValue*
  store %struct.TValue* %tvk, %struct.TValue** %k, align 8, !tbaa !2
  %10 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %tt_ = getelementptr inbounds %struct.TValue, %struct.TValue* %10, i32 0, i32 1
  %11 = load i16, i16* %tt_, align 8, !tbaa !13
  %conv = zext i16 %11 to i32
  %cmp = icmp eq i32 %conv, 32772
  br i1 %cmp, label %12, label %19

; <label>:12:                                     ; preds = %0
  %13 = load %struct.TValue*, %struct.TValue** %k, align 8, !tbaa !2
  %value_ = getelementptr inbounds %struct.TValue, %struct.TValue* %13, i32 0, i32 0
  %gc = bitcast %union.Value* %value_ to %struct.GCObject**
  %14 = load %struct.GCObject*, %struct.GCObject** %gc, align 8, !tbaa !16
  %15 = bitcast %struct.GCObject* %14 to %struct.TString*
  %16 = load %struct.TString*, %struct.TString** %key.addr, align 8, !tbaa !2
  %cmp2 = icmp eq %struct.TString* %15, %16
  br i1 %cmp2, label %17, label %19

; <label>:17:                                     ; preds = %12
  %18 = load %struct.Node*, %struct.Node** %n, align 8, !tbaa !2
  %i_val = getelementptr inbounds %struct.Node, %struct.Node* %18, i32 0, i32 0
  store %struct.TValue* %i_val, %struct.TValue** %retval, align 8
  store i32 1, i32* %cleanup.dest.slot, align 4
  br label %22

; <label>:19:                                     ; preds = %12, %0
  %20 = load %struct.TString*, %struct.TString** %key.addr, align 8, !tbaa !2
  %21 = load %struct.Table*, %struct.Table** %t.addr, align 8, !tbaa !2
  %call = call %struct.TValue* @luaH_getstr(%struct.Table* %21, %struct.TString* %20)
  store %struct.TValue* %call, %struct.TValue** %retval, align 8
  store i32 1, i32* %cleanup.dest.slot, align 4
  br label %22

; <label>:22:                                     ; preds = %19, %17
  %23 = bitcast %struct.TValue** %k to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %23) #3
  %24 = bitcast %struct.Node** %n to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %24) #3
  %25 = load %struct.TValue*, %struct.TValue** %retval, align 8
  ret %struct.TValue* %25
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #1

declare %struct.TValue* @luaH_getstr(%struct.Table*, %struct.TString*) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{!"clang version 6.0.0 (tags/RELEASE_600/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"any pointer", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !3, i64 24}
!7 = !{!"Table", !3, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !4, i64 11, !8, i64 12, !3, i64 16, !3, i64 24, !3, i64 32, !3, i64 40, !3, i64 48, !9, i64 56, !8, i64 80}
!8 = !{!"int", !4, i64 0}
!9 = !{!"RaviArray", !3, i64 0, !8, i64 8, !8, i64 12, !4, i64 16, !4, i64 17}
!10 = !{!11, !8, i64 12}
!11 = !{!"TString", !3, i64 0, !4, i64 8, !4, i64 9, !4, i64 10, !4, i64 11, !8, i64 12, !4, i64 16}
!12 = !{!7, !8, i64 80}
!13 = !{!14, !15, i64 8}
!14 = !{!"TValue", !4, i64 0, !15, i64 8}
!15 = !{!"short", !4, i64 0}
!16 = !{!4, !4, i64 0}
