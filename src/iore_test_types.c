/*
 * iore_test_types.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iore_test_types.h"

/*** VARIABLES ***************************************************************/

const char *test_type_lbl[IORE_TEST_TYPE_LENGTH] =
  { "write", "read" };
const char *test_file_mode_lbl[IORE_TEST_FMODE_LENGTH] =
  { "Nx1", "NxN" };

/*** FUNCTIONS ***************************************************************/

char *
test2str (const iore_test_t *test)
{
  char *str = NULL;

  if (test)
    {
      char *wkld = wkld2str (&test->wkld);
      char *afio = afio2str (&test->afio);
      char *afsb = afsb2str (test->afsb);

      int len = snprintf(str, 0, TEST2STR_FORMAT, test,
	  (test->type.write ? "true" : "false"),
	  (test->type.read ? "true" : "false"),
	  (test->write_flush ? "true" : "false"),
	  (test->write_flush_per_req ? "true" : "false"),
	  test->read_reorder_offset,
	  (test->intra_test_barrier ? "true": "false"),
	  test->inter_test_delay_secs,
	  test_file_mode_lbl[test->file_mode], test->file_name,
	  (test->file_name_append_sequence_num ? "true" : "false"),
	  (test->file_name_append_task_id ? "true" : "false"),
	  (test->file_dir_per_task ? "true" : "false"),
	  (test->file_keep ? "true" : "false"), wkld, afio, afsb) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf(str, len, TEST2STR_FORMAT, test,
		   (test->type.write ? "true" : "false"),
		   (test->type.read ? "true" : "false"),
		   (test->write_flush ? "true" : "false"),
		   (test->write_flush_per_req ? "true" : "false"),
		   test->read_reorder_offset,
		   (test->intra_test_barrier ? "true" : "false"),
		   test->inter_test_delay_secs,
		   test_file_mode_lbl[test->file_mode], test->file_name,
		   (test->file_name_append_sequence_num ? "true" : "false"),
		   (test->file_name_append_task_id ? "true" : "false"),
		   (test->file_dir_per_task ? "true" : "false"),
		   (test->file_keep ? "true" : "false"), wkld, afio, afsb);
	}
    }

  return str;
} /* test2str () */
