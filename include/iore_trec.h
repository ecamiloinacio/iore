/*
 * iore_trec.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_TREC_H_
#define INCLUDE_IORE_TREC_H_

#include "iore_trec_types.h"

/*** PROTOTYPES **************************************************************/

void
trec_init ();
void
trec_destroy ();

iore_trec_exp_t
get_trec_exp ();

void
trec_exp_start (unsigned int);
iore_trec_exp_t *
trec_exp_stop ();
void
trec_exp_repl_start (unsigned int, unsigned int);
iore_trec_exp_repl_t *
trec_exp_repl_stop ();
void
trec_run_start (unsigned int, unsigned int);
iore_trec_run_t *
trec_run_stop ();
void
trec_run_rept_start (unsigned int);
iore_trec_run_rept_t *
trec_run_rept_stop ();
void
trec_test_start (enum iore_test_type, enum iore_trec_action);
void
trec_test_stop (size_t);
iore_trec_test_t *
trec_test_commit ();

#endif /* INCLUDE_IORE_TREC_H_ */
