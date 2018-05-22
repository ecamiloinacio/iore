/*
 * iore_prng_geom.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <tinymt64.h>

#include "iore_prng.h"
#include "iore_error.h"
#include "iore_valid.h"

/*** TYPES *******************************************************************/

typedef struct iore_prng_geom
{
  iore_prng_t _super;
  double mean;
} iore_prng_geom_t;

/*** PROTOTYPES **************************************************************/

iore_prng_t *
geom_new (const iore_prng_dist_t *);
int
geom_dist_valid (const dict_t *);
double
geom_next_double (iore_prng_t *);

/*** VARIABLES ***************************************************************/

const iore_prng_vtable_t prng_geom =
  { geom_new, geom_dist_valid, geom_next_double };

/*** FUNCTIONS ***************************************************************/

iore_prng_t *
geom_new (const iore_prng_dist_t *dist)
{
  assert(dist);

  iore_prng_geom_t *geom = NULL;

  char *mean = NULL;
  mean = dict_get (&dist->params, PRNG_PARAM_MEAN);

  if (mean)
    {
      geom = calloc (1, sizeof(iore_prng_geom_t));
      assert(geom);
      geom->_super.type = IORE_PRNG_GEOM;
      geom->mean = strtod (mean, NULL);

      free (mean);
    }

  return (iore_prng_t *) geom;
} /* geom_new () */

int
geom_dist_valid (const dict_t *params)
{
  assert(params);

  int rerr = IORE_SUCCESS;

  if (dict_has_key (params, PRNG_PARAM_MEAN))
    {
      char *val, *endptr;

      val = dict_get (params, PRNG_PARAM_MEAN);
      strtod (val, &endptr);
      if (strlen (val) == 0 || *endptr != '\0')
	{
	  iore_errorf(VALID_MSG_INVALID_VALUE, PRNG_PARAM_MEAN);
	  return IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(VALID_MSG_REQUIRED_PARAM, PRNG_PARAM_MEAN,
		  prng_type_lbl[IORE_PRNG_GEOM]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* geom_dist_valid () */

double
geom_next_double (iore_prng_t *prng)
{
  assert(prng);

  iore_prng_geom_t *geom = (iore_prng_geom_t *) prng;
  double mean = geom->mean;
  double rnd = tinymt64_generate_double (&prng->_gen);

  return floor (log (rnd) / log (1 - (1.0 / mean)));
} /* geom_next_double () */
