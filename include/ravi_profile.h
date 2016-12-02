#ifndef RAVI_PROFILE_H
#define RAVI_PROFILE_H

#include "lua.h"
#include "lopcodes.h"

LUAI_DDEC unsigned long long raviV_profiledata[NUM_OPCODES];

LUAI_FUNC void raviV_init_profiledata(void);
LUAI_FUNC void raviV_add_profiledata(OpCode opcode);
LUAI_FUNC void raviV_print_profiledata(void); 

#endif