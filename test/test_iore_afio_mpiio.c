/*
 * test_iore_afio_mpiio.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "test_iore_afio.h"
#include "iore_error.h"
#include "iore_ctx.h"

#ifndef WITH_MPIIO_AFIO
#define WITH_MPIIO_AFIO
#endif

int
test_wr_oset01 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset02 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset03 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset04 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset05 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset06 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset07 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset08 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset01 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset02 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset03 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset04 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset05 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset06 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset07 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset08 (const iore_afio_vtable_t *, iore_file_t);
int
test_to_str ();

int
main (int argc, char **argv)
{
  fputs ("Initializing AFIO MPIIO test...\n", stdout);
  MPI_Init (&argc, &argv);
  ctx_init ();

  if (ctx.num_procs > MAX_PROCESSES)
    {
      MPI_Comm new_comm;
      MPI_Group grp, new_grp;
      int range[3] =
	{ 0, MAX_PROCESSES - 1, 1 };
      MPI_Comm_group (MPI_COMM_WORLD, &grp);
      MPI_Group_range_incl (grp, 1, &range, &new_grp);
      MPI_Comm_create (MPI_COMM_WORLD, new_grp, &new_comm);
      ctx.comm = new_comm;
    }

  if (ctx.comm != MPI_COMM_NULL)
    {
      const iore_afio_vtable_t *backend = afio_pool[IORE_AFIO_MPIIO];
      iore_file_t file =
	{ };
      file.name = TEST_FILE_NAME;

      fprintf (stdout, "[Task %d] Test Offset WR 01: ...\n", ctx.task_id);
      if (test_wr_oset01 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Offset WR 02: ...\n", ctx.task_id);
      if (test_wr_oset02 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Offset WR 03: ...\n", ctx.task_id);
      if (test_wr_oset03 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Offset WR 04: ...\n", ctx.task_id);
      if (test_wr_oset04 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Offset WR 05: ...\n", ctx.task_id);
      if (test_wr_oset05 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Offset WR 06: ...\n", ctx.task_id);
      if (test_wr_oset06 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Offset WR 07: ...\n", ctx.task_id);
      if (test_wr_oset07 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Offset WR 08: ...\n", ctx.task_id);
      if (test_wr_oset08 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Dataset WR 01: ...\n", ctx.task_id);
      if (test_wr_dset01 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Dataset WR 02: ...\n", ctx.task_id);
      if (test_wr_dset02 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Dataset WR 03: ...\n", ctx.task_id);
      if (test_wr_dset03 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test Dataset WR 04: ...\n", ctx.task_id);
      if (test_wr_dset04 (backend, file))
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);

      MPI_Barrier (ctx.comm);
      fprintf (stdout, "[Task %d] Test AFIO to string: ...\n", ctx.task_id);
      if (test_to_str ())
	fprintf (stdout, "[Task %d] FAIL!\n", ctx.task_id);
      else
	fprintf (stdout, "[Task %d] SUCCESS!\n", ctx.task_id);
    }

  MPI_Finalize ();
  fputs ("Finalizing AFIO MPIIO test.\n", stdout);
} /* main () */

int
test_wr_oset01 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset01 () */

int
test_wr_oset02 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.write_flush = false;
  test.write_flush_per_req = false;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset02 () */

int
test_wr_oset03 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf(file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset03 () */

int
test_wr_oset04 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.write_flush = false;
  test.write_flush_per_req = false;
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf(file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset04 () */

int
test_wr_oset05 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.wkld.u.oset.my_data_size = BASE_DATA_SIZE;
  test.wkld.u.oset.my_req_size = BASE_REQ_SIZE;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");
  dict_set (&test.afio.params, AFIO_PARAM_COLLECTIVE_IO, "true");
  dict_set (&test.afio.params, AFIO_PARAM_FILE_VIEW, "false");

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset05 () */

int
test_wr_oset06 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.write_flush = false;
  test.write_flush_per_req = false;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");
  dict_set (&test.afio.params, AFIO_PARAM_COLLECTIVE_IO, "false");
  dict_set (&test.afio.params, AFIO_PARAM_FILE_VIEW, "true");

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset06 () */

int
test_wr_oset07 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");
  dict_set (&test.afio.params, AFIO_PARAM_COLLECTIVE_IO, "true");
  dict_set (&test.afio.params, AFIO_PARAM_FILE_VIEW, "true");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf(file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset07 () */

int
test_wr_oset08 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.write_flush = false;
  test.write_flush_per_req = false;
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");
  dict_set (&test.afio.params, AFIO_PARAM_COLLECTIVE_IO, "false");
  dict_set (&test.afio.params, AFIO_PARAM_FILE_VIEW, "false");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf(file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset08 () */

int
test_wr_dset01 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset01 () */

int
test_wr_dset02 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.write_flush = false;
  test.write_flush_per_req = false;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset02 () */

int
test_wr_dset03 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf(file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset03 () */

int
test_wr_dset04 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.write_flush = false;
  test.write_flush_per_req = false;
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf(file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset04 () */

int
test_wr_dset05 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");
  dict_set (&test.afio.params, AFIO_PARAM_COLLECTIVE_IO, "false");
  dict_set (&test.afio.params, AFIO_PARAM_FILE_VIEW, "true");

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset05 () */

int
test_wr_dset06 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.write_flush = false;
  test.write_flush_per_req = false;
  test.wkld.u.dset.my_size = BASE_DATA_SIZE;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");
  dict_set (&test.afio.params, AFIO_PARAM_COLLECTIVE_IO, "true");
  dict_set (&test.afio.params, AFIO_PARAM_FILE_VIEW, "false");

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset06 () */

int
test_wr_dset07 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");
  dict_set (&test.afio.params, AFIO_PARAM_COLLECTIVE_IO, "false");
  dict_set (&test.afio.params, AFIO_PARAM_FILE_VIEW, "false");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf(file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset07 () */

int
test_wr_dset08 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.write_flush = false;
  test.write_flush_per_req = false;
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_MPIIO;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");
  dict_set (&test.afio.params, AFIO_PARAM_COLLECTIVE_IO, "true");
  dict_set (&test.afio.params, AFIO_PARAM_FILE_VIEW, "true");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf(file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset08 () */

int
test_to_str ()
{
  iore_afio_t afio =
    { };
  afio.type = IORE_AFIO_MPIIO;
  dict_set (&afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");
  dict_set (&afio.params, AFIO_PARAM_COLLECTIVE_IO, "true");
  dict_set (&afio.params, AFIO_PARAM_FILE_VIEW, "true");

  fprintf (stdout, "[Task %d]: %s\n", ctx.task_id, afio2str (&afio));

  return IORE_SUCCESS;
} /* test_to_str () */
