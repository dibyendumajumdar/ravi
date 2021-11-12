-- implement a date type using embedded C

C__decl [[
typedef struct {
    unsigned char d;
    unsigned char m;
    short y;
    int serial;
} Date;
]]

DateFunctions = {}
function DateFunctions.make_date(d: integer, m: integer, y: integer)
    local date = C__new('Date', 1)

    C__unsafe(date, d, m, y) [[
        Date *dateptr = (Date *) date.ptr;
        dateptr->d = (unsigned char)d;
        dateptr->m = (unsigned char)m;
        dateptr->y = (short)y;
        y -= m <= 2;
        int era = (y >= 0 ? y : y - 399) / 400;
        unsigned yoe = (unsigned)(y - era * 400);	   // [0, 399]
        unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0, 365]
        unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;	  // [0, 146096]
        dateptr->serial = era * 146097 + (int)doe - 719468 + 25569; // +25569 adjusts the serial number to match Excel
    ]]

    return date
end

function DateFunctions.print_date(date: any)

    local d: integer
    local m: integer
    local y: integer
    local j: integer
    C__unsafe(date, d, m, y, j) [[
            Date *dateptr = (Date *) date.ptr;
            d = dateptr->d;
            m = dateptr->m;
            y = dateptr->y;
            j = dateptr->serial;
    ]]

    print(d,m,y,j)
end

function DateFunctions.get_day(date: any)

    local v: integer
    C__unsafe(date, v) [[
            Date *dateptr = (Date *) date.ptr;
            v = dateptr->d;
    ]]

    return v
end

function DateFunctions.get_month(date: any)

    local v: integer
    C__unsafe(date, v) [[
            Date *dateptr = (Date *) date.ptr;
            v = dateptr->m;
    ]]

    return v
end

function DateFunctions.get_year(date: any)

    local v: integer
    C__unsafe(date, v) [[
            Date *dateptr = (Date *) date.ptr;
            v = dateptr->y;
    ]]

    return v
end

function DateFunctions.get_serial(date: any)

    local v: integer
    C__unsafe(date, v) [[
            Date *dateptr = (Date *) date.ptr;
            v = dateptr->serial;
    ]]

    return v
end

local d = DateFunctions.make_date(1,1,1900)
DateFunctions.print_date(d)
assert(1900 == DateFunctions.get_year(d))
assert(1 == DateFunctions.get_month(d))
assert(1 == DateFunctions.get_day(d))
assert(2 == DateFunctions.get_serial(d))

print '56 Ok'