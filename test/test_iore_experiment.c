/*
 * test_iore_experiment.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iore_ctx.h"
#include "iore_trec.h"
#include "iore_prng.h"
#include "iore_error.h"
#include "iore_experiment.h"
#include "iore_run.h"

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
test_05 ();

int
main (int argc, char **argv)
{
  fputs ("Initializing Experiment test...\n", stdout);
  MPI_Init (&argc, &argv);
  ctx_init ();
  trec_init ();
  prng_pool_init ();

  const int num_tests = 5;
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
	}
      fprintf (stdout, "[Task %d] Test %d:...%s!\n", ctx.task_id, i,
	       (rerr ? "FAILED" : "SUCCESS"));
    }

  MPI_Finalize ();
  fputs ("Finalizing Experiment test.\n", stdout);
} /* main () */

int
test_01 ()
{
  int rerr = IORE_SUCCESS;

  iore_exp_t exp;
  if (!exp_init (&exp))
    rerr = IORE_FAILURE;
  else
    {
      rerr = exp_exec (&exp);
    }

  exp_free (&exp);

  return rerr;
} /* test_01 () */

int
test_02 ()
{
  int rerr = IORE_SUCCESS;

  iore_exp_t exp;
  if (!exp_init (&exp))
    rerr = IORE_FAILURE;
  else
    {
      iore_run_t run;
      if (!run_init (&run))
	rerr = IORE_FAILURE;
      else
	{
	  exp.runs = malloc (sizeof(iore_run_t));
	  assert(exp.runs);
	  exp.runs[0] = run;
	  exp._runs_len = 1;
	  exp.num_replications = 5;
	  exp.run_order = IORE_RORDER_RANDOM;

	  rerr = exp_exec (&exp);
	}
    }

  exp_free (&exp);

  return rerr;
} /* test_02 () */

int
test_03 ()
{
  int rerr = IORE_SUCCESS;

  iore_exp_t exp;
  if (!exp_init (&exp))
    rerr = IORE_FAILURE;
  else
    {
      iore_run_t run;
      if (!run_init (&run))
	rerr = IORE_FAILURE;
      else
	{
	  run.test.wkld.num_tasks = MAX_PROCESSES;

	  exp.runs = malloc (sizeof(iore_run_t));
	  assert(exp.runs);
	  exp.runs[0] = run;
	  exp._runs_len = 1;

	  rerr = exp_exec (&exp);
	}
    }

  exp_free (&exp);

  return rerr;
} /* test_03 () */

int
test_04 ()
{
  int rerr = IORE_SUCCESS;

  iore_exp_t exp;
  if (!exp_init (&exp))
    rerr = IORE_FAILURE;
  else
    fprintf (stdout, "[Task %d] %s\n", ctx.task_id, exp2str (&exp));

  exp_free (&exp);

  return rerr;
} /* test_04 () */

int
test_05 ()
{
  int rerr = IORE_SUCCESS;

  iore_exp_t exp;
  if (!exp_init (&exp))
    rerr = IORE_FAILURE;
  else
    {
      iore_exp_repl_t repl;
      repl.id = 1;
      repl._runs_len = exp._runs_len;
      repl.runs = malloc (repl._runs_len * sizeof(iore_run_t *));
      assert(repl.runs);
      unsigned int i;
      for (i = 0; i < repl._runs_len; i++)
	repl.runs[i] = &exp.runs[i];

      fprintf (stdout, "[Task %d] %s\n", ctx.task_id, exprepl2str (&repl));

      free (repl.runs);
    }

  exp_free (&exp);

  return rerr;
} /* test_05 () */
