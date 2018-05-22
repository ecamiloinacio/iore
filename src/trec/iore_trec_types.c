/*
 * iore_trec.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "iore_trec_types.h"
#include "iore_util.h"

/*** PROTOTYPES **************************************************************/

static char *
arrrepl2str (const iore_trec_exp_repl_t *, int);
static char *
arrrun2str (const iore_trec_run_t *, int);
static char *
arrrept2str (const iore_trec_run_rept_t *, int);
static char *
arrtest2str (const iore_trec_test_t *, int);
static char *
arrtestact2str (const iore_time_t *, int);

/*** VARIABLES ***************************************************************/

const char * const trec_event_lbl[IORE_TREC_EVENT_LENGTH] =
  { "start", "stop" };
const char * const trec_action_lbl[IORE_TREC_ACTION_LENGTH] =
  { "create/open", "write/read", "close", "remove" };

/*** FUNCTIONS ***************************************************************/

char *
trecexp2str (const iore_trec_exp_t *exp)
{
  char *str = NULL;

  if (exp)
    {
      char *time = arrd2str (&exp->time[0], IORE_TREC_EVENT_LENGTH);
      char *exp_repl = arrrepl2str (exp->exp_repl, exp->num_exp_repl);

      int len = snprintf (str, 0, TRECEXP2STR_FORMAT, exp, time,
			  exp->num_exp_repl, exp_repl) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, TRECEXP2STR_FORMAT, exp, time, exp->num_exp_repl,
		    exp_repl);
	}
    }

  return str;
} /* trecexp2str () */

char *
trecexprepl2str (const iore_trec_exp_repl_t *repl)
{
  char *str = NULL;

  if (repl)
    {
      char *time = arrd2str (&repl->time[0], IORE_TREC_EVENT_LENGTH);
      char *run = arrrun2str (repl->run, repl->num_run);

      int len = snprintf (str, 0, TRECEXPREPL2STR_FORMAT, repl, repl->id, time,
			  repl->num_run, run) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, TRECEXPREPL2STR_FORMAT, repl, repl->id, time,
		    repl->num_run, run);
	}
    }

  return str;
} /* trecexprepl2str () */

char *
trecrun2str (const iore_trec_run_t *run)
{
  char *str = NULL;

  if (run)
    {
      char *time = arrd2str (&run->time[0], IORE_TREC_EVENT_LENGTH);
      char *run_rept = arrrept2str (run->run_rept, run->num_run_rept);

      int len = snprintf (str, 0, TRECRUN2STR_FORMAT, run, run->id, time,
			  run->num_run_rept, run_rept) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, TRECRUN2STR_FORMAT, run, run->id, time,
		    run->num_run_rept, run_rept);
	}
    }

  return str;
} /* trecrun2str () */

char *
trecrunrept2str (const iore_trec_run_rept_t *rept)
{
  char *str = NULL;

  if (rept)
    {
      char *time = arrd2str (&rept->time[0], IORE_TREC_EVENT_LENGTH);
      char *test = arrtest2str (&rept->test[0], IORE_TEST_TYPE_LENGTH);

      int len = snprintf (str, 0, TRECRUNREPT2STR_FORMAT, rept, rept->id, time,
			  test) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, TRECRUNREPT2STR_FORMAT, rept, rept->id, time,
		    test);
	}
    }

  return str;
} /* trecrunrept2str () */

char *
trectest2str (const iore_trec_test_t *test)
{
  char *str = NULL;

  if (test)
    {
      char *task_time = arrtestact2str (&test->task_time[0][0],
					IORE_TREC_EVENT_LENGTH);
      char *test_time = arrtestact2str (&test->test_time[0][0],
					IORE_TREC_EVENT_LENGTH);

      int len = snprintf (str, 0, TRECTEST2STR_FORMAT, test,
			  test_type_lbl[test->type], task_time,
			  test->task_data_size, test_time, test->test_data_size,
			  (test->committed ? "true" : "false")) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, TRECTEST2STR_FORMAT, test,
		    test_type_lbl[test->type], task_time, test->task_data_size,
		    test_time, test->test_data_size,
		    (test->committed ? "true" : "false"));
	}
    }

  return str;
} /* trectest2str () */

static char *
arrrepl2str (const iore_trec_exp_repl_t *repl, int len)
{
  char *str = NULL;

  if (repl && len > 0)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_l[i] = trecexprepl2str (&repl[i]);
	  swp_len = strlen (swp_l[i]);
	  str_len += swp_len;
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrrepl2str () */

static char *
arrrun2str (const iore_trec_run_t *run, int len)
{
  char *str = NULL;

  if (run && len > 0)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_l[i] = trecrun2str (&run[i]);
	  swp_len = strlen (swp_l[i]);
	  str_len += swp_len;
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrrun2str () */

static char *
arrrept2str (const iore_trec_run_rept_t *rept, int len)
{
  char *str = NULL;

  if (rept && len > 0)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_l[i] = trecrunrept2str (&rept[i]);
	  swp_len = strlen (swp_l[i]);
	  str_len += swp_len;
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrrept2str () */

static char *
arrtest2str (const iore_trec_test_t *test, int len)
{
  char *str = NULL;

  if (test && len > 0)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_l[i] = trectest2str (&test[i]);
	  swp_len = strlen (swp_l[i]);
	  str_len += swp_len;
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrtest2str () */

static char *
arrtestact2str (const iore_time_t *time, int len)
{
  char *str = NULL;

  if (time && len > 0)
    {
      size_t str_len = 0;

      char **swp_l = malloc (IORE_TREC_ACTION_LENGTH * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < IORE_TREC_ACTION_LENGTH; i++)
	{
	  char *t_str = arrd2str (&time[i * len], len);

	  swp_len = snprintf (swp_l[i], 0, "%s = [ %s ]", trec_action_lbl[i],
			      t_str);
	  if (swp_len > 0)
	    {
	      swp_l[i] = malloc (swp_len + 1);
	      assert(swp_l[i]);
	      snprintf (swp_l[i], swp_len + 1, "%s = [ %s ]",
			trec_action_lbl[i], t_str);
	      str_len += swp_len;
	    }
	}

      str = arrs2str (swp_l, IORE_TREC_ACTION_LENGTH, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrtestact2str () */
