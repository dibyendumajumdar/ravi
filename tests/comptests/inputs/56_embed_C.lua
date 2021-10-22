-- implement a date type using embedded C

C__decl [[
typedef struct {
    unsigned char d;
    unsigned char m;
    short y;
    int date;
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
        dateptr->date = era * 146097 + (int)doe - 719468 + 25569; // +25569 adjusts the serial number to match Excel
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
            j = dateptr->date;
    ]]

    print(d,m,y,j)
end

DateFunctions.print_date(DateFunctions.make_date(1,1,1900))
