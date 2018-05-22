/*
 * iore_workload.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_WORKLOAD_H_
#define INCLUDE_IORE_WORKLOAD_H_

#include <sys/types.h>

#include "iore_workload_types.h"
#include "iore_test_types.h"

/*** PROTOTYPES **************************************************************/

iore_wkld_t *
wkld_init (iore_wkld_t *);
void
wkld_free (iore_wkld_t *);
void
wkld_oset_free (iore_wkld_oset_t *);
void
wkld_dset_free (iore_wkld_dset_t *);
void
wkld_dset_cart_free (iore_wkld_dset_cart_t *);

off_t *
dset_to_off (const iore_wkld_dset_t *, enum iore_test_file_mode);
size_t
dset_var_size (enum iore_wkld_dset_var_type);

#endif /* INCLUDE_IORE_WORKLOAD_H_ */
