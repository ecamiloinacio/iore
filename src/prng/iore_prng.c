/*
 * iore_prng.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>
#include <sys/time.h>
#include <mpi.h>
#include <stdlib.h>
#include <limits.h>

#include "iore_prng.h"
#include "iore_ctx.h"
#include "iore_error.h"
#include "iore_valid.h"

/*** VARIABLES ***************************************************************/

extern const iore_prng_vtable_t prng_unif;
extern const iore_prng_vtable_t prng_norm;
extern const iore_prng_vtable_t prng_geom;
static const iore_prng_vtable_t *prng_pool[IORE_PRNG_LENGTH] =
  { &prng_unif, &prng_norm, &prng_geom };

/*** FUNCTIONS ***************************************************************/

iore_prng_t *
prng_new (const iore_prng_dist_t *dist)
{
  assert(dist);

  unsigned int seed = prng_gen_seed ();
  iore_prng_t *prng = prng_new_seed (dist, seed);

  return prng;
} /* prng_new () */

iore_prng_t *
prng_new_sync (const iore_prng_dist_t *dist)
{
  assert(dist);

  iore_prng_t *prng = NULL;

  unsigned int seed = 0;
  if (ctx.task_id == IORE_MASTER_TASK)
    seed = prng_gen_seed ();
  MPI_Bcast (&seed, 1, MPI_UNSIGNED, IORE_MASTER_TASK, ctx.comm);

  prng = prng_new_seed (dist, seed);

  return prng;
} /* prng_new_sync () */

iore_prng_t *
prng_new_seed (const iore_prng_dist_t *dist, unsigned int seed)
{
  assert(dist);

  iore_prng_t *prng = NULL;

  if (dist->type < IORE_PRNG_LENGTH)
    {
      prng = prng_pool[dist->type]->new (dist);
      if (prng)
	{
	  tinymt64_init (&prng->_gen, seed);
	  prng->_seed = seed;
	}
    }
  else
    iore_error("Invalid PRNG.");

  return prng;
} /* prng_new_seed () */

iore_prng_dist_t *
prng_dist_init (iore_prng_dist_t *this)
{
  assert(this);

  this->type = IORE_PRNG_UNIF;
  dict_init (&this->params);

  return this;
} /* prng_dist_init () */

void
prng_dist_free (iore_prng_dist_t *this)
{
  if (this)
    dict_free (&this->params);
} /* prng_dist_free () */

int
prng_dist_valid (iore_prng_dist_t *this)
{
  assert(this);

  int rerr = IORE_SUCCESS;

  if (this->type == IORE_PRNG_LENGTH)
    {
      iore_errorf(VALID_MSG_INVALID_OPTION, "PRNG distribution");
      rerr = IORE_FAILURE;
    }
  else
    rerr |= prng_pool[this->type]->dist_valid (&this->params);

  return rerr;
} /* prng_dist_valid () */

double
prng_next_double (iore_prng_t *prng)
{
  assert(prng);
  assert(prng->type < IORE_PRNG_LENGTH);

  return prng_pool[prng->type]->next_double (prng);
} /* prng_next_double () */

unsigned int
prng_next_uint (iore_prng_t *prng)
{
  assert(prng);
  assert(prng->type < IORE_PRNG_LENGTH);

  double d = prng_pool[prng->type]->next_double (prng);
  if (d < 0)
    return 0;
  else if (d > UINT_MAX)
    return UINT_MAX;
  else
    return (unsigned int) d;
} /* prng_next_uint () */

size_t
prng_next_size (iore_prng_t *prng)
{
  assert(prng);
  assert(prng->type < IORE_PRNG_LENGTH);

  double d = prng_pool[prng->type]->next_double (prng);
  if (d < 0)
    return 0;
  else if (d > SIZE_MAX)
    return SIZE_MAX;
  else
    return (size_t) d;
} /* prng_next_size () */

unsigned int
prng_gen_seed ()
{
  unsigned int seed;

  struct timeval tv;
  gettimeofday (&tv, NULL);

  seed = tv.tv_usec;

  return seed;
} /* prng_gen_seed () */
