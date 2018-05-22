/*
 * test_iore_prng.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

#include "iore_prng.h"
#include "iore_ctx.h"
#include "iore_error.h"
#include "iore_util.h"

#define MAX_PROCESSES 2
#define SAMPLE_SIZE 20

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
test_07 ();

int
main (int argc, char **argv)
{
  fputs ("Initializing PRNG test...\n", stdout);

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
      const int num_tests = 7;
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
	    case 5:
	      rerr = test_05 ();
	      break;
	    case 6:
	      rerr = test_06 ();
	      break;
	    case 7:
	      rerr = test_07 ();
	      break;
	    }

	  iore_debugf("Test %d: ...%s!", i, (rerr ? "FAIL" : "SUCCESS"));
	  fflush (stderr);
	  MPI_Barrier (ctx.comm);
	}
    }

  MPI_Finalize ();
  fputs ("Finalizing PRNG test.\n", stdout);
} /* main () */

int
test_01 ()
{
  int rerr = IORE_SUCCESS;

  unsigned int *list = calloc (SAMPLE_SIZE, sizeof(unsigned int));
  assert(list);

  iore_prng_dist_t dist;
  dist.type = IORE_PRNG_UNIF;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MIN, "0");
  dict_set (&dist.params, PRNG_PARAM_MAX, "15");

  unsigned int seed = ctx.task_id;
  iore_prng_t *prng = prng_new_seed (&dist, seed);
  dict_free (&dist.params);
  if (prng)
    {
      int i;
      for (i = 0; i < SAMPLE_SIZE; i++)
	list[i] = prng_next_uint (prng);
      free (prng);

      if (ctx.task_id == IORE_MASTER_TASK)
	iore_debug(arru2str (list, SAMPLE_SIZE));
      else
	{
	  sleep (1);
	  iore_debug(arru2str (list, SAMPLE_SIZE));
	}
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  return rerr;
} /* test_01 () */

int
test_02 ()
{
  int rerr = IORE_SUCCESS;

  unsigned int *list = calloc (SAMPLE_SIZE, sizeof(unsigned int));
  assert(list);

  iore_prng_dist_t dist;
  dist.type = IORE_PRNG_UNIF;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MIN, "0");
  dict_set (&dist.params, PRNG_PARAM_MAX, "15");

  iore_prng_t *prng = prng_new_sync (&dist);
  dict_free (&dist.params);
  if (prng)
    {
      int i;
      for (i = 0; i < SAMPLE_SIZE; i++)
	list[i] = prng_next_uint (prng);
      free (prng);

      if (ctx.task_id == IORE_MASTER_TASK)
	iore_debug(arru2str (list, SAMPLE_SIZE));
      else
	{
	  sleep (1);
	  iore_debug(arru2str (list, SAMPLE_SIZE));
	}
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  return rerr;
} /* test_02 () */

int
test_03 ()
{
  int rerr = IORE_SUCCESS;

  unsigned int *list = calloc (SAMPLE_SIZE, sizeof(unsigned int));
  assert(list);

  iore_prng_dist_t dist;
  dist.type = IORE_PRNG_NORM;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MEAN, "30");
  dict_set (&dist.params, PRNG_PARAM_STDEV, "10");

  unsigned int seed = ctx.task_id;
  iore_prng_t *prng = prng_new_seed (&dist, seed);
  dict_free (&dist.params);
  if (prng)
    {
      int i;
      for (i = 0; i < SAMPLE_SIZE; i++)
	list[i] = prng_next_uint (prng);
      free (prng);

      if (ctx.task_id == IORE_MASTER_TASK)
	iore_debug(arru2str (list, SAMPLE_SIZE));
      else
	{
	  sleep (1);
	  iore_debug(arru2str (list, SAMPLE_SIZE));
	}
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  return rerr;
} /* test_03 () */

int
test_04 ()
{
  int rerr = IORE_SUCCESS;

  unsigned int *list = calloc (SAMPLE_SIZE, sizeof(unsigned int));
  assert(list);

  iore_prng_dist_t dist;
  dist.type = IORE_PRNG_NORM;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MEAN, "30");
  dict_set (&dist.params, PRNG_PARAM_STDEV, "10");

  iore_prng_t *prng = prng_new_sync (&dist);
  dict_free (&dist.params);
  if (prng)
    {
      int i;
      for (i = 0; i < SAMPLE_SIZE; i++)
	list[i] = prng_next_uint (prng);
      free (prng);

      if (ctx.task_id == IORE_MASTER_TASK)
	iore_debug(arru2str (list, SAMPLE_SIZE));
      else
	{
	  sleep (1);
	  iore_debug(arru2str (list, SAMPLE_SIZE));
	}
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  return rerr;
} /* test_04 () */

int
test_05 ()
{
  int rerr = IORE_SUCCESS;

  unsigned int *list = calloc (SAMPLE_SIZE, sizeof(unsigned int));
  assert(list);

  iore_prng_dist_t dist;
  dist.type = IORE_PRNG_GEOM;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MEAN, "50");

  unsigned int seed = ctx.task_id;
  iore_prng_t *prng = prng_new_seed (&dist, seed);
  dict_free (&dist.params);
  if (prng)
    {
      int i;
      for (i = 0; i < SAMPLE_SIZE; i++)
	list[i] = prng_next_uint (prng);
      free (prng);

      if (ctx.task_id == IORE_MASTER_TASK)
	iore_debug(arru2str (list, SAMPLE_SIZE));
      else
	{
	  sleep (1);
	  iore_debug(arru2str (list, SAMPLE_SIZE));
	}
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  return rerr;
} /* test_05 () */

int
test_06 ()
{
  int rerr = IORE_SUCCESS;

  unsigned int *list = calloc (SAMPLE_SIZE, sizeof(unsigned int));
  assert(list);

  iore_prng_dist_t dist;
  dist.type = IORE_PRNG_GEOM;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MEAN, "50");

  iore_prng_t *prng = prng_new_sync (&dist);
  dict_free (&dist.params);
  if (prng)
    {
      int i;
      for (i = 0; i < SAMPLE_SIZE; i++)
	list[i] = prng_next_uint (prng);
      free (prng);

      if (ctx.task_id == IORE_MASTER_TASK)
	iore_debug(arru2str (list, SAMPLE_SIZE));
      else
	{
	  sleep (1);
	  iore_debug(arru2str (list, SAMPLE_SIZE));
	}
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  return rerr;
} /* test_06 () */

int
test_07 ()
{
  int rerr = IORE_SUCCESS;

  iore_prng_dist_t dist;

  dist.type = IORE_PRNG_UNIF;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MIN, "0");
  dict_set (&dist.params, PRNG_PARAM_MAX, "15");

  iore_debug(prngdist2str (&dist));

  dist.type = IORE_PRNG_NORM;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MEAN, "30");
  dict_set (&dist.params, PRNG_PARAM_STDEV, "10");

  iore_debug(prngdist2str (&dist));

  dist.type = IORE_PRNG_GEOM;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MEAN, "50");

  iore_debug(prngdist2str (&dist));

  return rerr;
} /* test_07 () */
