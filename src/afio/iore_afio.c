/*
 * iore_afio.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>

#include "iore_afio.h"
#include "iore_error.h"
#include "iore_valid.h"

/*** VARIABLES ***************************************************************/

extern const iore_afio_vtable_t afio_posix;
extern const iore_afio_vtable_t afio_cstream;
#ifdef WITH_MPIIO_AFIO
extern const iore_afio_vtable_t afio_mpiio;
#endif
#ifdef WITH_PROTO_AFIO
extern const iore_afio_vtable_t afio_proto;
#endif
const iore_afio_vtable_t *afio_pool[IORE_AFIO_LENGTH] =
  { &afio_posix, &afio_cstream
#ifdef WITH_MPIIO_AFIO
    , &afio_mpiio
#endif
#ifdef WITH_PROTO_AFIO
    , &afio_proto
#endif
};

/*** FUNCTIONS ***************************************************************/

iore_afio_t *
afio_init (iore_afio_t *this)
{
  assert(this);

  this->type = IORE_AFIO_POSIX;
  dict_init (&this->params);

  return this;
} /* afio_init () */

void
afio_free (iore_afio_t *this)
{
  if (this)
    dict_free (&this->params);
} /* afio_free () */

int
afio_valid (iore_afio_t *this)
{
  assert(this);

  int rerr = IORE_SUCCESS;

  if (this->type == IORE_AFIO_LENGTH)
    {
      iore_errorf(VALID_MSG_INVALID_OPTION, "AFIO implementation");
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* afio_valid () */
