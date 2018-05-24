/*
 * iore_workload.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>

#include "iore_workload.h"
#include "iore_test_types.h"
#include "iore_prng.h"
#include "iore_ctx.h"
#include "iore_error.h"
#include "iore_util.h"

/*** DEFINES *****************************************************************/

#define WKLD_DATA_SIZE_DFLT 1048576
#define WKLD_REQ_SIZE_DFLT 262144

/*** FUNCTIONS ***************************************************************/

iore_wkld_t *
wkld_init (iore_wkld_t *this)
{
  assert(this);

  this->num_tasks = ctx.num_procs;
  this->type = IORE_WKLD_OFFSET;
  this->u.oset.ac_pattern = IORE_WKLD_OSET_AP_SEQUENTIAL;
  this->u.oset.data_sizes = malloc (sizeof(size_t));
  assert(this->u.oset.data_sizes);
  this->u.oset.data_sizes[0] = WKLD_DATA_SIZE_DFLT;
  this->u.oset._data_sizes_len = 1;
  this->u.oset.req_sizes = malloc (sizeof(size_t));
  assert(this->u.oset.req_sizes);
  this->u.oset.req_sizes[0] = WKLD_REQ_SIZE_DFLT;
  this->u.oset._req_sizes_len = 1;
  this->u.oset.data_size_distrib = NULL;
  this->u.oset.req_size_distrib = NULL;

  return this;
} /* wkld_init () */

void
wkld_free (iore_wkld_t *this)
{
  if (this)
    {
      if (this->type == IORE_WKLD_OFFSET)
	wkld_oset_free (&this->u.oset);
      else if (this->type == IORE_WKLD_DATASET)
	wkld_dset_free (&this->u.dset);
    }
} /* wkld_free () */

void
wkld_oset_free (iore_wkld_oset_t *this)
{
  if (this)
    {
      if (this->data_sizes)
	{
	  free (this->data_sizes);
	  this->data_sizes = NULL;
	}
      if (this->req_sizes)
	{
	  free (this->req_sizes);
	  this->req_sizes = NULL;
	}
      if (this->data_size_distrib)
	prng_dist_free (this->data_size_distrib);

      if (this->req_size_distrib)
	prng_dist_free (this->req_size_distrib);
    }
} /* wkld_oset_free () */

void
wkld_dset_free (iore_wkld_dset_t *this)
{
  if (this)
    {
      if (this->var_types)
	{
	  free (this->var_types);
	  this->var_types = NULL;
	}

      if (this->type == IORE_WKLD_DSET_CARTESIAN)
	wkld_dset_cart_free (&this->u.cart);
    }
} /* wkld_dset_free () */

void
wkld_dset_cart_free (iore_wkld_dset_cart_t *this)
{
  if (this)
    {
      if (this->g_dim_sizes)
	{
	  free (this->g_dim_sizes);
	  this->g_dim_sizes = NULL;
	}
      if (this->g_dim_divs)
	{
	  free (this->g_dim_divs);
	  this->g_dim_divs = NULL;
	}
      if (this->my_dim_sizes)
	{
	  free (this->my_dim_sizes);
	  this->my_dim_sizes = NULL;
	}
      if (this->my_start_coord)
	{
	  free (this->my_start_coord);
	  this->my_start_coord = NULL;
	}
    }
} /* wkld_dset_cart_free () */

off_t *
dset_to_off (const iore_wkld_dset_t *dset, enum iore_test_file_mode file_mode)
{
  assert(dset);
  assert(file_mode < IORE_TEST_FMODE_LENGTH);

  off_t *offs = NULL;

  if (dset->type == IORE_WKLD_DSET_CARTESIAN)
    {
      const iore_wkld_dset_cart_t *cart = &dset->u.cart;

      unsigned int offs_len = 1;
      unsigned int i;
      for (i = 0; i < cart->num_dims - 1; i++)
	offs_len *= cart->my_dim_sizes[i];
      offs = malloc (offs_len * sizeof(off_t));
      assert(offs);

      if (file_mode == IORE_TEST_FMODE_NX1)
	{
	  unsigned int *coord = malloc (cart->num_dims * sizeof(int));
	  assert(coord);
	  for (i = 0; i < cart->num_dims; i++)
	    coord[i] = cart->my_start_coord[i];

	  /* computes all offsets */
	  size_t prod;
	  unsigned int j, k;
	  for (i = 0; i < offs_len; i++)
	    {
	      offs[i] = 0;
	      for (j = 0; j < cart->num_dims; j++)
		{
		  prod = coord[j];
		  for (k = j + 1; k < cart->num_dims; k++)
		    prod *= cart->g_dim_sizes[k];
		  offs[i] += prod;
		}
	      offs[i] *= dset->_vars_size;

	      /* updates coords for the next offset computation */
	      int l = cart->num_dims - 2;
	      coord[l]++;
	      while (l > 0
		  && (coord[l]
		      == (cart->my_start_coord[l] + cart->my_dim_sizes[l])))
		{
		  coord[l] = cart->my_start_coord[l];
		  l--;
		  coord[l]++;
		}
	    }

	}
      else if (file_mode == IORE_TEST_FMODE_NXN)
	{
	  for (i = 0; i < offs_len; i++)
	    offs[i] = i * cart->my_dim_sizes[cart->num_dims - 1]
		* dset->_vars_size;
	}
    }

  return offs;
} /* dset_to_off () */

size_t
dset_var_size (enum iore_wkld_dset_var_type type)
{
  size_t size = 0;

  switch (type)
    {
    case IORE_WKLD_DSET_DV_CHAR:
      size = sizeof(char);
      break;
    case IORE_WKLD_DSET_DV_INTEGER:
      size = sizeof(int);
      break;
    case IORE_WKLD_DSET_DV_LONG:
      size = sizeof(long);
      break;
    case IORE_WKLD_DSET_DV_LONG_LONG:
      size = sizeof(long long);
      break;
    case IORE_WKLD_DSET_DV_FLOAT:
      size = sizeof(float);
      break;
    case IORE_WKLD_DSET_DV_DOUBLE:
      size = sizeof(double);
      break;
    default:
      size = 0;
    }

  return size;
} /* dset_var_size () */
