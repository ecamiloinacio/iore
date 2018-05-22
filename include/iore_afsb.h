/*
 * iore_afsb.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_AFSB_H_
#define INCLUDE_IORE_AFSB_H_

#include "iore_afsb_types.h"

/*** PROTOTYPES **************************************************************/

iore_afsb_t *
afsb_init (iore_afsb_t *);
void
afsb_free (iore_afsb_t *);
int
afsb_valid (iore_afsb_t *);

#endif /* INCLUDE_IORE_AFSB_H_ */
