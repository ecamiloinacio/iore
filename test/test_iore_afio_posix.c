/*
 * test_iore_afio_posix.c
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

int
test_wr_oset01 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset02 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset03 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_oset04 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset01 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset02 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset03 (const iore_afio_vtable_t *, iore_file_t);
int
test_wr_dset04 (const iore_afio_vtable_t *, iore_file_t);
int
test_to_str ();

int
main (int argc, char **argv)
{
  fputs ("Initializing AFIO POSIX test...\n", stdout);
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
      const iore_afio_vtable_t *backend = afio_pool[IORE_AFIO_POSIX];
      iore_file_t file =
	{ };
      file.name = TEST_FILE_NAME;

      const int num_tests = 9;
      int i;
      int rerr;
      for (i = 1; i <= num_tests; i++)
	{
	  switch (i)
	    {
	    case 1:
	      rerr = test_wr_oset01 (backend, file);
	      break;
	    case 2:
	      rerr = test_wr_oset02 (backend, file);
	      break;
	    case 3:
	      rerr = test_wr_oset03 (backend, file);
	      break;
	    case 4:
	      rerr = test_wr_oset04 (backend, file);
	      break;
	    case 5:
	      rerr = test_wr_dset01 (backend, file);
	      break;
	    case 6:
	      rerr = test_wr_dset02 (backend, file);
	      break;
	    case 7:
	      rerr = test_wr_dset03 (backend, file);
	      break;
	    case 8:
	      rerr = test_wr_dset04 (backend, file);
	      break;
	    case 9:
	      rerr = test_to_str ();
	      break;
	    }
	  if (rerr)
	    fprintf (stdout, "[Task %d] Test %d: ...FAIL!\n", ctx.task_id, i);
	  else
	    fprintf (stdout, "[Task %d] Test %d: ...SUCCESS!\n", ctx.task_id,
		     i);
	}
    }

  MPI_Finalize ();
  fputs ("Finalizing AFIO POSIX test.\n", stdout);
} /* main () */

int
test_wr_oset01 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_oset_test ();
  test.afio.type = IORE_AFIO_POSIX;
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
  test.afio.type = IORE_AFIO_POSIX;
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
  test.wkld.u.oset._file_size = test.wkld.u.oset.my_data_size;
  test.afio.type = IORE_AFIO_POSIX;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf (file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

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
  test.wkld.u.oset._file_size = test.wkld.u.oset.my_data_size;
  test.afio.type = IORE_AFIO_POSIX;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf (file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_oset (backend, file, test);

  return rerr;
} /* test_wr_oset04 () */

int
test_wr_dset01 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.afio.type = IORE_AFIO_POSIX;
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
  test.afio.type = IORE_AFIO_POSIX;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "false");

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset02 () */

int
test_wr_dset03 (const iore_afio_vtable_t *backend, iore_file_t file)
{
  int rerr = IORE_SUCCESS;

  iore_test_t test = get_sample_dset_test ();
  test.file_mode = IORE_TEST_FMODE_NXN;
  test.afio.type = IORE_AFIO_POSIX;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf (file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

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
  test.afio.type = IORE_AFIO_POSIX;
  dict_set (&test.afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");

  char *swp = file.name;
  file.name = malloc (strlen (swp) + 2);
  assert(file.name);
  sprintf (file.name, "%s.%d", TEST_FILE_NAME, ctx.task_id);

  rerr = test_wr_dset (backend, file, test);

  return rerr;
} /* test_wr_dset04 () */

int
test_to_str ()
{
  iore_afio_t afio =
    { };
  afio.type = IORE_AFIO_POSIX;
  dict_set (&afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP, "true");

  fprintf (stdout, "[Task %d]: %s\n", ctx.task_id, afio2str (&afio));

  return IORE_SUCCESS;
} /* test_to_str () */
