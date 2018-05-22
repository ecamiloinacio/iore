/*
 * test_iore_trec.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

#include "iore_ctx.h"
#include "iore_trec.h"
#include "iore_test.h"

void
record (int, int, int);
void
show ();
void
tostr ();

int
main (int argc, char **argv)
{
  fputs ("Initializing Time Recorder test...\n", stdout);

  MPI_Init (&argc, &argv);
  ctx_init ();
  trec_init ();

  int num_exp_repl = 1;
  int num_run = 1;
  int num_repts = 1;
  fputs ("Recording performance metrics...\n", stdout);
  record (num_exp_repl, num_run, num_repts);
  fputs ("Performance metrics recorded.\n", stdout);

  fputs ("Display recorded metrics...\n", stdout);
  show ();
  fputs ("Metrics displayed.\n", stdout);

  fputs ("Displaying to string...\n", stdout);
  tostr ();
  fputs ("String displayed.\n", stdout);

  trec_destroy ();
  MPI_Finalize ();

  fputs ("Finalizing Time Recorder test.\n", stdout);
} /* main () */

void
record (int num_exp_repl, int num_run, int num_repts)
{
  trec_exp_start (num_exp_repl);
  fprintf (stdout, "Task %d exp_start\n", ctx.task_id);
  sleep (2);

  int i_repl;
  for (i_repl = 1; i_repl <= num_exp_repl; i_repl++)
    {
      trec_exp_repl_start (i_repl, num_run);
      sleep (i_repl + 1);
      fprintf (stdout, "Task %d exp_repl_start %d\n", ctx.task_id, i_repl);

      int i_run;
      for (i_run = 1; i_run <= num_run; i_run++)
	{
	  trec_run_start (i_run, num_repts);
	  sleep (i_run + 1);
	  fprintf (stdout, "Task %d run_start %d\n", ctx.task_id, i_run);

	  int i_rept;
	  for (i_rept = 1; i_rept <= num_repts; i_rept++)
	    {
	      trec_run_rept_start (i_rept);
	      sleep (i_rept + 1);
	      fprintf (stdout, "Task %d run_rept_start %d\n", ctx.task_id,
		       i_rept);

	      int i_test;
	      for (i_test = 0; i_test < IORE_TEST_TYPE_LENGTH; i_test++)
		{
		  trec_test_start (i_test, IORE_TREC_ACTION_CREATE_OPEN);
		  sleep (1);
		  fprintf (stdout, "Task %d test_start %s\n", ctx.task_id,
			   test_type_lbl[i_test]);
		  trec_test_stop (0);

		  trec_test_start (i_test, IORE_TREC_ACTION_WRITE_READ);
		  sleep (i_test + 1);
		  trec_test_stop (10);

		  trec_test_start (i_test, IORE_TREC_ACTION_CLOSE);
		  sleep (1);
		  trec_test_stop (0);

		  if (i_test == IORE_TEST_TYPE_READ)
		    {
		      trec_test_start (i_test, IORE_TREC_ACTION_REMOVE);
		      sleep (1);
		      trec_test_stop (0);
		    }

		  trec_test_commit ();
		  fprintf (stdout, "Task %d commit %s\n", ctx.task_id,
			   test_type_lbl[i_test]);
		}

	      trec_run_rept_stop ();
	      fprintf (stdout, "Task %d run_rept_stop %d\n", ctx.task_id,
		       i_rept);
	    }

	  trec_run_stop ();
	  fprintf (stdout, "Task %d run_stop %d\n", ctx.task_id, i_run);
	}

      trec_exp_repl_stop ();
      fprintf (stdout, "Task %d exp_repl_stop %d\n", ctx.task_id, i_repl);
    }

  trec_exp_stop ();
  fprintf (stdout, "Task %d exp_stop\n", ctx.task_id);
} /* register () */

