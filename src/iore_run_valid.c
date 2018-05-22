/*
 * iore_run_valid.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef SRC_IORE_RUN_VALID_C_
#define SRC_IORE_RUN_VALID_C_

#include "iore_run_valid.h"
#include "iore_test_valid.h"
#include "iore_valid.h"
#include "iore_error.h"

/*** FUNCTIONS ***************************************************************/

int
run_valid (iore_run_t *run)
{
  int rerr = IORE_SUCCESS;

  if (run->num_repetitions == 0)
    {
      iore_errorf(VALID_MSG_ONE_REQUIRED, "repetition");
      rerr = IORE_FAILURE;
    }

  if (run->id == 0)
    run->id = 1;

  rerr |= test_valid (&run->test);

  return rerr;
} /* run_valid () */

#endif /* SRC_IORE_RUN_VALID_C_ */
