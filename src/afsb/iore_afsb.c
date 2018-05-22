/*
 * iore_afsb.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>

#include "iore_afsb_types.h"
#include "iore_dict.h"
#include "iore_error.h"
#include "iore_valid.h"

/*** FUNCTIONS ***************************************************************/

iore_afsb_t *
afsb_init (iore_afsb_t *this)
{
  assert(this);

  this->type = IORE_AFSB_UNSET;
  dict_init (&this->params);

  return this;
} /* afsb_init () */

void
afsb_free (iore_afsb_t *this)
{
  if (this)
    dict_free (&this->params);
} /* afsb_free () */

int
afsb_valid (iore_afsb_t *this)
{
  assert(this);

  int rerr = IORE_SUCCESS;

  if (this->type == IORE_AFSB_LENGTH)
    {
      iore_errorf(VALID_MSG_INVALID_OPTION, "AFSB implementation");
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* afsb_valid () */
