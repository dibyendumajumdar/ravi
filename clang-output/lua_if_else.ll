; ModuleID = 'lua_if_else.c'
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

@.str = private unnamed_addr constant [12 x i8] c"value = %d\0A\00", align 1
@Proto = common global %struct.Proto zeroinitializer, align 4

; Function Attrs: nounwind
define void @testfunc(%struct.GCObject* nocapture readonly %obj) #0 {
entry:
  %tt = getelementptr inbounds %struct.GCObject* %obj, i32 0, i32 1
  %0 = load i8* %tt, align 1, !tbaa !1
  %conv = zext i8 %0 to i32
  %call = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([12 x i8]* @.str, i32 0, i32 0), i32 %conv) #2
  ret void
}

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) #0

; Function Attrs: nounwind
define void @test1(%struct.lua_State* %L) #0 {
entry:
  %ci1 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 6
  %0 = load %struct.CallInfoLua** %ci1, align 4, !tbaa !6
  %base2 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 4, i32 0
  %1 = load %struct.TValue** %base2, align 4, !tbaa !12
  %2 = bitcast %struct.CallInfoLua* %0 to %struct.LClosure***
  %3 = load %struct.LClosure*** %2, align 4, !tbaa !15
  %4 = load %struct.LClosure** %3, align 4, !tbaa !16
  %p = getelementptr inbounds %struct.LClosure* %4, i32 0, i32 5
  %5 = load %struct.Proto** %p, align 4, !tbaa !17
  %k3 = getelementptr inbounds %struct.Proto* %5, i32 0, i32 14
  %6 = load %struct.TValue** %k3, align 4, !tbaa !19
  %call = tail call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %1, %struct.TValue* %6) #2
  %7 = load %struct.TValue** %base2, align 4, !tbaa !12
  %cmp = icmp eq i32 %call, 0
  br i1 %cmp, label %label6, label %label3

label3:                                           ; preds = %entry
  %add.ptr16 = getelementptr inbounds %struct.TValue* %7, i32 1
  %add.ptr17 = getelementptr inbounds %struct.TValue* %6, i32 1
  %8 = bitcast %struct.TValue* %add.ptr16 to i8*
  %9 = bitcast %struct.TValue* %add.ptr17 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %8, i8* %9, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr20 = getelementptr inbounds %struct.TValue* %7, i32 2
  %top = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr20, %struct.TValue** %top, align 4, !tbaa !26
  %10 = load %struct.Proto** %p, align 4, !tbaa !17
  %sizep = getelementptr inbounds %struct.Proto* %10, i32 0, i32 10
  %11 = load i32* %sizep, align 4, !tbaa !27
  %cmp22 = icmp sgt i32 %11, 0
  br i1 %cmp22, label %if.then23, label %if.end25

if.then23:                                        ; preds = %label3
  %call24 = tail call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %7) #2
  br label %if.end25

if.end25:                                         ; preds = %if.then23, %label3
  %call26 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr16) #2
  %tobool = icmp eq i32 %call26, 0
  br i1 %tobool, label %return, label %if.then27

if.then27:                                        ; preds = %if.end25
  %top28 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %12 = load %struct.TValue** %top28, align 4, !tbaa !28
  store %struct.TValue* %12, %struct.TValue** %top, align 4, !tbaa !26
  br label %return

label6:                                           ; preds = %entry
  %add.ptr33 = getelementptr inbounds %struct.TValue* %6, i32 2
  %call34 = tail call i32 bitcast (i32 (...)* @luaV_equalobj to i32 (%struct.lua_State*, %struct.TValue*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %7, %struct.TValue* %add.ptr33) #2
  %13 = load %struct.TValue** %base2, align 4, !tbaa !12
  %cmp37 = icmp eq i32 %call34, 0
  %add.ptr67 = getelementptr inbounds %struct.TValue* %13, i32 1
  br i1 %cmp37, label %label11, label %label8

label8:                                           ; preds = %label6
  %add.ptr50 = getelementptr inbounds %struct.TValue* %6, i32 3
  %14 = bitcast %struct.TValue* %add.ptr67 to i8*
  %15 = bitcast %struct.TValue* %add.ptr50 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %14, i8* %15, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr53 = getelementptr inbounds %struct.TValue* %13, i32 2
  %top54 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr53, %struct.TValue** %top54, align 4, !tbaa !26
  %16 = load %struct.Proto** %p, align 4, !tbaa !17
  %sizep56 = getelementptr inbounds %struct.Proto* %16, i32 0, i32 10
  %17 = load i32* %sizep56, align 4, !tbaa !27
  %cmp57 = icmp sgt i32 %17, 0
  br i1 %cmp57, label %if.then58, label %if.end60

if.then58:                                        ; preds = %label8
  %call59 = tail call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %13) #2
  br label %if.end60

if.end60:                                         ; preds = %if.then58, %label8
  %call61 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr67) #2
  %tobool62 = icmp eq i32 %call61, 0
  br i1 %tobool62, label %return, label %if.then63

if.then63:                                        ; preds = %if.end60
  %top64 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %18 = load %struct.TValue** %top64, align 4, !tbaa !28
  store %struct.TValue* %18, %struct.TValue** %top54, align 4, !tbaa !26
  br label %return

