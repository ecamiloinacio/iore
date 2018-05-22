/*
 * iore_run_types.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iore_run_types.h"
#include "iore_util.h"

/*** FUNCTIONS ***************************************************************/

char *
run2str (const iore_run_t *run)
{
  char *str = NULL;

  if (run)
    {
      char *test = test2str (&run->test);

      int len = snprintf(str, 0, RUN2STR_FORMAT, run, run->id,
	  run->num_repetitions, test) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf(str, len, RUN2STR_FORMAT, run, run->id, run->num_repetitions,
		   test);
	}
    }

  return str;
} /* run2str () */

char *
runs2str (const iore_run_t *runs, int len)
{
  char *str = NULL;

  if (runs)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_l[i] = run2str (&runs[i]);
	  swp_len = strlen (swp_l[i]);
	  str_len += swp_len;
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* runs2str () */
