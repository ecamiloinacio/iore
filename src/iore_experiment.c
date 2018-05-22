/*
 * iore_experiment.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "iore_experiment.h"

#include "iore_stex.h"
#include "iore_trec.h"
#include "iore_error.h"
#include "iore_util.h"
#include "iore_run.h"
#include "iore_ctx.h"
#include "iore_prng.h"

/*** PROTOTYPES **************************************************************/

static int
exp_repl_prep (iore_exp_t *, iore_exp_repl_t *);
static int
exp_repl_exec (iore_exp_repl_t);
static int
exp_repl_post (iore_exp_repl_t *);

static void
exp_show_summary (iore_trec_exp_t *);
static void
exp_repl_show_summary (iore_trec_exp_repl_t *);

/*** FUNCTIONS ***************************************************************/

iore_exp_t *
exp_init (iore_exp_t *this)
{
  assert(this);

  this->num_replications = 1;
  this->runs = malloc (sizeof(iore_run_t));
  assert(this->runs);
  run_init (this->runs);
  this->_runs_len = 1;
  this->run_order = IORE_RORDER_FIXED;

  stex_init (&this->stex);

  return this;
} /* exp_init () */

void
exp_free (iore_exp_t *this)
{
  if (this)
    {
      if (this->runs)
	{
	  unsigned int i;
	  for (i = 0; i < this->_runs_len; i++)
	    run_free (&this->runs[i]);
	  this->runs = NULL;
	}

      stex_free (&this->stex);
    }
} /* exp_free () */

int
exp_exec (iore_exp_t *this)
{
  assert(this);

  int rerr = IORE_SUCCESS;

  trec_exp_start (this->num_replications);

  if (ctx.task_id == IORE_MASTER_TASK)
    fprintf (stdout, "Starting experiment at %s.\n\n", curtimestr ());

  unsigned int i;
  for (i = 1; i <= this->num_replications && !rerr; i++)
    {
      iore_exp_repl_t repl;
      repl.id = i;
      rerr = exp_repl_prep (this, &repl);
      if (!rerr)
	rerr = exp_repl_exec (repl);
      rerr = exp_repl_post (&repl);

      MPI_Barrier (ctx.comm);
    }

  iore_trec_exp_t *trec_exp = trec_exp_stop ();
  exp_show_summary (trec_exp);

  return rerr;
} /* exp_exec () */

static int
exp_repl_prep (iore_exp_t *exp, iore_exp_repl_t *repl)
{
  unsigned int runs_len = exp->_runs_len;
  repl->_runs_len = runs_len;

  repl->runs = malloc (runs_len * sizeof(iore_run_t *));
  assert(repl->runs);
  unsigned int i;
  for (i = 0; i < runs_len; i++)
    repl->runs[i] = &exp->runs[i];
  if (exp->run_order == IORE_RORDER_RANDOM)
    {
      unsigned int seed = 0;
      if (ctx.task_id == IORE_MASTER_TASK)
	seed = prng_gen_seed ();
      MPI_Bcast (&seed, 1, MPI_UNSIGNED, IORE_MASTER_TASK, ctx.comm);

      if (shuffle (repl->runs, runs_len, sizeof(iore_run_t *), seed))
	return IORE_FAILURE;
    }

  return IORE_SUCCESS;
} /* exp_repl_prep () */

static int
exp_repl_exec (iore_exp_repl_t repl)
{
  int rerr = IORE_SUCCESS;

  trec_exp_repl_start (repl.id, repl._runs_len);

  if (ctx.task_id == IORE_MASTER_TASK)
    fprintf (stdout, "> Experiment replication #%d: \n", repl.id);

  unsigned int i;
  for (i = 0; i < repl._runs_len && !rerr; i++)
    rerr = run_exec (repl.runs[i], repl.id);

  iore_trec_exp_repl_t *trec_exp_repl = trec_exp_repl_stop ();
  exp_repl_show_summary (trec_exp_repl);

  return rerr;
} /* exp_repl_exec () */

static int
exp_repl_post (iore_exp_repl_t *repl)
{
  if (repl->runs)
    {
      free (repl->runs);
      repl->runs = NULL;
    }

  return IORE_SUCCESS;
} /* exp_repl_post () */

static void
exp_show_summary (iore_trec_exp_t *trec_exp)
{
  assert(trec_exp);

  if (ctx.verb_lvl > VERB_LVL_NORMAL)
    {
      iore_time_t min_time, max_time;

      MPI_Reduce (&trec_exp->time[IORE_TREC_EVENT_START], &min_time, 1,
      MPI_DOUBLE,
		  MPI_MIN,
		  IORE_MASTER_TASK,
		  ctx.comm);
      MPI_Reduce (&trec_exp->time[IORE_TREC_EVENT_STOP], &max_time, 1,
      MPI_DOUBLE,
		  MPI_MAX,
		  IORE_MASTER_TASK,
		  ctx.comm);

      if (ctx.task_id == IORE_MASTER_TASK)
	fprintf (stdout, "Experiment executed in %.5f seconds.\n",
		 (max_time - min_time));
    }
} /* exp_show_summary () */

static void
exp_repl_show_summary (iore_trec_exp_repl_t *trec_exp_repl)
{
  assert(trec_exp_repl);

  if (ctx.verb_lvl > VERB_LVL_NORMAL)
    {
      iore_time_t min_time, max_time;

      MPI_Reduce (&trec_exp_repl->time[IORE_TREC_EVENT_START], &min_time, 1,
      MPI_DOUBLE,
		  MPI_MIN,
		  IORE_MASTER_TASK,
		  ctx.comm);
      MPI_Reduce (&trec_exp_repl->time[IORE_TREC_EVENT_STOP], &max_time, 1,
      MPI_DOUBLE,
		  MPI_MAX,
		  IORE_MASTER_TASK,
		  ctx.comm);

      if (ctx.task_id == IORE_MASTER_TASK)
	fprintf (stdout, "Replication executed in %.5f seconds.\n\n",
		 (max_time - min_time));
    }
} /* exp_repl_show_summary () */
