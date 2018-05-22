/*
 * iore_afio.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_AFIO_H_
#define INCLUDE_IORE_AFIO_H_

#include <sys/types.h>

#include "iore_afio_types.h"
#include "iore_test_types.h"
#include "iore_file.h"

/*** TYPES *******************************************************************/

typedef struct iore_afio_vtable
{
  int
  (*create) (iore_file_t *, const iore_test_t *);
  int
  (*open) (iore_file_t *, const iore_test_t *);
  ssize_t
  (*write_oset) (iore_file_t, const void *, const off_t *, const iore_test_t *);
  ssize_t
  (*read_oset) (iore_file_t, void *, const off_t *, const iore_test_t *);
  ssize_t
  (*write_dset) (iore_file_t, const void *, const iore_test_t *);
  ssize_t
  (*read_dset) (iore_file_t, void *, const iore_test_t *);
  int
  (*close) (iore_file_t *);
  int
  (*remove) (iore_file_t);
} iore_afio_vtable_t;

/*** PROTOTYPES **************************************************************/

iore_afio_t *
afio_init (iore_afio_t *);
void
afio_free (iore_afio_t *);
int
afio_valid (iore_afio_t *);

/*** VARIABLES ***************************************************************/

extern const iore_afio_vtable_t *afio_pool[];

#endif /* INCLUDE_IORE_AFIO_H_ */
