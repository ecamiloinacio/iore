/*
 * iore_afsb_types.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "iore_afsb_types.h"
#include "iore_dict.h"

/*** VARIABLES ***************************************************************/

const char * const afsb_lbl[IORE_AFSB_LENGTH] =
  { "unset"
#ifdef WITH_ORANGEFS_AFSB
    , "orangefs"
#endif
#ifdef WITH_LUSTRE_AFSB
    , "lustre"
#endif
};

/*** FUNCTIONS ***************************************************************/

char *
afsb2str (const iore_afsb_t *afsb)
{
  char *str = NULL;

  if (afsb)
    {
      char *params_str = dict2str (&afsb->params);
      int len = snprintf (str, 0, AFSB2STR_FORMAT, afsb, afsb_lbl[afsb->type],
	  params_str) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf(str, len, AFSB2STR_FORMAT, afsb, afsb_lbl[afsb->type],
		   params_str);
	}
    }

  return str;
} /* afsb2str () */
