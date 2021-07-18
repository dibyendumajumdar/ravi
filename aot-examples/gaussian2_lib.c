/* Following is an IR Dump from the compiler
define Proc%1
L0 (entry)
	LOADGLOBAL {Upval(_ENV), 'assert' Ks(0)} {T(0)}
	MOV {T(0)} {local(assert, 0)}
	LOADGLOBAL {Upval(_ENV), 'table' Ks(1)} {T(0)}
	GETsk {T(0), 'slice' Ks(2)} {T(1)}
	LOADGLOBAL {Upval(_ENV), 'table' Ks(1)} {T(2)}
	GETsk {T(2), 'numarray' Ks(3)} {T(3)}
	LOADGLOBAL {Upval(_ENV), 'table' Ks(1)} {T(4)}
	GETsk {T(4), 'intarray' Ks(4)} {T(5)}
	MOV {T(1)} {local(slice, 1)}
	MOV {T(3)} {local(numarray, 2)}
	MOV {T(5)} {local(intarray, 3)}
	LOADGLOBAL {Upval(_ENV), 'io' Ks(5)} {T(5)}
	GETsk {T(5), 'write' Ks(6)} {T(3)}
	MOV {T(3)} {local(write, 4)}
	CLOSURE {Proc%2} {T(3)}
	MOV {T(3)} {local(copy, 5)}
	CLOSURE {Proc%3} {T(3)}
	MOV {T(3)} {local(partial_pivot, 6)}
	CLOSURE {Proc%4} {T(3)}
	MOV {T(3)} {local(dump_matrix, 7)}
	CLOSURE {Proc%5} {T(3)}
	MOV {T(3)} {local(gaussian_solve, 8)}
	NEWTABLE {T(3)}
	TPUTsk {local(gaussian_solve, 8)} {T(3), 'gaussian_solve' Ks(7)}
	RET {T(3)} {L1}
L1 (exit)
define Proc%2
L0 (entry)
	TOFARRAY {local(a, 0)}
	MOV {Upval(0, Proc%1, numarray)} {T(0)}
	LENi {local(a, 0)} {Tint(0)}
	CALL {T(0), Tint(0), 0.000000000000 Kflt(0)} {T(0..), 1 Kint(0)}
	TOFARRAY {T(0[0..])}
	MOV {T(0[0..])} {local(c, 1)}
	MOV {1 Kint(0)} {Tint(1)}
	LENi {local(a, 0)} {Tint(2)}
	MOV {Tint(2)} {Tint(3)}
	MOV {1 Kint(0)} {Tint(4)}
	SUBii {Tint(1), Tint(4)} {Tint(1)}
	BR {L2}
L1 (exit)
L2
	ADDii {Tint(1), Tint(4)} {Tint(1)}
	BR {L3}
L3
	LIii {Tint(3), Tint(1)} {Tbool(5)}
	CBR {Tbool(5)} {L5, L4}
L4
	MOV {Tint(1)} {Tint(0)}
	FAGETik {local(a, 0), Tint(0)} {Tflt(1)}
	FAPUTfv {Tflt(1)} {local(c, 1), Tint(0)}
	BR {L2}
L5
	RET {local(c, 1)} {L1}
define Proc%3
L0 (entry)
	TOTABLE {local(columns, 0)}
	TOIARRAY {local(nrow, 1)}
	TOINT {local(i, 2)}
	TOINT {local(n, 3)}
	MOV {local(i, 2)} {Tint(1)}
	MOVi {Tint(1)} {Tint(0)}
	MOVf {0.000000000000 Kflt(0)} {Tflt(0)}
	TGETik {local(columns, 0), local(i, 2)} {T(0)}
	TOFARRAY {T(0)}
	MOV {T(0)} {local(a, 4)}
	MOV {false} {local(max_set, 5)}
	MOV {local(i, 2)} {Tint(2)}
	MOV {local(n, 3)} {Tint(3)}
	MOV {1 Kint(0)} {Tint(4)}
	SUBii {Tint(2), Tint(4)} {Tint(2)}
	BR {L2}
L1 (exit)
L2
	ADDii {Tint(2), Tint(4)} {Tint(2)}
	BR {L3}
L3
	LIii {Tint(3), Tint(2)} {Tbool(5)}
	CBR {Tbool(5)} {L5, L4}
L4
	MOV {Tint(2)} {Tint(1)}
	IAGETik {local(nrow, 1), Tint(1)} {Tint(6)}
	FAGETik {local(a, 4), Tint(6)} {Tflt(2)}
	MOVf {Tflt(2)} {Tflt(1)}
	BR {L6}
L5
	BR {L14}
L6
	LTff {Tflt(1), 0.000000000000 Kflt(0)} {Tbool(7)}
	CBR {Tbool(7)} {L7, L8}
L7
	UNMf {Tflt(1)} {Tflt(2)}
	MOVf {Tflt(2)} {Tflt(1)}
	BR {L8}
L8
	BR {L9}
L9
	NOT {local(max_set, 5)} {T(0)}
	CBR {T(0)} {L11, L10}
L10
	LTff {Tflt(0), Tflt(1)} {Tbool(8)}
	CBR {Tbool(8)} {L12, L13}
L11
	MOVf {Tflt(1)} {Tflt(0)}
	MOV {true} {local(max_set, 5)}
	MOVi {Tint(1)} {Tint(0)}
	BR {L13}
L12
	MOVi {Tint(1)} {Tint(0)}
	MOVf {Tflt(1)} {Tflt(0)}
	BR {L13}
L13
	BR {L2}
L14
	FAGETik {local(a, 4), Tint(0)} {Tflt(1)}
	EQff {Tflt(1), 0.000000000000 Kflt(0)} {Tbool(2)}
	CBR {Tbool(2)} {L15, L16}
L15
	LOADGLOBAL {Upval(_ENV), 'error' Ks(0)} {T(1)}
	MOV {T(1)} {T(2)}
	CALL {T(2), 'no unique solution exists' Ks(1)} {T(2..), 1 Kint(0)}
	BR {L16}
L16
	BR {L17}
L17
	IAGETik {local(nrow, 1), local(i, 2)} {Tint(3)}
	IAGETik {local(nrow, 1), Tint(0)} {Tint(4)}
	EQii {Tint(3), Tint(4)} {Tbool(5)}
	NOT {Tbool(5)} {Tbool(1)}
	CBR {Tbool(1)} {L18, L19}
L18
	MOV {Upval(1, Proc%1, write)} {T(3)}
	CALL {T(3), 'Performing row interchange ' Ks(2), local(i, 2), ' will be swapped with ' Ks(3), Tint(0), '\n' Ks(4)} {T(3..), 1 Kint(0)}
	IAGETik {local(nrow, 1), local(i, 2)} {Tint(3)}
	MOVi {Tint(3)} {Tint(4)}
	IAGETik {local(nrow, 1), Tint(0)} {Tint(5)}
	IAPUTiv {Tint(5)} {local(nrow, 1), local(i, 2)}
	IAPUTiv {Tint(4)} {local(nrow, 1), Tint(0)}
	BR {L19}
L19
	RET {L1}
define Proc%4
L0 (entry)
	TOTABLE {local(columns, 0)}
	TOINT {local(m, 1)}
	TOINT {local(n, 2)}
	TOIARRAY {local(nrow, 3)}
	MOV {1 Kint(0)} {Tint(1)}
	MOV {local(m, 1)} {Tint(2)}
	MOV {1 Kint(0)} {Tint(3)}
	SUBii {Tint(1), Tint(3)} {Tint(1)}
	BR {L2}
L1 (exit)
L2
	ADDii {Tint(1), Tint(3)} {Tint(1)}
	BR {L3}
L3
	LIii {Tint(2), Tint(1)} {Tbool(4)}
	CBR {Tbool(4)} {L5, L4}
L4
	MOV {Tint(1)} {Tint(0)}
	MOV {1 Kint(0)} {Tint(6)}
	MOV {local(n, 2)} {Tint(7)}
	MOV {1 Kint(0)} {Tint(8)}
	SUBii {Tint(6), Tint(8)} {Tint(6)}
	BR {L6}
L5
	RET {L1}
L6
	ADDii {Tint(6), Tint(8)} {Tint(6)}
	BR {L7}
L7
	LIii {Tint(7), Tint(6)} {Tbool(9)}
	CBR {Tbool(9)} {L9, L8}
L8
	MOV {Tint(6)} {Tint(5)}
	MOV {Upval(0, Proc%1, write)} {T(0)}
	TGETik {local(columns, 0), Tint(5)} {T(1)}
	IAGETik {local(nrow, 3), Tint(0)} {Tint(10)}
	GETik {T(1), Tint(10)} {T(2)}
	CALL {T(0), T(2), ' ' Ks(0)} {T(0..), 1 Kint(0)}
	BR {L6}
L9
	MOV {Upval(0, Proc%1, write)} {T(2)}
	CALL {T(2), '\n' Ks(1)} {T(2..), 1 Kint(0)}
	BR {L2}
define Proc%5
L0 (entry)
	TOFARRAY {local(A, 0)}
	TOFARRAY {local(b, 1)}
	TOINT {local(m, 2)}
	TOINT {local(n, 3)}
	MOV {Upval(0, Proc%1, copy)} {T(0)}
	CALL {T(0), local(A, 0)} {T(0..), 1 Kint(0)}
	TOFARRAY {T(0[0..])}
	MOV {T(0[0..])} {local(A, 0)}
	MOV {Upval(0, Proc%1, copy)} {T(1)}
	CALL {T(1), local(b, 1)} {T(1..), 1 Kint(0)}
	TOFARRAY {T(1[1..])}
	MOV {T(1[1..])} {local(b, 1)}
	MOV {Upval(1, Proc%1, assert)} {T(2)}
	EQii {local(m, 2), local(n, 3)} {Tbool(0)}
	CALL {T(2), Tbool(0)} {T(2..), 1 Kint(0)}
	MOV {Upval(1, Proc%1, assert)} {T(3)}
	LENi {local(b, 1)} {Tint(0)}
	EQii {Tint(0), local(m, 2)} {Tbool(1)}
	CALL {T(3), Tbool(1)} {T(3..), 1 Kint(0)}
	MOV {Upval(2, Proc%1, intarray)} {T(4)}
	CALL {T(4), local(n, 3)} {T(4..), 1 Kint(0)}
	TOIARRAY {T(4[4..])}
	MOV {T(4[4..])} {local(nrow, 4)}
	NEWTABLE {T(5)}
	MOV {T(5)} {local(columns, 5)}
	MOV {1 Kint(0)} {Tint(0)}
	MOV {local(n, 3)} {Tint(2)}
	MOV {1 Kint(0)} {Tint(3)}
	SUBii {Tint(0), Tint(3)} {Tint(0)}
	BR {L2}
L1 (exit)
L2
	ADDii {Tint(0), Tint(3)} {Tint(0)}
	BR {L3}
L3
	LIii {Tint(2), Tint(0)} {Tbool(4)}
	CBR {Tbool(4)} {L5, L4}
L4
	MOV {Tint(0)} {Tint(1)}
	MOV {Upval(3, Proc%1, slice)} {T(6)}
	SUBii {Tint(1), 1 Kint(0)} {Tint(5)}
	MULii {Tint(5), local(m, 2)} {Tint(6)}
	ADDii {Tint(6), 1 Kint(0)} {Tint(5)}
	CALL {T(6), local(A, 0), Tint(5), local(m, 2)} {T(6..), 1 Kint(0)}
	TPUTik {T(6[6..])} {local(columns, 5), Tint(1)}
	BR {L2}
L5
	ADDii {local(n, 3), 1 Kint(0)} {Tint(0)}
	MOV {local(b, 1)} {T(7)}
	TPUTik {T(7)} {local(columns, 5), Tint(0)}
	MOV {1 Kint(0)} {Tint(3)}
	MOV {local(n, 3)} {Tint(4)}
	MOV {1 Kint(0)} {Tint(1)}
	SUBii {Tint(3), Tint(1)} {Tint(3)}
	BR {L6}
L6
	ADDii {Tint(3), Tint(1)} {Tint(3)}
	BR {L7}
L7
	LIii {Tint(4), Tint(3)} {Tbool(5)}
	CBR {Tbool(5)} {L9, L8}
L8
	MOV {Tint(3)} {Tint(2)}
	IAPUTiv {Tint(2)} {local(nrow, 4), Tint(2)}
	BR {L6}
L9
	MOV {1 Kint(0)} {Tint(4)}
	SUBii {local(n, 3), 1 Kint(0)} {Tint(1)}
	MOV {Tint(1)} {Tint(5)}
	MOV {1 Kint(0)} {Tint(2)}
	SUBii {Tint(4), Tint(2)} {Tint(4)}
	BR {L10}
L10
	ADDii {Tint(4), Tint(2)} {Tint(4)}
	BR {L11}
L11
	LIii {Tint(5), Tint(4)} {Tbool(6)}
	CBR {Tbool(6)} {L13, L12}
L12
	MOV {Tint(4)} {Tint(3)}
	MOV {Upval(4, Proc%1, partial_pivot)} {T(7)}
	CALL {T(7), local(columns, 5), local(nrow, 4), Tint(3), local(m, 2)} {T(7..), 1 Kint(0)}
	MOV {Upval(5, Proc%1, dump_matrix)} {T(5)}
	ADDii {local(n, 3), 1 Kint(0)} {Tint(7)}
	CALL {T(5), local(columns, 5), local(n, 3), Tint(7), local(nrow, 4)} {T(5..), 1 Kint(0)}
	ADDii {Tint(3), 1 Kint(0)} {Tint(8)}
	MOV {Tint(8)} {Tint(9)}
	MOV {local(m, 2)} {Tint(10)}
	MOV {1 Kint(0)} {Tint(11)}
	SUBii {Tint(9), Tint(11)} {Tint(9)}
	BR {L14}
L13
	BR {L22}
L14
	ADDii {Tint(9), Tint(11)} {Tint(9)}
	BR {L15}
L15
	LIii {Tint(10), Tint(9)} {Tbool(12)}
	CBR {Tbool(12)} {L17, L16}
L16
	MOV {Tint(9)} {Tint(7)}
	TGETik {local(columns, 5), Tint(3)} {T(8)}
	TOFARRAY {T(8)}
	MOV {T(8)} {local(column, 8)}
	IAGETik {local(nrow, 4), Tint(7)} {Tint(13)}
	FAGETik {local(column, 8), Tint(13)} {Tflt(1)}
	IAGETik {local(nrow, 4), Tint(3)} {Tint(14)}
	FAGETik {local(column, 8), Tint(14)} {Tflt(2)}
	DIVff {Tflt(1), Tflt(2)} {Tflt(3)}
	MOVf {Tflt(3)} {Tflt(0)}
	MOV {Upval(6, Proc%1, write)} {T(8)}
	CONCAT {'m(' Ks(0), Tint(7), ',' Ks(1), Tint(3), ') = ' Ks(2)} {T(9)}
	IAGETik {local(nrow, 4), Tint(7)} {Tint(15)}
	FAGETik {local(column, 8), Tint(15)} {Tflt(3)}
	IAGETik {local(nrow, 4), Tint(3)} {Tint(16)}
	FAGETik {local(column, 8), Tint(16)} {Tflt(2)}
	CALL {T(8), T(9), Tflt(3), ' / ' Ks(3), Tflt(2), '\n' Ks(4)} {T(8..), 1 Kint(0)}
	MOV {Upval(6, Proc%1, write)} {T(9)}
	CONCAT {'Performing R(' Ks(5), Tint(7), ') = R(' Ks(6), Tint(7), ') - m(' Ks(7), Tint(7), ',' Ks(1), Tint(3), ') * R(' Ks(8), Tint(3), ')\n' Ks(9)} {T(10)}
	CALL {T(9), T(10)} {T(9..), 1 Kint(0)}
	MOV {Tint(3)} {Tint(18)}
	ADDii {local(n, 3), 1 Kint(0)} {Tint(19)}
	MOV {Tint(19)} {Tint(20)}
	MOV {1 Kint(0)} {Tint(21)}
	SUBii {Tint(18), Tint(21)} {Tint(18)}
	BR {L18}
L17
	MOV {Upval(6, Proc%1, write)} {T(10)}
	CALL {T(10), 'Post elimination column ' Ks(10), Tint(3), '\n' Ks(4)} {T(10..), 1 Kint(0)}
	MOV {Upval(5, Proc%1, dump_matrix)} {T(11)}
	ADDii {local(n, 3), 1 Kint(0)} {Tint(9)}
	CALL {T(11), local(columns, 5), local(n, 3), Tint(9), local(nrow, 4)} {T(11..), 1 Kint(0)}
	BR {L10}
L18
	ADDii {Tint(18), Tint(21)} {Tint(18)}
	BR {L19}
L19
	LIii {Tint(20), Tint(18)} {Tbool(22)}
	CBR {Tbool(22)} {L21, L20}
L20
	MOV {Tint(18)} {Tint(17)}
	TGETik {local(columns, 5), Tint(17)} {T(10)}
	TOFARRAY {T(10)}
	MOV {T(10)} {local(col, 9)}
	IAGETik {local(nrow, 4), Tint(7)} {Tint(23)}
	IAGETik {local(nrow, 4), Tint(7)} {Tint(24)}
	FAGETik {local(col, 9), Tint(24)} {Tflt(2)}
	IAGETik {local(nrow, 4), Tint(3)} {Tint(25)}
	FAGETik {local(col, 9), Tint(25)} {Tflt(1)}
	MULff {Tflt(0), Tflt(1)} {Tflt(4)}
	SUBff {Tflt(2), Tflt(4)} {Tflt(1)}
	FAPUTfv {Tflt(1)} {local(col, 9), Tint(23)}
	BR {L18}
L21
	BR {L14}
L22
	TGETik {local(columns, 5), local(n, 3)} {T(12)}
	IAGETik {local(nrow, 4), local(n, 3)} {Tint(4)}
	GETik {T(12), Tint(4)} {T(13)}
	EQ {T(13), 0.000000000000 Kflt(0)} {T(14)}
	CBR {T(14)} {L23, L24}
L23
	LOADGLOBAL {Upval(_ENV), 'error' Ks(11)} {T(13)}
	MOV {T(13)} {T(15)}
	CALL {T(15), 'no unique solution exists' Ks(12)} {T(15..), 1 Kint(0)}
	BR {L24}
L24
	MOV {Upval(8, Proc%1, numarray)} {T(16)}
	CALL {T(16), local(n, 3), 0.000000000000 Kflt(0)} {T(16..), 1 Kint(0)}
	TOFARRAY {T(16[16..])}
	MOV {T(16[16..])} {local(x, 6)}
	TGETik {local(columns, 5), local(n, 3)} {T(17)}
	TOFARRAY {T(17)}
	MOV {T(17)} {local(a, 7)}
	MOV {Upval(6, Proc%1, write)} {T(17)}
	CALL {T(17), 'Performing back substitution\n' Ks(13)} {T(17..), 1 Kint(0)}
	IAGETik {local(nrow, 4), local(n, 3)} {Tint(5)}
	FAGETik {local(b, 1), Tint(5)} {Tflt(1)}
	IAGETik {local(nrow, 4), local(n, 3)} {Tint(2)}
	FAGETik {local(a, 7), Tint(2)} {Tflt(3)}
	DIVff {Tflt(1), Tflt(3)} {Tflt(4)}
	FAPUTfv {Tflt(4)} {local(x, 6), local(n, 3)}
	MOV {Upval(6, Proc%1, write)} {T(18)}
	FAGETik {local(x, 6), local(n, 3)} {Tflt(4)}
	CALL {T(18), 'x[' Ks(14), local(n, 3), '] = b[' Ks(15), local(n, 3), '] / a[' Ks(16), local(n, 3), '] = ' Ks(17), Tflt(4), '\n' Ks(4)} {T(18..), 1 Kint(0)}
	SUBii {local(n, 3), 1 Kint(0)} {Tint(3)}
	MOV {Tint(3)} {Tint(9)}
	MOV {1 Kint(0)} {Tint(10)}
	MOV {-1 Kint(1)} {Tint(11)}
	LIii {0 Kint(2), Tint(11)} {Tbool(12)}
	SUBii {Tint(9), Tint(11)} {Tint(9)}
	BR {L25}
L25
	ADDii {Tint(9), Tint(11)} {Tint(9)}
	CBR {Tbool(12)} {L26, L27}
L26
	LIii {Tint(10), Tint(9)} {Tbool(7)}
	CBR {Tbool(7)} {L29, L28}
L27
	LIii {Tint(9), Tint(10)} {Tbool(7)}
	CBR {Tbool(7)} {L29, L28}
L28
	MOV {Tint(9)} {Tint(6)}
	INIT {Tflt(4)}
	ADDii {Tint(6), 1 Kint(0)} {Tint(20)}
	MOV {Tint(20)} {Tint(21)}
	MOV {local(n, 3)} {Tint(22)}
	MOV {1 Kint(0)} {Tint(17)}
	SUBii {Tint(21), Tint(17)} {Tint(21)}
	BR {L30}
L29
	RET {local(x, 6)} {L1}
L30
	ADDii {Tint(21), Tint(17)} {Tint(21)}
	BR {L31}
L31
	LIii {Tint(22), Tint(21)} {Tbool(26)}
	CBR {Tbool(26)} {L33, L32}
L32
	MOV {Tint(21)} {Tint(18)}
	TGETik {local(columns, 5), Tint(18)} {T(19)}
	TOFARRAY {T(19)}
	MOV {T(19)} {local(a, 7)}
	IAGETik {local(nrow, 4), Tint(6)} {Tint(27)}
	FAGETik {local(a, 7), Tint(27)} {Tflt(0)}
	FAGETik {local(x, 6), Tint(18)} {Tflt(3)}
	MULff {Tflt(0), Tflt(3)} {Tflt(1)}
	ADDff {Tflt(4), Tflt(1)} {Tflt(3)}
	MOVf {Tflt(3)} {Tflt(4)}
	BR {L34}
L33
	MOV {Upval(6, Proc%1, write)} {T(22)}
	CALL {T(22), 'sum = ' Ks(18), Tflt(4), '\n' Ks(4)} {T(22..), 1 Kint(0)}
	TGETik {local(columns, 5), Tint(6)} {T(23)}
	TOFARRAY {T(23)}
	MOV {T(23)} {local(a, 7)}
	IAGETik {local(nrow, 4), Tint(6)} {Tint(21)}
	FAGETik {local(b, 1), Tint(21)} {Tflt(1)}
	SUBff {Tflt(1), Tflt(4)} {Tflt(0)}
	IAGETik {local(nrow, 4), Tint(6)} {Tint(22)}
	FAGETik {local(a, 7), Tint(22)} {Tflt(1)}
	DIVff {Tflt(0), Tflt(1)} {Tflt(2)}
	FAPUTfv {Tflt(2)} {local(x, 6), Tint(6)}
	MOV {Upval(6, Proc%1, write)} {T(23)}
	FAGETik {local(x, 6), Tint(6)} {Tflt(2)}
	CALL {T(23), 'x[' Ks(14), Tint(6), '] = (b[' Ks(24), Tint(6), '] - sum) / a[' Ks(25), Tint(6), ', ' Ks(21), Tint(6), '] = ' Ks(17), Tflt(2), '\n' Ks(4)} {T(23..), 1 Kint(0)}
	BR {L25}
L34
	ADDii {Tint(6), 1 Kint(0)} {Tint(28)}
	EQii {Tint(18), Tint(28)} {Tbool(29)}
	CBR {Tbool(29)} {L35, L36}
L35
	MOV {Upval(6, Proc%1, write)} {T(19)}
	CALL {T(19), 'sum = ' Ks(18)} {T(19..), 1 Kint(0)}
	BR {L37}
L36
	MOV {Upval(6, Proc%1, write)} {T(20)}
	CALL {T(20), 'sum = sum + ' Ks(19)} {T(20..), 1 Kint(0)}
	BR {L37}
L37
	MOV {Upval(6, Proc%1, write)} {T(21)}
	CALL {T(21), 'a[' Ks(20), Tint(6), ', ' Ks(21), Tint(18), '] * x[' Ks(22), Tint(18), ']' Ks(23), '\n' Ks(4)} {T(21..), 1 Kint(0)}
	BR {L30}

End of IR dump*/
#ifdef __MIRC__
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __INTPTR_TYPE__ intptr_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT16_TYPE__ int16_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __INT8_TYPE__ int8_t;
typedef __UINT8_TYPE__ uint8_t;
#define NULL ((void *)0)
#define EXPORT
#else
#include <stddef.h>
#include <stdint.h>
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#endif
typedef size_t lu_mem;
typedef unsigned char lu_byte;
typedef uint16_t LuaType;
typedef struct lua_State lua_State;
#define LUA_TNONE		(-1)
#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8
#define LUA_OK  0
typedef enum {TM_INDEX,TM_NEWINDEX,TM_GC,
	TM_MODE,TM_LEN,TM_EQ,TM_ADD,TM_SUB,TM_MUL,
	TM_MOD,TM_POW,TM_DIV,TM_IDIV,TM_BAND,TM_BOR,
	TM_BXOR,TM_SHL,TM_SHR,TM_UNM,TM_BNOT,TM_LT,
	TM_LE,TM_CONCAT,TM_CALL,TM_N
} TMS;
typedef double lua_Number;
typedef int64_t lua_Integer;
typedef uint64_t lua_Unsigned;
typedef int (*lua_CFunction) (lua_State *L);
typedef union {
	lua_Number n;
	double u;
	void *s;
	lua_Integer i;
	long l;
} L_Umaxalign;
#define lua_assert(c)		((void)0)
#define check_exp(c,e)		(e)
#define lua_longassert(c)	((void)0)
#define luai_apicheck(l,e)	lua_assert(e)
#define api_check(l,e,msg)	luai_apicheck(l,(e) && msg)
#define UNUSED(x)	((void)(x))
#define cast(t, exp)	((t)(exp))
#define cast_void(i)	cast(void, (i))
#define cast_byte(i)	cast(lu_byte, (i))
#define cast_num(i)	cast(lua_Number, (i))
#define cast_int(i)	cast(int, (i))
#define cast_uchar(i)	cast(unsigned char, (i))
#define l_castS2U(i)	((lua_Unsigned)(i))
#define l_castU2S(i)	((lua_Integer)(i))
#define l_noret		void
typedef unsigned int Instruction;
#define luai_numidiv(L,a,b)     ((void)L, l_floor(luai_numdiv(L,a,b)))
#define luai_numdiv(L,a,b)      ((a)/(b))
#define luai_nummod(L,a,b,m)  \
  { (m) = l_mathop(fmod)(a,b); if ((m)*(b) < 0) (m) += (b); }
