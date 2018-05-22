/*
 * iore_prng_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_PRNG_TYPES_H_
#define INCLUDE_IORE_PRNG_TYPES_H_

#include <tinymt64.h>

#include "iore_dict.h"

/*** DEFINES *****************************************************************/

#define PRNG_PARAM_MIN "min"
#define PRNG_PARAM_MAX "max"
#define PRNG_PARAM_MEAN "mean"
#define PRNG_PARAM_STDEV "stdev"

#define PRNGDIST2STR_FORMAT "iore_prng_dist_t (%p) { type = '%s', params = %s }"

/*** TYPES *******************************************************************/

enum iore_prng_type
{
  IORE_PRNG_UNIF, /* uniform distribution */
  IORE_PRNG_NORM, /* normal (Gaussian) distribution */
  IORE_PRNG_GEOM, /* geometric distribution */
  IORE_PRNG_LENGTH
};

typedef struct iore_prng_dist
{
  enum iore_prng_type type;
  dict_t params;
} iore_prng_dist_t;

typedef struct iore_prng
{
  enum iore_prng_type type;
  tinymt64_t _gen;
  unsigned int _seed;
} iore_prng_t;

/*** PROTOTYPES **************************************************************/

char *
prngdist2str (const iore_prng_dist_t *);

/*** VARIABLES ***************************************************************/

extern const char * const prng_type_lbl[];

#endif /* INCLUDE_IORE_PRNG_TYPES_H_ */
