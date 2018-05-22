/*
 * test_iore_stats_exporter.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "iore_stex.h"
#include "iore_ctx.h"
#include "iore_trec.h"
#include "iore_error.h"

void
record (int, int, int);
int
export (const iore_stex_t);
void
tostr (iore_stex_t);

int
main (int argc, char **argv)
{
  fputs ("Initializing Stats Exporter test...\n", stdout);
  MPI_Init (&argc, &argv);
  ctx_init ();
  trec_init ();

  int num_exp_repl = 1;
  int num_run = 1;
  int num_repts = 1;
  fputs ("Recording performance metrics...\n", stdout);
  record (num_exp_repl, num_run, num_repts);
  fputs ("Performance metrics recorded.\n", stdout);

  fputs ("Exporting data...\n", stdout);
  iore_stex_t stex =
    { };
  stex.report_type.task = true;
  stex.report_type.test = true;
  stex.data_format = IORE_STEX_FORMAT_CSV;
  stex.export_dir = "/tmp/";
  if (export (stex))
    fputs ("Failed exporting data.\n", stderr);
  else
    fputs ("Data exported.\n", stdout);

  fputs ("Displaying to string...\n", stdout);
  tostr (stex);
  fputs ("String displayed.\n", stdout);

  trec_destroy ();
  MPI_Finalize ();
  fputs ("Finalizing Stats Exporter test.\n", stdout);
} /* main */

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

int
export (const iore_stex_t stex)
{
  int rerr;

  iore_trec_exp_t exp = get_trec_exp ();
  rerr = stex_export (exp, stex);

  return rerr;
} /* export () */

void
tostr (iore_stex_t stex)
{
  fprintf (stdout, "Task %d: %s\n", ctx.task_id, stex2str (&stex));
} /* tostr () */