#define LUA_TLCL	(LUA_TFUNCTION | (0 << 4))
#define LUA_TLCF	(LUA_TFUNCTION | (1 << 4))
#define LUA_TCCL	(LUA_TFUNCTION | (2 << 4))
#define RAVI_TFCF	(LUA_TFUNCTION | (4 << 4))
#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))
#define LUA_TLNGSTR	(LUA_TSTRING | (1 << 4))
#define LUA_TNUMFLT	(LUA_TNUMBER | (0 << 4))
#define LUA_TNUMINT	(LUA_TNUMBER | (1 << 4))
#define RAVI_TIARRAY (LUA_TTABLE | (1 << 4))
#define RAVI_TFARRAY (LUA_TTABLE | (2 << 4))
#define BIT_ISCOLLECTABLE	(1 << 15)
#define ctb(t)			((t) | BIT_ISCOLLECTABLE)
typedef struct GCObject GCObject;
#define CommonHeader	GCObject *next; lu_byte tt; lu_byte marked
struct GCObject {
  CommonHeader;
};
typedef union Value {
  GCObject *gc;
  void *p;
  int b;
  lua_CFunction f;
  lua_Integer i;
  lua_Number n;
} Value;
#define TValuefields	Value value_; LuaType tt_
typedef struct lua_TValue {
  TValuefields;
} TValue;
#define NILCONSTANT	{NULL}, LUA_TNIL
#define val_(o)		((o)->value_)
#define rttype(o)	((o)->tt_)
#define novariant(x)	((x) & 0x0F)
#define ttype(o)	(rttype(o) & 0x7F)
#define ttnov(o)	(novariant(rttype(o)))
#define checktag(o,t)		(rttype(o) == (t))
#define checktype(o,t)		(ttnov(o) == (t))
#define ttisnumber(o)		checktype((o), LUA_TNUMBER)
#define ttisfloat(o)		checktag((o), LUA_TNUMFLT)
#define ttisinteger(o)		checktag((o), LUA_TNUMINT)
#define ttisnil(o)		checktag((o), LUA_TNIL)
#define ttisboolean(o)		checktag((o), LUA_TBOOLEAN)
#define ttislightuserdata(o)	checktag((o), LUA_TLIGHTUSERDATA)
#define ttisstring(o)		checktype((o), LUA_TSTRING)
#define ttisshrstring(o)	checktag((o), ctb(LUA_TSHRSTR))
#define ttislngstring(o)	checktag((o), ctb(LUA_TLNGSTR))
#define ttistable(o)		checktype((o), LUA_TTABLE)
#define ttisiarray(o)    checktag((o), ctb(RAVI_TIARRAY))
#define ttisfarray(o)    checktag((o), ctb(RAVI_TFARRAY))
#define ttisarray(o)     (ttisiarray(o) || ttisfarray(o))
#define ttisLtable(o)    checktag((o), ctb(LUA_TTABLE))
#define ttisfunction(o)		checktype(o, LUA_TFUNCTION)
#define ttisclosure(o)		((rttype(o) & 0x1F) == LUA_TFUNCTION)
#define ttisCclosure(o)		checktag((o), ctb(LUA_TCCL))
#define ttisLclosure(o)		checktag((o), ctb(LUA_TLCL))
#define ttislcf(o)		checktag((o), LUA_TLCF)
#define ttisfcf(o) (ttype(o) == RAVI_TFCF)
#define ttisfulluserdata(o)	checktag((o), ctb(LUA_TUSERDATA))
#define ttisthread(o)		checktag((o), ctb(LUA_TTHREAD))
#define ttisdeadkey(o)		checktag((o), LUA_TDEADKEY)
#define ivalue(o)	check_exp(ttisinteger(o), val_(o).i)
#define fltvalue(o)	check_exp(ttisfloat(o), val_(o).n)
#define nvalue(o)	check_exp(ttisnumber(o), \
	(ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))
