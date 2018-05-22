/*
 * iore_stex_csv.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <mpi.h>

#include "iore_stex.h"
#include "iore_ctx.h"
#include "iore_error.h"
#include "iore_test_types.h"
#include "iore_trec.h"
#include "iore_util.h"
#include "iore_file.h"

/*** DEFINES *****************************************************************/

/**
 * Task report constants.
 */
#define CSV_STEX_TASK_FILE_NAME_FORMAT "%s/iore_task_%09d_%04d%02d%02d%02d%02d%02d.csv"
#define CSV_STEX_TASK_HEADER "exp_repl_id,run_id,run_rept_id,task_id,test,action,start_time,stop_time,latency,throughput\n"
#define CSV_STEX_TASK_ROW_FORMAT "%d,%d,%d,%d,%s,%s,%.6f,%.6f,%.6f,%.6f\n"

/**
 * Test report constants.
 */
#define CSV_STEX_TEST_FILE_NAME_FORMAT "%s/iore_test_%04d%02d%02d%02d%02d%02d.csv"
#define CSV_STEX_TEST_HEADER "exp_repl_id,run_id,run_rept_id,test,action,start_time,stop_time,latency,throughput\n"
#define CSV_STEX_TEST_ROW_FORMAT "%d,%d,%d,%s,%s,%.6f,%.6f,%.6f,%.6f\n"

/*** PROTOTYPES **************************************************************/

int
csv_export_task (const iore_trec_exp_t, const char *);
int
csv_export_test (const iore_trec_exp_t, const char *);

static int
csv_export (const iore_trec_exp_t, const char *, enum iore_stex_report_type);
static int
csv_export_header (FILE *, enum iore_stex_report_type);
static int
csv_export_rows (FILE *, const iore_trec_exp_t, enum iore_stex_report_type);
static int
csv_export_row_action (FILE *, unsigned int, unsigned int, unsigned int,
		       const iore_trec_test_t, enum iore_trec_action,
		       enum iore_stex_report_type);
static int
csv_get_file_name (enum iore_stex_report_type, const char *, char *);

/*** VARIABLES ***************************************************************/

const iore_stex_vtable_t stex_csv =
  { csv_export_task, csv_export_test };

/*** FUNCTIONS ***************************************************************/

int
csv_export_task (const iore_trec_exp_t trec_exp, const char *dir)
{
  assert(dir);

  int rerr = csv_export (trec_exp, dir, IORE_STEX_REPORT_TASK);
  return rerr;
} /* csv_export_task () */

int
csv_export_test (const iore_trec_exp_t trec_exp, const char *dir)
{
  assert(dir);

  int rerr = IORE_SUCCESS;
  if (ctx.task_id == IORE_MASTER_TASK)
    rerr = csv_export (trec_exp, dir, IORE_STEX_REPORT_TEST);

  MPI_Bcast (&rerr, 1, MPI_INT, IORE_MASTER_TASK, ctx.comm);
  return rerr;
} /* csv_export_test () */

static int
csv_export (const iore_trec_exp_t trec_exp, const char *dir,
	    enum iore_stex_report_type type)
{
  if (!file_is_dir (dir))
    {
      iore_errorf_all("'%s' is not a directory.", dir);
      return IORE_FAILURE;
    }

  int amode = F_OK | W_OK;
  if (access (dir, amode))
    {
      iore_errorf_all("Insufficient permissions on directory '%s'.", dir);
      return IORE_FAILURE;
    }

  char fname[FILENAME_MAX];
  if (csv_get_file_name (type, dir, fname))
    return IORE_FAILURE;

  FILE *file = fopen (fname, "a");
  assert(file);

  int rerr = csv_export_header (file, type);
  if (rerr)
    return IORE_FAILURE;

  rerr = csv_export_rows (file, trec_exp, type);

  if (rerr)
    return IORE_FAILURE;

  return IORE_SUCCESS;
} /* csv_export () */

static int
csv_export_header (FILE *file, enum iore_stex_report_type type)
{
  if (type == IORE_STEX_REPORT_TASK)
    {
      fputs (CSV_STEX_TASK_HEADER, file);
      fflush (file);
    }
  else if (type == IORE_STEX_REPORT_TEST && ctx.task_id == IORE_MASTER_TASK)
    {
      fputs (CSV_STEX_TEST_HEADER, file);
      fflush (file);
    }
  else
    {
      iore_error("Unsupported report type.");
      return IORE_FAILURE;
    }

  return IORE_SUCCESS;
} /* csv_export_header () */

