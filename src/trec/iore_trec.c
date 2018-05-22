/*
 * iore_trec.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdlib.h>
#include <assert.h>

#include "iore_trec.h"
#include "iore_ctx.h"

/*** PROTOTYPES **************************************************************/

static iore_time_t
trec_curtime ();
static void
trec_run_rept_reduce ();

/*** VARIABLES ***************************************************************/

static iore_trec_exp_t trec_exp =
  { };
static iore_trec_st_t state =
  { };
static iore_time_t wclock_delta = 0.0;

/*** FUNCTIONS ***************************************************************/

void
trec_init ()
{
  trec_exp.num_exp_repl = 0;
  trec_exp.exp_repl = NULL;

  state.trec_repl = NULL;
  state.trec_run = NULL;
  state.trec_rept = NULL;
  state.trec_test = NULL;
  state.test_action = IORE_TREC_ACTION_LENGTH;

  iore_time_t now, ref_time;

  MPI_Barrier (MPI_COMM_WORLD);
  now = trec_curtime ();
  if (ctx.task_id == IORE_MASTER_TASK)
    ref_time = now;
  MPI_Bcast (&ref_time, 1, MPI_DOUBLE, IORE_MASTER_TASK, MPI_COMM_WORLD);
  wclock_delta = now - ref_time;
} /* trec_init () */

void
trec_destroy ()
{
  unsigned int i_repl, i_run;
  for (i_repl = 0; i_repl < trec_exp.num_exp_repl; i_repl++)
    {
      iore_trec_exp_repl_t trec_repl = trec_exp.exp_repl[i_repl];
      for (i_run = 0; i_run < trec_repl.num_run; i_run++)
	{
	  iore_trec_run_t trec_run = trec_repl.run[i_run];
	  if (trec_run.num_run_rept > 0)
	    {
	      free (trec_run.run_rept);
	      trec_run.run_rept = NULL;
	      trec_run.num_run_rept = 0;
	    }
	} /* end of run loop */

      if (trec_repl.num_run > 0)
	{
	  free (trec_repl.run);
	  trec_repl.run = NULL;
	  trec_repl.num_run = 0;
	}
    } /* end of exp_repl loop */

  if (trec_exp.num_exp_repl > 0)
    {
      free (trec_exp.exp_repl);
      trec_exp.exp_repl = NULL;
      trec_exp.num_exp_repl = 0;
    }
} /* trec_destroy () */

iore_trec_exp_t
get_trec_exp ()
{
  return trec_exp;
} /* get_trec_exp () */

void
trec_exp_start (unsigned int num_exp_repl)
{
  trec_exp.time[IORE_TREC_EVENT_START] = trec_curtime ();

  if (num_exp_repl > 0)
    {
      trec_exp.num_exp_repl = num_exp_repl;
      trec_exp.exp_repl = calloc (num_exp_repl, sizeof(iore_trec_exp_repl_t));
      assert(trec_exp.exp_repl);
    }
} /* trec_exp_start () */

iore_trec_exp_t *
trec_exp_stop ()
{
  trec_exp.time[IORE_TREC_EVENT_STOP] = trec_curtime ();
  return &trec_exp;
} /* trec_exp_stop () */

void
trec_exp_repl_start (unsigned int id, unsigned int num_run)
{
  if (id <= trec_exp.num_exp_repl)
    {
      iore_trec_exp_repl_t *trec_repl = &trec_exp.exp_repl[id - 1];
      trec_repl->time[IORE_TREC_EVENT_START] = trec_curtime ();

      trec_repl->id = id;
      trec_repl->num_run = num_run;
      trec_repl->run = calloc (num_run, sizeof(iore_trec_run_t));
      assert(trec_repl->run);

      state.trec_repl = trec_repl;
    }
} /* trec_exp_repl_start () */

iore_trec_exp_repl_t *
trec_exp_repl_stop ()
{
  iore_trec_exp_repl_t *trec_exp_repl = NULL;

  if (state.trec_repl)
    {
      state.trec_repl->time[IORE_TREC_EVENT_STOP] = trec_curtime ();
      trec_exp_repl = state.trec_repl;
      state.trec_repl = NULL;
    }

  return trec_exp_repl;
} /* trec_exp_repl_stop () */

void
trec_run_start (unsigned int id, unsigned int num_repts)
{
  if (state.trec_repl && id <= state.trec_repl->num_run)
    {
      iore_trec_run_t *trec_run = &state.trec_repl->run[id - 1];
      trec_run->time[IORE_TREC_EVENT_START] = trec_curtime ();

      trec_run->id = id;
      trec_run->num_run_rept = num_repts;
      trec_run->run_rept = calloc (num_repts, sizeof(iore_trec_run_rept_t));
      assert(trec_run->run_rept);

      state.trec_run = trec_run;
    }
} /* trec_run_start () */

