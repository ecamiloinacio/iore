/*
 * iore_test_valid.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <string.h>

#include "iore_test_valid.h"
#include "iore_workload_valid.h"
#include "iore_valid.h"
#include "iore_error.h"
#include "iore_afio.h"
#include "iore_afsb.h"

/*** FUNCTIONS ***************************************************************/

int
test_valid (iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  if (!(test->type.write || test->type.read))
    {
      iore_error("Validation: At least a write or read test must be set.");
      rerr = IORE_SUCCESS;
    }
  else if (!test->type.write && test->type.read)
    {
      iore_warn("Read exclusive tests are not currently supported."
		"Setting write test.");
      test->type.write = true;
    }

  if (test->file_mode == IORE_TEST_FMODE_LENGTH)
    {
      iore_errorf(VALID_MSG_INVALID_OPTION, "file mode");
      rerr = IORE_FAILURE;
    }
  if (test->file_name == NULL || strlen (test->file_name) == 0)
    {
      iore_error("Validation: Empty file name.");
      rerr = IORE_FAILURE;
    }

  rerr |= wkld_valid (&test->wkld);
  rerr |= afio_valid (&test->afio);
  if (test->afsb)
    rerr |= afsb_valid (test->afsb);

  return rerr;
} /* test_valid () */