#define gcvalue(o)	check_exp(iscollectable(o), val_(o).gc)
#define pvalue(o)	check_exp(ttislightuserdata(o), val_(o).p)
#define tsvalue(o)	check_exp(ttisstring(o), gco2ts(val_(o).gc))
#define uvalue(o)	check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))
#define clvalue(o)	check_exp(ttisclosure(o), gco2cl(val_(o).gc))
#define clLvalue(o)	check_exp(ttisLclosure(o), gco2lcl(val_(o).gc))
#define clCvalue(o)	check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))
#define fvalue(o)	check_exp(ttislcf(o), val_(o).f)
#define fcfvalue(o) check_exp(ttisfcf(o), val_(o).p)
#define hvalue(o)	check_exp(ttistable(o), gco2t(val_(o).gc))
#define arrvalue(o) check_exp(ttisarray(o), gco2array(val_(o).gc))
#define arrvalue(o) check_exp(ttisarray(o), gco2array(val_(o).gc))
#define bvalue(o)	check_exp(ttisboolean(o), val_(o).b)
#define thvalue(o)	check_exp(ttisthread(o), gco2th(val_(o).gc))
#define deadvalue(o)	check_exp(ttisdeadkey(o), cast(void *, val_(o).gc))
#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))
#define iscollectable(o)	(rttype(o) & BIT_ISCOLLECTABLE)
#define righttt(obj)		(ttype(obj) == gcvalue(obj)->tt)
#define checkliveness(L,obj) \
	lua_longassert(!iscollectable(obj) || \
		(righttt(obj) && (L == NULL || !isdead(G(L),gcvalue(obj)))))
#define settt_(o,t)	((o)->tt_=(t))
#define setfltvalue(obj,x) \
  { TValue *io=(obj); val_(io).n=(x); settt_(io, LUA_TNUMFLT); }
#define chgfltvalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisfloat(io)); val_(io).n=(x); }
#define setivalue(obj,x) \
  { TValue *io=(obj); val_(io).i=(x); settt_(io, LUA_TNUMINT); }
#define chgivalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisinteger(io)); val_(io).i=(x); }
#define setnilvalue(obj) settt_(obj, LUA_TNIL)
#define setfvalue(obj,x) \
  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_TLCF); }
#define setfvalue_fastcall(obj, x, tag) \
{ \
    TValue *io = (obj);   \
    lua_assert(tag >= 1 && tag < 0x80); \
    val_(io).p = (x);     \
    settt_(io, ((tag << 8) | RAVI_TFCF)); \
}
#define setpvalue(obj,x) \
  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_TLIGHTUSERDATA); }
#define setbvalue(obj,x) \
  { TValue *io=(obj); val_(io).b=(x); settt_(io, LUA_TBOOLEAN); }
#define setgcovalue(L,obj,x) \
  { TValue *io = (obj); GCObject *i_g=(x); \
    val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }
#define setsvalue(L,obj,x) \
  { TValue *io = (obj); TString *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \
    checkliveness(L,io); }
#define setuvalue(L,obj,x) \
  { TValue *io = (obj); Udata *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TUSERDATA)); \
    checkliveness(L,io); }
#define setthvalue(L,obj,x) \
  { TValue *io = (obj); lua_State *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTHREAD)); \
    checkliveness(L,io); }
#define setclLvalue(L,obj,x) \
  { TValue *io = (obj); LClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TLCL)); \
    checkliveness(L,io); }
#define setclCvalue(L,obj,x) \
  { TValue *io = (obj); CClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TCCL)); \
    checkliveness(L,io); }
#define sethvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTABLE)); \
    checkliveness(L,io); }
#define setiarrayvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TIARRAY)); \
    checkliveness(L,io); }
#define setfarrayvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TFARRAY)); \
    checkliveness(L,io); }
#define setdeadvalue(obj)	settt_(obj, LUA_TDEADKEY)
#define setobj(L,obj1,obj2) \
	{ TValue *io1=(obj1); const TValue *io2=(obj2); io1->tt_ = io2->tt_; val_(io1).n = val_(io2).n; \
	  (void)L; checkliveness(L,io1); }
#define setobjs2s	setobj
#define setobj2s	setobj
#define setsvalue2s	setsvalue
#define sethvalue2s	sethvalue
#define setptvalue2s	setptvalue
#define setobjt2t	setobj
#define setobj2n	setobj
#define setsvalue2n	setsvalue
#define setobj2t	setobj
typedef TValue *StkId;
typedef struct TString {
	CommonHeader;
	lu_byte extra;
	lu_byte shrlen;
	unsigned int hash;
	union {
		size_t lnglen;
		struct TString *hnext;
	} u;
} TString;
typedef union UTString {
	L_Umaxalign dummy;
	TString tsv;
} UTString;
#define getstr(ts)  \
  check_exp(sizeof((ts)->extra), cast(char *, (ts)) + sizeof(UTString))
#define svalue(o)       getstr(tsvalue(o))
#define tsslen(s)	((s)->tt == LUA_TSHRSTR ? (s)->shrlen : (s)->u.lnglen)
#define vslen(o)	tsslen(tsvalue(o))
typedef struct Udata {
	CommonHeader;
	LuaType ttuv_;
	struct Table *metatable;
	size_t len;
	union Value user_;
} Udata;
typedef union UUdata {
	L_Umaxalign dummy;
	Udata uv;
} UUdata;
#define getudatamem(u)  \
  check_exp(sizeof((u)->ttuv_), (cast(char*, (u)) + sizeof(UUdata)))
#define setuservalue(L,u,o) \
	{ const TValue *io=(o); Udata *iu = (u); \
	  iu->user_ = io->value_; iu->ttuv_ = rttype(io); \
	  checkliveness(L,io); }
#define getuservalue(L,u,o) \
	{ TValue *io=(o); const Udata *iu = (u); \
	  io->value_ = iu->user_; settt_(io, iu->ttuv_); \
	  checkliveness(L,io); }
typedef enum {
RAVI_TI_NIL,
RAVI_TI_FALSE,
RAVI_TI_TRUE,
RAVI_TI_INTEGER,
RAVI_TI_FLOAT,
RAVI_TI_INTEGER_ARRAY,
RAVI_TI_FLOAT_ARRAY,
RAVI_TI_TABLE,
RAVI_TI_STRING,
RAVI_TI_FUNCTION,
RAVI_TI_USERDATA,
RAVI_TI_OTHER
} ravi_type_index;
typedef uint32_t ravi_type_map;
#define RAVI_TM_NIL (((ravi_type_map)1)<<RAVI_TI_NIL)
#define RAVI_TM_FALSE (((ravi_type_map)1)<<RAVI_TI_FALSE)
#define RAVI_TM_TRUE (((ravi_type_map)1)<<RAVI_TI_TRUE)
#define RAVI_TM_INTEGER (((ravi_type_map)1)<<RAVI_TI_INTEGER)
#define RAVI_TM_FLOAT (((ravi_type_map)1)<<RAVI_TI_FLOAT)
#define RAVI_TM_INTEGER_ARRAY (((ravi_type_map)1)<<RAVI_TI_INTEGER_ARRAY)
#define RAVI_TM_FLOAT_ARRAY (((ravi_type_map)1)<<RAVI_TI_FLOAT_ARRAY)
#define RAVI_TM_TABLE (((ravi_type_map)1)<<RAVI_TI_TABLE)
#define RAVI_TM_STRING (((ravi_type_map)1)<<RAVI_TI_STRING)
#define RAVI_TM_FUNCTION (((ravi_type_map)1)<<RAVI_TI_FUNCTION)
#define RAVI_TM_USERDATA (((ravi_type_map)1)<<RAVI_TI_USERDATA)
#define RAVI_TM_OTHER (((ravi_type_map)1)<<RAVI_TI_OTHER)
#define RAVI_TM_FALSISH (RAVI_TM_NIL | RAVI_TM_FALSE)
#define RAVI_TM_TRUISH (~RAVI_TM_FALSISH)
#define RAVI_TM_BOOLEAN (RAVI_TM_FALSE | RAVI_TM_TRUE)
#define RAVI_TM_NUMBER (RAVI_TM_INTEGER | RAVI_TM_FLOAT)
#define RAVI_TM_INDEXABLE (RAVI_TM_INTEGER_ARRAY | RAVI_TM_FLOAT_ARRAY | RAVI_TM_TABLE)
#define RAVI_TM_STRING_OR_NIL (RAVI_TM_STRING | RAVI_TM_NIL)
#define RAVI_TM_FUNCTION_OR_NIL (RAVI_TM_FUNCTION | RAVI_TM_NIL)
#define RAVI_TM_BOOLEAN_OR_NIL (RAVI_TM_BOOLEAN | RAVI_TM_NIL)
#define RAVI_TM_USERDATA_OR_NIL (RAVI_TM_USERDATA | RAVI_TM_NIL)
#define RAVI_TM_ANY (~0)
typedef enum {
RAVI_TNIL = RAVI_TM_NIL,           /* NIL */
RAVI_TNUMINT = RAVI_TM_INTEGER,    /* integer number */
RAVI_TNUMFLT = RAVI_TM_FLOAT,        /* floating point number */
RAVI_TNUMBER = RAVI_TM_NUMBER,
RAVI_TARRAYINT = RAVI_TM_INTEGER_ARRAY,      /* array of ints */
RAVI_TARRAYFLT = RAVI_TM_FLOAT_ARRAY,      /* array of doubles */
RAVI_TTABLE = RAVI_TM_TABLE,         /* Lua table */
RAVI_TSTRING = RAVI_TM_STRING_OR_NIL,        /* string */
RAVI_TFUNCTION = RAVI_TM_FUNCTION_OR_NIL,      /* Lua or C Function */
RAVI_TBOOLEAN = RAVI_TM_BOOLEAN_OR_NIL,       /* boolean */
RAVI_TTRUE = RAVI_TM_TRUE,
RAVI_TFALSE = RAVI_TM_FALSE,
RAVI_TUSERDATA = RAVI_TM_USERDATA_OR_NIL,      /* userdata or lightuserdata */
RAVI_TANY = RAVI_TM_ANY,      /* Lua dynamic type */
} ravitype_t;
typedef struct Upvaldesc {
	TString *name;
	TString *usertype;
	ravi_type_map ravi_type;
	lu_byte instack;
	lu_byte idx;
} Upvaldesc;
typedef struct LocVar {
	TString *varname;
	TString *usertype;
	int startpc;
	int endpc;
	ravi_type_map ravi_type;
} LocVar;
typedef enum {
	RAVI_JIT_NOT_COMPILED = 0,
	RAVI_JIT_CANT_COMPILE = 1,
	RAVI_JIT_COMPILED = 2
} ravi_jit_status_t;
typedef enum {
	RAVI_JIT_FLAG_NONE = 0,
	RAVI_JIT_FLAG_HASFORLOOP = 1
} ravi_jit_flag_t;
typedef struct RaviJITProto {
	lu_byte jit_status;
	lu_byte jit_flags;
	unsigned short execution_count;
	void *jit_data;
	lua_CFunction jit_function;
} RaviJITProto;
typedef struct Proto {
	CommonHeader;
	lu_byte numparams;
	lu_byte is_vararg;
	lu_byte maxstacksize;
	int sizeupvalues;
	int sizek;
	int sizecode;
	int sizelineinfo;
	int sizep;
	int sizelocvars;
	int linedefined;
	int lastlinedefined;
	TValue *k;
	Instruction *code;
	struct Proto **p;
	int *lineinfo;
	LocVar *locvars;
	Upvaldesc *upvalues;
	struct LClosure *cache;
	TString  *source;
	GCObject *gclist;
	RaviJITProto ravi_jit;
} Proto;
typedef struct UpVal UpVal;
#define ClosureHeader \
	CommonHeader; lu_byte nupvalues; GCObject *gclist
typedef struct CClosure {
	ClosureHeader;
	lua_CFunction f;
	TValue upvalue[1];
} CClosure;
typedef struct LClosure {
	ClosureHeader;
	struct Proto *p;
	UpVal *upvals[1];
} LClosure;
typedef union Closure {
	CClosure c;
	LClosure l;
} Closure;
#define isLfunction(o)	ttisLclosure(o)
#define getproto(o)	(clLvalue(o)->p)
typedef union TKey {
	struct {
		TValuefields;
		int next;
	} nk;
	TValue tvk;
} TKey;
#define setnodekey(L,key,obj) \
	{ TKey *k_=(key); const TValue *io_=(obj); \
	  k_->nk.value_ = io_->value_; k_->nk.tt_ = io_->tt_; \
	  (void)L; checkliveness(L,io_); }
typedef struct Node {
	TValue i_val;
	TKey i_key;
} Node;
typedef enum RaviArrayModifer {
 RAVI_ARRAY_SLICE = 1,
 RAVI_ARRAY_FIXEDSIZE = 2,
 RAVI_ARRAY_ALLOCATED = 4,
 RAVI_ARRAY_ISFLOAT = 8
} RaviArrayModifier;
enum {
 RAVI_ARRAY_MAX_INLINE = 3,
};
typedef struct RaviArray {
 CommonHeader;
 lu_byte flags;
 unsigned int len;
 unsigned int size;
 union {
  lua_Number numarray[RAVI_ARRAY_MAX_INLINE];
  lua_Integer intarray[RAVI_ARRAY_MAX_INLINE];
  struct RaviArray* parent;
 };
 char *data;
 struct Table *metatable;
} RaviArray;
typedef struct Table {
 CommonHeader;
 lu_byte flags;
 lu_byte lsizenode;
 unsigned int sizearray;
 TValue *array;
 Node *node;
 Node *lastfree;
 struct Table *metatable;
 GCObject *gclist;
 unsigned int hmask;
} Table;
typedef struct Mbuffer {
	char *buffer;
	size_t n;
	size_t buffsize;
} Mbuffer;
typedef struct stringtable {
	TString **hash;
	int nuse;
	int size;
} stringtable;
struct lua_Debug;
typedef intptr_t lua_KContext;
typedef int(*lua_KFunction)(struct lua_State *L, int status, lua_KContext ctx);
typedef void *(*lua_Alloc)(void *ud, void *ptr, size_t osize,
	size_t nsize);
typedef void(*lua_Hook)(struct lua_State *L, struct lua_Debug *ar);
typedef struct CallInfo {
	StkId func;
	StkId	top;
	struct CallInfo *previous, *next;
	union {
		struct {
			StkId base;
			const Instruction *savedpc;
		} l;
		struct {
			lua_KFunction k;
			ptrdiff_t old_errfunc;
			lua_KContext ctx;
		} c;
	} u;
	ptrdiff_t extra;
	short nresults;
	unsigned short callstatus;
	unsigned short stacklevel;
	lu_byte jitstatus;
   lu_byte magic;
} CallInfo;
#define CIST_OAH	(1<<0)
#define CIST_LUA	(1<<1)
#define CIST_HOOKED	(1<<2)
#define CIST_FRESH	(1<<3)
#define CIST_YPCALL	(1<<4)
#define CIST_TAIL	(1<<5)
#define CIST_HOOKYIELD	(1<<6)
#define CIST_LEQ	(1<<7)
#define CIST_FIN	(1<<8)
#define isLua(ci)	((ci)->callstatus & CIST_LUA)
#define isJITed(ci) ((ci)->jitstatus)
#define setoah(st,v)	((st) = ((st) & ~CIST_OAH) | (v))
#define getoah(st)	((st) & CIST_OAH)
typedef struct global_State global_State;
struct lua_State {
	CommonHeader;
	lu_byte status;
	StkId top;
	global_State *l_G;
	CallInfo *ci;
	const Instruction *oldpc;
	StkId stack_last;
	StkId stack;
	UpVal *openupval;
	GCObject *gclist;
	struct lua_State *twups;
	struct lua_longjmp *errorJmp;
	CallInfo base_ci;
	volatile lua_Hook hook;
	ptrdiff_t errfunc;
	int stacksize;
	int basehookcount;
	int hookcount;
	unsigned short nny;
	unsigned short nCcalls;
	lu_byte hookmask;
	lu_byte allowhook;
	unsigned short nci;
   lu_byte magic;
};
#define G(L)	(L->l_G)
union GCUnion {
	GCObject gc;
	struct TString ts;
	struct Udata u;
	union Closure cl;
	struct Table h;
   struct RaviArray arr;
	struct Proto p;
	struct lua_State th;
};
struct UpVal {
	TValue *v;
	lu_mem refcount;
	union {
		struct {
			UpVal *next;
			int touched;
		} open;
		TValue value;
	} u;
};
#define cast_u(o)	cast(union GCUnion *, (o))
#define gco2ts(o)  \
	check_exp(novariant((o)->tt) == LUA_TSTRING, &((cast_u(o))->ts))
