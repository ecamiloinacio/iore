/*
 * iore_run.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdio.h>
#include <assert.h>

#include "iore_run.h"
#include "iore_test.h"
#include "iore_error.h"
#include "iore_trec.h"
#include "iore_ctx.h"

/*** PROTOTYPES **************************************************************/

static int
run_prep (iore_run_t *);
static int
run_post ();
static int
run_rept_exec (iore_run_t *, unsigned int, unsigned int);

static void
run_show_summary (iore_trec_run_t *);
static void
run_rept_show_summary (iore_trec_run_rept_t *);

/*** FUNCTIONS ***************************************************************/

iore_run_t *
run_init (iore_run_t *this)
{
  assert(this);

  this->id = 1;
  this->num_repetitions = 1;

  test_init (&this->test);

  return this;
} /* run_init () */

void
run_free (iore_run_t *this)
{
  if (this)
    test_free (&this->test);
} /* run_free () */

int
run_exec (iore_run_t *this, unsigned int rpl_id)
{
  assert(this);

  int rerr = IORE_SUCCESS;

  trec_run_start (this->id, this->num_repetitions);

  rerr = run_prep (this);
  if (!rerr && ctx.comm != MPI_COMM_NULL)
    {
      unsigned int i;
      for (i = 1; i <= this->num_repetitions && !rerr; i++)
	rerr = run_rept_exec (this, rpl_id, i);
    }
  rerr = run_post ();

  iore_trec_run_t *trec_run = trec_run_stop ();
  run_show_summary (trec_run);

  MPI_Barrier (ctx.comm);

  return rerr;
} /* run_exec () */

static int
run_prep (iore_run_t *run)
{
  if (run->test.wkld.num_tasks > (unsigned int) ctx.num_procs)
    {
      if (ctx.task_id == IORE_MASTER_TASK)
	{
	  iore_warnf(
	      "Number of tasks '%d' greater than the available "
	      "number of processes '%d'.\nUsing '%d' as number of tasks.\n",
	      run->test.wkld.num_tasks, ctx.num_procs, ctx.num_procs);
	}
      run->test.wkld.num_tasks = ctx.num_procs;
    }

  MPI_Comm new_comm;
  MPI_Group grp, new_grp;
  int range[3] =
    { 0, run->test.wkld.num_tasks - 1, 1 };
  MPI_Comm_group (MPI_COMM_WORLD, &grp);
  MPI_Group_range_incl (grp, 1, &range, &new_grp);
  MPI_Comm_create (MPI_COMM_WORLD, new_grp, &new_comm);
  ctx.comm = new_comm;

  return IORE_SUCCESS;
} /* run_prep () */

static int
run_post ()
{
  if (ctx.comm != MPI_COMM_NULL)
    MPI_Comm_free (&ctx.comm);
  ctx.comm = MPI_COMM_WORLD;

  return IORE_SUCCESS;
} /* run_post () */

static int
run_rept_exec (iore_run_t *run, unsigned int rpl_id, unsigned int rpt_id)
{
  int rerr = IORE_SUCCESS;

  trec_run_rept_start (rpt_id);

  if (ctx.task_id == IORE_MASTER_TASK)
    {
      fprintf (stdout, ">> Run #%d, repetition #%d:\n", run->id, rpt_id);
      fprintf (stdout, "%-5s %10s %10s %10s %10s %10s %15s\n", "test",
	       "cr/op (s)", "wr/rd (s)", "close (s)", "rm (s)", "total (s)",
	       "tput");
    }
  rerr = test_exec (&run->test, rpl_id, run->id, rpt_id);

  iore_trec_run_rept_t *trec_run_rept = trec_run_rept_stop ();
  run_rept_show_summary (trec_run_rept);

  MPI_Barrier (ctx.comm);

  return rerr;
} /* run_rept_exec () */

static void
run_show_summary (iore_trec_run_t *trec_run)
{
  assert(trec_run);

  if (ctx.verb_lvl > VERB_LVL_NORMAL)
    {
      iore_time_t min_time, max_time;

      MPI_Reduce (&trec_run->time[IORE_TREC_EVENT_START], &min_time, 1,
      MPI_DOUBLE,
		  MPI_MIN,
		  IORE_MASTER_TASK,
		  ctx.comm);
      MPI_Reduce (&trec_run->time[IORE_TREC_EVENT_STOP], &max_time, 1,
      MPI_DOUBLE,
		  MPI_MAX,
		  IORE_MASTER_TASK,
		  ctx.comm);

      if (ctx.task_id == IORE_MASTER_TASK)
	fprintf (stdout, "Run repetitions executed in %.5f seconds.\n\n",
		 (max_time - min_time));
    }
} /* run_show_summary () */

static void
run_rept_show_summary (iore_trec_run_rept_t *trec_run_rept)
{
  assert(trec_run_rept);

  if (ctx.verb_lvl > VERB_LVL_NORMAL)
    {
      iore_time_t min_time, max_time;

      MPI_Reduce (&trec_run_rept->time[IORE_TREC_EVENT_START], &min_time, 1,
      MPI_DOUBLE,
		  MPI_MIN,
		  IORE_MASTER_TASK,
		  ctx.comm);
      MPI_Reduce (&trec_run_rept->time[IORE_TREC_EVENT_STOP], &max_time, 1,
      MPI_DOUBLE,
		  MPI_MAX,
		  IORE_MASTER_TASK,
		  ctx.comm);

      if (ctx.task_id == IORE_MASTER_TASK)
	fprintf (stdout, "\nRepetition executed in %.5f seconds.\n\n",
		 (max_time - min_time));
    }
} /* run_rept_show_summary () */
