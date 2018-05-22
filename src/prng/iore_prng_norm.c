/*
 * iore_prng_norm.c
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

typedef struct iore_prng_norm
{
  iore_prng_t _super;
  double *_next;
  double _next_val;
  double mean;
  double stdev;
} iore_prng_norm_t;

/*** PROTOTYPES **************************************************************/

iore_prng_t *
norm_new (const iore_prng_dist_t *);
int
norm_dist_valid (const dict_t *);
double
norm_next_double (iore_prng_t *);

static double
norm_next_double01 (iore_prng_t *);

/*** VARIABLES ***************************************************************/

const iore_prng_vtable_t prng_norm =
  { norm_new, norm_dist_valid, norm_next_double };

/*** FUNCTIONS ***************************************************************/

iore_prng_t *
norm_new (const iore_prng_dist_t *dist)
{
  assert(dist);

  iore_prng_norm_t *norm = NULL;

  char *mean, *stdev = NULL;
  mean = dict_get (&dist->params, PRNG_PARAM_MEAN);
  stdev = dict_get (&dist->params, PRNG_PARAM_STDEV);

  if (mean && stdev)
    {
      norm = calloc (1, sizeof(iore_prng_norm_t));
      assert(norm);
      norm->_super.type = IORE_PRNG_NORM;
      norm->_next_val = 0.0;
      norm->_next = NULL;
      norm->mean = strtod (mean, NULL);
      norm->stdev = strtod (stdev, NULL);

      free (mean);
      free (stdev);
    }

  return (iore_prng_t *) norm;
} /* norm_new () */

int
norm_dist_valid (const dict_t *params)
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
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(VALID_MSG_REQUIRED_PARAM, PRNG_PARAM_MEAN,
		  prng_type_lbl[IORE_PRNG_NORM]);
      rerr = IORE_FAILURE;
    }

  if (dict_has_key (params, PRNG_PARAM_STDEV))
    {
      char *val, *endptr;

      val = dict_get (params, PRNG_PARAM_STDEV);
      strtod (val, &endptr);
      if (strlen (val) == 0 || *endptr != '\0')
	{
	  iore_errorf(VALID_MSG_INVALID_VALUE, PRNG_PARAM_STDEV);
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(VALID_MSG_REQUIRED_PARAM, PRNG_PARAM_STDEV,
		  prng_type_lbl[IORE_PRNG_NORM]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* norm_dist_valid () */

double
norm_next_double (iore_prng_t *prng)
{
  assert(prng);

  iore_prng_norm_t *norm = (iore_prng_norm_t *) prng;
  double mean = norm->mean;
  double stdev = norm->stdev;
  double d01 = norm_next_double01 (prng);

  return (d01 * stdev + mean);
} /* norm_next_double () */

static double
norm_next_double01 (iore_prng_t *prng)
{
  assert(prng);

  double d = 0.0;

  iore_prng_norm_t *norm = (iore_prng_norm_t *) prng;

  if (norm->_next)
    {
      d = (*norm->_next);
      norm->_next = NULL;
    }
  else
    {
      double v1, v2, rsq, fac;
      do
	{
	  v1 = 2.0 * tinymt64_generate_double (&prng->_gen) - 1;
	  v2 = 2.0 * tinymt64_generate_double (&prng->_gen) - 1;
	  rsq = v1 * v1 + v2 * v2;
	}
      while (rsq >= 1.0 || rsq == 0.0);

      fac = sqrt (-2.0 * log (rsq) / rsq);

      d = (v1 * fac);
      norm->_next_val = (v2 * fac);
      norm->_next = &norm->_next_val;
    }

  return d;
} /* norm_next_double01 () */
