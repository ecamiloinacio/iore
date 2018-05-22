/*
 * iore_parser_file_json.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "iore_parser_file_json_attr.h"
#include "iore_parser_file.h"
#include "iore_error.h"
#include "iore_run.h"
#include "iore_workload.h"
#include "iore_util.h"
#include "iore_prng.h"

/*** DEFINES *****************************************************************/

#define JSON_BASE_STR_SIZE 10

/**
 * Common messages.
 */
#define JSON_MSG_INVALID_OPTION "JSON Parser: '%s': Invalid option."
#define JSON_MSG_INVALID_SIZE "JSON Parser: '%s': Invalid size."
#define JSON_MSG_INVALID_ENUM "JSON Parser: '%s': Invalid value."
#define JSON_MSG_INVALID_INT "JSON Parser: '%s': Invalid integer."
#define JSON_MSG_OBJECT_OPTION "JSON Parser: '%s': Must be an object."
#define JSON_MSG_BOOL_OPTION "JSON Parser: '%s': Must be either true or false."
#define JSON_MSG_STRING_OPTION "JSON Parser: '%s': Must be a string."
#define JSON_MSG_INT_OPTION "JSON Parser: '%s': Must be an integer."
#define JSON_MSG_INT_OUT_OF_RANGE "JSON Parser: '%s': Integer out of range."
#define JSON_MSG_INT_POSITIVE "JSON Parser: '%s': Must be a positive integer."
#define JSON_MSG_ARRAY_OPTION "JSON Parser: '%s': Must be an array."
#define JSON_MSG_ARRAY_SIZE "JSON Parser: '%s': Must match the size of '%s'."

/*** PROTOTYPES **************************************************************/

int
jsonp_parse_file (const char *, iore_exp_t *);

static int
json_check_uint (json_value *, const char *, unsigned int *);
static int
json_check_str (json_value *, const char *, char **);
static int
json_check_enum (json_value *, const char *, const char * const *, int, int *);
static int
json_check_bool (json_value *, const char *, bool *);
static int
json_check_size (json_value *, const char *, size_t *);

static size_t
json_read_file (const char *, json_char **);
static int
json_exp (json_value *, iore_exp_t *);
static int
json_exp_num_repls (json_value *, iore_exp_t *);
static int
json_exp_run_order (json_value *, iore_exp_t *);
static int
json_exp_runs (json_value *, iore_exp_t *);
static int
json_stex (json_value *, iore_stex_t *);
static int
json_stex_report_type (json_value *, iore_stex_t *);
static int
json_stex_data_format (json_value *, iore_stex_t *);
static int
json_stex_export_dir (json_value *, iore_stex_t *);
static int
json_run (json_value *, iore_run_t *);
static int
json_run_num_repts (json_value *, iore_run_t *);
static int
json_test (json_value *, iore_test_t *);
static int
json_test_type (json_value *, iore_test_t *);
static int
json_test_write_flush (json_value *, iore_test_t *);
static int
json_test_write_flush_per_req (json_value *, iore_test_t *);
static int
json_test_read_reorder_offset (json_value *, iore_test_t *);
static int
json_test_intra_test_barrier (json_value *, iore_test_t *);
static int
json_test_inter_test_delay_secs (json_value *, iore_test_t *);
static int
json_test_file_mode (json_value *, iore_test_t *);
static int
json_test_file_name (json_value *, iore_test_t *);
static int
json_test_file_name_append_seq_num (json_value *, iore_test_t *);
static int
json_test_file_name_append_task_id (json_value *, iore_test_t *);
static int
json_test_file_dir_per_task (json_value *, iore_test_t *);
static int
json_test_file_keep (json_value *, iore_test_t *);
static int
json_wkld (json_value *, iore_wkld_t *);
static int
json_wkld_num_tasks (json_value *, iore_wkld_t *);
static int
json_wkld_oset (json_value *, iore_wkld_oset_t *);
static int
json_wkld_oset_ac_pattern (json_value *, iore_wkld_oset_t *);
static int
json_wkld_oset_data_sizes (json_value *, iore_wkld_oset_t *);
static int
json_wkld_oset_req_sizes (json_value *, iore_wkld_oset_t *);
static int
json_wkld_oset_data_size_distrib (json_value *, iore_wkld_oset_t *);
static int
json_wkld_oset_req_size_distrib (json_value *, iore_wkld_oset_t *);
static int
json_wkld_oset_distrib_params (json_value *, iore_prng_dist_t *);
static int
json_wkld_dset (json_value *, iore_wkld_dset_t *);
static int
json_wkld_dset_num_vars (json_value *, iore_wkld_dset_t *);
static int
json_wkld_dset_var_types (json_value *, iore_wkld_dset_t *);
static int
json_wkld_dset_cart (json_value *, iore_wkld_dset_cart_t *);
static int
json_wkld_dset_cart_num_dims (json_value *, iore_wkld_dset_cart_t *);
static int
json_wkld_dset_cart_dim_sizes (json_value *, iore_wkld_dset_cart_t *);
static int
json_wkld_dset_cart_dim_divs (json_value *, iore_wkld_dset_cart_t *);
static int
json_afio (json_value *, iore_afio_t *);
static int
json_afio_params (json_value *, iore_afio_t *);
static int
json_afsb (json_value *, iore_afsb_t **);
static int
json_afsb_params (json_value *, iore_afsb_t *);

/*** VARIABLES ***************************************************************/

const iore_prsr_vtable_t prsr_json =
  { "json", jsonp_parse_file };

/*** FUNCTIONS ***************************************************************/

