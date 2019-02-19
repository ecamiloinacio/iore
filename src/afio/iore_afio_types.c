/*
 * iore_afio_types.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "iore_afio_types.h"
#include "iore_dict.h"

/*** VARIABLES ***************************************************************/

const char * const afio_lbl[IORE_AFIO_LENGTH] =
  { "posix", "cstream"
#ifdef WITH_MPIIO_AFIO
    , "mpiio"
#endif
#ifdef WITH_OFSPROTO_AFIO
    , "ofsproto"
#endif
};

/*** FUNCTIONS ***************************************************************/

char *
afio2str (const iore_afio_t *afio)
{
  char *str = NULL;

  if (afio)
    {
      char *params_str = dict2str (&afio->params);
      int len = snprintf (str, 0, AFIO2STR_FORMAT, afio, afio_lbl[afio->type],
	  params_str) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf(str, len, AFIO2STR_FORMAT, afio, afio_lbl[afio->type],
		   params_str);
	}
    }

  return str;
} /* afio2str () */
