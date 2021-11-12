-- implement a date type using embedded C

C__decl [[
typedef struct {
    int m,n;
    double data[];
} Matrix;
]]

MatrixFunctions = {}
function MatrixFunctions.new(m: integer, n: integer)
    local M = C__new('Matrix', m*n)
    C__unsafe(m,n,M) [[
        Matrix *matrix = (Matrix *)M.ptr;
        matrix->m = m;
        matrix->n = n;
        for (int i = 0; i < m*n; i++)
            matrix->data[i] = 0.0;
    ]]
    return M
end

function MatrixFunctions.dim(M)
    local m: integer
    local n: integer
    C__unsafe(m,n,M) [[
        Matrix *matrix = (Matrix *)M.ptr;
        m = matrix->m;
        n = matrix->n;
    ]]
    return m, n
end

local M = MatrixFunctions.new(10,11)
local m, n = MatrixFunctions.dim(M)
assert(m == 10)
assert(n == 11)

print '57 Ok'