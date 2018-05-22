/*
 * iore_workload_valid.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include "iore_workload_valid.h"
#include "iore_valid.h"
#include "iore_error.h"
#include "iore_ctx.h"
#include "iore_prng.h"

/*** PROTOTYPES **************************************************************/

static int
wkld_oset_valid (iore_wkld_oset_t *);
static int
wkld_dset_valid (iore_wkld_dset_t *, unsigned int);
static int
wkld_dset_cart_valid (iore_wkld_dset_cart_t *, unsigned int);

/*** FUNCTIONS ***************************************************************/

int
wkld_valid (iore_wkld_t *wkld)
{
  int rerr = IORE_SUCCESS;

  if (wkld->num_tasks > (unsigned int) ctx.num_procs)
    {
      iore_warnf("Validation: number of tasks (%d) > number of processes (%d).",
		 wkld->num_tasks, ctx.num_procs);
      iore_warnf("The number of tasks will be set to %d.", ctx.num_procs);
      wkld->num_tasks = ctx.num_procs;
    }

  if (wkld->type >= IORE_WKLD_LENGTH)
    {
      iore_errorf(VALID_MSG_INVALID_OPTION, "workload type");
      rerr = IORE_FAILURE;
    }
  else if (wkld->type == IORE_WKLD_OFFSET)
    rerr |= wkld_oset_valid (&wkld->u.oset);
  else if (wkld->type == IORE_WKLD_DATASET)
    rerr |= wkld_dset_valid (&wkld->u.dset, wkld->num_tasks);

  return rerr;
} /* wkld_valid () */

static int
wkld_oset_valid (iore_wkld_oset_t *oset)
{
  int rerr = IORE_SUCCESS;

  if (oset->ac_pattern == IORE_WKLD_OSET_AP_LENGTH)
    {
      iore_errorf(VALID_MSG_INVALID_OPTION, "access pattern");
      rerr = IORE_FAILURE;
    }

  if ((oset->_data_sizes_len == 0 || oset->data_sizes == NULL)
      && oset->data_size_distrib == NULL)
    {
      iore_error("Validation: Data sizes must be passed either explicitly "
		 "or through a statistical distribution.");
      rerr = IORE_FAILURE;
    }
  else if (oset->data_size_distrib != NULL)
    rerr |= prng_dist_valid (oset->data_size_distrib);

  if ((oset->_req_sizes_len == 0 || oset->req_sizes == NULL)
      && oset->req_size_distrib == NULL)
    {
      iore_error("Validation: Request sizes must be passed either explicitly "
		 "or through a statistical distribution.");
      rerr = IORE_FAILURE;
    }
  else if (oset->req_size_distrib != NULL)
    rerr |= prng_dist_valid (oset->req_size_distrib);

  return rerr;
} /* wkld_oset_valid () */

static int
wkld_dset_valid (iore_wkld_dset_t *dset, unsigned int num_tasks)
{
  int rerr = IORE_SUCCESS;

  if (dset->num_vars == 0 || dset->var_types == NULL)
    {
      iore_errorf(VALID_MSG_ONE_REQUIRED, "variable");
      rerr = IORE_FAILURE;
    }
  else if (dset->var_types != NULL)
    {
      unsigned int i;
      for (i = 0; i < dset->num_vars; i++)
	{
	  if (dset->var_types[i] >= IORE_WKLD_DSET_DV_LENGTH)
	    {
	      iore_errorf(VALID_MSG_INVALID_OPTION, "variable type");
	      rerr = IORE_FAILURE;
	    }
	}
    }

  if (dset->type >= IORE_WKLD_DSET_LENGTH)
    {
      iore_errorf(VALID_MSG_INVALID_OPTION, "dataset type");
      rerr = IORE_FAILURE;
    }
  else
    rerr |= wkld_dset_cart_valid (&dset->u.cart, num_tasks);

  if (dset->_vars_size == 0)
    {
      iore_fatal("Validation: _vars_size not computed.");
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* wkld_dset_valid () */

static int
wkld_dset_cart_valid (iore_wkld_dset_cart_t *cart, unsigned int num_tasks)
{
  int rerr = IORE_SUCCESS;

  if (cart->num_dims
      == 0|| cart->g_dim_sizes == NULL || cart->g_dim_divs == NULL)
    {
      iore_errorf(VALID_MSG_ONE_REQUIRED, "dimension");
      rerr = IORE_FAILURE;
    }
  else if (cart->g_dim_sizes != NULL && cart->g_dim_divs != NULL)
    {
      unsigned int num_divs = 1;
      unsigned int i;
      for (i = 0; i < cart->num_dims; i++)
	{
	  if (cart->g_dim_sizes[i] == 0)
	    {
	      iore_errorf(VALID_MSG_GREATER_ZERO, "dimension size");
	      rerr = IORE_FAILURE;
	    }

	  if (cart->g_dim_divs[i] == 0)
	    {
	      iore_errorf(VALID_MSG_GREATER_ZERO, "dimension division");
	      rerr = IORE_FAILURE;
	    }
	  else if (cart->g_dim_divs[i] > cart->g_dim_sizes[i])
	    {
	      iore_error("Validation: dimension division must be at most "
			 "the dimension size.");
	      rerr = IORE_FAILURE;
	    }
	  else
	    num_divs *= cart->g_dim_divs[i];
	}

      if (num_divs != num_tasks)
	{
	  iore_error("Validation: number of divisions must be equal to "
		     "the number of tasks.");
	  rerr = IORE_FAILURE;
	}
    }

  return rerr;
} /* wkld_dset_cart_valid () */
