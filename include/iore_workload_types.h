/*
 * iore_workload_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_WORKLOAD_TYPES_H_
#define INCLUDE_IORE_WORKLOAD_TYPES_H_

#include <stddef.h>

#include "iore_prng_types.h"

/*** DEFINES *****************************************************************/

#define WKLD2STR_FORMAT "workload (%p) { num_tasks = %u, type = %s }"

/*** TYPES *******************************************************************/

enum iore_wkld_type
{
  IORE_WKLD_OFFSET, /* offset-based workload */
  IORE_WKLD_DATASET, /* dataset-based workload */
  IORE_WKLD_LENGTH
};

enum iore_wkld_oset_access_pattern
{
  IORE_WKLD_OSET_AP_SEQUENTIAL, /* requests to sequential offsets */
  IORE_WKLD_OSET_AP_RANDOM, /* requests to random offsets */
  IORE_WKLD_OSET_AP_LENGTH
};

enum iore_wkld_dset_type
{
  IORE_WKLD_DSET_CARTESIAN, /* Cartesian dataset */
  IORE_WKLD_DSET_LENGTH
};

enum iore_wkld_dset_var_type
{
  IORE_WKLD_DSET_DV_CHAR,
  IORE_WKLD_DSET_DV_INTEGER,
  IORE_WKLD_DSET_DV_LONG,
  IORE_WKLD_DSET_DV_LONG_LONG,
  IORE_WKLD_DSET_DV_FLOAT,
  IORE_WKLD_DSET_DV_DOUBLE,
  IORE_WKLD_DSET_DV_LENGTH
};

typedef struct iore_wkld_dset_cart
{
  unsigned int num_dims;
  unsigned int *g_dim_sizes;
  unsigned int *g_dim_divs;

  unsigned int *my_dim_sizes;
  unsigned int *my_start_coord;
} iore_wkld_dset_cart_t;

typedef struct iore_wkld_dset
{
  unsigned int num_vars;
  enum iore_wkld_dset_var_type *var_types;
  enum iore_wkld_dset_type type;
  union
  {
    iore_wkld_dset_cart_t cart;
  } u;
  size_t my_size;
  size_t _vars_size;
} iore_wkld_dset_t;

typedef struct iore_wkld_oset
{
  enum iore_wkld_oset_access_pattern ac_pattern;

  size_t *data_sizes;
  unsigned int _data_sizes_len;
  size_t *req_sizes;
  unsigned int _req_sizes_len;

  iore_prng_dist_t *data_size_distrib;
  iore_prng_dist_t *req_size_distrib;

  size_t my_data_size;
  size_t my_req_size;
  size_t _file_size;
} iore_wkld_oset_t;

typedef struct iore_wkld
{
  unsigned int num_tasks;
  enum iore_wkld_type type;
  union
  {
    iore_wkld_oset_t oset;
    iore_wkld_dset_t dset;
  } u;
} iore_wkld_t;

/*** PROTOTYPES **************************************************************/

char *
wkld2str (const iore_wkld_t *);

/*** VARIABLES ***************************************************************/

extern const char * const wkld_type_lbl[];
extern const char * const wkld_oset_access_pattern_lbl[];
extern const char * const wkld_dset_type_lbl[];
extern const char * const wkld_dset_var_type_lbl[];

#endif /* INCLUDE_IORE_WORKLOAD_TYPES_H_ */
