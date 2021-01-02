#ifndef ravicomp_CFG_H
#define ravicomp_CFG_H

#include "linearizer.h"

#include <stdio.h>

int raviX_construct_cfg(Proc *proc);
void raviX_output_cfg(Proc *proc, FILE *fp);

#endif