static int
csv_export_rows (FILE *file, const iore_trec_exp_t trec_exp,
		 enum iore_stex_report_type type)
{
  int rerr = IORE_SUCCESS;

  unsigned int i_repl, i_run, i_rept, i_test, i_act;
  for (i_repl = 0; i_repl < trec_exp.num_exp_repl; i_repl++)
    {
      const iore_trec_exp_repl_t trec_repl = trec_exp.exp_repl[i_repl];
      for (i_run = 0; i_run < trec_repl.num_run; i_run++)
	{
	  const iore_trec_run_t trec_run = trec_repl.run[i_run];
	  for (i_rept = 0; i_rept < trec_run.num_run_rept; i_rept++)
	    {
	      const iore_trec_run_rept_t trec_rept = trec_run.run_rept[i_rept];
	      for (i_test = 0; i_test < IORE_TEST_TYPE_LENGTH; i_test++)
		{
		  const iore_trec_test_t trec_test = trec_rept.test[i_test];
		  if (trec_test.committed)
		    {
		      for (i_act = 0; i_act < IORE_TREC_ACTION_LENGTH; i_act++)
			{
			  rerr = csv_export_row_action (file, trec_repl.id,
							trec_run.id,
							trec_rept.id, trec_test,
							i_act, type);
			  if (rerr)
			    return IORE_FAILURE;
			} /* end of action loop */
		    }
		} /* end of test loop */
	    } /* end of run_rept loop */
	} /* end of run loop */
    } /* end of exp_repl loop */

  return IORE_SUCCESS;
} /* csv_export_rows () */

static int
csv_export_row_action (FILE *file, unsigned int exp_repl_id,
		       unsigned int run_id, unsigned int run_rept_id,
		       iore_trec_test_t trec_test, enum iore_trec_action action,
		       enum iore_stex_report_type type)
{
  iore_time_t start_time, stop_time, latency;
  double tput;

  if (type == IORE_STEX_REPORT_TASK)
    {
      start_time = trec_test.task_time[action][IORE_TREC_EVENT_START];
      stop_time = trec_test.task_time[action][IORE_TREC_EVENT_STOP];
      latency = stop_time - start_time;
      if (action == IORE_TREC_ACTION_WRITE_READ)
	{
	  tput = trec_test.task_data_size / latency;
	}
      else
	{
	  tput = 0;
	}

      fprintf (file, CSV_STEX_TASK_ROW_FORMAT, exp_repl_id, run_id, run_rept_id,
	       ctx.task_id, test_type_lbl[trec_test.type],
	       trec_action_lbl[action], start_time, stop_time, latency, tput);
    }
  else if (type == IORE_STEX_REPORT_TEST)
    {
      start_time = trec_test.test_time[action][IORE_TREC_EVENT_START];
      stop_time = trec_test.test_time[action][IORE_TREC_EVENT_STOP];
      latency = stop_time - start_time;
      if (action == IORE_TREC_ACTION_WRITE_READ)
	{
	  tput = trec_test.task_data_size / latency;
	}
      else
	{
	  tput = 0;
	}

      fprintf (file, CSV_STEX_TEST_ROW_FORMAT, exp_repl_id, run_id, run_rept_id,
	       test_type_lbl[trec_test.type], trec_action_lbl[action],
	       start_time, stop_time, latency, tput);
    }
  else
    {
      iore_error("Unsupported report type.");
      return IORE_FAILURE;
    }

  return IORE_SUCCESS;
} /* csv_export_row_action () */

/**
 * Returns a NULL pointer in case of failure.
 */
static int
csv_get_file_name (enum iore_stex_report_type type, const char *dir,
		   char *fname)
{
  time_t t = time (NULL);
  struct tm *tm = localtime (&t);

  if (type == IORE_STEX_REPORT_TASK)
    snprintf (fname, FILENAME_MAX, CSV_STEX_TASK_FILE_NAME_FORMAT, dir,
	      ctx.task_id, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	      tm->tm_hour, tm->tm_min, tm->tm_sec);
  else if (type == IORE_STEX_REPORT_TEST)
    snprintf (fname, FILENAME_MAX, CSV_STEX_TEST_FILE_NAME_FORMAT, dir,
	      tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
	      tm->tm_min, tm->tm_sec);
  else
    {
      iore_error("Unsupported report type.");
      return IORE_FAILURE;
    }

  return IORE_SUCCESS;
} /* csv_get_file_name () */