/**
 * Assumes @path refers to a readable regular file.
 */
int
jsonp_parse_file (const char *path, iore_exp_t *exp)
{
  assert(path);
  assert(exp);

  int rerr = IORE_SUCCESS;

  json_char *json = NULL;
  size_t fsize = json_read_file (path, &json);
  if (fsize > 0)
    {
      json_value *root = json_parse (json, fsize);
      if (root)
	{
	  rerr = json_exp (root, exp);
	  json_value_free (root);
	}
      else
	{
	  iore_error("JSON Parser: Invalid experiment definition.");
	  rerr = IORE_FAILURE;
	}

      free (json);
    }
  else
    /* fsize == 0 */
    rerr = IORE_FAILURE;

  return rerr;
} /* jsonp_parse_file () */

static int
json_check_uint (json_value *jval, const char *jname, unsigned int *val)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_string)
    {
      char *endptr;
      long v = strtol (jval->u.string.ptr, &endptr, 10);
      if (endptr == jval->u.string.ptr || *endptr != '\0'
	  || (errno == ERANGE && (v == LONG_MAX || v == LONG_MIN))
	  || (errno != 0 && v == 0))
	{
	  iore_errorf(JSON_MSG_INVALID_INT, jname);
	  rerr = IORE_FAILURE;
	}
      else if (v > UINT_MAX)
	{
	  iore_errorf(JSON_MSG_INT_OUT_OF_RANGE, jname);
	  rerr = IORE_FAILURE;
	}
      else if (v < 0)
	{
	  iore_errorf(JSON_MSG_INT_POSITIVE, jname);
	  rerr = IORE_FAILURE;
	}
      else
	*val = (unsigned int) v;
    }
  else if (jval->type == json_integer)
    {
      if (jval->u.integer >= 0)
	*val = (unsigned int) jval->u.integer;
      else
	{
	  iore_errorf(JSON_MSG_INT_POSITIVE, jname);
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(JSON_MSG_INT_OPTION, jname);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_check_uint () */

static int
json_check_str (json_value *jval, const char *jname, char **val)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_string)
    *val = strdup (jval->u.string.ptr);
  else
    {
      iore_errorf(JSON_MSG_STRING_OPTION, jname);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_check_str () */

static int
json_check_enum (json_value *jval, const char *jname, const char * const *list,
		 int lsize, int *val)
{
  int rerr = IORE_SUCCESS;

  char *str;
  rerr = json_check_str (jval, jname, &str);
  if (!rerr)
    {
      int i;
      for (i = 0; i < lsize; i++)
	{
	  if (strcasecmp (str, list[i]) == 0)
	    {
	      *val = i;
	      break;
	    }
	}
      if (i == lsize)
	{
	  iore_errorf(JSON_MSG_INVALID_ENUM, jname);
	  rerr = IORE_FAILURE;
	}
    }

  return rerr;
} /* json_check_enum () */

static int
json_check_bool (json_value *jval, const char *jname, bool *val)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_boolean)
    *val = jval->u.boolean;
  else
    {
      iore_errorf(JSON_MSG_BOOL_OPTION, jname);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_check_bool () */

static int
json_check_size (json_value *jval, const char *jname, size_t *val)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_string)
    {
      char *endptr;
      unsigned long long v = strtoll (jval->u.string.ptr, &endptr, 10);
      if (endptr == jval->u.string.ptr || (errno == ERANGE && v == ULLONG_MAX)
	  || (errno != 0 && v == 0))
	{
	  iore_errorf(JSON_MSG_INVALID_SIZE, jname);
	  rerr = IORE_FAILURE;
	}
      else if (*endptr == '\0')
	*val = v;
      else if ((*endptr == 'k' || *endptr == 'K') && *(endptr + 1) == '\0')
	*val = v * KIBIBYTE;
      else if ((*endptr == 'm' || *endptr == 'M') && *(endptr + 1) == '\0')
	*val = v * MEBIBYTE;
      else if ((*endptr == 'g' || *endptr == 'G') && *(endptr + 1) == '\0')
	*val = v * GIBIBYTE;
      else if ((*endptr == 't' || *endptr == 'T') && *(endptr + 1) == '\0')
	*val = v * TEBIBYTE;
      else
	{
	  iore_errorf(JSON_MSG_INVALID_SIZE, jname);
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf("JSON Parser: '%s': Must be a string with an integer "
		  "number of bytes, optionally followed by a multiplier.",
		  jname);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_check_size () */

static size_t
json_read_file (const char *path, json_char **json)
{
  FILE *fp = fopen (path, "r");
  if (!fp)
    iore_fatalf("JSON Parser: Failed opening file '%s'.", path);

  fseek (fp, 0, SEEK_END);
  long int fsize = ftell (fp);
  if (fsize == -1)
    {
      fclose (fp);
      iore_fatalf("JSON Parser: Failed get file '%s' size.", path);
    }
  else if (fsize == 0)
    {
      fclose (fp);
      iore_errorf("JSON Parser: Empty file '%s'.", path);
      return 0;
    }
  else
    rewind (fp);

  *json = malloc (fsize);
  assert(*json);
  if (fread (*json, fsize, 1, fp) != 1)
    {
      fclose (fp);
      free (*json);
      iore_fatalf("JSON Parser: Failed reading file '%s'.", path);
    }

  fclose (fp);

  return fsize;
} /* json_read_file () */

static int
json_exp (json_value *jexp, iore_exp_t *exp)
{
  int rerr = IORE_SUCCESS;

  if (jexp->type == json_object)
    {
      unsigned int len = jexp->u.object.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  char *jname = jexp->u.object.values[i].name;
	  json_value *jval = jexp->u.object.values[i].value;

	  if (strcasecmp (jname, JSON_ATTR_NUM_REPLICATIONS) == 0)
	    rerr |= json_exp_num_repls (jval, exp);
	  else if (strcasecmp (jname, JSON_ATTR_RUN_ORDER) == 0)
	    rerr |= json_exp_run_order (jval, exp);
	  else if (strcasecmp (jname, JSON_ATTR_RUNS) == 0)
	    rerr |= json_exp_runs (jval, exp);
	  else if (strcasecmp (jname, JSON_ATTR_STEX) == 0)
	    rerr |= json_stex (jval, &exp->stex);
	  else
	    {
	      iore_errorf(JSON_MSG_INVALID_OPTION, jname);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_error("JSON Parser: Root element must be an object.");
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_exp () */

static int
json_exp_num_repls (json_value *jval, iore_exp_t *exp)
{
  int rerr = json_check_uint (jval, JSON_ATTR_NUM_REPLICATIONS,
			      &exp->num_replications);
  return rerr;
} /* json_exp_num_repls () */

static int
json_exp_run_order (json_value *jval, iore_exp_t *exp)
{
  int rerr = json_check_enum (jval, JSON_ATTR_RUN_ORDER, exp_run_order_lbl,
			      IORE_RORDER_LENGTH, (int *) &exp->run_order);
  return rerr;
} /* json_exp_run_order () */

static int
json_exp_runs (json_value *jval, iore_exp_t *exp)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_array)
    {
      unsigned int len = jval->u.array.length;
      if (len > 0)
	{
	  if (exp->runs)
	    {
	      unsigned int i;
	      for (i = 0; i < exp->_runs_len; i++)
		run_free (&exp->runs[i]);
	    }
	  exp->runs = calloc (len, sizeof(iore_run_t));
	  assert(exp->runs);
	  exp->_runs_len = len;

	  unsigned int i;
	  for (i = 0; i < len; i++)
	    {
	      run_init (&exp->runs[i]);
	      exp->runs[i].id = i + 1;
	      rerr |= json_run (jval->u.array.values[i], &exp->runs[i]);
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_ARRAY_OPTION, JSON_ATTR_RUNS);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_exp_runs () */

static int
json_stex (json_value *jstex, iore_stex_t *stex)
{
  int rerr = IORE_SUCCESS;

  if (jstex->type == json_object)
    {
      unsigned int len = jstex->u.object.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  char *jname = jstex->u.object.values[i].name;
	  json_value *jval = jstex->u.object.values[i].value;

	  if (strcasecmp (jname, JSON_ATTR_STEX_REPORT_TYPE) == 0)
	    rerr |= json_stex_report_type (jval, stex);
	  else if (strcasecmp (jname, JSON_ATTR_STEX_DATA_FORMAT) == 0)
	    rerr |= json_stex_data_format (jval, stex);
	  else if (strcasecmp (jname, JSON_ATTR_STEX_EXPORT_DIR) == 0)
	    rerr |= json_stex_export_dir (jval, stex);
	  else
	    {
	      iore_errorf(JSON_MSG_INVALID_OPTION, jname);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, JSON_ATTR_STEX);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_stex () */

static int
json_stex_report_type (json_value *jval, iore_stex_t *stex)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_array)
    {
      unsigned int len = jval->u.array.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  int val;
	  json_value *jval2 = jval->u.array.values[i];
	  int rerr2 = json_check_enum (jval2, JSON_ATTR_STEX_REPORT_TYPE,
				       stex_report_lbl, IORE_STEX_REPORT_LENGTH,
				       &val);
	  if (!rerr2)
	    {
	      if (val == IORE_STEX_REPORT_TASK)
		stex->report_type.task = true;
	      else if (val == IORE_STEX_REPORT_TEST)
		stex->report_type.test = true;
	    }
	  rerr |= rerr2;
	}
    }
  else
    {
      iore_errorf(JSON_MSG_ARRAY_OPTION, JSON_ATTR_STEX_REPORT_TYPE);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_stex_report_type () */

static int
json_stex_data_format (json_value *jval, iore_stex_t *stex)
{
  int rerr = json_check_enum (jval, JSON_ATTR_STEX_REPORT_TYPE, stex_format_lbl,
			      IORE_STEX_FORMAT_LENGTH,
			      (int *) &stex->data_format);
  return rerr;
} /* json_stex_data_format () */

static int
json_stex_export_dir (json_value *jval, iore_stex_t *stex)
{
  int rerr = json_check_str (jval, JSON_ATTR_STEX_EXPORT_DIR,
			     &stex->export_dir);
  return rerr;
} /* json_stex_export_dir () */

static int
json_run (json_value *jrun, iore_run_t *run)
{
  int rerr = IORE_SUCCESS;

  if (jrun->type == json_object)
    {
      unsigned int len = jrun->u.object.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  char *jname = jrun->u.object.values[i].name;
	  json_value *jval = jrun->u.object.values[i].value;

	  if (strcasecmp (jname, JSON_ATTR_RUN_NUM_REPETITIONS) == 0)
	    rerr |= json_run_num_repts (jval, run);
	  else if (strcasecmp (jname, JSON_ATTR_TEST) == 0)
	    rerr |= json_test (jval, &run->test);
	  else
	    {
	      iore_errorf(JSON_MSG_INVALID_OPTION, jname);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_error("JSON Parser: Each run must be an object.");
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_run () */

static int
json_run_num_repts (json_value *jval, iore_run_t *run)
{
  int rerr = json_check_uint (jval, JSON_ATTR_RUN_NUM_REPETITIONS,
			      &run->num_repetitions);
  return rerr;
} /* json_run_num_repts () */

static int
json_test (json_value *jtest, iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  if (jtest->type == json_object)
    {
      unsigned int len = jtest->u.object.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  char *jname = jtest->u.object.values[i].name;
	  json_value *jval = jtest->u.object.values[i].value;

	  if (strcasecmp (jname, JSON_ATTR_TEST_TYPE) == 0)
	    rerr |= json_test_type (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_WRITE_FLUSH) == 0)
	    rerr |= json_test_write_flush (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_WRITE_FLUSH_PER_REQ) == 0)
	    rerr |= json_test_write_flush_per_req (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_READ_REORDER_OFFSET) == 0)
	    rerr |= json_test_read_reorder_offset (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_INTRA_TEST_BARRIER) == 0)
	    rerr |= json_test_intra_test_barrier (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_INTER_TEST_DELAY_SECS)
	      == 0)
	    rerr |= json_test_inter_test_delay_secs (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_FILE_MODE) == 0)
	    rerr |= json_test_file_mode (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_FILE_NAME) == 0)
	    rerr |= json_test_file_name (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_FILE_NAME_APPEND_SEQ_NUM)
	      == 0)
	    rerr |= json_test_file_name_append_seq_num (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_FILE_NAME_APPEND_TASK_ID)
	      == 0)
	    rerr |= json_test_file_name_append_task_id (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_FILE_DIR_PER_TASK) == 0)
	    rerr |= json_test_file_dir_per_task (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_TEST_FILE_KEEP) == 0)
	    rerr |= json_test_file_keep (jval, test);
	  else if (strcasecmp (jname, JSON_ATTR_WKLD) == 0)
	    rerr |= json_wkld (jval, &test->wkld);
	  else if (strcasecmp (jname, JSON_ATTR_AFIO) == 0)
	    rerr |= json_afio (jval, &test->afio);
	  else if (strcasecmp (jname, JSON_ATTR_AFSB) == 0)
	    rerr |= json_afsb (jval, &test->afsb);
	  else
	    {
	      iore_errorf(JSON_MSG_INVALID_OPTION, jname);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, JSON_ATTR_TEST);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_test () */

static int
json_test_type (json_value *jval, iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_array)
    {
      test->type.write = false;
      test->type.read = false;

      unsigned int len = jval->u.array.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  int val;
	  json_value *jval2 = jval->u.array.values[i];
	  int rerr2 = json_check_enum (jval2, JSON_ATTR_TEST_TYPE,
				       test_type_lbl, IORE_TEST_TYPE_LENGTH,
				       &val);
	  if (!rerr2)
	    {
	      if (val == IORE_TEST_TYPE_WRITE)
		test->type.write = true;
	      else if (val == IORE_TEST_TYPE_READ)
		test->type.read = true;
	    }
	  rerr |= rerr2;
	}
    }
  else
    {
      iore_errorf(JSON_MSG_ARRAY_OPTION, JSON_ATTR_TEST_TYPE);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_test_type () */

static int
json_test_write_flush (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_bool (jval, JSON_ATTR_TEST_WRITE_FLUSH,
			      &test->write_flush);
  return rerr;
} /* json_test_write_flush () */

static int
json_test_write_flush_per_req (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_bool (jval, JSON_ATTR_TEST_WRITE_FLUSH_PER_REQ,
			      &test->write_flush_per_req);
  return rerr;
} /* json_test_write_flush_pre_req () */

static int
json_test_read_reorder_offset (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_uint (jval, JSON_ATTR_TEST_READ_REORDER_OFFSET,
			      &test->read_reorder_offset);
  return rerr;
} /* json_test_read_reorder_offset () */

static int
json_test_intra_test_barrier (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_bool (jval, JSON_ATTR_TEST_INTRA_TEST_BARRIER,
			      &test->intra_test_barrier);
  return rerr;
} /* json_test_intra_test_barrier () */

static int
json_test_inter_test_delay_secs (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_uint (jval, JSON_ATTR_TEST_INTER_TEST_DELAY_SECS,
			      &test->inter_test_delay_secs);
  return rerr;
} /* json_test_inter_test_delay_secs () */

static int
json_test_file_mode (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_enum (jval, JSON_ATTR_TEST_FILE_MODE,
			      test_file_mode_lbl, IORE_TEST_FMODE_LENGTH,
			      (int *) &test->file_mode);
  return rerr;
} /* json_test_file_mode () */

static int
json_test_file_name (json_value *jval, iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  if (test->file_name)
    free (test->file_name);
  rerr = json_check_str (jval, JSON_ATTR_TEST_FILE_NAME, &test->file_name);

  return rerr;
} /* json_test_file_name () */

static int
json_test_file_name_append_seq_num (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_bool (jval, JSON_ATTR_TEST_FILE_NAME_APPEND_SEQ_NUM,
			      &test->file_name_append_sequence_num);
  return rerr;
} /* json_test_file_name_append_seq_num () */

static int
json_test_file_name_append_task_id (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_bool (jval, JSON_ATTR_TEST_FILE_NAME_APPEND_TASK_ID,
			      &test->file_name_append_task_id);
  return rerr;
} /* json_test_file_name_append_task_id () */

static int
json_test_file_dir_per_task (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_bool (jval, JSON_ATTR_TEST_FILE_DIR_PER_TASK,
			      &test->file_dir_per_task);
  return rerr;
} /* json_test_file_dir_per_task () */

static int
json_test_file_keep (json_value *jval, iore_test_t *test)
{
  int rerr = json_check_bool (jval, JSON_ATTR_TEST_FILE_KEEP, &test->file_keep);
  return rerr;
} /* json_test_file_keep () */

static int
json_wkld (json_value *jwkld, iore_wkld_t *wkld)
{
  int rerr = IORE_SUCCESS;

  if (jwkld->type == json_object)
    {
      unsigned int len = jwkld->u.object.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  char *jname = jwkld->u.object.values[i].name;
	  json_value *jval = jwkld->u.object.values[i].value;

	  if (strcasecmp (jname, JSON_ATTR_WKLD_NUM_TASKS) == 0)
	    rerr |= json_wkld_num_tasks (jval, wkld);
	  else if (strcasecmp (jname, wkld_type_lbl[IORE_WKLD_OFFSET]) == 0)
	    {
	      wkld->type = IORE_WKLD_OFFSET;
	      rerr |= json_wkld_oset (jval, &wkld->u.oset);
	    }
	  else if (strcasecmp (jname, wkld_type_lbl[IORE_WKLD_DATASET]) == 0)
	    {
	      wkld->type = IORE_WKLD_DATASET;
	      rerr |= json_wkld_dset (jval, &wkld->u.dset);
	    }
	  else
	    {
	      iore_errorf(JSON_MSG_INVALID_OPTION, jname);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, JSON_ATTR_WKLD);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld () */

static int
json_wkld_num_tasks (json_value *jval, iore_wkld_t *wkld)
{
  int rerr = json_check_uint (jval, JSON_ATTR_WKLD_NUM_TASKS, &wkld->num_tasks);
  return rerr;
} /* json_wkld_num_tasks () */

static int
json_wkld_oset (json_value *joset, iore_wkld_oset_t *oset)
{
  int rerr = IORE_SUCCESS;

  if (joset->type == json_object)
    {
      unsigned int len = joset->u.object.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  char *jname = joset->u.object.values[i].name;
	  json_value *jval = joset->u.object.values[i].value;

	  if (strcasecmp (jname, JSON_ATTR_WKLD_OSET_AC_PATTERN) == 0)
	    rerr |= json_wkld_oset_ac_pattern (jval, oset);
	  else if (strcasecmp (jname, JSON_ATTR_WKLD_OSET_DATA_SIZES) == 0)
	    rerr |= json_wkld_oset_data_sizes (jval, oset);
	  else if (strcasecmp (jname, JSON_ATTR_WKLD_OSET_REQ_SIZES) == 0)
	    rerr |= json_wkld_oset_req_sizes (jval, oset);
	  else if (strcasecmp (jname, JSON_ATTR_WKLD_OSET_DATA_SIZE_DISTRIB)
	      == 0)
	    rerr |= json_wkld_oset_data_size_distrib (jval, oset);
	  else if (strcasecmp (jname, JSON_ATTR_WKLD_OSET_REQ_SIZE_DISTRIB)
	      == 0)
	    rerr |= json_wkld_oset_req_size_distrib (jval, oset);
	  else
	    {
	      iore_errorf(JSON_MSG_INVALID_OPTION, jname);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, wkld_type_lbl[IORE_WKLD_OFFSET]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_oset () */

static int
json_wkld_oset_ac_pattern (json_value *jval, iore_wkld_oset_t *oset)
{
  int rerr = json_check_enum (jval, JSON_ATTR_WKLD_OSET_AC_PATTERN,
			      wkld_oset_access_pattern_lbl,
			      IORE_WKLD_OSET_AP_LENGTH,
			      (int *) &oset->ac_pattern);
  return rerr;
} /* json_wkld_oset_ac_pattern () */

static int
json_wkld_oset_data_sizes (json_value *jval, iore_wkld_oset_t *oset)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_array)
    {
      unsigned int len = jval->u.array.length;
      if (len > 0)
	{
	  if (oset->data_sizes)
	    free (oset->data_sizes);
	  oset->data_sizes = malloc (len * sizeof(size_t));
	  assert(oset->data_sizes);
	  oset->_data_sizes_len = len;

	  unsigned int i;
	  for (i = 0; i < len; i++)
	    rerr |= json_check_size (jval->u.array.values[i],
	    JSON_ATTR_WKLD_OSET_DATA_SIZES,
				     &oset->data_sizes[i]);

	  if (oset->data_size_distrib)
	    {
	      free (oset->data_size_distrib);
	      oset->data_size_distrib = NULL;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_ARRAY_OPTION, JSON_ATTR_WKLD_OSET_DATA_SIZES);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_oset_data_sizes () */

static int
json_wkld_oset_req_sizes (json_value *jval, iore_wkld_oset_t *oset)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_array)
    {
      unsigned int len = jval->u.array.length;
      if (len > 0)
	{
	  if (oset->req_sizes)
	    free (oset->req_sizes);
	  oset->req_sizes = malloc (len * sizeof(size_t));
	  assert(oset->req_sizes);
	  oset->_req_sizes_len = len;

	  unsigned int i;
	  for (i = 0; i < len; i++)
	    rerr |= json_check_size (jval->u.array.values[i],
	    JSON_ATTR_WKLD_OSET_REQ_SIZES,
				     &oset->req_sizes[i]);

	  if (oset->req_size_distrib)
	    {
	      free (oset->req_size_distrib);
	      oset->req_size_distrib = NULL;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_ARRAY_OPTION, JSON_ATTR_WKLD_OSET_REQ_SIZES);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_oset_req_sizes () */

static int
json_wkld_oset_data_size_distrib (json_value *jval, iore_wkld_oset_t *oset)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_object)
    {
      unsigned int len = jval->u.object.length;
      if (len == 1)
	{
	  if (oset->data_size_distrib)
	    {
	      prng_dist_free (oset->data_size_distrib);
	      free (oset->data_size_distrib);
	    }
	  oset->data_size_distrib = malloc (sizeof(iore_prng_dist_t));
	  assert(oset->data_size_distrib);

	  unsigned int i;
	  for (i = 0; i < len; i++)
	    {
	      char *jname = jval->u.object.values[i].name;
	      int j, type;
	      for (j = 0; j < IORE_PRNG_LENGTH; j++)
		{
		  if (strcasecmp (jname, prng_type_lbl[j]) == 0)
		    {
		      type = j;
		      j = IORE_PRNG_LENGTH;
		    }
		}
	      oset->data_size_distrib->type = type;

	      json_value *jval2 = jval->u.object.values[i].value;
	      rerr |= json_wkld_oset_distrib_params (jval2,
						     oset->data_size_distrib);
	    }

	  if (oset->data_sizes)
	    {
	      free (oset->data_sizes);
	      oset->data_sizes = NULL;
	    }
	  oset->_data_sizes_len = 0;
	}
      else
	{
	  if (oset->data_size_distrib)
	    {
	      free (oset->data_size_distrib);
	      oset->data_size_distrib = NULL;
	    }
	  iore_errorf("JSON Parser: '%s': Must have one distribution.",
		      JSON_ATTR_WKLD_OSET_DATA_SIZE_DISTRIB);
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION,
		  JSON_ATTR_WKLD_OSET_DATA_SIZE_DISTRIB);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_oset_data_size_distrib () */

static int
json_wkld_oset_req_size_distrib (json_value *jval, iore_wkld_oset_t *oset)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_object)
    {
      unsigned int len = jval->u.object.length;
      if (len == 1)
	{
	  if (oset->req_size_distrib)
	    {
	      prng_dist_free (oset->req_size_distrib);
	      free (oset->req_size_distrib);
	    }
	  oset->req_size_distrib = malloc (sizeof(iore_prng_dist_t));
	  assert(oset->req_size_distrib);

	  unsigned int i;
	  for (i = 0; i < len; i++)
	    {
	      char *jname = jval->u.object.values[i].name;
	      int j, type;
	      for (j = 0; j < IORE_PRNG_LENGTH; j++)
		{
		  if (strcasecmp (jname, prng_type_lbl[j]) == 0)
		    {
		      type = j;
		      j = IORE_PRNG_LENGTH;
		    }
		}
	      oset->req_size_distrib->type = type;

	      json_value *jval2 = jval->u.object.values[i].value;
	      rerr |= json_wkld_oset_distrib_params (jval2,
						     oset->req_size_distrib);
	    }

	  if (oset->req_sizes)
	    {
	      free (oset->req_sizes);
	      oset->req_sizes = NULL;
	    }
	  oset->_req_sizes_len = 0;
	}
      else
	{
	  if (oset->req_size_distrib)
	    {
	      free (oset->req_size_distrib);
	      oset->req_size_distrib = NULL;
	    }
	  iore_errorf("JSON Parser: '%s': Must have one distribution.",
		      JSON_ATTR_WKLD_OSET_REQ_SIZE_DISTRIB);
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, JSON_ATTR_WKLD_OSET_REQ_SIZE_DISTRIB);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_oset_req_size_distrib () */

static int
json_wkld_oset_distrib_params (json_value *jprng, iore_prng_dist_t *prng)
{
  int rerr = IORE_SUCCESS;

  if (jprng->type == json_object)
    {
      unsigned int len = jprng->u.object.length;
      if (len > 0)
	{
	  dict_initc (&prng->params, len);
	  unsigned int i;
	  for (i = 0; i < len; i++)
	    {
	      char *jname = jprng->u.object.values[i].name;
	      json_value *jval = jprng->u.object.values[i].value;
	      size_t val;
	      rerr |= json_check_size (jval, jname, &val);
	      if (!rerr)
		{
		  char *param = zutoa (val);
		  dict_set (&prng->params, jname, param);
		}
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, prng_type_lbl[prng->type]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_oset_distrib_params () */

static int
json_wkld_dset (json_value *jdset, iore_wkld_dset_t *dset)
{
  int rerr = IORE_SUCCESS;

  if (jdset->type == json_object)
    {
      unsigned int len = jdset->u.object.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  char *jname = jdset->u.object.values[i].name;
	  json_value *jval = jdset->u.object.values[i].value;

	  if (strcasecmp (jname, JSON_ATTR_WKLD_DSET_NUM_VARS) == 0)
	    rerr |= json_wkld_dset_num_vars (jval, dset);
	  else if (strcasecmp (jname, JSON_ATTR_WKLD_DSET_VAR_TYPES) == 0)
	    rerr |= json_wkld_dset_var_types (jval, dset);
	  else if (strcasecmp (jname,
			       wkld_dset_type_lbl[IORE_WKLD_DSET_CARTESIAN])
	      == 0)
	    {
	      dset->type = IORE_WKLD_DSET_CARTESIAN;
	      rerr |= json_wkld_dset_cart (jval, &dset->u.cart);
	    }
	  else
	    {
	      iore_errorf(JSON_MSG_INVALID_OPTION, jname);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, wkld_type_lbl[IORE_WKLD_DATASET]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_dset () */

static int
json_wkld_dset_num_vars (json_value *jval, iore_wkld_dset_t *dset)
{
  int rerr = IORE_SUCCESS;

  unsigned int val = 0;
  if (!json_check_uint (jval, JSON_ATTR_WKLD_DSET_NUM_VARS, &val))
    {
      if (dset->num_vars == 0)
	dset->num_vars = val;
      else if (dset->num_vars > 0 && val != dset->num_vars)
	{
	  iore_errorf(JSON_MSG_ARRAY_SIZE, JSON_ATTR_WKLD_DSET_NUM_VARS,
		      JSON_ATTR_WKLD_DSET_VAR_TYPES);
	  rerr = IORE_FAILURE;
	}
    }
  else
    rerr = IORE_FAILURE;

  return rerr;
} /* json_wkld_dset_num_vars () */

static int
json_wkld_dset_var_types (json_value *jval, iore_wkld_dset_t *dset)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_array)
    {
      unsigned int len = jval->u.array.length;
      if (len > 0)
	{
	  if (dset->num_vars == 0 || len == dset->num_vars)
	    {
	      dset->num_vars = len;

	      if (dset->var_types)
		free (dset->var_types);
	      dset->var_types = malloc (
		  len * sizeof(enum iore_wkld_dset_var_type));
	      assert(dset->var_types);

	      size_t vars_size = 0;
	      unsigned int i;
	      for (i = 0; i < len; i++)
		{
		  json_value *jval2 = jval->u.array.values[i];
		  rerr |= json_check_enum (jval2, JSON_ATTR_WKLD_DSET_VAR_TYPES,
					   wkld_dset_var_type_lbl,
					   IORE_WKLD_DSET_DV_LENGTH,
					   (int *) &dset->var_types[i]);
		  if (!rerr)
		    vars_size += dset_var_size (dset->var_types[i]);
		}
	      dset->_vars_size = vars_size;
	    }
	  else
	    {
	      iore_errorf(JSON_MSG_ARRAY_SIZE, JSON_ATTR_WKLD_DSET_VAR_TYPES,
			  JSON_ATTR_WKLD_DSET_NUM_VARS);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_ARRAY_OPTION, JSON_ATTR_WKLD_DSET_VAR_TYPES);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_dset_var_types () */

static int
json_wkld_dset_cart (json_value *jcart, iore_wkld_dset_cart_t *cart)
{
  int rerr = IORE_SUCCESS;

  if (jcart->type == json_object)
    {
      cart->num_dims = 0;
      cart->g_dim_sizes = NULL;
      cart->g_dim_divs = NULL;
      cart->my_dim_sizes = NULL;
      cart->my_start_coord = NULL;

      unsigned int len = jcart->u.object.length;
      unsigned int i;
      for (i = 0; i < len; i++)
	{
	  char *jname = jcart->u.object.values[i].name;
	  json_value *jval = jcart->u.object.values[i].value;

	  if (strcasecmp (jname, JSON_ATTR_WKLD_DSET_CART_NUM_DIMS) == 0)
	    rerr |= json_wkld_dset_cart_num_dims (jval, cart);
	  else if (strcasecmp (jname, JSON_ATTR_WKLD_DSET_CART_G_DIM_SIZES)
	      == 0)
	    rerr |= json_wkld_dset_cart_dim_sizes (jval, cart);
	  else if (strcasecmp (jname, JSON_ATTR_WKLD_DSET_CART_G_DIM_DIVS) == 0)
	    rerr |= json_wkld_dset_cart_dim_divs (jval, cart);
	  else
	    {
	      iore_errorf(JSON_MSG_INVALID_OPTION, jname);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION,
		  wkld_dset_type_lbl[IORE_WKLD_DSET_CARTESIAN]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_dset_cart () */

static int
json_wkld_dset_cart_num_dims (json_value *jval, iore_wkld_dset_cart_t *cart)
{
  int rerr = IORE_SUCCESS;

  unsigned int val = 0;
  if (!json_check_uint (jval, JSON_ATTR_WKLD_DSET_CART_NUM_DIMS, &val))
    {
      if (cart->num_dims == 0)
	cart->num_dims = val;
      else if (cart->num_dims > 0 && val != cart->num_dims)
	{
	  iore_errorf(
	      JSON_MSG_ARRAY_SIZE,
	      JSON_ATTR_WKLD_DSET_CART_NUM_DIMS,
	      JSON_ATTR_WKLD_DSET_CART_G_DIM_SIZES ", " JSON_ATTR_WKLD_DSET_CART_G_DIM_DIVS);
	  rerr = IORE_FAILURE;
	}
    }
  else
    rerr = IORE_FAILURE;

  return rerr;
} /* json_wkld_dset_cart_num_dims () */

static int
json_wkld_dset_cart_dim_sizes (json_value *jval, iore_wkld_dset_cart_t *cart)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_array)
    {
      unsigned int len = jval->u.array.length;
      if (len > 0)
	{
	  if (cart->num_dims == 0 || len == cart->num_dims)
	    {
	      cart->num_dims = len;

	      if (cart->g_dim_sizes)
		free (cart->g_dim_sizes);
	      cart->g_dim_sizes = malloc (len * sizeof(unsigned int));
	      assert(cart->g_dim_sizes);

	      unsigned int i;
	      for (i = 0; i < len; i++)
		{
		  json_value *jval2 = jval->u.array.values[i];
		  rerr |= json_check_uint (jval2,
		  JSON_ATTR_WKLD_DSET_CART_G_DIM_SIZES,
					   &cart->g_dim_sizes[i]);
		}
	    }
	  else
	    {
	      iore_errorf(JSON_MSG_ARRAY_SIZE,
			  JSON_ATTR_WKLD_DSET_CART_G_DIM_SIZES,
			  JSON_ATTR_WKLD_DSET_CART_NUM_DIMS);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_ARRAY_OPTION, JSON_ATTR_WKLD_DSET_CART_G_DIM_SIZES);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_dset_cart_dim_sizes () */

static int
json_wkld_dset_cart_dim_divs (json_value *jval, iore_wkld_dset_cart_t *cart)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_array)
    {
      unsigned int len = jval->u.array.length;
      if (len > 0)
	{
	  if (cart->num_dims == 0 || len == cart->num_dims)
	    {
	      cart->num_dims = len;

	      if (cart->g_dim_divs)
		free (cart->g_dim_divs);
	      cart->g_dim_divs = malloc (len * sizeof(unsigned int));
	      assert(cart->g_dim_divs);

	      unsigned int i;
	      for (i = 0; i < len; i++)
		{
		  json_value *jval2 = jval->u.array.values[i];
		  rerr |= json_check_uint (jval2,
		  JSON_ATTR_WKLD_DSET_CART_G_DIM_DIVS,
					   &cart->g_dim_divs[i]);
		}
	    }
	  else
	    {
	      iore_errorf(JSON_MSG_ARRAY_SIZE,
			  JSON_ATTR_WKLD_DSET_CART_G_DIM_DIVS,
			  JSON_ATTR_WKLD_DSET_CART_NUM_DIMS);
	      rerr = IORE_FAILURE;
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_ARRAY_OPTION, JSON_ATTR_WKLD_DSET_CART_G_DIM_DIVS);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_wkld_dset_cart_dim_divs () */

static int
json_afio (json_value *jafio, iore_afio_t *afio)
{
  int rerr = IORE_SUCCESS;

  if (jafio->type == json_object)
    {
      unsigned int len = jafio->u.object.length;
      if (len == 1)
	{
	  char *jname = jafio->u.object.values[0].name;
	  int i, afio_type;
	  for (i = 0; i < IORE_AFIO_LENGTH; i++)
	    {
	      if (strcasecmp (jname, afio_lbl[i]) == 0)
		{
		  afio_type = i;
		  i = IORE_AFIO_LENGTH;
		}
	    }
	  afio->type = afio_type;

	  json_value *jval = jafio->u.object.values[0].value;
	  rerr |= json_afio_params (jval, afio);
	}
      else
	{
	  afio->type = IORE_AFIO_LENGTH;
	  iore_errorf("JSON Parser: '%s': Must have one AFIO definition.",
		      JSON_ATTR_AFIO);
	  rerr = IORE_FAILURE;
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, JSON_ATTR_AFIO);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_afio () */

static int
json_afio_params (json_value *jval, iore_afio_t *afio)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_object)
    {
      unsigned int len = jval->u.object.length;
      if (len > 0)
	{
	  dict_initc (&afio->params, len);
	  unsigned int i;
	  for (i = 0; i < len; i++)
	    {
	      char *jname = jval->u.object.values[i].name;
	      json_value *jval2 = jval->u.object.values[i].value;
	      char *val;
	      rerr |= json_check_str (jval2, jname, &val);
	      if (!rerr)
		dict_set (&afio->params, jname, val);
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, afio_lbl[afio->type]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_afio_params () */

static int
json_afsb (json_value *jafsb, iore_afsb_t **afsb)
{
  int rerr = IORE_SUCCESS;

  if (jafsb->type == json_object)
    {

      unsigned int len = jafsb->u.object.length;
      if (len == 1)
	{
	  *afsb = malloc (sizeof(iore_afsb_t));
	  assert(*afsb);

	  char *jname = jafsb->u.object.values[0].name;
	  int i, afsb_type;
	  for (i = 0; i < IORE_AFSB_LENGTH; i++)
	    {
	      if (strcasecmp (jname, afsb_lbl[i]) == 0)
		{
		  afsb_type = i;
		  i = IORE_AFSB_LENGTH;
		}
	    }
	  (*afsb)->type = afsb_type;

	  json_value *jval = jafsb->u.object.values[0].value;
	  rerr |= json_afsb_params (jval, *afsb);
	}
      else
	{
	  *afsb = NULL;
	  iore_errorf("JSON Parser: '%s': Must have one AFSB definition.",
		      JSON_ATTR_AFSB);
	  rerr = IORE_FAILURE;

	}
    }
  else
    {
      *afsb = NULL;
      iore_errorf(JSON_MSG_OBJECT_OPTION, JSON_ATTR_AFSB);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_afsb () */

static int
json_afsb_params (json_value *jval, iore_afsb_t *afsb)
{
  int rerr = IORE_SUCCESS;

  if (jval->type == json_object)
    {
      unsigned int len = jval->u.object.length;
      if (len > 0)
	{
	  dict_initc (&afsb->params, len);
	  unsigned int i;
	  for (i = 0; i < len; i++)
	    {
	      char *jname = jval->u.object.values[i].name;
	      json_value *jval2 = jval->u.object.values[i].value;
	      char *val;
	      rerr |= json_check_str (jval2, jname, &val);
	      if (!rerr)
		dict_set (&afsb->params, jname, val);
	    }
	}
    }
  else
    {
      iore_errorf(JSON_MSG_OBJECT_OPTION, afsb_lbl[afsb->type]);
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* json_afsb_params () */
