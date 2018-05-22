/*
 * iore_stex.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_STEX_H_
#define INCLUDE_IORE_STEX_H_

#include "iore_stex_types.h"
#include "iore_trec_types.h"

/*** TYPES *******************************************************************/

typedef struct iore_stex_vtable
{
  int
  (*export_task) (const iore_trec_exp_t, const char *);
  int
  (*export_test) (const iore_trec_exp_t, const char *);
} iore_stex_vtable_t;

/*** PROTOTYPES **************************************************************/

iore_stex_t *
stex_init (iore_stex_t *);
void
stex_free (iore_stex_t *);
int
stex_export (const iore_trec_exp_t, const iore_stex_t);

#endif /* INCLUDE_IORE_STEX_H_ */
