/*
 * iore_stex_types.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iore_stex_types.h"

/*** VARIABLES ***************************************************************/

const char * const stex_format_lbl[IORE_STEX_FORMAT_LENGTH] =
  { "csv" };
const char * const stex_report_lbl[IORE_STEX_REPORT_LENGTH] =
  { "task", "test" };

/*** FUNCTIONS ***************************************************************/

char *
stex2str (const iore_stex_t *stex)
{
  char *str = NULL;

  if (stex)
    {
      int len = snprintf(str, 0, STEX2STR_FORMAT, stex,
	  (stex->report_type.task ? "true" : "false"),
	  (stex->report_type.test ? "true" : "false"),
	  stex_format_lbl[stex->data_format], stex->export_dir) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf(str, len, STEX2STR_FORMAT, stex,
		   (stex->report_type.task ? "true" : "false"),
		   (stex->report_type.test ? "true" : "false"),
		   stex_format_lbl[stex->data_format], stex->export_dir);
	}
    }

  return str;
} /* stex2str () */
