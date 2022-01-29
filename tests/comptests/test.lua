dofile("inputs/00_ret.lua")
dofile("inputs/01_mov.lua")
dofile("inputs/02_cbr_br.lua")
dofile("inputs/03_cbr_br.lua")
dofile("inputs/04_cbr_br.lua")
dofile("inputs/05_cbr_br.lua")
dofile("inputs/06_storeglobal.lua")
dofile("inputs/07_loadglobal.lua")
--dofile("inputs/08_loadstoreglobal.lua")
dofile("inputs/09_call.lua")
dofile("inputs/10_call.lua")
dofile("inputs/11_call.lua")
dofile("inputs/12_call.lua")
dofile("inputs/13_call.lua")
dofile("inputs/14_iarray.lua")
dofile("inputs/15_iarray_sieve.lua")
dofile("inputs/16_farray.lua")
dofile("inputs/17_iarray_sieve.lua")
dofile("inputs/18_table.lua")
dofile("inputs/19_arith.lua")
dofile("inputs/20_upvals.lua")
dofile("inputs/21_matrixmul.lua")
dofile("inputs/22_cmp.lua")
dofile("inputs/23_arith2.lua")
dofile("inputs/24_arith.lua")
dofile("inputs/25_bits.lua")
dofile("inputs/26_bits.lua")
dofile("inputs/27_fornum.lua")
dofile("inputs/28_fornum.lua")
dofile("inputs/29_if.lua")
dofile("inputs/30_if.lua")
dofile("inputs/31_assign.lua")
dofile("inputs/32_assign.lua")
dofile("inputs/33_assign.lua")
--dofile("inputs/34_assign.lua")
dofile("inputs/35_assign.lua")
dofile("inputs/36_assign.lua")
dofile("inputs/37_concat.lua")
dofile("inputs/38_concat.lua")
dofile("inputs/39_concat.lua")
dofile("inputs/40_genericfor.lua")
dofile("inputs/41_upvals.lua")
dofile("inputs/42_dateimpl.lua")
dofile("inputs/43_gauss1.lua")
dofile("inputs/43_gauss2.lua")
dofile("inputs/44_gauss3.lua")
dofile("inputs/45_notequal.lua")
dofile("inputs/46_notequal.lua")
dofile("inputs/47_bug.lua")
dofile("inputs/48_bug.lua")
dofile("inputs/49_bug.lua")
dofile("inputs/50_bug.lua")
dofile("inputs/51_concat.lua")
dofile("inputs/52_local_s.lua")

function runfile(file)
    local f = compiler.loadfile(file)
    assert(f and type(f) == 'function')
    f()
end

runfile("inputs/53_ravi_tests.lua")
runfile("inputs/54_embed_C.lua")
runfile("inputs/55_embed_C.lua")
runfile("inputs/56_embed_C.lua")
runfile("inputs/57_embed_C.lua")

print 'SUCCESS'

