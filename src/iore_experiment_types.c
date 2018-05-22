/*
 * iore_experiment_types.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iore_experiment_types.h"

/*** VARIABLES ***************************************************************/

const char *exp_run_order_lbl[IORE_RORDER_LENGTH] =
  { "fixed", "random" };

/*** FUNCTIONS ***************************************************************/

char *
exp2str (const iore_exp_t *exp)
{
  char *str = NULL;

  if (exp)
    {
      char *runs = runs2str (exp->runs, exp->_runs_len);
      char *stex = stex2str (&exp->stex);

      int len = snprintf(str, 0, EXP2STR_FORMAT, exp, exp->num_replications,
	  runs, exp->_runs_len,
	  exp_run_order_lbl[exp->run_order], stex) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf(str, len, EXP2STR_FORMAT, exp, exp->num_replications, runs,
		   exp->_runs_len, exp_run_order_lbl[exp->run_order], stex);
	}
    }

  return str;
} /* exp2str () */

char *
exprepl2str (const iore_exp_repl_t *repl)
{
  char *str = NULL;

  if (repl)
    {
      char *runs = runs2str (*repl->runs, repl->_runs_len);

      int len = snprintf(str, 0, EXPREPL2STR_FORMAT, repl, repl->id, runs,
	  repl->_runs_len) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf(str, len, EXPREPL2STR_FORMAT, repl, repl->id, runs,
		   repl->_runs_len);
	}
    }

  return str;
} /* exprepl2str () */