iore_trec_run_t *
trec_run_stop ()
{
  iore_trec_run_t *trec_run = NULL;

  if (state.trec_run)
    {
      state.trec_run->time[IORE_TREC_EVENT_STOP] = trec_curtime ();
      trec_run = state.trec_run;
      state.trec_run = NULL;
    }

  return trec_run;
} /* trec_run_stop () */

void
trec_run_rept_start (unsigned int id)
{
  if (state.trec_run && id <= state.trec_run->num_run_rept)
    {
      iore_trec_run_rept_t *trec_rept = &state.trec_run->run_rept[id - 1];
      trec_rept->time[IORE_TREC_EVENT_START] = trec_curtime ();

      trec_rept->id = id;

      state.trec_rept = trec_rept;
    }
} /* trec_run_rept_start () */

iore_trec_run_rept_t *
trec_run_rept_stop ()
{
  iore_trec_run_rept_t *trec_run_rept = NULL;

  if (state.trec_rept)
    {
      trec_run_rept_reduce ();

      state.trec_rept->time[IORE_TREC_EVENT_STOP] = trec_curtime ();
      trec_run_rept = state.trec_rept;
      state.trec_rept = NULL;
    }

  return trec_run_rept;
} /* trec_run_rept_stop () */

void
trec_test_start (enum iore_test_type type, enum iore_trec_action action)
{
  if (state.trec_rept && type < IORE_TEST_TYPE_LENGTH
      && action < IORE_TREC_ACTION_LENGTH)
    {
      iore_trec_test_t *trec_test = &state.trec_rept->test[type];

      trec_test->type = type;
      state.trec_test = trec_test;
      state.test_action = action;

      trec_test->task_time[action][IORE_TREC_EVENT_START] = trec_curtime ();
    }
} /* trec_test_start () */

void
trec_test_stop (size_t data_size)
{
  if (state.trec_test && state.test_action < IORE_TREC_ACTION_LENGTH)
    {
      state.trec_test->task_time[state.test_action][IORE_TREC_EVENT_STOP] =
	  trec_curtime ();

      if (state.test_action == IORE_TREC_ACTION_WRITE_READ)
	state.trec_test->task_data_size = data_size;
      state.test_action = IORE_TREC_ACTION_LENGTH;
    }
} /* trec_test_stop () */

iore_trec_test_t *
trec_test_commit ()
{
  iore_trec_test_t *trec_test = NULL;

  if (state.trec_test && state.test_action == IORE_TREC_ACTION_LENGTH)
    {
      state.trec_test->committed = true;
      trec_test = state.trec_test;
      state.trec_test = NULL;
    }

  return trec_test;
} /* trec_test_commit () */

static iore_time_t
trec_curtime ()
{
  return (MPI_Wtime () - wclock_delta);
} /* trec_curtime () */

static void
trec_run_rept_reduce ()
{
  if (state.trec_rept)
    {
      int i_type, i_action;
      for (i_type = 0; i_type < IORE_TEST_TYPE_LENGTH; i_type++)
	{
	  iore_trec_test_t *trec_test = &state.trec_rept->test[i_type];
	  for (i_action = 0; i_action < IORE_TREC_ACTION_LENGTH; i_action++)
	    {
	      MPI_Reduce (
		  &trec_test->task_time[i_action][IORE_TREC_EVENT_START],
		  &trec_test->test_time[i_action][IORE_TREC_EVENT_START], 1,
		  MPI_DOUBLE,
		  MPI_MIN, IORE_MASTER_TASK, ctx.comm);
	      MPI_Reduce (&trec_test->task_time[i_action][IORE_TREC_EVENT_STOP],
			  &trec_test->test_time[i_action][IORE_TREC_EVENT_STOP],
			  1,
			  MPI_DOUBLE,
			  MPI_MAX, IORE_MASTER_TASK, ctx.comm);
	      MPI_Reduce (&trec_test->task_data_size,
			  &trec_test->test_data_size, 1, MPI_AINT, MPI_SUM,
			  IORE_MASTER_TASK,
			  ctx.comm);
	      bool committed = false;
	      MPI_Allreduce (&trec_test->committed, &committed, 1, MPI_C_BOOL,
	      MPI_LAND,
			     ctx.comm);
	      trec_test->committed = committed;
	    } /* end of action loop */
	} /* end of type loop */
    }
} /* trec_run_rept_reduce () */