void
show ()
{
  iore_trec_exp_t exp = get_trec_exp ();

  unsigned int i_repl;
  for (i_repl = 0; i_repl < exp.num_exp_repl; i_repl++)
    {
      iore_trec_exp_repl_t repl = exp.exp_repl[i_repl];

      unsigned int i_run;
      for (i_run = 0; i_run < repl.num_run; i_run++)
	{
	  iore_trec_run_t run = repl.run[i_run];

	  unsigned int i_rept;
	  for (i_rept = 0; i_rept < run.num_run_rept; i_rept++)
	    {
	      iore_trec_run_rept_t rept = run.run_rept[i_rept];

	      int i_test;
	      for (i_test = 0; i_test < IORE_TEST_TYPE_LENGTH; i_test++)
		{
		  iore_trec_test_t test = rept.test[i_test];

		  fprintf (
		      stdout,
		      "Task %d; Test: %s, Commited: %d, Data Size: %lu, "
		      "Create/Open: %f s, Write/Read: %f s, Close: %f s, "
		      "Remove: %f s\n",
		      ctx.task_id,
		      test_type_lbl[i_test],
		      test.committed,
		      test.task_data_size,
		      (test.task_time[IORE_TREC_ACTION_CREATE_OPEN][IORE_TREC_EVENT_STOP]
			  - test.task_time[IORE_TREC_ACTION_CREATE_OPEN][IORE_TREC_EVENT_START]),
		      (test.task_time[IORE_TREC_ACTION_WRITE_READ][IORE_TREC_EVENT_STOP]
			  - test.task_time[IORE_TREC_ACTION_WRITE_READ][IORE_TREC_EVENT_START]),
		      (test.task_time[IORE_TREC_ACTION_CLOSE][IORE_TREC_EVENT_STOP]
			  - test.task_time[IORE_TREC_ACTION_CLOSE][IORE_TREC_EVENT_START]),
		      (test.task_time[IORE_TREC_ACTION_REMOVE][IORE_TREC_EVENT_STOP]
			  - test.task_time[IORE_TREC_ACTION_REMOVE][IORE_TREC_EVENT_START]));

		  if (ctx.task_id == IORE_MASTER_TASK)
		    {
		      fprintf (
			  stdout,
			  "Aggregate; Test: %s, Commited: %d, Data Size: %lu, "
			  "Create/Open: %f s, Write/Read: %f s, Close: %f s, "
			  "Remove: %f s\n",
			  test_type_lbl[i_test],
			  test.committed,
			  test.test_data_size,
			  (test.test_time[IORE_TREC_ACTION_CREATE_OPEN][IORE_TREC_EVENT_STOP]
			      - test.test_time[IORE_TREC_ACTION_CREATE_OPEN][IORE_TREC_EVENT_START]),
			  (test.test_time[IORE_TREC_ACTION_WRITE_READ][IORE_TREC_EVENT_STOP]
			      - test.test_time[IORE_TREC_ACTION_WRITE_READ][IORE_TREC_EVENT_START]),
			  (test.test_time[IORE_TREC_ACTION_CLOSE][IORE_TREC_EVENT_STOP]
			      - test.test_time[IORE_TREC_ACTION_CLOSE][IORE_TREC_EVENT_START]),
			  (test.test_time[IORE_TREC_ACTION_REMOVE][IORE_TREC_EVENT_STOP]
			      - test.test_time[IORE_TREC_ACTION_REMOVE][IORE_TREC_EVENT_START]));
		    }
		}

	      fprintf (
		  stdout,
		  "Task %d; Run Rept.: %d, %f s\n",
		  ctx.task_id,
		  rept.id,
		  (rept.time[IORE_TREC_EVENT_STOP]
		      - rept.time[IORE_TREC_EVENT_START]));
	    }

	  fprintf (
	      stdout,
	      "Task %d; Run: %d, %f s\n",
	      ctx.task_id,
	      run.id,
	      (run.time[IORE_TREC_EVENT_STOP] - run.time[IORE_TREC_EVENT_START]));
	}

      fprintf (
	  stdout, "Task %d; Exp. Repl: %d, %f s\n", ctx.task_id, repl.id,
	  (repl.time[IORE_TREC_EVENT_STOP] - repl.time[IORE_TREC_EVENT_START]));
    }

  fprintf (stdout, "Task %d; Exp: %f s\n", ctx.task_id,
	   (exp.time[IORE_TREC_EVENT_STOP] - exp.time[IORE_TREC_EVENT_START]));
} /* show () */

void
tostr ()
{
  iore_trec_exp_t exp = get_trec_exp ();

  fprintf (stdout, "Task %d: %s\n", ctx.task_id, trecexp2str (&exp));
} /* tostr () */
