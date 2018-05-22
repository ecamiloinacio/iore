/*
 * iore_workload_types.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iore_workload_types.h"
#include "iore_util.h"

/*** DEFINES *****************************************************************/

#define WKLDOSET2STR_FORMAT "offset (%p) { ac_pattern = '%s', data_sizes (%p) = [ %s ], _data_sizes_len = %u, req_sizes (%p) = [ %s ], _req_sizes_len = %u, data_size_distrib = %s, req_size_distrib = %s, my_data_size = %zu, my_req_size = %zu, _file_size = %zu }"
#define WKLDDSET2STR_FORMAT "dataset (%p) { num_vars = %u, var_types (%p) = [ %s ], type = %s, my_size = %zu, _vars_size = %zu }"
#define WKLDDSETCART2STR_FORMAT "cartesian (%p) { num_dims = %u, g_dim_sizes (%p) = [ %s ], g_dim_divs (%p) = [ %s ], my_dim_sizes (%p) = [ %s ], my_start_coord  (%p) = [ %s ] }"

/*** PROTOTYPES **************************************************************/

static char *
wkldoset2str (const iore_wkld_oset_t *);
static char *
wklddset2str (const iore_wkld_dset_t *);
static char *
wklddsetvartypes2str (const enum iore_wkld_dset_var_type *, int);
static char *
wklddsetcart2str (const iore_wkld_dset_cart_t *);

/*** VARIABLES ***************************************************************/

const char * const wkld_type_lbl[IORE_WKLD_LENGTH] =
  { "offset", "dataset" };
const char * const wkld_oset_access_pattern_lbl[IORE_WKLD_OSET_AP_LENGTH] =
  { "sequential", "random" };
const char * const wkld_dset_type_lbl[IORE_WKLD_DSET_LENGTH] =
  { "cartesian" };
const char * const wkld_dset_var_type_lbl[IORE_WKLD_DSET_DV_LENGTH] =
  { "char", "integer", "long", "long long", "float", "double" };

/*** FUNCTIONS ***************************************************************/

char *
wkld2str (const iore_wkld_t *wkld)
{
  char *str = NULL;

  if (wkld)
    {
      char *type = NULL;
      if (wkld->type == IORE_WKLD_OFFSET)
	type = wkldoset2str (&wkld->u.oset);
      else if (wkld->type == IORE_WKLD_DATASET)
	type = wklddset2str (&wkld->u.dset);

      int len = snprintf (str, 0, WKLD2STR_FORMAT, wkld, wkld->num_tasks, type)
	  + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, WKLD2STR_FORMAT, wkld, wkld->num_tasks, type);
	}
    }

  return str;
} /* wkld2str () */

static char *
wkldoset2str (const iore_wkld_oset_t *oset)
{
  char *str = NULL;

  if (oset)
    {
      unsigned int data_sizes_len = oset->_data_sizes_len;
      char *data_sizes = arrzu2str (oset->data_sizes, data_sizes_len);
      unsigned int req_sizes_len = oset->_req_sizes_len;
      char *req_sizes = arrzu2str (oset->req_sizes, req_sizes_len);
      char *data_size_distrib = prngdist2str (oset->data_size_distrib);
      char *req_size_distrib = prngdist2str (oset->req_size_distrib);

      int len = snprintf (str, 0, WKLDOSET2STR_FORMAT, oset,
			  wkld_oset_access_pattern_lbl[oset->ac_pattern],
			  oset->data_sizes, data_sizes, data_sizes_len,
			  oset->req_sizes, req_sizes, req_sizes_len,
			  data_size_distrib, req_size_distrib,
			  oset->my_data_size, oset->my_req_size,
			  oset->_file_size) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, WKLDOSET2STR_FORMAT, oset,
		    wkld_oset_access_pattern_lbl[oset->ac_pattern],
		    oset->data_sizes, data_sizes, data_sizes_len,
		    oset->req_sizes, req_sizes, req_sizes_len,
		    data_size_distrib, req_size_distrib, oset->my_data_size,
		    oset->my_req_size, oset->_file_size);
	}
    }

  return str;
} /* wkldoset2str () */

static char *
wklddset2str (const iore_wkld_dset_t *dset)
{
  char *str = NULL;

  if (dset)
    {
      unsigned int num_vars = dset->num_vars;
      char *var_types = wklddsetvartypes2str (dset->var_types, num_vars);
      char *type = NULL;
      if (dset->type == IORE_WKLD_DSET_CARTESIAN)
	type = wklddsetcart2str (&dset->u.cart);

      int len = snprintf (str, 0, WKLDDSET2STR_FORMAT, dset, num_vars,
			  dset->var_types, var_types, type, dset->my_size,
			  dset->_vars_size) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, WKLDDSET2STR_FORMAT, dset, num_vars,
		    dset->var_types, var_types, type, dset->my_size,
		    dset->_vars_size);
	}
    }

  return str;
} /* wklddset2str () */

static char *
wklddsetvartypes2str (const enum iore_wkld_dset_var_type *var_types, int len)
{
  char *str = NULL;

  if (var_types)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_len = strlen (wkld_dset_var_type_lbl[var_types[i]]);
	  swp_l[i] = malloc (swp_len + 1);
	  assert(swp_l[i]);
	  strncpy (swp_l[i], wkld_dset_var_type_lbl[var_types[i]], swp_len + 1);
	  str_len += swp_len;
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* wklddsetvartypes2str () */

static char *
wklddsetcart2str (const iore_wkld_dset_cart_t *cart)
{
  char *str = NULL;

  if (cart)
    {
      unsigned int num_dims = cart->num_dims;
      char *g_dim_sizes = arru2str (cart->g_dim_sizes, num_dims);
      char *g_dim_divs = arru2str (cart->g_dim_divs, num_dims);
      char *my_dims_sizes = arru2str (cart->my_dim_sizes, num_dims);
      char *my_start_coord = arru2str (cart->my_start_coord, num_dims);

      int len = snprintf (str, 0, WKLDDSETCART2STR_FORMAT, cart, num_dims,
			  cart->g_dim_sizes, g_dim_sizes, cart->g_dim_divs,
			  g_dim_divs, cart->my_dim_sizes, my_dims_sizes,
			  cart->my_start_coord, my_start_coord) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, WKLDDSETCART2STR_FORMAT, cart, num_dims,
		    cart->g_dim_sizes, g_dim_sizes, cart->g_dim_divs,
		    g_dim_divs, cart->my_dim_sizes, my_dims_sizes,
		    cart->my_start_coord, my_start_coord);
	}
    }

  return str;
} /* wklddsetcart2str () */