label11:                                          ; preds = %label6
  %add.ptr68 = getelementptr inbounds %struct.TValue* %6, i32 4
  %19 = bitcast %struct.TValue* %add.ptr67 to i8*
  %20 = bitcast %struct.TValue* %add.ptr68 to i8*
  tail call void @llvm.memcpy.p0i8.p0i8.i32(i8* %19, i8* %20, i32 16, i32 8, i1 false), !tbaa.struct !21
  %add.ptr71 = getelementptr inbounds %struct.TValue* %13, i32 2
  %top72 = getelementptr inbounds %struct.lua_State* %L, i32 0, i32 4
  store %struct.TValue* %add.ptr71, %struct.TValue** %top72, align 4, !tbaa !26
  %21 = load %struct.Proto** %p, align 4, !tbaa !17
  %sizep74 = getelementptr inbounds %struct.Proto* %21, i32 0, i32 10
  %22 = load i32* %sizep74, align 4, !tbaa !27
  %cmp75 = icmp sgt i32 %22, 0
  br i1 %cmp75, label %if.then76, label %if.end78

if.then76:                                        ; preds = %label11
  %call77 = tail call i32 bitcast (i32 (...)* @luaF_close to i32 (%struct.lua_State*, %struct.TValue*)*)(%struct.lua_State* %L, %struct.TValue* %13) #2
  br label %if.end78

if.end78:                                         ; preds = %if.then76, %label11
  %call79 = tail call i32 @luaD_poscall(%struct.lua_State* %L, %struct.TValue* %add.ptr67) #2
  %tobool80 = icmp eq i32 %call79, 0
  br i1 %tobool80, label %return, label %if.then81

if.then81:                                        ; preds = %if.end78
  %top82 = getelementptr inbounds %struct.CallInfoLua* %0, i32 0, i32 1
  %23 = load %struct.TValue** %top82, align 4, !tbaa !28
  store %struct.TValue* %23, %struct.TValue** %top72, align 4, !tbaa !26
  br label %return

return:                                           ; preds = %if.end78, %if.end60, %if.end25, %if.then81, %if.then63, %if.then27
  ret void
}

declare i32 @luaV_equalobj(...) #1

declare i32 @luaF_close(...) #1

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i32(i8* nocapture, i8* nocapture readonly, i32, i32, i1) #2

declare i32 @luaD_poscall(%struct.lua_State*, %struct.TValue*) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.6.0 (trunk)"}
!1 = metadata !{metadata !2, metadata !4, i64 4}
!2 = metadata !{metadata !"GCObject", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5}
!3 = metadata !{metadata !"any pointer", metadata !4, i64 0}
!4 = metadata !{metadata !"omnipotent char", metadata !5, i64 0}
!5 = metadata !{metadata !"Simple C/C++ TBAA"}
!6 = metadata !{metadata !7, metadata !3, i64 16}
!7 = metadata !{metadata !"lua_State", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, metadata !3, i64 16, metadata !3, i64 20, metadata !3, i64 24, metadata !3, i64 28, metadata !3, i64 32, metadata !3, i64 36, metadata !3, i64 40, metadata !3, i64 44, metadata !8, i64 48, metadata !3, i64 104, metadata !9, i64 112, metadata !11, i64 120, metadata !11, i64 124, metadata !11, i64 128, metadata !10, i64 132, metadata !10, i64 134, metadata !4, i64 136, metadata !4, i64 137}
!8 = metadata !{metadata !"CallInfo", metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, metadata !3, i64 12, metadata !4, i64 16, metadata !9, i64 40, metadata !10, i64 48, metadata !4, i64 50}
!9 = metadata !{metadata !"long long", metadata !4, i64 0}
!10 = metadata !{metadata !"short", metadata !4, i64 0}
!11 = metadata !{metadata !"int", metadata !4, i64 0}
!12 = metadata !{metadata !13, metadata !3, i64 16}
!13 = metadata !{metadata !"CallInfoLua", metadata !3, i64 0, metadata !3, i64 4, metadata !3, i64 8, metadata !3, i64 12, metadata !14, i64 16, metadata !9, i64 32, metadata !10, i64 40, metadata !4, i64 42}
!14 = metadata !{metadata !"CallInfoL", metadata !3, i64 0, metadata !3, i64 4, metadata !9, i64 8}
!15 = metadata !{metadata !13, metadata !3, i64 0}
!16 = metadata !{metadata !3, metadata !3, i64 0}
!17 = metadata !{metadata !18, metadata !3, i64 12}
!18 = metadata !{metadata !"LClosure", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !3, i64 8, metadata !3, i64 12, metadata !4, i64 16}
!19 = metadata !{metadata !20, metadata !3, i64 44}
!20 = metadata !{metadata !"Proto", metadata !3, i64 0, metadata !4, i64 4, metadata !4, i64 5, metadata !4, i64 6, metadata !4, i64 7, metadata !4, i64 8, metadata !11, i64 12, metadata !11, i64 16, metadata !11, i64 20, metadata !11, i64 24, metadata !11, i64 28, metadata !11, i64 32, metadata !11, i64 36, metadata !11, i64 40, metadata !3, i64 44, metadata !3, i64 48, metadata !3, i64 52, metadata !3, i64 56, metadata !3, i64 60, metadata !3, i64 64, metadata !3, i64 68, metadata !3, i64 72, metadata !3, i64 76}
!21 = metadata !{i64 0, i64 4, metadata !16, i64 0, i64 4, metadata !16, i64 0, i64 4, metadata !22, i64 0, i64 4, metadata !16, i64 0, i64 8, metadata !23, i64 0, i64 8, metadata !24, i64 8, i64 4, metadata !22}
!22 = metadata !{metadata !11, metadata !11, i64 0}
!23 = metadata !{metadata !9, metadata !9, i64 0}
!24 = metadata !{metadata !25, metadata !25, i64 0}
!25 = metadata !{metadata !"double", metadata !4, i64 0}
!26 = metadata !{metadata !7, metadata !3, i64 8}
!27 = metadata !{metadata !20, metadata !11, i64 28}
!28 = metadata !{metadata !13, metadata !3, i64 4}
