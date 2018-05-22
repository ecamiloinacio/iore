/*
 * iore_prng_types.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "iore_prng_types.h"
#include "iore_dict.h"

/*** VARIABLES ***************************************************************/

const char * const prng_type_lbl[IORE_PRNG_LENGTH] =
  { "unif", "norm", "geom" };

/*** FUNCTIONS ***************************************************************/

char *
prngdist2str (const iore_prng_dist_t *dist)
{
  char *str = NULL;

  if (dist)
    {
      char *params_str = dict2str (&dist->params);
      int len = snprintf (str, 0, PRNGDIST2STR_FORMAT, dist,
			  prng_type_lbl[dist->type], params_str) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, PRNGDIST2STR_FORMAT, dist,
		    prng_type_lbl[dist->type], params_str);
	}
    }

  return str;
} /* prngdist2str () */
