/*
 * iore_prng.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_PRNG_H_
#define INCLUDE_IORE_PRNG_H_

#include "iore_prng_types.h"

/*** TYPES *******************************************************************/

typedef struct iore_prng_vtable
{
  iore_prng_t *
  (*new) (const iore_prng_dist_t *);
  int
  (*dist_valid) (const dict_t *);
  double
  (*next_double) (iore_prng_t *);
} iore_prng_vtable_t;

/*** PROTOTYPES **************************************************************/

iore_prng_t *
prng_new (const iore_prng_dist_t *);
iore_prng_t *
prng_new_sync (const iore_prng_dist_t *);
iore_prng_t *
prng_new_seed (const iore_prng_dist_t *, unsigned int seed);
iore_prng_dist_t *
prng_dist_init (iore_prng_dist_t *);
void
prng_dist_free (iore_prng_dist_t *);
int
prng_dist_valid (iore_prng_dist_t *);

double
prng_next_double (iore_prng_t *);
unsigned int
prng_next_uint (iore_prng_t *);
size_t
prng_next_size (iore_prng_t *);

unsigned int
prng_gen_seed ();

#endif /* INCLUDE_IORE_PRNG_H_ */
