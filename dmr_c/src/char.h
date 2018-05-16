/*
* sparse/char.h
*
* Copyright (C) 2003 Transmeta Corp.
*               2003 Linus Torvalds
*
* This version is part of the dmr_c project.
* Copyright (C) 2017 Dibyendu Majumdar
*/
#ifndef DMR_C_CHAR_H
#define DMR_C_CHAR_H

#ifdef __cplusplus
extern "C" {
#endif

extern void dmrC_get_char_constant(struct dmr_C *C, struct token *, unsigned long long *);
extern struct token *dmrC_get_string_constant(struct dmr_C *C, struct token *, struct expression *);

#ifdef __cplusplus
}
#endif


#endif