/*
 * iore_experiment_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_EXPERIMENT_TYPES_H_
#define INCLUDE_IORE_EXPERIMENT_TYPES_H_

#include "iore_run_types.h"
#include "iore_stex_types.h"

/*** DEFINES *****************************************************************/

#define EXP2STR_FORMAT "iore_exp_t (%p) { num_replications = %u, runs = [ %s ], _runs_len = %u, run_order = '%s', stex = %s }"
#define EXPREPL2STR_FORMAT "iore_exp_repl_t (%p) { id = %u, runs = [ %s ], _runs_len = %u }"

/*** TYPES *******************************************************************/

enum iore_exp_run_order
{
  IORE_RORDER_FIXED, /* experiment runs executed as specified by the user */
  IORE_RORDER_RANDOM, /* experiment runs executed in randomized order */
  IORE_RORDER_LENGTH
};

typedef struct iore_exp
{
  unsigned int num_replications;

  iore_run_t *runs;
  unsigned int _runs_len;
  enum iore_exp_run_order run_order;

  iore_stex_t stex;
} iore_exp_t;

typedef struct iore_exp_repl
{
  unsigned int id;

  iore_run_t **runs;
  unsigned int _runs_len;
} iore_exp_repl_t;

/*** PROTOTYPES **************************************************************/

char *
exp2str (const iore_exp_t *);
char *
exprepl2str (const iore_exp_repl_t *);

/*** VARIABLES ***************************************************************/

extern const char *exp_run_order_lbl[];

#endif /* INCLUDE_IORE_EXPERIMENT_TYPES_H_ */
