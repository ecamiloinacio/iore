/*
 * test_iore_test_dset.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iore_test.h"
#include "iore_ctx.h"
#include "iore_error.h"
#include "iore_trec.h"
#include "iore_prng.h"
#include "iore_workload.h"

#define MAX_PROCESSES 2

int
default_wkld (iore_wkld_t *);
int
test_01 ();
int
test_02 ();
int
test_03 ();
int
test_04 ();
int
test_05 ();
int
test_06 ();

int
main (int argc, char **argv)
{
  fputs ("Initializing Dataset test...\n", stdout);
  MPI_Init (&argc, &argv);
  ctx_init ();
  trec_init ();
  prng_pool_init ();

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
      const int num_tests = 6;
      int i;
      int rerr;
      for (i = 1; i <= num_tests; i++)
	{
	  trec_exp_start (1);
	  trec_exp_repl_start (1, 1);
	  trec_run_start (1, 1);
	  trec_run_rept_start (1);
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
	    case 5:
	      rerr = test_05 ();
	      break;
	    case 6:
	      rerr = test_06 ();
	      break;
	    }
	  if (rerr)
	    fprintf (stdout, "[Task %d] Test %d: ...FAIL!\n", ctx.task_id, i);
	  else
	    fprintf (stdout, "[Task %d] Test %d: ...SUCCESS!\n", ctx.task_id,
		     i);
	  trec_run_rept_stop ();
	  trec_run_stop ();
	  trec_exp_repl_stop ();
	  trec_exp_stop ();
	}
    }

  MPI_Finalize ();
  fputs ("Finalizing Dataset test.\n", stdout);
} /* main () */

int
default_wkld (iore_wkld_t *wkld)
{
  wkld_free (wkld);

  wkld->type = IORE_WKLD_DATASET;
  int num_vars = 5;
  wkld->u.dset.num_vars = num_vars;
  wkld->u.dset.var_types = malloc (
      num_vars * sizeof(enum iore_wkld_dset_var_type));
  assert(wkld->u.dset.var_types);
  wkld->u.dset._vars_size = 0;
  int i;
  for (i = 0; i < num_vars; i++)
    {
      enum iore_wkld_dset_var_type var_type = (i
	  % (IORE_WKLD_DSET_DV_LENGTH - 1)) + 1;
      wkld->u.dset.var_types[i] = var_type;
      wkld->u.dset._vars_size += dset_var_size (var_type);
    }

  wkld->u.dset.type = IORE_WKLD_DSET_CARTESIAN;
  int num_dims = 3;
  wkld->u.dset.u.cart.num_dims = num_dims;
  size_t size = num_dims * sizeof(unsigned int);
  wkld->u.dset.u.cart.g_dim_sizes = malloc (size);
  assert(wkld->u.dset.u.cart.g_dim_sizes);
  wkld->u.dset.u.cart.g_dim_divs = malloc (size);
  assert(wkld->u.dset.u.cart.g_dim_divs);
  for (i = 0; i < num_dims; i++)
    {
      wkld->u.dset.u.cart.g_dim_sizes[i] = i + 1;
      wkld->u.dset.u.cart.g_dim_divs[i] = (i % 2 == 0) ? 1 : 2;
    }

  return IORE_SUCCESS;
} /* default_wkld () */

int
test_01 ()
{
  int rerr = IORE_SUCCESS;

  iore_test_t test;
  if (!test_init (&test) || default_wkld (&test.wkld))
    rerr = IORE_FAILURE;
  else
    rerr = test_exec (&test, 0, 0, 0);

  test_free (&test);

  return rerr;
} /* test_01 () */

int
test_02 ()
{
  int rerr = IORE_SUCCESS;

  iore_test_t test;
  if (!test_init (&test) || default_wkld (&test.wkld))
    rerr = IORE_FAILURE;
  else
    {
      test.type.read = false;
      rerr = test_exec (&test, 0, 0, 0);
    }

  test_free (&test);

  return rerr;
} /* test_02 () */

int
test_03 ()
{
  int rerr = IORE_SUCCESS;

  iore_test_t test;
  if (!test_init (&test) || default_wkld (&test.wkld))
    rerr = IORE_FAILURE;
  else
    {
      test.read_reorder_offset = 1;
      test.intra_test_barrier = true;
      rerr = test_exec (&test, 0, 0, 0);
    }

  test_free (&test);

  return rerr;
} /* test_03 () */

int
test_04 ()
{
  int rerr = IORE_SUCCESS;

  iore_test_t test;
  if (!test_init (&test) || default_wkld (&test.wkld))
    rerr = IORE_FAILURE;
  else
    {
      test.file_mode = IORE_TEST_FMODE_NXN;
      test.file_name_append_sequence_num = true;
      test.file_name_append_task_id = true;
      test.file_dir_per_task = true;
      test.file_keep = true;
      rerr = test_exec (&test, 0, 0, 0);
    }

  test_free (&test);

  return rerr;
} /* test_04 () */

int
test_05 ()
{
  int rerr = IORE_SUCCESS;

  iore_test_t test;
  if (!test_init (&test) || default_wkld (&test.wkld))
    rerr = IORE_FAILURE;
  else
    {
      const char *file_name = "./othertestfile";
      size_t size = strlen (file_name) + 1;
      free (test.file_name);
      test.file_name = malloc (size);
      assert(test.file_name);
      strncpy(test.file_name, file_name, size);
      test.file_keep = true;
      rerr = test_exec (&test, 0, 0, 0);
    }

  test_free (&test);

  return rerr;
} /* test_05 () */

int
test_06 ()
{
  int rerr = IORE_SUCCESS;

  iore_test_t test;
  if (!test_init (&test) || default_wkld (&test.wkld))
    rerr = IORE_FAILURE;
  else
    fprintf (stdout, "[Task %d] %s\n", ctx.task_id, test2str (&test));

  test_free (&test);

  return rerr;
} /* test_06 () */
