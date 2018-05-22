/*
 * test_iore_wkld.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iore_workload.h"
#include "iore_ctx.h"
#include "iore_error.h"
#include "iore_prng.h"

#define MAX_PROCESSES 2

int
test_01 ();
int
test_02 ();
int
test_03 ();
int
test_04 ();

int
main (int argc, char **argv)
{
  fputs ("Initializing Workload test...\n", stdout);
  MPI_Init (&argc, &argv);
  ctx_init ();

  if (ctx.num_procs > MAX_PROCESSES)
    {
      MPI_Comm new_comm;
      MPI_Group grp, new_grp;
      int range[3] =
	{ 0, MAX_PROCESSES - 1, 1 };
      MPI_Comm_group (MPI_COMM_WORLD, &grp);
      MPI_Group_range_incl (grp, 1, &range, &new_grp);
      MPI_Comm_create (MPI_COMM_WORLD, new_grp, &new_comm);
      ctx.comm = new_comm;
    }

  if (ctx.comm != MPI_COMM_NULL)
    {
      const int num_tests = 4;
      int i;
      int rerr;
      for (i = 1; i <= num_tests; i++)
	{
	  switch (i)
	    {
	    case 1:
	      rerr = test_01 ();
	      break;
	    case 2:
	      rerr = test_02 ();
	      break;
	    case 3:
	      rerr = test_03 ();
	      break;
	    case 4:
	      rerr = test_04 ();
	      break;
	    }
	  if (rerr)
	    fprintf (stdout, "[Task %d] Test %d: ...FAIL!\n", ctx.task_id, i);
	  else
	    fprintf (stdout, "[Task %d] Test %d: ...SUCCESS!\n", ctx.task_id,
		     i);
	}
    }

  MPI_Finalize ();
  fputs ("Finalizing Workload test.\n", stdout);
} /* main () */

int
test_01 ()
{
  int rerr = IORE_SUCCESS;

  iore_wkld_t wkld =
    { };
  wkld_init (&wkld);

  fprintf (stdout, "[Task %d] %s\n", ctx.task_id, wkld2str (&wkld));

  return rerr;
} /* test_01 () */

int
test_02 ()
{
  int rerr = IORE_SUCCESS;

  iore_wkld_t wkld =
    { };
  wkld_init (&wkld);
  free (wkld.u.oset.data_sizes);
  wkld.u.oset._data_sizes_len = 3;
  wkld.u.oset.data_sizes = malloc (
      wkld.u.oset._data_sizes_len * sizeof(size_t));
  assert(wkld.u.oset.data_sizes);
  wkld.u.oset.data_sizes[0] = 1024;
  wkld.u.oset.data_sizes[1] = 2048;
  wkld.u.oset.data_sizes[2] = 1048576;
  free (wkld.u.oset.req_sizes);
  wkld.u.oset._req_sizes_len = 4;
  wkld.u.oset.req_sizes = malloc (wkld.u.oset._req_sizes_len * sizeof(size_t));
  assert(wkld.u.oset.req_sizes);
  wkld.u.oset.req_sizes[0] = 256;
  wkld.u.oset.req_sizes[1] = 512;
  wkld.u.oset.req_sizes[2] = 768;
  wkld.u.oset.req_sizes[3] = 262144;
  wkld.u.oset.ac_pattern = IORE_WKLD_OSET_AP_RANDOM;
  wkld.u.oset.my_data_size = 2048;
  wkld.u.oset.my_req_size = 768;

  fprintf (stdout, "[Task %d] %s\n", ctx.task_id, wkld2str (&wkld));

  return rerr;
} /* test_02 () */

int
test_03 ()
{
  int rerr = IORE_SUCCESS;

  iore_wkld_t wkld =
    { };
  wkld_init (&wkld);
  free (wkld.u.oset.data_sizes);
  wkld.u.oset.data_sizes = NULL;
  wkld.u.oset._data_sizes_len = 0;
  free (wkld.u.oset.req_sizes);
  wkld.u.oset.req_sizes = NULL;
  wkld.u.oset._req_sizes_len = 0;
  wkld.u.oset.data_size_distrib = malloc (sizeof(iore_prng_dist_t));
  assert(wkld.u.oset.data_size_distrib);
  prng_dist_init (wkld.u.oset.data_size_distrib);
  wkld.u.oset.data_size_distrib->type = IORE_PRNG_UNIF;
  dict_set (&wkld.u.oset.data_size_distrib->params, PRNG_PARAM_MIN, "1024");
  dict_set (&wkld.u.oset.data_size_distrib->params, PRNG_PARAM_MAX, "4096");
  wkld.u.oset.req_size_distrib = malloc (sizeof(iore_prng_dist_t));
  assert(wkld.u.oset.req_size_distrib);
  prng_dist_init (wkld.u.oset.req_size_distrib);
  wkld.u.oset.req_size_distrib->type = IORE_PRNG_NORM;
  dict_set (&wkld.u.oset.req_size_distrib->params, PRNG_PARAM_MEAN, "1048576");
  dict_set (&wkld.u.oset.req_size_distrib->params, PRNG_PARAM_STDEV, "262144");

  fprintf (stdout, "[Task %d] %s\n", ctx.task_id, wkld2str (&wkld));

  return rerr;
} /* test_03 () */

int
test_04 ()
{
  int rerr = IORE_SUCCESS;

  iore_wkld_t wkld =
    { };
  wkld.num_tasks = ctx.num_procs;
  wkld.type = IORE_WKLD_DATASET;
  wkld.u.dset.num_vars = 4;
  wkld.u.dset.var_types = malloc (
      wkld.u.dset.num_vars * sizeof(enum iore_wkld_dset_var_type));
  assert(wkld.u.dset.var_types);
  wkld.u.dset.var_types[0] = IORE_WKLD_DSET_DV_LONG_LONG;
  wkld.u.dset.var_types[1] = IORE_WKLD_DSET_DV_FLOAT;
  wkld.u.dset.var_types[2] = IORE_WKLD_DSET_DV_CHAR;
  wkld.u.dset.var_types[3] = IORE_WKLD_DSET_DV_INTEGER;
  wkld.u.dset.type = IORE_WKLD_DSET_CARTESIAN;
  wkld.u.dset.u.cart.num_dims = 3;
  wkld.u.dset.u.cart.g_dim_sizes = malloc (
      wkld.u.dset.u.cart.num_dims * sizeof(unsigned int));
  assert(wkld.u.dset.u.cart.g_dim_sizes);
  wkld.u.dset.u.cart.g_dim_sizes[0] = 2;
  wkld.u.dset.u.cart.g_dim_sizes[1] = 3;
  wkld.u.dset.u.cart.g_dim_sizes[2] = 4;
  wkld.u.dset.u.cart.g_dim_divs = malloc (
      wkld.u.dset.u.cart.num_dims * sizeof(unsigned int));
  assert(wkld.u.dset.u.cart.g_dim_divs);
  wkld.u.dset.u.cart.g_dim_divs[0] = 1;
  wkld.u.dset.u.cart.g_dim_divs[1] = 3;
  wkld.u.dset.u.cart.g_dim_divs[2] = 2;

  fprintf (stdout, "[Task %d] %s\n", ctx.task_id, wkld2str (&wkld));

  return rerr;
} /* test_04 () */
