; ModuleID = 'ravi_module_ravif1'
target triple = "x86_64-pc-windows-msvc-elf"

%0 = type { %ravi.TValue*, i32*, i64 }
%ravi.lua_State = type { %ravi.GCObject*, i8, i8, i8, %ravi.TValue*, %ravi.globa
l_State*, %ravi.CallInfo*, i32*, %ravi.TValue*, %ravi.TValue*, %ravi.UpVal*, %ra
vi.GCObject*, %ravi.lua_State*, %ravi.lua_longjmp*, %ravi.CallInfo, i8* (%ravi.l
ua_State*, %ravi.lua_Debug*)*, i64, i32, i32, i32, i16, i16, i8, i8 }
%ravi.global_State = type opaque
%ravi.TValue = type { %ravi.Value, i32 }
%ravi.Value = type { double }
%ravi.UpVal = type opaque
%ravi.GCObject = type { %ravi.GCObject*, i8, i8 }
%ravi.lua_longjmp = type opaque
%ravi.CallInfo = type { %ravi.TValue*, %ravi.TValue*, %ravi.CallInfo*, %ravi.Cal
lInfo*, %0, i64, i16, i8 }
%ravi.lua_Debug = type opaque
%ravi.LClosure = type { %ravi.GCObject*, i8, i8, i8, %ravi.GCObject*, %ravi.Prot
o*, [1 x %ravi.UpVal*] }
%ravi.Proto = type { %ravi.GCObject*, i8, i8, i8, i8, i8, i32, i32, i32, i32, i3
2, i32, i32, i32, %ravi.TValue*, i32*, %ravi.Proto**, i32*, %ravi.LocVar*, %ravi
.Upvaldesc*, %ravi.LClosure*, %ravi.TString*, %ravi.GCObject*, %ravi.RaviJITProt
o* }
%ravi.LocVar = type { %ravi.TString*, i32, i32, i32 }
%ravi.Upvaldesc = type { %ravi.TString*, i8, i8 }
%ravi.TString = type { %ravi.GCObject*, i8, i8, i8, i32, i64, %ravi.TString* }
%ravi.RaviJITProto = type opaque

define i32 @ravif1(%ravi.lua_State* %L) {
entry:
  %L_ci = getelementptr inbounds %ravi.lua_State* %L, i64 0, i32 6
  %0 = load %ravi.CallInfo** %L_ci, align 8
  %1 = bitcast %ravi.CallInfo* %0 to %ravi.LClosure***
  %2 = load %ravi.LClosure*** %1, align 8
  %3 = load %ravi.LClosure** %2, align 8
  %Proto = getelementptr inbounds %ravi.LClosure* %3, i64 0, i32 5
  %4 = load %ravi.Proto** %Proto, align 8
  %k = getelementptr inbounds %ravi.Proto* %4, i64 0, i32 14
  %5 = load %ravi.TValue** %k, align 8
  %base = getelementptr inbounds %ravi.CallInfo* %0, i64 0, i32 4, i32 0
  %6 = load %ravi.TValue** %base, align 8
  %srcvalue = getelementptr inbounds %ravi.TValue* %5, i64 0, i32 0, i32 0
  %destvalue = getelementptr inbounds %ravi.TValue* %6, i64 0, i32 0, i32 0
  %7 = load double* %srcvalue, align 8
  store double %7, double* %destvalue, align 8
  %srctype = getelementptr inbounds %ravi.TValue* %5, i64 0, i32 1
  %desttype = getelementptr inbounds %ravi.TValue* %6, i64 0, i32 1
  %8 = load i32* %srctype, align 4
  store i32 %8, i32* %desttype, align 4
  %9 = load %ravi.CallInfo** %L_ci, align 8
  %base1 = getelementptr inbounds %ravi.CallInfo* %9, i64 0, i32 4, i32 0
  %10 = load %ravi.TValue** %base1, align 8
  %srcvalue2 = getelementptr inbounds %ravi.TValue* %5, i64 1, i32 0, i32 0
  %destvalue3 = getelementptr inbounds %ravi.TValue* %10, i64 1, i32 0, i32 0
  %11 = load double* %srcvalue2, align 8
  store double %11, double* %destvalue3, align 8
  %srctype4 = getelementptr inbounds %ravi.TValue* %5, i64 1, i32 1
  %desttype5 = getelementptr inbounds %ravi.TValue* %10, i64 1, i32 1
  %12 = load i32* %srctype4, align 4
  store i32 %12, i32* %desttype5, align 4
  %13 = load %ravi.CallInfo** %L_ci, align 8
  %base6 = getelementptr inbounds %ravi.CallInfo* %13, i64 0, i32 4, i32 0
  %14 = load %ravi.TValue** %base6, align 8
  %srcvalue7 = getelementptr inbounds %ravi.TValue* %5, i64 2, i32 0, i32 0
  %destvalue8 = getelementptr inbounds %ravi.TValue* %14, i64 2, i32 0, i32 0
  %15 = load double* %srcvalue7, align 8
  store double %15, double* %destvalue8, align 8
  %srctype9 = getelementptr inbounds %ravi.TValue* %5, i64 2, i32 1
  %desttype10 = getelementptr inbounds %ravi.TValue* %14, i64 2, i32 1
  %16 = load i32* %srctype9, align 4
  store i32 %16, i32* %desttype10, align 4
  %17 = load %ravi.CallInfo** %L_ci, align 8
  %base11 = getelementptr inbounds %ravi.CallInfo* %17, i64 0, i32 4, i32 0
  %18 = load %ravi.TValue** %base11, align 8
  %19 = getelementptr inbounds %ravi.TValue* %18, i64 1
  %20 = getelementptr inbounds %ravi.TValue* %18, i64 3
  %L_top = getelementptr inbounds %ravi.lua_State* %L, i64 0, i32 4
  store %ravi.TValue* %20, %ravi.TValue** %L_top, align 8
  %sizep = getelementptr inbounds %ravi.Proto* %4, i64 0, i32 10
  %21 = load i32* %sizep, align 4
  %22 = icmp sgt i32 %21, 0
  br i1 %22, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  call void @luaF_close(%ravi.lua_State* %L, %ravi.TValue* %18)
  br label %if.else

if.else:                                          ; preds = %if.then, %entry
  %23 = call i32 @luaD_poscall(%ravi.lua_State* %L, %ravi.TValue* %19)
  %24 = icmp eq i32 %23, 0
  br i1 %24, label %if.else13, label %if.then12

if.then12:                                        ; preds = %if.else
  %ci_top = getelementptr inbounds %ravi.CallInfo* %17, i64 0, i32 1
  %25 = load %ravi.TValue** %ci_top, align 8
  store %ravi.TValue* %25, %ravi.TValue** %L_top, align 8
  br label %if.else13

if.else13:                                        ; preds = %if.else, %if.then12

  ret i32 1
}

declare i32 @luaD_poscall(%ravi.lua_State*, %ravi.TValue*)

declare void @luaF_close(%ravi.lua_State*, %ravi.TValue*)

declare i32 @luaV_equalobj(%ravi.lua_State*, %ravi.TValue*, %ravi.TValue*)

declare i32 @luaV_lessthan(%ravi.lua_State*, %ravi.TValue*, %ravi.TValue*)

declare i32 @luaV_lessequal(%ravi.lua_State*, %ravi.TValue*, %ravi.TValue*)

declare void @luaG_runerror(%ravi.lua_State*, i8*, ...)