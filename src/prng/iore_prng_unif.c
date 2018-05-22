/*
 * iore_prng_unif.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <tinymt64.h>

#include "iore_prng.h"
#include "iore_error.h"
#include "iore_valid.h"

/*** TYPES *******************************************************************/

typedef struct iore_prng_unif
{
  iore_prng_t _super;
  double min;
  double max;
} iore_prng_unif_t;

/*** PROTOTYPES **************************************************************/

iore_prng_t *
unif_new (const iore_prng_dist_t *);
int
unif_dist_valid (const dict_t *);
double
unif_next_double (iore_prng_t *);

/*** VARIABLES ***************************************************************/

const iore_prng_vtable_t prng_unif =
  { unif_new, unif_dist_valid, unif_next_double };

/*** FUNCTIONS ***************************************************************/

iore_prng_t *
unif_new (const iore_prng_dist_t *dist)
{
  assert(dist);

  iore_prng_unif_t *unif = NULL;

  char *min, *max = NULL;
  min = dict_get (&dist->params, PRNG_PARAM_MIN);
  max = dict_get (&dist->params, PRNG_PARAM_MAX);

  if (min && max)
    {
      unif = calloc (1, sizeof(iore_prng_unif_t));
      assert(unif);
      unif->_super.type = IORE_PRNG_UNIF;
      unif->min = strtod (min, NULL);
      unif->max = strtod (max, NULL);

      free (min);
      free (max);
    }

  return (iore_prng_t *) unif;
} /* unif_new () */

int
unif_dist_valid (const dict_t *params)
{
  assert(params);

  int rerr = IORE_SUCCESS;

  if (dict_has_key (params, PRNG_PARAM_MIN))
    {
      char *val, *endptr;

      val = dict_get (params, PRNG_PARAM_MIN);
      strtod (val, &endptr);
      if (strlen (val) == 0 || *endptr != '\0')
	{
	  iore_errorf(VALID_MSG_INVALID_VALUE, PRNG_PARAM_MIN);
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(VALID_MSG_REQUIRED_PARAM, PRNG_PARAM_MIN,
		  prng_type_lbl[IORE_PRNG_UNIF]);
      rerr = IORE_FAILURE;
    }

  if (dict_has_key (params, PRNG_PARAM_MAX))
    {
      char *val, *endptr;

      val = dict_get (params, PRNG_PARAM_MAX);
      strtod (val, &endptr);
      if (strlen (val) == 0 || *endptr != '\0')
	{
	  iore_errorf(VALID_MSG_INVALID_VALUE, PRNG_PARAM_MAX);
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(VALID_MSG_REQUIRED_PARAM, PRNG_PARAM_MAX,
		  prng_type_lbl[IORE_PRNG_UNIF]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* unif_dist_valid () */

double
unif_next_double (iore_prng_t *prng)
{
  assert(prng);

  iore_prng_unif_t *unif = (iore_prng_unif_t *) prng;
  double min = unif->min;
  double max = unif->max;
  double rnd = tinymt64_generate_double (&prng->_gen);

  return (min + ((max - min) * rnd));
} /* unif_next_double () */