#define gco2u(o)  check_exp((o)->tt == LUA_TUSERDATA, &((cast_u(o))->u))
#define gco2lcl(o)  check_exp((o)->tt == LUA_TLCL, &((cast_u(o))->cl.l))
#define gco2ccl(o)  check_exp((o)->tt == LUA_TCCL, &((cast_u(o))->cl.c))
#define gco2cl(o)  \
	check_exp(novariant((o)->tt) == LUA_TFUNCTION, &((cast_u(o))->cl))
#define gco2t(o)  check_exp((o)->tt == LUA_TTABLE, &((cast_u(o))->h))
#define gco2array(o)  check_exp(((o)->tt == RAVI_TIARRAY || (o)->tt == RAVI_TFARRAY), &((cast_u(o))->arr))
#define gco2p(o)  check_exp((o)->tt == LUA_TPROTO, &((cast_u(o))->p))
#define gco2th(o)  check_exp((o)->tt == LUA_TTHREAD, &((cast_u(o))->th))
#define obj2gco(v) \
	check_exp(novariant((v)->tt) < LUA_TDEADKEY, (&(cast_u(v)->gc)))
#define LUA_FLOORN2I		0
#define tonumber(o,n) \
  (ttisfloat(o) ? (*(n) = fltvalue(o), 1) : luaV_tonumber_(o,n))
#define tointeger(o,i) \
  (ttisinteger(o) ? (*(i) = ivalue(o), 1) : luaV_tointeger(o,i,LUA_FLOORN2I))
