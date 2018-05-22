/*
 * iore_experiment_valid.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include "iore_experiment_valid.h"
#include "iore_run_valid.h"
#include "iore_stex_valid.h"
#include "iore_valid.h"
#include "iore_error.h"
#include "iore_ctx.h"

/*** FUNCTIONS ***************************************************************/

int
exp_valid (iore_exp_t *exp)
{
  int rerr = IORE_SUCCESS;

  if (exp->num_replications == 0)
    {
      iore_errorf(VALID_MSG_ONE_REQUIRED, "replication");
      rerr = IORE_FAILURE;
    }

  if (exp->run_order >= IORE_RORDER_LENGTH)
    {
      iore_errorf(VALID_MSG_INVALID_OPTION, "run order");
      rerr = IORE_FAILURE;
    }

  if (exp->_runs_len == 0 || exp->runs == NULL)
    {
      iore_errorf(VALID_MSG_ONE_REQUIRED, "run definition");
      rerr = IORE_FAILURE;
    }
  else if (exp->runs != NULL)
    {
      unsigned int i;
      for (i = 0; i < exp->_runs_len; i++)
	rerr |= run_valid (&exp->runs[i]);
    }

  rerr |= stex_valid (&exp->stex);

  return rerr;
} /* exp_valid () */

