/*
 * iore_stex_valid.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <string.h>

#include "iore_stex_valid.h"
#include "iore_valid.h"
#include "iore_error.h"

/*** FUNCTIONS ***************************************************************/

int
stex_valid (iore_stex_t *stex)
{
  int rerr = IORE_SUCCESS;

  if (stex->report_type.task || stex->report_type.test)
    {
      if (stex->data_format >= IORE_STEX_FORMAT_LENGTH)
	{
	  iore_errorf(VALID_MSG_INVALID_OPTION, "data format");
	  rerr = IORE_FAILURE;
	}

      if (stex->export_dir == NULL || strlen (stex->export_dir) == 0)
	{
	  iore_error("Validation: Empty directory path.");
	  rerr = IORE_FAILURE;
	}
    }

  return rerr;
} /* stex_valid () */
