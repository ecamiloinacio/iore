/*
 * test_iore_run.c
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
main (int argc, char **argv)
{
  fputs ("Initializing Run test...\n", stdout);
  MPI_Init (&argc, &argv);
  ctx_init ();
  trec_init ();
  prng_pool_init ();

  const int num_tests = 4;
  int i;
  int rerr;
  for (i = 1; i <= num_tests; i++)
    {
      trec_exp_start (1);
      trec_exp_repl_start (1, 1);
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
      fprintf (stdout, "[Task %d] Test %d:...%s!\n", ctx.task_id, i,
	       (rerr ? "FAILED" : "SUCCESS"));
      trec_exp_repl_stop ();
      trec_exp_stop ();
    }

  MPI_Finalize ();
  fputs ("Finalizing Run test.\n", stdout);
} /* main () */

int
test_01 ()
{
  int rerr = IORE_SUCCESS;

  iore_run_t run;
  if (!run_init (&run))
    rerr = IORE_FAILURE;
  else
    rerr = run_exec (&run, 0);

  run_free (&run);

  return rerr;
} /* test_01 () */

int
test_02 ()
{
  int rerr = IORE_SUCCESS;

  iore_run_t run;
  if (!run_init (&run))
    rerr = IORE_FAILURE;
  else
    {
      run.num_repetitions = 10;
      rerr = run_exec (&run, 0);
    }

  run_free (&run);

  return rerr;
} /* test_02 () */

int
test_03 ()
{
  int rerr = IORE_SUCCESS;

  iore_run_t run;
  if (!run_init (&run))
    rerr = IORE_FAILURE;
  else
    {
      run.test.wkld.num_tasks = MAX_PROCESSES;
      rerr = run_exec (&run, 0);
    }

  run_free (&run);

  return rerr;
} /* test_03 () */

int
test_04 ()
{
  int rerr = IORE_SUCCESS;

  iore_run_t run;
  if (!run_init (&run))
    rerr = IORE_FAILURE;
  else
    fprintf (stdout, "[Task %d] %s\n", ctx.task_id, run2str (&run));

  run_free (&run);

  return rerr;
} /* test_04 () */
