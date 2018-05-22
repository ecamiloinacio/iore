/*
 * test_iore_afio.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_iore_afio.h"
#include "iore_error.h"
#include "iore_ctx.h"
#include "iore_workload.h"
#include "iore_util.h"

iore_test_t
get_sample_oset_test ()
{
  iore_test_t test =
    { };
  int data_sizes_len = MAX_PROCESSES + 1;
  int req_sizes_len = MAX_PROCESSES;
  int i;

  test.write_flush = true;
  test.write_flush_per_req = true;

  test.file_mode = IORE_TEST_FMODE_NX1;

  test.wkld.num_tasks = ctx.num_procs;
  test.wkld.type = IORE_WKLD_OFFSET;
  test.wkld.u.oset.data_sizes = malloc (data_sizes_len * sizeof(size_t));
  assert(test.wkld.u.oset.data_sizes);
  for (i = 0; i < data_sizes_len; i++)
    test.wkld.u.oset.data_sizes[i] = (i + 1) * BASE_DATA_SIZE;
  test.wkld.u.oset._data_sizes_len = data_sizes_len;
  test.wkld.u.oset.req_sizes = malloc (req_sizes_len * sizeof(size_t));
  assert(test.wkld.u.oset.req_sizes);
  for (i = 0; i < req_sizes_len; i++)
    test.wkld.u.oset.req_sizes[i] = (i + 1) * BASE_REQ_SIZE;
  test.wkld.u.oset._req_sizes_len = req_sizes_len;

  test.wkld.u.oset.my_data_size = test.wkld.u.oset.data_sizes[ctx.task_id
      % data_sizes_len];
  test.wkld.u.oset.my_req_size = test.wkld.u.oset.req_sizes[ctx.task_id
      & req_sizes_len];

  size_t file_size = 0;
  size_t *data_sizes = test.wkld.u.oset.data_sizes;
  int num_tasks = test.wkld.num_tasks;
  for (i = 0; i < num_tasks; i++)
    file_size += data_sizes[i];
  test.wkld.u.oset._file_size = file_size;

  return test;
} /* get_sample_oset_test () */

iore_test_t
get_sample_dset_test ()
{
  iore_test_t test =
    { };
  int i;
  size_t vars_size = 0;
  size_t dset_size = 0;

  test.write_flush = true;
  test.write_flush_per_req = true;

  test.file_mode = IORE_TEST_FMODE_NX1;

  test.wkld.type = IORE_WKLD_DATASET;
  test.wkld.u.dset.num_vars = NUM_VARS;
  test.wkld.u.dset.var_types = malloc (
  NUM_VARS * sizeof(enum iore_wkld_dset_var_type));
  assert(test.wkld.u.dset.var_types);
  for (i = 0; i < NUM_VARS; i++)
    {
      test.wkld.u.dset.var_types[i] = (i % IORE_WKLD_DSET_DV_LENGTH);
      vars_size += dset_var_size (test.wkld.u.dset.var_types[i]);
    }

  test.wkld.u.dset._vars_size = vars_size;

  test.wkld.u.dset.type = IORE_WKLD_DSET_CARTESIAN;
  test.wkld.u.dset.u.cart.num_dims = NUM_DIMS;
  test.wkld.u.dset.u.cart.g_dim_sizes = malloc (
  NUM_DIMS * sizeof(unsigned int));
  assert(test.wkld.u.dset.u.cart.g_dim_sizes);
  for (i = 0; i < NUM_DIMS; i++)
    test.wkld.u.dset.u.cart.g_dim_sizes[i] = (i + 1);
  test.wkld.u.dset.u.cart.g_dim_divs = malloc (NUM_DIMS * sizeof(unsigned int));
  assert(test.wkld.u.dset.u.cart.g_dim_divs);
  if (ctx.num_procs > 1)
    {
      for (i = 0; i < NUM_DIMS; i++)
	test.wkld.u.dset.u.cart.g_dim_divs[i] = (i % 2) + 1;
    }
  else
    {
      for (i = 0; i < NUM_DIMS; i++)
	test.wkld.u.dset.u.cart.g_dim_divs[i] = 1;
    }

  test.wkld.u.dset.u.cart.my_dim_sizes = malloc (
  NUM_DIMS * sizeof(unsigned int));
  assert(test.wkld.u.dset.u.cart.my_dim_sizes);
  dset_size = test.wkld.u.dset._vars_size;
  for (i = 0; i < NUM_DIMS; i++)
    {
      test.wkld.u.dset.u.cart.my_dim_sizes[i] =
	  (test.wkld.u.dset.u.cart.g_dim_sizes[i]
	      / test.wkld.u.dset.u.cart.g_dim_divs[i]);
      dset_size *= test.wkld.u.dset.u.cart.my_dim_sizes[i];
    }
  test.wkld.u.dset.u.cart.my_start_coord = malloc (
  NUM_DIMS * sizeof(unsigned int));
  assert(test.wkld.u.dset.u.cart.my_start_coord);
  if (ctx.task_id == 0)
    {
      for (i = 0; i < NUM_DIMS; i++)
	test.wkld.u.dset.u.cart.my_start_coord[i] = 0;
    }
  else if (ctx.task_id == 1)
    {
      for (i = 0; i < NUM_DIMS; i++)
	test.wkld.u.dset.u.cart.my_start_coord[i] =
	    test.wkld.u.dset.u.cart.my_dim_sizes[i] - 1;
    }

  test.wkld.u.dset.my_size = dset_size;

  return test;
} /* get_sample_dset_test () */

