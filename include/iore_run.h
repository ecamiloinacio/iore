/*
 * iore_run.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_RUN_H_
#define INCLUDE_IORE_RUN_H_

#include "iore_run_types.h"

/*** PROTOTYPES **************************************************************/

iore_run_t *
run_init (iore_run_t *);
void
run_free (iore_run_t *);
int
run_exec (iore_run_t *, unsigned int);

#endif /* INCLUDE_IORE_RUN_H_ */
