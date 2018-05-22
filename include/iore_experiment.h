/*
 * iore_experiment.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_EXPERIMENT_H_
#define INCLUDE_IORE_EXPERIMENT_H_

#include "iore_experiment_types.h"

/*** PROTOTYPES **************************************************************/

iore_exp_t *
exp_init (iore_exp_t *);
void
exp_free (iore_exp_t *);
int
exp_exec (iore_exp_t *);

#endif /* INCLUDE_IORE_EXPERIMENT_H_ */