int
fill_buffer (char **buf, size_t size)
{
  *buf = malloc (size);
  assert(*buf);
  memset (*buf, ctx.task_id, size);

  return IORE_SUCCESS;
} /* fill_buffer () */

int
fill_offsets (off_t **offs, size_t data_size, size_t req_size,
	      enum iore_test_file_mode file_mode)
{
  int offs_len = ((data_size + req_size - 1) / req_size);
  *offs = malloc (offs_len * sizeof(off_t));
  assert(*offs);

  int i;
  if (ctx.task_id == 0)
    {
      for (i = 0; i < offs_len; i++)
	(*offs)[i] = i * req_size;
    }
  else if (ctx.task_id == 1)
    {
      if (file_mode == IORE_TEST_FMODE_NX1)
	{
	  off_t start_off = BASE_DATA_SIZE;
	  for (i = 0; i < offs_len; i++)
	    (*offs)[i] = (i * req_size) + start_off;
	}
      else if (file_mode == IORE_TEST_FMODE_NXN)
	{
	  for (i = 0; i < offs_len; i++)
	    (*offs)[i] = i * req_size;
	}
      else
	return IORE_FAILURE;
    }

  return IORE_SUCCESS;
} /* fill_offsets () */

int
test_wr_oset (const iore_afio_vtable_t *backend, iore_file_t file,
	      iore_test_t test)
{
  int rerr;

  char *buf;
  if (fill_buffer (&buf, test.wkld.u.oset.my_req_size))
    return IORE_FAILURE;

  off_t *offs;
  if (fill_offsets (&offs, test.wkld.u.oset.my_data_size,
		    test.wkld.u.oset.my_req_size, test.file_mode))
    return IORE_FAILURE;

  ssize_t nbytes;
  /***** write test *****/
  /* create */
  rerr = backend->create (&file, &test);
  /* write */
  if (rerr)
    fprintf (stderr, "Error: task %d: create\n", ctx.task_id);
  else
    {
      nbytes = backend->write_oset (file, buf, offs, &test);
      rerr = (nbytes == -1 || (size_t) nbytes != test.wkld.u.oset.my_data_size);
    }
  /* close */
  if (rerr)
    fprintf (stderr, "Error: task %d: write_oset\n", ctx.task_id);
  else
    rerr = backend->close (&file);

  /***** read test *****/
  /* open */
  if (rerr)
    fprintf (stderr, "Error: task %d: wclose\n", ctx.task_id);
  else
    rerr = backend->open (&file, &test);
  /* read */
  if (rerr)
    fprintf (stderr, "Error: task %d: open\n", ctx.task_id);
  else
    {
      nbytes = backend->read_oset (file, buf, offs, &test);
      rerr = (nbytes == -1 || (size_t) nbytes != test.wkld.u.oset.my_data_size);
    }
  /* close */
  if (rerr)
    fprintf (stderr, "Error: task %d: read_oset\n", ctx.task_id);
  else
    rerr = backend->close (&file);
  /* remove */
  MPI_Barrier (ctx.comm);
  if (rerr)
    fprintf (stderr, "Error: task %d: rclose\n", ctx.task_id);
  else if (!rerr && (test.file_mode == IORE_TEST_FMODE_NX1)
      && (ctx.task_id == IORE_MASTER_TASK))
    rerr = backend->remove (file);
  else if (!rerr && (test.file_mode == IORE_TEST_FMODE_NXN))
    rerr = backend->remove (file);

  if (rerr)
    fprintf (stderr, "Error: task %d: remove\n", ctx.task_id);

  free (buf);
  free (offs);

  return rerr;
} /* test_wr_oset () */

int
test_wr_dset (const iore_afio_vtable_t *backend, iore_file_t file,
	      iore_test_t test)
{
  int rerr;

  char *buf;
  if (fill_buffer (&buf, test.wkld.u.dset.my_size))
    return IORE_FAILURE;

  ssize_t nbytes;
  /***** write test *****/
  /* create */
  rerr = backend->create (&file, &test);
  /* write */
  if (rerr)
    fprintf (stderr, "Error: task %d: create\n", ctx.task_id);
  else
    {
      nbytes = backend->write_dset (file, buf, &test);
      rerr = (nbytes == -1 || (size_t) nbytes != test.wkld.u.dset.my_size);
    }
  /* close */
  if (rerr)
    fprintf (stderr, "Error: task %d: write_dset\n", ctx.task_id);
  else
    rerr = backend->close (&file);

  /***** read test *****/
  /* open */
  if (rerr)
    fprintf (stderr, "Error: task %d: wclose\n", ctx.task_id);
  else
    rerr = backend->open (&file, &test);
  /* read */
  if (rerr)
    fprintf (stderr, "Error: task %d: open\n", ctx.task_id);
  else
    {
      nbytes = backend->read_dset (file, buf, &test);
      rerr = (nbytes == -1 || (size_t) nbytes != test.wkld.u.dset.my_size);
    }
  /* close */
  if (rerr)
    fprintf (stderr, "Error: task %d: read_dset\n", ctx.task_id);
  else
    rerr = backend->close (&file);
  /* remove */
  MPI_Barrier (ctx.comm);
  if (rerr)
    fprintf (stderr, "Error: task %d: rclose\n", ctx.task_id);
  else if (!rerr && (test.file_mode == IORE_TEST_FMODE_NX1)
      && (ctx.task_id == IORE_MASTER_TASK))
    rerr = backend->remove (file);
  else if (!rerr && (test.file_mode == IORE_TEST_FMODE_NXN))
    rerr = backend->remove (file);

  if (rerr)
    fprintf (stderr, "Error: task %d: remove\n", ctx.task_id);

  free (buf);

  return rerr;
} /* test_wr_dset () */