#define tointegerns(o, i) (ttisinteger(o) ? (*(i) = ivalue(o), 1) : luaV_tointegerns(o, i, LUA_FLOORN2I))
extern int luaV_tonumber_(const TValue *obj, lua_Number *n);
extern int luaV_tointeger(const TValue *obj, lua_Integer *p, int mode);
extern int luaV_tointegerns(const TValue *obj, lua_Integer *p, int mode);
extern void luaF_close (lua_State *L, StkId level);
extern int luaD_poscall (lua_State *L, CallInfo *ci, StkId firstResult, int nres);
extern void luaD_growstack (lua_State *L, int n);
extern int luaV_equalobj(lua_State *L, const TValue *t1, const TValue *t2);
extern int luaV_lessthan(lua_State *L, const TValue *l, const TValue *r);
extern int luaV_lessequal(lua_State *L, const TValue *l, const TValue *r);
extern void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val);
extern void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val);
extern int luaV_execute(lua_State *L);
extern int luaD_precall (lua_State *L, StkId func, int nresults, int op_call);
extern void raviV_op_newtable(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);
extern void raviV_op_newarrayint(lua_State *L, CallInfo *ci, TValue *ra);
extern void raviV_op_newarrayfloat(lua_State *L, CallInfo *ci, TValue *ra);
extern void luaO_arith (lua_State *L, int op, const TValue *p1, const TValue *p2, TValue *res);
extern void raviV_op_setlist(lua_State *L, CallInfo *ci, TValue *ra, int b, int c);
extern void raviV_op_concat(lua_State *L, CallInfo *ci, int a, int b, int c);
extern void raviV_op_closure(lua_State *L, CallInfo *ci, LClosure *cl, int a, int Bx);
extern void raviV_op_vararg(lua_State *L, CallInfo *ci, LClosure *cl, int a, int b);
extern void luaV_objlen (lua_State *L, StkId ra, const TValue *rb);
extern int luaV_forlimit(const TValue *obj, lua_Integer *p, lua_Integer step, int *stopnow);
extern void raviV_op_setupval(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvali(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvalf(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvalai(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvalaf(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_op_setupvalt(lua_State *L, LClosure *cl, TValue *ra, int b);
extern void raviV_raise_error(lua_State *L, int errorcode);
extern void raviV_raise_error_with_info(lua_State *L, int errorcode, const char *info);
extern void luaD_call (lua_State *L, StkId func, int nResults);
extern void raviH_set_int(lua_State *L, RaviArray *t, lua_Unsigned key, lua_Integer value);
extern void raviH_set_float(lua_State *L, RaviArray *t, lua_Unsigned key, lua_Number value);
extern int raviV_check_usertype(lua_State *L, TString *name, const TValue *o);
extern void luaT_trybinTM (lua_State *L, const TValue *p1, const TValue *p2, TValue *res, TMS event);
extern void raviV_gettable_sskey(lua_State *L, const TValue *t, TValue *key, TValue *val);
extern void raviV_settable_sskey(lua_State *L, const TValue *t, TValue *key, TValue *val);
extern void raviV_gettable_i(lua_State *L, const TValue *t, TValue *key, TValue *val);
extern void raviV_settable_i(lua_State *L, const TValue *t, TValue *key, TValue *val);
extern lua_Integer luaV_shiftl(lua_Integer x, lua_Integer y);
extern void ravi_dump_value(lua_State *L, const struct lua_TValue *v);
extern void raviV_op_bnot(lua_State *L, TValue *ra, TValue *rb);
extern void luaV_concat (lua_State *L, int total);
extern void *luaM_realloc_ (lua_State *L, void *block, size_t osize, size_t nsize);
extern LClosure *luaF_newLclosure (lua_State *L, int n);
extern TString *luaS_newlstr (lua_State *L, const char *str, size_t l);
extern Proto *luaF_newproto (lua_State *L);
extern void luaD_inctop (lua_State *L);
#define luaM_reallocv(L,b,on,n,e) luaM_realloc_(L, (b), (on)*(e), (n)*(e))
#define luaM_newvector(L,n,t) cast(t *, luaM_reallocv(L, NULL, 0, n, sizeof(t)))
#define R(i) (base + i)
#define K(i) (k + i)
#define S(i) (stackbase + i)
#define stackoverflow(L, n) (((int)(L->top - L->stack) + (n) + 5) >= L->stacksize)
#define savestack(L,p)		((char *)(p) - (char *)L->stack)
#define restorestack(L,n)	((TValue *)((char *)L->stack + (n)))
#define tonumberns(o,n) \
	(ttisfloat(o) ? ((n) = fltvalue(o), 1) : \
	(ttisinteger(o) ? ((n) = cast_num(ivalue(o)), 1) : 0))
#define intop(op,v1,v2) l_castU2S(l_castS2U(v1) op l_castS2U(v2))
#define nan (0./0.)
#define inf (1./0.)
#define luai_numunm(L,a)        (-(a))
static int __ravifunc_1(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// LOADGLOBAL {Upval(_ENV), 'assert' Ks(0)} {T(0)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(0);
 TValue *dst = R(9);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(0)} {local(assert, 0)}
{
 const TValue *src_reg = R(9);
 TValue *dst_reg = R(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// LOADGLOBAL {Upval(_ENV), 'table' Ks(1)} {T(0)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(1);
 TValue *dst = R(9);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// GETsk {T(0), 'slice' Ks(2)} {T(1)}
{
 TValue *tab = R(9);
 TValue *key = K(2);
 TValue *dst = R(10);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// LOADGLOBAL {Upval(_ENV), 'table' Ks(1)} {T(2)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(1);
 TValue *dst = R(11);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// GETsk {T(2), 'numarray' Ks(3)} {T(3)}
{
 TValue *tab = R(11);
 TValue *key = K(3);
 TValue *dst = R(12);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// LOADGLOBAL {Upval(_ENV), 'table' Ks(1)} {T(4)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(1);
 TValue *dst = R(13);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// GETsk {T(4), 'intarray' Ks(4)} {T(5)}
{
 TValue *tab = R(13);
 TValue *key = K(4);
 TValue *dst = R(14);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(1)} {local(slice, 1)}
{
 const TValue *src_reg = R(10);
 TValue *dst_reg = R(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {T(3)} {local(numarray, 2)}
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = R(2);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {T(5)} {local(intarray, 3)}
{
 const TValue *src_reg = R(14);
 TValue *dst_reg = R(3);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// LOADGLOBAL {Upval(_ENV), 'io' Ks(5)} {T(5)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(5);
 TValue *dst = R(14);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// GETsk {T(5), 'write' Ks(6)} {T(3)}
{
 TValue *tab = R(14);
 TValue *key = K(6);
 TValue *dst = R(12);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(3)} {local(write, 4)}
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = R(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CLOSURE {Proc%2} {T(3)}
raviV_op_closure(L, ci, cl, 12, 0);
base = ci->u.l.base;
// MOV {T(3)} {local(copy, 5)}
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = R(5);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CLOSURE {Proc%3} {T(3)}
raviV_op_closure(L, ci, cl, 12, 1);
base = ci->u.l.base;
// MOV {T(3)} {local(partial_pivot, 6)}
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = R(6);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CLOSURE {Proc%4} {T(3)}
raviV_op_closure(L, ci, cl, 12, 2);
base = ci->u.l.base;
// MOV {T(3)} {local(dump_matrix, 7)}
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = R(7);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CLOSURE {Proc%5} {T(3)}
raviV_op_closure(L, ci, cl, 12, 3);
base = ci->u.l.base;
// MOV {T(3)} {local(gaussian_solve, 8)}
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = R(8);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// NEWTABLE {T(3)}
{
 TValue *ra = R(12);
 raviV_op_newtable(L, ci, ra, 0, 0);
base = ci->u.l.base;
}
// TPUTsk {local(gaussian_solve, 8)} {T(3), 'gaussian_solve' Ks(7)}
{
 TValue *tab = R(12);
 TValue *key = K(7);
 TValue *src = R(8);
 raviV_settable_sskey(L, tab, key, src);
 base = ci->u.l.base;
}
// RET {T(3)} {L1}
luaF_close(L, base);
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 1;
 int j = 0;
 if (0 < wanted) {
{
 const TValue *src_reg = R(12);
 TValue *dst_reg = S(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
 }
 j++;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
}
static int __ravifunc_2(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
lua_Integer i_0 = 0, i_1 = 0, i_2 = 0, i_3 = 0, i_4 = 0, i_5 = 0;
lua_Number f_0 = 0, f_1 = 0;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// TOFARRAY {local(a, 0)}
{
 TValue *ra = R(0);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {Upval(0, Proc%1, numarray)} {T(0)}
{
 const TValue *src_reg = cl->upvals[0]->v;
 TValue *dst_reg = R(2);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// LENi {local(a, 0)} {Tint(0)}
{
 TValue *len = &ival0; ival0.value_.i = i_0;
 TValue *obj = R(0);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
 i_0 = ival0.value_.i;
}
// CALL {T(0), Tint(0), 0.000000000000 Kflt(0)} {T(0..), 1 Kint(0)}
 if (stackoverflow(L,4)) { luaD_growstack(L, 4); base = ci->u.l.base; }
 L->top = R(2) + 3;
{
 TValue *dst_reg = R(4);
 setfltvalue(dst_reg, 0);
}
{
 TValue *dst_reg = R(3);
 setivalue(dst_reg, i_0);
}
{
 TValue *ra = R(2);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOFARRAY {T(0[0..])}
{
 TValue *ra = R(2);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(0[0..])} {local(c, 1)}
{
 const TValue *src_reg = R(2);
 TValue *dst_reg = R(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {1 Kint(0)} {Tint(1)}
i_1 = 1;
// LENi {local(a, 0)} {Tint(2)}
{
 TValue *len = &ival0; ival0.value_.i = i_2;
 TValue *obj = R(0);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
 i_2 = ival0.value_.i;
}
// MOV {Tint(2)} {Tint(3)}
i_3 = i_2;
// MOV {1 Kint(0)} {Tint(4)}
i_4 = 1;
// SUBii {Tint(1), Tint(4)} {Tint(1)}
{ i_1 = i_1 - i_4; }
// BR {L2}
goto L2;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
L2:
// ADDii {Tint(1), Tint(4)} {Tint(1)}
{ i_1 = i_1 + i_4; }
// BR {L3}
goto L3;
L3:
// LIii {Tint(3), Tint(1)} {Tbool(5)}
{ i_5 = i_3 < i_1; }
// CBR {Tbool(5)} {L5, L4}
{ if (i_5 != 0) goto L5; else goto L4; }
L4:
// MOV {Tint(1)} {Tint(0)}
i_0 = i_1;
// FAGETik {local(a, 0), Tint(0)} {Tflt(1)}
{
 RaviArray *arr = arrvalue(R(0));
 lua_Unsigned ukey = (lua_Unsigned) i_0;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_1 = iptr[ukey];
}
// FAPUTfv {Tflt(1)} {local(c, 1), Tint(0)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) i_0;
 lua_Number *iptr = (lua_Number *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = f_1;
} else {
 raviH_set_float(L, arr, ukey, f_1);
}
}
// BR {L2}
goto L2;
L5:
// RET {local(c, 1)} {L1}
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 1;
 int j = 0;
 if (0 < wanted) {
{
 const TValue *src_reg = R(1);
 TValue *dst_reg = S(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
 }
 j++;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
}
static int __ravifunc_3(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
lua_Integer i_0 = 0, i_1 = 0, i_2 = 0, i_3 = 0, i_4 = 0, i_5 = 0, i_6 = 0, i_7 = 0, i_8 = 0;
lua_Number f_0 = 0, f_1 = 0, f_2 = 0;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// TOTABLE {local(columns, 0)}
{
 TValue *ra = R(0);
 if (!ttisLtable(ra)) {
  error_code = 4;
  goto Lraise_error;
 }
}
// TOIARRAY {local(nrow, 1)}
{
 TValue *ra = R(1);
 if (!ttisiarray(ra)) {
  error_code = 2;
  goto Lraise_error;
 }
}
// TOINT {local(i, 2)}
{
 TValue *ra = R(2);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// TOINT {local(n, 3)}
{
 TValue *ra = R(3);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// MOV {local(i, 2)} {Tint(1)}
{
TValue *reg = R(2);
i_1 = ivalue(reg);
}
// MOVi {Tint(1)} {Tint(0)}
i_0 = i_1;
// MOVf {0.000000000000 Kflt(0)} {Tflt(0)}
f_0 = 0;
// TGETik {local(columns, 0), local(i, 2)} {T(0)}
{
 TValue *tab = R(0);
 TValue *key = R(2);
 TValue *dst = R(6);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// TOFARRAY {T(0)}
{
 TValue *ra = R(6);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(0)} {local(a, 4)}
{
 const TValue *src_reg = R(6);
 TValue *dst_reg = R(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {false} {local(max_set, 5)}
{
 TValue *dst_reg = R(5);
 setbvalue(dst_reg, 0);
}
// MOV {local(i, 2)} {Tint(2)}
{
TValue *reg = R(2);
i_2 = ivalue(reg);
}
// MOV {local(n, 3)} {Tint(3)}
{
TValue *reg = R(3);
i_3 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(4)}
i_4 = 1;
// SUBii {Tint(2), Tint(4)} {Tint(2)}
{ i_2 = i_2 - i_4; }
// BR {L2}
goto L2;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
L2:
// ADDii {Tint(2), Tint(4)} {Tint(2)}
{ i_2 = i_2 + i_4; }
// BR {L3}
goto L3;
L3:
// LIii {Tint(3), Tint(2)} {Tbool(5)}
{ i_5 = i_3 < i_2; }
// CBR {Tbool(5)} {L5, L4}
{ if (i_5 != 0) goto L5; else goto L4; }
L4:
// MOV {Tint(2)} {Tint(1)}
i_1 = i_2;
// IAGETik {local(nrow, 1), Tint(1)} {Tint(6)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) i_1;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_6 = iptr[ukey];
}
// FAGETik {local(a, 4), Tint(6)} {Tflt(2)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_6;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_2 = iptr[ukey];
}
// MOVf {Tflt(2)} {Tflt(1)}
f_1 = f_2;
// BR {L6}
goto L6;
L5:
// BR {L14}
goto L14;
L6:
// LTff {Tflt(1), 0.000000000000 Kflt(0)} {Tbool(7)}
{ i_7 = f_1 < 0; }
// CBR {Tbool(7)} {L7, L8}
{ if (i_7 != 0) goto L7; else goto L8; }
L7:
// UNMf {Tflt(1)} {Tflt(2)}
{
 f_2 = -f_1;
}
// MOVf {Tflt(2)} {Tflt(1)}
f_1 = f_2;
// BR {L8}
goto L8;
L8:
// BR {L9}
goto L9;
L9:
// NOT {local(max_set, 5)} {T(0)}
{
 TValue *ra = R(6);
 TValue *rb = R(5);
 int result = l_isfalse(rb);
 setbvalue(ra, result);
}
// CBR {T(0)} {L11, L10}
{
const TValue *src_reg = R(6);
if (!l_isfalse(src_reg)) goto L11;
else goto L10;
}
L10:
// LTff {Tflt(0), Tflt(1)} {Tbool(8)}
{ i_8 = f_0 < f_1; }
// CBR {Tbool(8)} {L12, L13}
{ if (i_8 != 0) goto L12; else goto L13; }
L11:
// MOVf {Tflt(1)} {Tflt(0)}
f_0 = f_1;
// MOV {true} {local(max_set, 5)}
{
 TValue *dst_reg = R(5);
 setbvalue(dst_reg, 1);
}
// MOVi {Tint(1)} {Tint(0)}
i_0 = i_1;
// BR {L13}
goto L13;
L12:
// MOVi {Tint(1)} {Tint(0)}
i_0 = i_1;
// MOVf {Tflt(1)} {Tflt(0)}
f_0 = f_1;
// BR {L13}
goto L13;
L13:
// BR {L2}
goto L2;
L14:
// FAGETik {local(a, 4), Tint(0)} {Tflt(1)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_0;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_1 = iptr[ukey];
}
// EQff {Tflt(1), 0.000000000000 Kflt(0)} {Tbool(2)}
{ i_2 = f_1 == 0; }
// CBR {Tbool(2)} {L15, L16}
{ if (i_2 != 0) goto L15; else goto L16; }
L15:
// LOADGLOBAL {Upval(_ENV), 'error' Ks(0)} {T(1)}
{
 TValue *tab = cl->upvals[0]->v;
 TValue *key = K(0);
 TValue *dst = R(7);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(1)} {T(2)}
{
 const TValue *src_reg = R(7);
 TValue *dst_reg = R(8);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(2), 'no unique solution exists' Ks(1)} {T(2..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(8) + 2;
{
 TValue *dst_reg = R(9);
 TValue *src_reg = K(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(8);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L16}
goto L16;
L16:
// BR {L17}
goto L17;
L17:
// IAGETik {local(nrow, 1), local(i, 2)} {Tint(3)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) ivalue(R(2));
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_3 = iptr[ukey];
}
// IAGETik {local(nrow, 1), Tint(0)} {Tint(4)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) i_0;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_4 = iptr[ukey];
}
// EQii {Tint(3), Tint(4)} {Tbool(5)}
{ i_5 = i_3 == i_4; }
// NOT {Tbool(5)} {Tbool(1)}
{ i_1 = (0 == i_5); }
// CBR {Tbool(1)} {L18, L19}
{ if (i_1 != 0) goto L18; else goto L19; }
L18:
// MOV {Upval(1, Proc%1, write)} {T(3)}
{
 const TValue *src_reg = cl->upvals[1]->v;
 TValue *dst_reg = R(9);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(3), 'Performing row interchange ' Ks(2), local(i, 2), ' will be swapped with ' Ks(3), Tint(0), '\n' Ks(4)} {T(3..), 1 Kint(0)}
 if (stackoverflow(L,7)) { luaD_growstack(L, 7); base = ci->u.l.base; }
 L->top = R(9) + 6;
{
 TValue *dst_reg = R(14);
 TValue *src_reg = K(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(13);
 setivalue(dst_reg, i_0);
}
{
 TValue *dst_reg = R(12);
 TValue *src_reg = K(3);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 const TValue *src_reg = R(2);
 TValue *dst_reg = R(11);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *dst_reg = R(10);
 TValue *src_reg = K(2);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(9);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// IAGETik {local(nrow, 1), local(i, 2)} {Tint(3)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) ivalue(R(2));
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_3 = iptr[ukey];
}
// MOVi {Tint(3)} {Tint(4)}
i_4 = i_3;
// IAGETik {local(nrow, 1), Tint(0)} {Tint(5)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) i_0;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_5 = iptr[ukey];
}
// IAPUTiv {Tint(5)} {local(nrow, 1), local(i, 2)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) ivalue(R(2));
 lua_Integer *iptr = (lua_Integer *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = i_5;
} else {
 raviH_set_int(L, arr, ukey, i_5);
}
}
// IAPUTiv {Tint(4)} {local(nrow, 1), Tint(0)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) i_0;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = i_4;
} else {
 raviH_set_int(L, arr, ukey, i_4);
}
}
// BR {L19}
goto L19;
L19:
// RET {L1}
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 0;
 int j = 0;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
}
static int __ravifunc_4(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
lua_Integer i_0 = 0, i_1 = 0, i_2 = 0, i_3 = 0, i_4 = 0, i_5 = 0, i_6 = 0, i_7 = 0, i_8 = 0, i_9 = 0, i_10 = 0;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// TOTABLE {local(columns, 0)}
{
 TValue *ra = R(0);
 if (!ttisLtable(ra)) {
  error_code = 4;
  goto Lraise_error;
 }
}
// TOINT {local(m, 1)}
{
 TValue *ra = R(1);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// TOINT {local(n, 2)}
{
 TValue *ra = R(2);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// TOIARRAY {local(nrow, 3)}
{
 TValue *ra = R(3);
 if (!ttisiarray(ra)) {
  error_code = 2;
  goto Lraise_error;
 }
}
// MOV {1 Kint(0)} {Tint(1)}
i_1 = 1;
// MOV {local(m, 1)} {Tint(2)}
{
TValue *reg = R(1);
i_2 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(3)}
i_3 = 1;
// SUBii {Tint(1), Tint(3)} {Tint(1)}
{ i_1 = i_1 - i_3; }
// BR {L2}
goto L2;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
L2:
// ADDii {Tint(1), Tint(3)} {Tint(1)}
{ i_1 = i_1 + i_3; }
// BR {L3}
goto L3;
L3:
// LIii {Tint(2), Tint(1)} {Tbool(4)}
{ i_4 = i_2 < i_1; }
// CBR {Tbool(4)} {L5, L4}
{ if (i_4 != 0) goto L5; else goto L4; }
L4:
// MOV {Tint(1)} {Tint(0)}
i_0 = i_1;
// MOV {1 Kint(0)} {Tint(6)}
i_6 = 1;
// MOV {local(n, 2)} {Tint(7)}
{
TValue *reg = R(2);
i_7 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(8)}
i_8 = 1;
// SUBii {Tint(6), Tint(8)} {Tint(6)}
{ i_6 = i_6 - i_8; }
// BR {L6}
goto L6;
L5:
// RET {L1}
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 0;
 int j = 0;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
L6:
// ADDii {Tint(6), Tint(8)} {Tint(6)}
{ i_6 = i_6 + i_8; }
// BR {L7}
goto L7;
L7:
// LIii {Tint(7), Tint(6)} {Tbool(9)}
{ i_9 = i_7 < i_6; }
// CBR {Tbool(9)} {L9, L8}
{ if (i_9 != 0) goto L9; else goto L8; }
L8:
// MOV {Tint(6)} {Tint(5)}
i_5 = i_6;
// MOV {Upval(0, Proc%1, write)} {T(0)}
{
 const TValue *src_reg = cl->upvals[0]->v;
 TValue *dst_reg = R(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// TGETik {local(columns, 0), Tint(5)} {T(1)}
{
 TValue *tab = R(0);
 TValue *key = &ival0; ival0.value_.i = i_5;
 TValue *dst = R(5);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// IAGETik {local(nrow, 3), Tint(0)} {Tint(10)}
{
 RaviArray *arr = arrvalue(R(3));
 lua_Unsigned ukey = (lua_Unsigned) i_0;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_10 = iptr[ukey];
}
// GETik {T(1), Tint(10)} {T(2)}
{
 TValue *tab = R(5);
 TValue *key = &ival0; ival0.value_.i = i_10;
 TValue *dst = R(6);
 luaV_gettable(L, tab, key, dst);
 base = ci->u.l.base;
}
// CALL {T(0), T(2), ' ' Ks(0)} {T(0..), 1 Kint(0)}
 if (stackoverflow(L,4)) { luaD_growstack(L, 4); base = ci->u.l.base; }
 L->top = R(4) + 3;
{
 TValue *dst_reg = R(6);
 TValue *src_reg = K(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 const TValue *src_reg = R(6);
 TValue *dst_reg = R(5);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(4);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L6}
goto L6;
L9:
// MOV {Upval(0, Proc%1, write)} {T(2)}
{
 const TValue *src_reg = cl->upvals[0]->v;
 TValue *dst_reg = R(6);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(2), '\n' Ks(1)} {T(2..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(6) + 2;
{
 TValue *dst_reg = R(7);
 TValue *src_reg = K(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(6);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L2}
goto L2;
}
static int __ravifunc_5(lua_State *L) {
int error_code = 0;
int result = 0;
CallInfo *ci = L->ci;
LClosure *cl = clLvalue(ci->func);
TValue *k = cl->p->k;
StkId base = ci->u.l.base;
lua_Integer i_0 = 0, i_1 = 0, i_2 = 0, i_3 = 0, i_4 = 0, i_5 = 0, i_6 = 0, i_7 = 0, i_8 = 0, i_9 = 0, i_10 = 0, i_11 = 0, i_12 = 0, i_13 = 0, i_14 = 0, i_15 = 0, i_16 = 0, i_17 = 0, i_18 = 0, i_19 = 0, i_20 = 0, i_21 = 0, i_22 = 0, i_23 = 0, i_24 = 0, i_25 = 0, i_26 = 0, i_27 = 0, i_28 = 0, i_29 = 0;
lua_Number f_0 = 0, f_1 = 0, f_2 = 0, f_3 = 0, f_4 = 0;
TValue ival0; settt_(&ival0, LUA_TNUMINT);
TValue fval0; settt_(&fval0, LUA_TNUMFLT);
TValue bval0; settt_(&bval0, LUA_TBOOLEAN);
TValue ival1; settt_(&ival1, LUA_TNUMINT);
TValue fval1; settt_(&fval1, LUA_TNUMFLT);
TValue bval1; settt_(&bval1, LUA_TBOOLEAN);
TValue ival2; settt_(&ival2, LUA_TNUMINT);
TValue fval2; settt_(&fval2, LUA_TNUMFLT);
TValue bval2; settt_(&bval2, LUA_TBOOLEAN);
TValue nilval; setnilvalue(&nilval);
L0:
// TOFARRAY {local(A, 0)}
{
 TValue *ra = R(0);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// TOFARRAY {local(b, 1)}
{
 TValue *ra = R(1);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// TOINT {local(m, 2)}
{
 TValue *ra = R(2);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// TOINT {local(n, 3)}
{
 TValue *ra = R(3);
 if (!ttisinteger(ra)) {
  error_code = 0;
  goto Lraise_error;
 }
}
// MOV {Upval(0, Proc%1, copy)} {T(0)}
{
 const TValue *src_reg = cl->upvals[0]->v;
 TValue *dst_reg = R(10);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(0), local(A, 0)} {T(0..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(10) + 2;
{
 const TValue *src_reg = R(0);
 TValue *dst_reg = R(11);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(10);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOFARRAY {T(0[0..])}
{
 TValue *ra = R(10);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(0[0..])} {local(A, 0)}
{
 const TValue *src_reg = R(10);
 TValue *dst_reg = R(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {Upval(0, Proc%1, copy)} {T(1)}
{
 const TValue *src_reg = cl->upvals[0]->v;
 TValue *dst_reg = R(11);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(1), local(b, 1)} {T(1..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(11) + 2;
{
 const TValue *src_reg = R(1);
 TValue *dst_reg = R(12);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(11);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOFARRAY {T(1[1..])}
{
 TValue *ra = R(11);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(1[1..])} {local(b, 1)}
{
 const TValue *src_reg = R(11);
 TValue *dst_reg = R(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {Upval(1, Proc%1, assert)} {T(2)}
{
 const TValue *src_reg = cl->upvals[1]->v;
 TValue *dst_reg = R(12);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// EQii {local(m, 2), local(n, 3)} {Tbool(0)}
{ i_0 = ivalue(R(2)) == ivalue(R(3)); }
// CALL {T(2), Tbool(0)} {T(2..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(12) + 2;
{
 TValue *dst_reg = R(13);
 setbvalue(dst_reg, i_0);
}
{
 TValue *ra = R(12);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// MOV {Upval(1, Proc%1, assert)} {T(3)}
{
 const TValue *src_reg = cl->upvals[1]->v;
 TValue *dst_reg = R(13);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// LENi {local(b, 1)} {Tint(0)}
{
 TValue *len = &ival0; ival0.value_.i = i_0;
 TValue *obj = R(1);
 luaV_objlen(L, len, obj);
base = ci->u.l.base;
 i_0 = ival0.value_.i;
}
// EQii {Tint(0), local(m, 2)} {Tbool(1)}
{ i_1 = i_0 == ivalue(R(2)); }
// CALL {T(3), Tbool(1)} {T(3..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(13) + 2;
{
 TValue *dst_reg = R(14);
 setbvalue(dst_reg, i_1);
}
{
 TValue *ra = R(13);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// MOV {Upval(2, Proc%1, intarray)} {T(4)}
{
 const TValue *src_reg = cl->upvals[2]->v;
 TValue *dst_reg = R(14);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(4), local(n, 3)} {T(4..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(14) + 2;
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(15);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(14);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOIARRAY {T(4[4..])}
{
 TValue *ra = R(14);
 if (!ttisiarray(ra)) {
  error_code = 2;
  goto Lraise_error;
 }
}
// MOV {T(4[4..])} {local(nrow, 4)}
{
 const TValue *src_reg = R(14);
 TValue *dst_reg = R(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// NEWTABLE {T(5)}
{
 TValue *ra = R(15);
 raviV_op_newtable(L, ci, ra, 0, 0);
base = ci->u.l.base;
}
// MOV {T(5)} {local(columns, 5)}
{
 const TValue *src_reg = R(15);
 TValue *dst_reg = R(5);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {1 Kint(0)} {Tint(0)}
i_0 = 1;
// MOV {local(n, 3)} {Tint(2)}
{
TValue *reg = R(3);
i_2 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(3)}
i_3 = 1;
// SUBii {Tint(0), Tint(3)} {Tint(0)}
{ i_0 = i_0 - i_3; }
// BR {L2}
goto L2;
L1:
 return result;
Lraise_error:
 raviV_raise_error(L, error_code); /* does not return */
 return result;
L2:
// ADDii {Tint(0), Tint(3)} {Tint(0)}
{ i_0 = i_0 + i_3; }
// BR {L3}
goto L3;
L3:
// LIii {Tint(2), Tint(0)} {Tbool(4)}
{ i_4 = i_2 < i_0; }
// CBR {Tbool(4)} {L5, L4}
{ if (i_4 != 0) goto L5; else goto L4; }
L4:
// MOV {Tint(0)} {Tint(1)}
i_1 = i_0;
// MOV {Upval(3, Proc%1, slice)} {T(6)}
{
 const TValue *src_reg = cl->upvals[3]->v;
 TValue *dst_reg = R(16);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// SUBii {Tint(1), 1 Kint(0)} {Tint(5)}
{ i_5 = i_1 - 1; }
// MULii {Tint(5), local(m, 2)} {Tint(6)}
{ i_6 = i_5 * ivalue(R(2)); }
// ADDii {Tint(6), 1 Kint(0)} {Tint(5)}
{ i_5 = i_6 + 1; }
// CALL {T(6), local(A, 0), Tint(5), local(m, 2)} {T(6..), 1 Kint(0)}
 if (stackoverflow(L,5)) { luaD_growstack(L, 5); base = ci->u.l.base; }
 L->top = R(16) + 4;
{
 const TValue *src_reg = R(2);
 TValue *dst_reg = R(19);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *dst_reg = R(18);
 setivalue(dst_reg, i_5);
}
{
 const TValue *src_reg = R(0);
 TValue *dst_reg = R(17);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(16);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TPUTik {T(6[6..])} {local(columns, 5), Tint(1)}
{
 TValue *tab = R(5);
 TValue *key = &ival0; ival0.value_.i = i_1;
 TValue *src = R(16);
 raviV_settable_i(L, tab, key, src);
 base = ci->u.l.base;
}
// BR {L2}
goto L2;
L5:
// ADDii {local(n, 3), 1 Kint(0)} {Tint(0)}
{ i_0 = ivalue(R(3)) + 1; }
// MOV {local(b, 1)} {T(7)}
{
 const TValue *src_reg = R(1);
 TValue *dst_reg = R(17);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// TPUTik {T(7)} {local(columns, 5), Tint(0)}
{
 TValue *tab = R(5);
 TValue *key = &ival0; ival0.value_.i = i_0;
 TValue *src = R(17);
 raviV_settable_i(L, tab, key, src);
 base = ci->u.l.base;
}
// MOV {1 Kint(0)} {Tint(3)}
i_3 = 1;
// MOV {local(n, 3)} {Tint(4)}
{
TValue *reg = R(3);
i_4 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(1)}
i_1 = 1;
// SUBii {Tint(3), Tint(1)} {Tint(3)}
{ i_3 = i_3 - i_1; }
// BR {L6}
goto L6;
L6:
// ADDii {Tint(3), Tint(1)} {Tint(3)}
{ i_3 = i_3 + i_1; }
// BR {L7}
goto L7;
L7:
// LIii {Tint(4), Tint(3)} {Tbool(5)}
{ i_5 = i_4 < i_3; }
// CBR {Tbool(5)} {L9, L8}
{ if (i_5 != 0) goto L9; else goto L8; }
L8:
// MOV {Tint(3)} {Tint(2)}
i_2 = i_3;
// IAPUTiv {Tint(2)} {local(nrow, 4), Tint(2)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_2;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = i_2;
} else {
 raviH_set_int(L, arr, ukey, i_2);
}
}
// BR {L6}
goto L6;
L9:
// MOV {1 Kint(0)} {Tint(4)}
i_4 = 1;
// SUBii {local(n, 3), 1 Kint(0)} {Tint(1)}
{ i_1 = ivalue(R(3)) - 1; }
// MOV {Tint(1)} {Tint(5)}
i_5 = i_1;
// MOV {1 Kint(0)} {Tint(2)}
i_2 = 1;
// SUBii {Tint(4), Tint(2)} {Tint(4)}
{ i_4 = i_4 - i_2; }
// BR {L10}
goto L10;
L10:
// ADDii {Tint(4), Tint(2)} {Tint(4)}
{ i_4 = i_4 + i_2; }
// BR {L11}
goto L11;
L11:
// LIii {Tint(5), Tint(4)} {Tbool(6)}
{ i_6 = i_5 < i_4; }
// CBR {Tbool(6)} {L13, L12}
{ if (i_6 != 0) goto L13; else goto L12; }
L12:
// MOV {Tint(4)} {Tint(3)}
i_3 = i_4;
// MOV {Upval(4, Proc%1, partial_pivot)} {T(7)}
{
 const TValue *src_reg = cl->upvals[4]->v;
 TValue *dst_reg = R(17);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(7), local(columns, 5), local(nrow, 4), Tint(3), local(m, 2)} {T(7..), 1 Kint(0)}
 if (stackoverflow(L,6)) { luaD_growstack(L, 6); base = ci->u.l.base; }
 L->top = R(17) + 5;
{
 const TValue *src_reg = R(2);
 TValue *dst_reg = R(21);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *dst_reg = R(20);
 setivalue(dst_reg, i_3);
}
{
 const TValue *src_reg = R(4);
 TValue *dst_reg = R(19);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 const TValue *src_reg = R(5);
 TValue *dst_reg = R(18);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(17);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// MOV {Upval(5, Proc%1, dump_matrix)} {T(5)}
{
 const TValue *src_reg = cl->upvals[5]->v;
 TValue *dst_reg = R(15);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// ADDii {local(n, 3), 1 Kint(0)} {Tint(7)}
{ i_7 = ivalue(R(3)) + 1; }
// CALL {T(5), local(columns, 5), local(n, 3), Tint(7), local(nrow, 4)} {T(5..), 1 Kint(0)}
 if (stackoverflow(L,6)) { luaD_growstack(L, 6); base = ci->u.l.base; }
 L->top = R(15) + 5;
{
 const TValue *src_reg = R(4);
 TValue *dst_reg = R(19);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *dst_reg = R(18);
 setivalue(dst_reg, i_7);
}
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(17);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 const TValue *src_reg = R(5);
 TValue *dst_reg = R(16);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(15);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// ADDii {Tint(3), 1 Kint(0)} {Tint(8)}
{ i_8 = i_3 + 1; }
// MOV {Tint(8)} {Tint(9)}
i_9 = i_8;
// MOV {local(m, 2)} {Tint(10)}
{
TValue *reg = R(2);
i_10 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(11)}
i_11 = 1;
// SUBii {Tint(9), Tint(11)} {Tint(9)}
{ i_9 = i_9 - i_11; }
// BR {L14}
goto L14;
L13:
// BR {L22}
goto L22;
L14:
// ADDii {Tint(9), Tint(11)} {Tint(9)}
{ i_9 = i_9 + i_11; }
// BR {L15}
goto L15;
L15:
// LIii {Tint(10), Tint(9)} {Tbool(12)}
{ i_12 = i_10 < i_9; }
// CBR {Tbool(12)} {L17, L16}
{ if (i_12 != 0) goto L17; else goto L16; }
L16:
// MOV {Tint(9)} {Tint(7)}
i_7 = i_9;
// TGETik {local(columns, 5), Tint(3)} {T(8)}
{
 TValue *tab = R(5);
 TValue *key = &ival0; ival0.value_.i = i_3;
 TValue *dst = R(18);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// TOFARRAY {T(8)}
{
 TValue *ra = R(18);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(8)} {local(column, 8)}
{
 const TValue *src_reg = R(18);
 TValue *dst_reg = R(8);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// IAGETik {local(nrow, 4), Tint(7)} {Tint(13)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_7;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_13 = iptr[ukey];
}
// FAGETik {local(column, 8), Tint(13)} {Tflt(1)}
{
 RaviArray *arr = arrvalue(R(8));
 lua_Unsigned ukey = (lua_Unsigned) i_13;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_1 = iptr[ukey];
}
// IAGETik {local(nrow, 4), Tint(3)} {Tint(14)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_3;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_14 = iptr[ukey];
}
// FAGETik {local(column, 8), Tint(14)} {Tflt(2)}
{
 RaviArray *arr = arrvalue(R(8));
 lua_Unsigned ukey = (lua_Unsigned) i_14;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_2 = iptr[ukey];
}
// DIVff {Tflt(1), Tflt(2)} {Tflt(3)}
{ f_3 = f_1 / f_2; }
// MOVf {Tflt(3)} {Tflt(0)}
f_0 = f_3;
// MOV {Upval(6, Proc%1, write)} {T(8)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(18);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CONCAT {'m(' Ks(0), Tint(7), ',' Ks(1), Tint(3), ') = ' Ks(2)} {T(9)}
 if (stackoverflow(L,5)) { luaD_growstack(L, 5); base = ci->u.l.base; }
{
{
 TValue *dst_reg = R(34);
 TValue *src_reg = K(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(35);
 setivalue(dst_reg, i_7);
}
{
 TValue *dst_reg = R(36);
 TValue *src_reg = K(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(37);
 setivalue(dst_reg, i_3);
}
{
 TValue *dst_reg = R(38);
 TValue *src_reg = K(2);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
 L->top = R(34) + 5;
 luaV_concat(L, 5);
 base = ci->u.l.base;
 L->top = ci->top;
{
 const TValue *src_reg = R(34);
 TValue *dst_reg = R(19);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
}
// IAGETik {local(nrow, 4), Tint(7)} {Tint(15)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_7;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_15 = iptr[ukey];
}
// FAGETik {local(column, 8), Tint(15)} {Tflt(3)}
{
 RaviArray *arr = arrvalue(R(8));
 lua_Unsigned ukey = (lua_Unsigned) i_15;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_3 = iptr[ukey];
}
// IAGETik {local(nrow, 4), Tint(3)} {Tint(16)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_3;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_16 = iptr[ukey];
}
// FAGETik {local(column, 8), Tint(16)} {Tflt(2)}
{
 RaviArray *arr = arrvalue(R(8));
 lua_Unsigned ukey = (lua_Unsigned) i_16;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_2 = iptr[ukey];
}
// CALL {T(8), T(9), Tflt(3), ' / ' Ks(3), Tflt(2), '\n' Ks(4)} {T(8..), 1 Kint(0)}
 if (stackoverflow(L,7)) { luaD_growstack(L, 7); base = ci->u.l.base; }
 L->top = R(18) + 6;
{
 TValue *dst_reg = R(23);
 TValue *src_reg = K(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(22);
 setfltvalue(dst_reg, f_2);
}
{
 TValue *dst_reg = R(21);
 TValue *src_reg = K(3);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(20);
 setfltvalue(dst_reg, f_3);
}
{
 TValue *ra = R(18);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// MOV {Upval(6, Proc%1, write)} {T(9)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(19);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CONCAT {'Performing R(' Ks(5), Tint(7), ') = R(' Ks(6), Tint(7), ') - m(' Ks(7), Tint(7), ',' Ks(1), Tint(3), ') * R(' Ks(8), Tint(3), ')\n' Ks(9)} {T(10)}
 if (stackoverflow(L,11)) { luaD_growstack(L, 11); base = ci->u.l.base; }
{
{
 TValue *dst_reg = R(34);
 TValue *src_reg = K(5);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(35);
 setivalue(dst_reg, i_7);
}
{
 TValue *dst_reg = R(36);
 TValue *src_reg = K(6);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(37);
 setivalue(dst_reg, i_7);
}
{
 TValue *dst_reg = R(38);
 TValue *src_reg = K(7);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(39);
 setivalue(dst_reg, i_7);
}
{
 TValue *dst_reg = R(40);
 TValue *src_reg = K(1);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(41);
 setivalue(dst_reg, i_3);
}
{
 TValue *dst_reg = R(42);
 TValue *src_reg = K(8);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(43);
 setivalue(dst_reg, i_3);
}
{
 TValue *dst_reg = R(44);
 TValue *src_reg = K(9);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
 L->top = R(34) + 11;
 luaV_concat(L, 11);
 base = ci->u.l.base;
 L->top = ci->top;
{
 const TValue *src_reg = R(34);
 TValue *dst_reg = R(20);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
}
// CALL {T(9), T(10)} {T(9..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(19) + 2;
{
 TValue *ra = R(19);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// MOV {Tint(3)} {Tint(18)}
i_18 = i_3;
// ADDii {local(n, 3), 1 Kint(0)} {Tint(19)}
{ i_19 = ivalue(R(3)) + 1; }
// MOV {Tint(19)} {Tint(20)}
i_20 = i_19;
// MOV {1 Kint(0)} {Tint(21)}
i_21 = 1;
// SUBii {Tint(18), Tint(21)} {Tint(18)}
{ i_18 = i_18 - i_21; }
// BR {L18}
goto L18;
L17:
// MOV {Upval(6, Proc%1, write)} {T(10)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(20);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(10), 'Post elimination column ' Ks(10), Tint(3), '\n' Ks(4)} {T(10..), 1 Kint(0)}
 if (stackoverflow(L,5)) { luaD_growstack(L, 5); base = ci->u.l.base; }
 L->top = R(20) + 4;
{
 TValue *dst_reg = R(23);
 TValue *src_reg = K(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(22);
 setivalue(dst_reg, i_3);
}
{
 TValue *dst_reg = R(21);
 TValue *src_reg = K(10);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(20);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// MOV {Upval(5, Proc%1, dump_matrix)} {T(11)}
{
 const TValue *src_reg = cl->upvals[5]->v;
 TValue *dst_reg = R(21);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// ADDii {local(n, 3), 1 Kint(0)} {Tint(9)}
{ i_9 = ivalue(R(3)) + 1; }
// CALL {T(11), local(columns, 5), local(n, 3), Tint(9), local(nrow, 4)} {T(11..), 1 Kint(0)}
 if (stackoverflow(L,6)) { luaD_growstack(L, 6); base = ci->u.l.base; }
 L->top = R(21) + 5;
{
 const TValue *src_reg = R(4);
 TValue *dst_reg = R(25);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *dst_reg = R(24);
 setivalue(dst_reg, i_9);
}
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(23);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 const TValue *src_reg = R(5);
 TValue *dst_reg = R(22);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(21);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L10}
goto L10;
L18:
// ADDii {Tint(18), Tint(21)} {Tint(18)}
{ i_18 = i_18 + i_21; }
// BR {L19}
goto L19;
L19:
// LIii {Tint(20), Tint(18)} {Tbool(22)}
{ i_22 = i_20 < i_18; }
// CBR {Tbool(22)} {L21, L20}
{ if (i_22 != 0) goto L21; else goto L20; }
L20:
// MOV {Tint(18)} {Tint(17)}
i_17 = i_18;
// TGETik {local(columns, 5), Tint(17)} {T(10)}
{
 TValue *tab = R(5);
 TValue *key = &ival0; ival0.value_.i = i_17;
 TValue *dst = R(20);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// TOFARRAY {T(10)}
{
 TValue *ra = R(20);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(10)} {local(col, 9)}
{
 const TValue *src_reg = R(20);
 TValue *dst_reg = R(9);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// IAGETik {local(nrow, 4), Tint(7)} {Tint(23)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_7;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_23 = iptr[ukey];
}
// IAGETik {local(nrow, 4), Tint(7)} {Tint(24)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_7;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_24 = iptr[ukey];
}
// FAGETik {local(col, 9), Tint(24)} {Tflt(2)}
{
 RaviArray *arr = arrvalue(R(9));
 lua_Unsigned ukey = (lua_Unsigned) i_24;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_2 = iptr[ukey];
}
// IAGETik {local(nrow, 4), Tint(3)} {Tint(25)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_3;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_25 = iptr[ukey];
}
// FAGETik {local(col, 9), Tint(25)} {Tflt(1)}
{
 RaviArray *arr = arrvalue(R(9));
 lua_Unsigned ukey = (lua_Unsigned) i_25;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_1 = iptr[ukey];
}
// MULff {Tflt(0), Tflt(1)} {Tflt(4)}
{ f_4 = f_0 * f_1; }
// SUBff {Tflt(2), Tflt(4)} {Tflt(1)}
{ f_1 = f_2 - f_4; }
// FAPUTfv {Tflt(1)} {local(col, 9), Tint(23)}
{
 RaviArray *arr = arrvalue(R(9));
 lua_Unsigned ukey = (lua_Unsigned) i_23;
 lua_Number *iptr = (lua_Number *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = f_1;
} else {
 raviH_set_float(L, arr, ukey, f_1);
}
}
// BR {L18}
goto L18;
L21:
// BR {L14}
goto L14;
L22:
// TGETik {local(columns, 5), local(n, 3)} {T(12)}
{
 TValue *tab = R(5);
 TValue *key = R(3);
 TValue *dst = R(22);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// IAGETik {local(nrow, 4), local(n, 3)} {Tint(4)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) ivalue(R(3));
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_4 = iptr[ukey];
}
// GETik {T(12), Tint(4)} {T(13)}
{
 TValue *tab = R(22);
 TValue *key = &ival0; ival0.value_.i = i_4;
 TValue *dst = R(23);
 luaV_gettable(L, tab, key, dst);
 base = ci->u.l.base;
}
// EQ {T(13), 0.000000000000 Kflt(0)} {T(14)}
{
 int result = 0;
 TValue *rb = R(23);
 TValue *rc = &fval1; fval1.value_.n = 0;
 if (ttisinteger(rb) && ttisinteger(rc))
  result = (ivalue(rb) == ivalue(rc));
 else {
  result = luaV_equalobj(L, rb, rc);
  base = ci->u.l.base;
 }
 setbvalue(R(24), result != 0);
}
// CBR {T(14)} {L23, L24}
{
const TValue *src_reg = R(24);
if (!l_isfalse(src_reg)) goto L23;
else goto L24;
}
L23:
// LOADGLOBAL {Upval(_ENV), 'error' Ks(11)} {T(13)}
{
 TValue *tab = cl->upvals[7]->v;
 TValue *key = K(11);
 TValue *dst = R(23);
 raviV_gettable_sskey(L, tab, key, dst);
 base = ci->u.l.base;
}
// MOV {T(13)} {T(15)}
{
 const TValue *src_reg = R(23);
 TValue *dst_reg = R(25);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(15), 'no unique solution exists' Ks(12)} {T(15..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(25) + 2;
{
 TValue *dst_reg = R(26);
 TValue *src_reg = K(12);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(25);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L24}
goto L24;
L24:
// MOV {Upval(8, Proc%1, numarray)} {T(16)}
{
 const TValue *src_reg = cl->upvals[8]->v;
 TValue *dst_reg = R(26);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(16), local(n, 3), 0.000000000000 Kflt(0)} {T(16..), 1 Kint(0)}
 if (stackoverflow(L,4)) { luaD_growstack(L, 4); base = ci->u.l.base; }
 L->top = R(26) + 3;
{
 TValue *dst_reg = R(28);
 setfltvalue(dst_reg, 0);
}
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(27);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *ra = R(26);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TOFARRAY {T(16[16..])}
{
 TValue *ra = R(26);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(16[16..])} {local(x, 6)}
{
 const TValue *src_reg = R(26);
 TValue *dst_reg = R(6);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// TGETik {local(columns, 5), local(n, 3)} {T(17)}
{
 TValue *tab = R(5);
 TValue *key = R(3);
 TValue *dst = R(27);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// TOFARRAY {T(17)}
{
 TValue *ra = R(27);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(17)} {local(a, 7)}
{
 const TValue *src_reg = R(27);
 TValue *dst_reg = R(7);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// MOV {Upval(6, Proc%1, write)} {T(17)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(27);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(17), 'Performing back substitution\n' Ks(13)} {T(17..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(27) + 2;
{
 TValue *dst_reg = R(28);
 TValue *src_reg = K(13);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(27);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// IAGETik {local(nrow, 4), local(n, 3)} {Tint(5)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) ivalue(R(3));
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_5 = iptr[ukey];
}
// FAGETik {local(b, 1), Tint(5)} {Tflt(1)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) i_5;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_1 = iptr[ukey];
}
// IAGETik {local(nrow, 4), local(n, 3)} {Tint(2)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) ivalue(R(3));
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_2 = iptr[ukey];
}
// FAGETik {local(a, 7), Tint(2)} {Tflt(3)}
{
 RaviArray *arr = arrvalue(R(7));
 lua_Unsigned ukey = (lua_Unsigned) i_2;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_3 = iptr[ukey];
}
// DIVff {Tflt(1), Tflt(3)} {Tflt(4)}
{ f_4 = f_1 / f_3; }
// FAPUTfv {Tflt(4)} {local(x, 6), local(n, 3)}
{
 RaviArray *arr = arrvalue(R(6));
 lua_Unsigned ukey = (lua_Unsigned) ivalue(R(3));
 lua_Number *iptr = (lua_Number *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = f_4;
} else {
 raviH_set_float(L, arr, ukey, f_4);
}
}
// MOV {Upval(6, Proc%1, write)} {T(18)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(28);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// FAGETik {local(x, 6), local(n, 3)} {Tflt(4)}
{
 RaviArray *arr = arrvalue(R(6));
 lua_Unsigned ukey = (lua_Unsigned) ivalue(R(3));
 lua_Number *iptr = (lua_Number *)arr->data;
 f_4 = iptr[ukey];
}
// CALL {T(18), 'x[' Ks(14), local(n, 3), '] = b[' Ks(15), local(n, 3), '] / a[' Ks(16), local(n, 3), '] = ' Ks(17), Tflt(4), '\n' Ks(4)} {T(18..), 1 Kint(0)}
 if (stackoverflow(L,11)) { luaD_growstack(L, 11); base = ci->u.l.base; }
 L->top = R(28) + 10;
{
 TValue *dst_reg = R(37);
 TValue *src_reg = K(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(36);
 setfltvalue(dst_reg, f_4);
}
{
 TValue *dst_reg = R(35);
 TValue *src_reg = K(17);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(34);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *dst_reg = R(33);
 TValue *src_reg = K(16);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(32);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *dst_reg = R(31);
 TValue *src_reg = K(15);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 const TValue *src_reg = R(3);
 TValue *dst_reg = R(30);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
{
 TValue *dst_reg = R(29);
 TValue *src_reg = K(14);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(28);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// SUBii {local(n, 3), 1 Kint(0)} {Tint(3)}
{ i_3 = ivalue(R(3)) - 1; }
// MOV {Tint(3)} {Tint(9)}
i_9 = i_3;
// MOV {1 Kint(0)} {Tint(10)}
i_10 = 1;
// MOV {-1 Kint(1)} {Tint(11)}
i_11 = -1;
// LIii {0 Kint(2), Tint(11)} {Tbool(12)}
{ i_12 = 0 < i_11; }
// SUBii {Tint(9), Tint(11)} {Tint(9)}
{ i_9 = i_9 - i_11; }
// BR {L25}
goto L25;
L25:
// ADDii {Tint(9), Tint(11)} {Tint(9)}
{ i_9 = i_9 + i_11; }
// CBR {Tbool(12)} {L26, L27}
{ if (i_12 != 0) goto L26; else goto L27; }
L26:
// LIii {Tint(10), Tint(9)} {Tbool(7)}
{ i_7 = i_10 < i_9; }
// CBR {Tbool(7)} {L29, L28}
{ if (i_7 != 0) goto L29; else goto L28; }
L27:
// LIii {Tint(9), Tint(10)} {Tbool(7)}
{ i_7 = i_9 < i_10; }
// CBR {Tbool(7)} {L29, L28}
{ if (i_7 != 0) goto L29; else goto L28; }
L28:
// MOV {Tint(9)} {Tint(6)}
i_6 = i_9;
// INIT {Tflt(4)}
f_4 = (lua_Number)0;
// ADDii {Tint(6), 1 Kint(0)} {Tint(20)}
{ i_20 = i_6 + 1; }
// MOV {Tint(20)} {Tint(21)}
i_21 = i_20;
// MOV {local(n, 3)} {Tint(22)}
{
TValue *reg = R(3);
i_22 = ivalue(reg);
}
// MOV {1 Kint(0)} {Tint(17)}
i_17 = 1;
// SUBii {Tint(21), Tint(17)} {Tint(21)}
{ i_21 = i_21 - i_17; }
// BR {L30}
goto L30;
L29:
// RET {local(x, 6)} {L1}
{
 TValue *stackbase = ci->func;
 int wanted = ci->nresults;
 result = wanted == -1 ? 0 : 1;
 if (wanted == -1) wanted = 1;
 int j = 0;
 if (0 < wanted) {
{
 const TValue *src_reg = R(6);
 TValue *dst_reg = S(0);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
 }
 j++;
 while (j < wanted) {
  setnilvalue(S(j));
  j++;
 }
 L->top = S(0) + wanted;
 L->ci = ci->previous;
}
goto L1;
L30:
// ADDii {Tint(21), Tint(17)} {Tint(21)}
{ i_21 = i_21 + i_17; }
// BR {L31}
goto L31;
L31:
// LIii {Tint(22), Tint(21)} {Tbool(26)}
{ i_26 = i_22 < i_21; }
// CBR {Tbool(26)} {L33, L32}
{ if (i_26 != 0) goto L33; else goto L32; }
L32:
// MOV {Tint(21)} {Tint(18)}
i_18 = i_21;
// TGETik {local(columns, 5), Tint(18)} {T(19)}
{
 TValue *tab = R(5);
 TValue *key = &ival0; ival0.value_.i = i_18;
 TValue *dst = R(29);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// TOFARRAY {T(19)}
{
 TValue *ra = R(29);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(19)} {local(a, 7)}
{
 const TValue *src_reg = R(29);
 TValue *dst_reg = R(7);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// IAGETik {local(nrow, 4), Tint(6)} {Tint(27)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_6;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_27 = iptr[ukey];
}
// FAGETik {local(a, 7), Tint(27)} {Tflt(0)}
{
 RaviArray *arr = arrvalue(R(7));
 lua_Unsigned ukey = (lua_Unsigned) i_27;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_0 = iptr[ukey];
}
// FAGETik {local(x, 6), Tint(18)} {Tflt(3)}
{
 RaviArray *arr = arrvalue(R(6));
 lua_Unsigned ukey = (lua_Unsigned) i_18;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_3 = iptr[ukey];
}
// MULff {Tflt(0), Tflt(3)} {Tflt(1)}
{ f_1 = f_0 * f_3; }
// ADDff {Tflt(4), Tflt(1)} {Tflt(3)}
{ f_3 = f_4 + f_1; }
// MOVf {Tflt(3)} {Tflt(4)}
f_4 = f_3;
// BR {L34}
goto L34;
L33:
// MOV {Upval(6, Proc%1, write)} {T(22)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(32);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(22), 'sum = ' Ks(18), Tflt(4), '\n' Ks(4)} {T(22..), 1 Kint(0)}
 if (stackoverflow(L,5)) { luaD_growstack(L, 5); base = ci->u.l.base; }
 L->top = R(32) + 4;
{
 TValue *dst_reg = R(35);
 TValue *src_reg = K(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(34);
 setfltvalue(dst_reg, f_4);
}
{
 TValue *dst_reg = R(33);
 TValue *src_reg = K(18);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(32);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// TGETik {local(columns, 5), Tint(6)} {T(23)}
{
 TValue *tab = R(5);
 TValue *key = &ival0; ival0.value_.i = i_6;
 TValue *dst = R(33);
 raviV_gettable_i(L, tab, key, dst);
 base = ci->u.l.base;
}
// TOFARRAY {T(23)}
{
 TValue *ra = R(33);
 if (!ttisfarray(ra)) {
  error_code = 3;
  goto Lraise_error;
 }
}
// MOV {T(23)} {local(a, 7)}
{
 const TValue *src_reg = R(33);
 TValue *dst_reg = R(7);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// IAGETik {local(nrow, 4), Tint(6)} {Tint(21)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_6;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_21 = iptr[ukey];
}
// FAGETik {local(b, 1), Tint(21)} {Tflt(1)}
{
 RaviArray *arr = arrvalue(R(1));
 lua_Unsigned ukey = (lua_Unsigned) i_21;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_1 = iptr[ukey];
}
// SUBff {Tflt(1), Tflt(4)} {Tflt(0)}
{ f_0 = f_1 - f_4; }
// IAGETik {local(nrow, 4), Tint(6)} {Tint(22)}
{
 RaviArray *arr = arrvalue(R(4));
 lua_Unsigned ukey = (lua_Unsigned) i_6;
 lua_Integer *iptr = (lua_Integer *)arr->data;
 i_22 = iptr[ukey];
}
// FAGETik {local(a, 7), Tint(22)} {Tflt(1)}
{
 RaviArray *arr = arrvalue(R(7));
 lua_Unsigned ukey = (lua_Unsigned) i_22;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_1 = iptr[ukey];
}
// DIVff {Tflt(0), Tflt(1)} {Tflt(2)}
{ f_2 = f_0 / f_1; }
// FAPUTfv {Tflt(2)} {local(x, 6), Tint(6)}
{
 RaviArray *arr = arrvalue(R(6));
 lua_Unsigned ukey = (lua_Unsigned) i_6;
 lua_Number *iptr = (lua_Number *)arr->data;
 if (ukey < (lua_Unsigned)(arr->len)) {
 iptr[ukey] = f_2;
} else {
 raviH_set_float(L, arr, ukey, f_2);
}
}
// MOV {Upval(6, Proc%1, write)} {T(23)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(33);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// FAGETik {local(x, 6), Tint(6)} {Tflt(2)}
{
 RaviArray *arr = arrvalue(R(6));
 lua_Unsigned ukey = (lua_Unsigned) i_6;
 lua_Number *iptr = (lua_Number *)arr->data;
 f_2 = iptr[ukey];
}
// CALL {T(23), 'x[' Ks(14), Tint(6), '] = (b[' Ks(24), Tint(6), '] - sum) / a[' Ks(25), Tint(6), ', ' Ks(21), Tint(6), '] = ' Ks(17), Tflt(2), '\n' Ks(4)} {T(23..), 1 Kint(0)}
 if (stackoverflow(L,13)) { luaD_growstack(L, 13); base = ci->u.l.base; }
 L->top = R(33) + 12;
{
 TValue *dst_reg = R(44);
 TValue *src_reg = K(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(43);
 setfltvalue(dst_reg, f_2);
}
{
 TValue *dst_reg = R(42);
 TValue *src_reg = K(17);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(41);
 setivalue(dst_reg, i_6);
}
{
 TValue *dst_reg = R(40);
 TValue *src_reg = K(21);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(39);
 setivalue(dst_reg, i_6);
}
{
 TValue *dst_reg = R(38);
 TValue *src_reg = K(25);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(37);
 setivalue(dst_reg, i_6);
}
{
 TValue *dst_reg = R(36);
 TValue *src_reg = K(24);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(35);
 setivalue(dst_reg, i_6);
}
{
 TValue *dst_reg = R(34);
 TValue *src_reg = K(14);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(33);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L25}
goto L25;
L34:
// ADDii {Tint(6), 1 Kint(0)} {Tint(28)}
{ i_28 = i_6 + 1; }
// EQii {Tint(18), Tint(28)} {Tbool(29)}
{ i_29 = i_18 == i_28; }
// CBR {Tbool(29)} {L35, L36}
{ if (i_29 != 0) goto L35; else goto L36; }
L35:
// MOV {Upval(6, Proc%1, write)} {T(19)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(29);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(19), 'sum = ' Ks(18)} {T(19..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(29) + 2;
{
 TValue *dst_reg = R(30);
 TValue *src_reg = K(18);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(29);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L37}
goto L37;
L36:
// MOV {Upval(6, Proc%1, write)} {T(20)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(30);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(20), 'sum = sum + ' Ks(19)} {T(20..), 1 Kint(0)}
 if (stackoverflow(L,3)) { luaD_growstack(L, 3); base = ci->u.l.base; }
 L->top = R(30) + 2;
{
 TValue *dst_reg = R(31);
 TValue *src_reg = K(19);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(30);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L37}
goto L37;
L37:
// MOV {Upval(6, Proc%1, write)} {T(21)}
{
 const TValue *src_reg = cl->upvals[6]->v;
 TValue *dst_reg = R(31);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.n = src_reg->value_.n;
}
// CALL {T(21), 'a[' Ks(20), Tint(6), ', ' Ks(21), Tint(18), '] * x[' Ks(22), Tint(18), ']' Ks(23), '\n' Ks(4)} {T(21..), 1 Kint(0)}
 if (stackoverflow(L,10)) { luaD_growstack(L, 10); base = ci->u.l.base; }
 L->top = R(31) + 9;
{
 TValue *dst_reg = R(39);
 TValue *src_reg = K(4);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(38);
 TValue *src_reg = K(23);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(37);
 setivalue(dst_reg, i_18);
}
{
 TValue *dst_reg = R(36);
 TValue *src_reg = K(22);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(35);
 setivalue(dst_reg, i_18);
}
{
 TValue *dst_reg = R(34);
 TValue *src_reg = K(21);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *dst_reg = R(33);
 setivalue(dst_reg, i_6);
}
{
 TValue *dst_reg = R(32);
 TValue *src_reg = K(20);
 dst_reg->tt_ = src_reg->tt_;
 dst_reg->value_.gc = src_reg->value_.gc;
}
{
 TValue *ra = R(31);
 int result = luaD_precall(L, ra, 1, 1);
 if (result) {
  if (result == 1 && 1 >= 0)
   L->top = ci->top;
 }
 else {  /* Lua function */
  result = luaV_execute(L);
  if (result) L->top = ci->top;
 }
 base = ci->u.l.base;
}
// BR {L30}
goto L30;
}
EXPORT LClosure *mymain(lua_State *L) {
 LClosure *cl = luaF_newLclosure(L, 1);
 setclLvalue(L, L->top, cl);
 luaD_inctop(L);
 cl->p = luaF_newproto(L);
 Proto *f = cl->p;
 f->ravi_jit.jit_function = __ravifunc_1;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 0;
 f->is_vararg = 0;
 f->maxstacksize = 15;
 f->k = luaM_newvector(L, 8, TValue);
 f->sizek = 8;
 for (int i = 0; i < 8; i++)
  setnilvalue(&f->k[i]);
 {
  TValue *o = &f->k[0];
  setsvalue2n(L, o, luaS_newlstr(L, "assert", 6));
 }
 {
  TValue *o = &f->k[1];
  setsvalue2n(L, o, luaS_newlstr(L, "table", 5));
 }
 {
  TValue *o = &f->k[7];
  setsvalue2n(L, o, luaS_newlstr(L, "gaussian_solve", 14));
 }
 {
  TValue *o = &f->k[3];
  setsvalue2n(L, o, luaS_newlstr(L, "numarray", 8));
 }
 {
  TValue *o = &f->k[6];
  setsvalue2n(L, o, luaS_newlstr(L, "write", 5));
 }
 {
  TValue *o = &f->k[2];
  setsvalue2n(L, o, luaS_newlstr(L, "slice", 5));
 }
 {
  TValue *o = &f->k[5];
  setsvalue2n(L, o, luaS_newlstr(L, "io", 2));
 }
 {
  TValue *o = &f->k[4];
  setsvalue2n(L, o, luaS_newlstr(L, "intarray", 8));
 }
 f->upvalues = luaM_newvector(L, 1, Upvaldesc);
 f->sizeupvalues = 1;
 f->upvalues[0].instack = 1;
 f->upvalues[0].idx = 0;
 f->upvalues[0].name = NULL; // _ENV
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = 128;
 f->p = luaM_newvector(L, 4, Proto *);
 f->sizep = 4;
 for (int i = 0; i < 4; i++)
   f->p[i] = NULL;
 f->p[0] = luaF_newproto(L);
{ 
 Proto *parent = f; f = f->p[0];
 f->ravi_jit.jit_function = __ravifunc_2;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 1;
 f->is_vararg = 0;
 f->maxstacksize = 3;
 f->k = luaM_newvector(L, 0, TValue);
 f->sizek = 0;
 for (int i = 0; i < 0; i++)
  setnilvalue(&f->k[i]);
 f->upvalues = luaM_newvector(L, 1, Upvaldesc);
 f->sizeupvalues = 1;
 f->upvalues[0].instack = 1;
 f->upvalues[0].idx = 2;
 f->upvalues[0].name = NULL; // numarray
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = -1;
 f = parent;
}
 f->p[1] = luaF_newproto(L);
{ 
 Proto *parent = f; f = f->p[1];
 f->ravi_jit.jit_function = __ravifunc_3;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 4;
 f->is_vararg = 0;
 f->maxstacksize = 10;
 f->k = luaM_newvector(L, 5, TValue);
 f->sizek = 5;
 for (int i = 0; i < 5; i++)
  setnilvalue(&f->k[i]);
 {
  TValue *o = &f->k[4];
  setsvalue2n(L, o, luaS_newlstr(L, "\n", 2));
 }
 {
  TValue *o = &f->k[1];
  setsvalue2n(L, o, luaS_newlstr(L, "no unique solution exists", 25));
 }
 {
  TValue *o = &f->k[3];
  setsvalue2n(L, o, luaS_newlstr(L, " will be swapped with ", 22));
 }
 {
  TValue *o = &f->k[0];
  setsvalue2n(L, o, luaS_newlstr(L, "error", 5));
 }
 {
  TValue *o = &f->k[2];
  setsvalue2n(L, o, luaS_newlstr(L, "Performing row interchange ", 27));
 }
 f->upvalues = luaM_newvector(L, 2, Upvaldesc);
 f->sizeupvalues = 2;
 f->upvalues[0].instack = 0;
 f->upvalues[0].idx = 0;
 f->upvalues[0].name = NULL; // _ENV
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = 128;
 f->upvalues[1].instack = 1;
 f->upvalues[1].idx = 4;
 f->upvalues[1].name = NULL; // write
 f->upvalues[1].usertype = NULL;
 f->upvalues[1].ravi_type = -1;
 f = parent;
}
 f->p[2] = luaF_newproto(L);
{ 
 Proto *parent = f; f = f->p[2];
 f->ravi_jit.jit_function = __ravifunc_4;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 4;
 f->is_vararg = 0;
 f->maxstacksize = 7;
 f->k = luaM_newvector(L, 2, TValue);
 f->sizek = 2;
 for (int i = 0; i < 2; i++)
  setnilvalue(&f->k[i]);
 {
  TValue *o = &f->k[0];
  setsvalue2n(L, o, luaS_newlstr(L, " ", 1));
 }
 {
  TValue *o = &f->k[1];
  setsvalue2n(L, o, luaS_newlstr(L, "\n", 2));
 }
 f->upvalues = luaM_newvector(L, 1, Upvaldesc);
 f->sizeupvalues = 1;
 f->upvalues[0].instack = 1;
 f->upvalues[0].idx = 4;
 f->upvalues[0].name = NULL; // write
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = -1;
 f = parent;
}
 f->p[3] = luaF_newproto(L);
{ 
 Proto *parent = f; f = f->p[3];
 f->ravi_jit.jit_function = __ravifunc_5;
 f->ravi_jit.jit_status = RAVI_JIT_COMPILED;
 f->numparams = 4;
 f->is_vararg = 0;
 f->maxstacksize = 34;
 f->k = luaM_newvector(L, 26, TValue);
 f->sizek = 26;
 for (int i = 0; i < 26; i++)
  setnilvalue(&f->k[i]);
 {
  TValue *o = &f->k[10];
  setsvalue2n(L, o, luaS_newlstr(L, "Post elimination column ", 24));
 }
 {
  TValue *o = &f->k[13];
  setsvalue2n(L, o, luaS_newlstr(L, "Performing back substitution\n", 30));
 }
 {
  TValue *o = &f->k[5];
  setsvalue2n(L, o, luaS_newlstr(L, "Performing R(", 13));
 }
 {
  TValue *o = &f->k[4];
  setsvalue2n(L, o, luaS_newlstr(L, "\n", 2));
 }
 {
  TValue *o = &f->k[8];
  setsvalue2n(L, o, luaS_newlstr(L, ") * R(", 6));
 }
 {
  TValue *o = &f->k[1];
  setsvalue2n(L, o, luaS_newlstr(L, ",", 1));
 }
 {
  TValue *o = &f->k[18];
  setsvalue2n(L, o, luaS_newlstr(L, "sum = ", 6));
 }
 {
  TValue *o = &f->k[0];
  setsvalue2n(L, o, luaS_newlstr(L, "m(", 2));
 }
 {
  TValue *o = &f->k[2];
  setsvalue2n(L, o, luaS_newlstr(L, ") = ", 4));
 }
 {
  TValue *o = &f->k[3];
  setsvalue2n(L, o, luaS_newlstr(L, " / ", 3));
 }
 {
  TValue *o = &f->k[16];
  setsvalue2n(L, o, luaS_newlstr(L, "] / a[", 6));
 }
 {
  TValue *o = &f->k[6];
  setsvalue2n(L, o, luaS_newlstr(L, ") = R(", 6));
 }
 {
  TValue *o = &f->k[23];
  setsvalue2n(L, o, luaS_newlstr(L, "]", 1));
 }
 {
  TValue *o = &f->k[15];
  setsvalue2n(L, o, luaS_newlstr(L, "] = b[", 6));
 }
 {
  TValue *o = &f->k[19];
  setsvalue2n(L, o, luaS_newlstr(L, "sum = sum + ", 12));
 }
 {
  TValue *o = &f->k[14];
  setsvalue2n(L, o, luaS_newlstr(L, "x[", 2));
 }
 {
  TValue *o = &f->k[12];
  setsvalue2n(L, o, luaS_newlstr(L, "no unique solution exists", 25));
 }
 {
  TValue *o = &f->k[7];
  setsvalue2n(L, o, luaS_newlstr(L, ") - m(", 6));
 }
 {
  TValue *o = &f->k[21];
  setsvalue2n(L, o, luaS_newlstr(L, ", ", 2));
 }
 {
  TValue *o = &f->k[9];
  setsvalue2n(L, o, luaS_newlstr(L, ")\n", 3));
 }
 {
  TValue *o = &f->k[25];
  setsvalue2n(L, o, luaS_newlstr(L, "] - sum) / a[", 13));
 }
 {
  TValue *o = &f->k[17];
  setsvalue2n(L, o, luaS_newlstr(L, "] = ", 4));
 }
 {
  TValue *o = &f->k[22];
  setsvalue2n(L, o, luaS_newlstr(L, "] * x[", 6));
 }
 {
  TValue *o = &f->k[24];
  setsvalue2n(L, o, luaS_newlstr(L, "] = (b[", 7));
 }
 {
  TValue *o = &f->k[11];
  setsvalue2n(L, o, luaS_newlstr(L, "error", 5));
 }
 {
  TValue *o = &f->k[20];
  setsvalue2n(L, o, luaS_newlstr(L, "a[", 2));
 }
 f->upvalues = luaM_newvector(L, 9, Upvaldesc);
 f->sizeupvalues = 9;
 f->upvalues[0].instack = 1;
 f->upvalues[0].idx = 5;
 f->upvalues[0].name = NULL; // copy
 f->upvalues[0].usertype = NULL;
 f->upvalues[0].ravi_type = 513;
 f->upvalues[1].instack = 1;
 f->upvalues[1].idx = 0;
 f->upvalues[1].name = NULL; // assert
 f->upvalues[1].usertype = NULL;
 f->upvalues[1].ravi_type = -1;
 f->upvalues[2].instack = 1;
 f->upvalues[2].idx = 3;
 f->upvalues[2].name = NULL; // intarray
 f->upvalues[2].usertype = NULL;
 f->upvalues[2].ravi_type = -1;
 f->upvalues[3].instack = 1;
 f->upvalues[3].idx = 1;
 f->upvalues[3].name = NULL; // slice
 f->upvalues[3].usertype = NULL;
 f->upvalues[3].ravi_type = -1;
 f->upvalues[4].instack = 1;
 f->upvalues[4].idx = 6;
 f->upvalues[4].name = NULL; // partial_pivot
 f->upvalues[4].usertype = NULL;
 f->upvalues[4].ravi_type = 513;
 f->upvalues[5].instack = 1;
 f->upvalues[5].idx = 7;
 f->upvalues[5].name = NULL; // dump_matrix
 f->upvalues[5].usertype = NULL;
 f->upvalues[5].ravi_type = 513;
 f->upvalues[6].instack = 1;
 f->upvalues[6].idx = 4;
 f->upvalues[6].name = NULL; // write
 f->upvalues[6].usertype = NULL;
 f->upvalues[6].ravi_type = -1;
 f->upvalues[7].instack = 0;
 f->upvalues[7].idx = 0;
 f->upvalues[7].name = NULL; // _ENV
 f->upvalues[7].usertype = NULL;
 f->upvalues[7].ravi_type = 128;
 f->upvalues[8].instack = 1;
 f->upvalues[8].idx = 2;
 f->upvalues[8].name = NULL; // numarray
 f->upvalues[8].usertype = NULL;
 f->upvalues[8].ravi_type = -1;
 f = parent;
}
 return cl;
}
