/*
 * iore_afio_cstream.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <assert.h>

#include "iore_afio.h"
#include "iore_error.h"
#include "iore_ctx.h"
#include "iore_util.h"
#include "iore_workload.h"

/*** PROTOTYPES **************************************************************/

int
cstream_create (iore_file_t *, const iore_test_t *);
int
cstream_open (iore_file_t *, const iore_test_t *);
ssize_t
cstream_write_oset (iore_file_t, const void *, const off_t *,
		    const iore_test_t *);
ssize_t
cstream_read_oset (iore_file_t, void *, const off_t *, const iore_test_t *);
ssize_t
cstream_write_dset (iore_file_t, const void *, const iore_test_t *);
ssize_t
cstream_read_dset (iore_file_t, void *, const iore_test_t *);
int
cstream_close (iore_file_t *);
int
cstream_remove (iore_file_t);

/*** VARIABLES ***************************************************************/

const iore_afio_vtable_t afio_cstream =
  { cstream_create, cstream_open, cstream_write_oset, cstream_read_oset,
      cstream_write_dset, cstream_read_dset, cstream_close, cstream_remove };

/*** FUNCTIONS ***************************************************************/

int
cstream_create (iore_file_t *file, const iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  FILE *fptr;
  char *mode;

  if (test->file_mode == IORE_TEST_FMODE_NX1)
    {
      if (ctx.task_id == IORE_MASTER_TASK)
	{
	  mode = "w+";
	  fptr = fopen (file->name, mode);
	}
      MPI_Barrier (ctx.comm);
      if (ctx.task_id != IORE_MASTER_TASK)
	{
	  mode = "r+";
	  fptr = fopen (file->name, mode);
	}
    }
  else /* IORE_FM_NxN */
    {
      mode = "w+";
      fptr = fopen (file->name, mode);
    }

  if (fptr)
    file->hdle.fptr = fptr;
  else
    rerr = IORE_FAILURE;

  return rerr;
} /* cstream_create () */

int
cstream_open (iore_file_t *file, const iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  FILE *fptr;
  char *mode = "r";

  fptr = fopen (file->name, mode);
  if (fptr)
    file->hdle.fptr = fptr;
  else
    rerr = IORE_FAILURE;

  return rerr;
} /* cstream_open () */

ssize_t
cstream_write_oset (iore_file_t file, const void *buf, const off_t *offs,
		    const iore_test_t *test)
{
  assert(buf);
  assert(offs);
  assert(test);

  ssize_t nbytes = 0;

  FILE *fp = file.hdle.fptr;
  size_t file_size = test->wkld.u.oset._file_size;
  size_t remaining = test->wkld.u.oset.my_data_size;
  size_t max_req_size = test->wkld.u.oset.my_req_size;
  size_t req_size;
  size_t nmemb;
  size_t xferd;

  while (remaining && nbytes >= 0)
    {
      if (fseek (fp, *offs, SEEK_SET) < 0)
	nbytes = -1;
      else
	{
	  req_size = (file_size - *offs);
	  if (req_size > max_req_size)
	    req_size = max_req_size;
	  if (req_size > remaining)
	    req_size = remaining;
	  nmemb = fwrite (buf, sizeof(char), req_size, fp);
	  xferd = nmemb * sizeof(char);
	  if (xferd < req_size)
	    nbytes = -1;
	  else
	    {
	      nbytes += xferd;
	      if (test->write_flush_per_req)
		fflush (fp);
	    }
	}
      remaining -= req_size;
      offs++;
    }

  if (nbytes > 0 && test->write_flush)
    fflush (fp);

  return nbytes;
} /* cstream_write_oset () */

ssize_t
cstream_read_oset (iore_file_t file, void *buf, const off_t *offs,
		   const iore_test_t *test)
{
  ssize_t nbytes = 0;

  FILE *fp = file.hdle.fptr;
  size_t file_size = test->wkld.u.oset._file_size;
  size_t remaining = test->wkld.u.oset.my_data_size;
  size_t max_req_size = test->wkld.u.oset.my_req_size;
  size_t req_size;
  size_t nmemb;
  size_t xferd;

  while (remaining && nbytes >= 0)
    {
      if (fseek (fp, *offs, SEEK_SET) < 0)
	nbytes = -1;
      else
	{
	  req_size = (file_size - *offs);
	  if (req_size > max_req_size)
	    req_size = max_req_size;
	  if (req_size > remaining)
	    req_size = remaining;
	  nmemb = fread (buf, sizeof(char), req_size, fp);
	  xferd = nmemb * sizeof(char);
	  if (xferd < req_size)
	    nbytes = -1;
	  else
	    nbytes += xferd;
	}
      remaining -= req_size;
      offs++;
    }

  return nbytes;
} /* cstream_read_oset () */

ssize_t
cstream_write_dset (iore_file_t file, const void *buf, const iore_test_t *test)
{
  assert(buf);
  assert(test);

  ssize_t nbytes = 0;

  FILE *fp = file.hdle.fptr;
  size_t dset_size = test->wkld.u.dset.my_size;
  size_t req_size;
  size_t nmemb;
  size_t xferd;

  if (test->wkld.u.dset.type == IORE_WKLD_DSET_CARTESIAN)
    {
      req_size =
	  (test->wkld.u.dset._vars_size
	      * test->wkld.u.dset.u.cart.my_dim_sizes[test->wkld.u.dset.u.cart.num_dims
		  - 1]);
    }
  else /* unsupported dataset type */
    {
      return -1;
    }

  off_t *offs = dset_to_off (&test->wkld.u.dset, test->file_mode);
  if (!offs)
    return -1;
  /* pointer kept for freeing it later */
  off_t *first_off = offs;

  while (nbytes < (ssize_t) dset_size && nbytes >= 0)
    {
      if (fseek (fp, *offs, SEEK_SET) < 0)
	nbytes = -1;
      else
	{
	  nmemb = fwrite (buf + nbytes, sizeof(char), req_size, fp);
	  xferd = nmemb * sizeof(char);
	  if (xferd < req_size)
	    nbytes = -1;
	  else
	    {
	      nbytes += xferd;
	      if (test->write_flush_per_req)
		fflush (fp);
	    }
	}
      offs++;
    }

  if (nbytes > 0 && test->write_flush)
    fflush (fp);

  free (first_off);

  return nbytes;
} /* cstream_write_dset () */

ssize_t
cstream_read_dset (iore_file_t file, void *buf, const iore_test_t *test)
{
  assert(buf);
  assert(test);

  ssize_t nbytes = 0;

  FILE *fp = file.hdle.fptr;
  size_t dset_size = test->wkld.u.dset.my_size;
  size_t req_size;
  size_t nmemb;
  size_t xferd;

  if (test->wkld.u.dset.type == IORE_WKLD_DSET_CARTESIAN)
    {
      req_size =
	  (test->wkld.u.dset._vars_size
	      * test->wkld.u.dset.u.cart.my_dim_sizes[test->wkld.u.dset.u.cart.num_dims
		  - 1]);
    }
  else /* unsupported dataset type */
    {
      return -1;
    }

  off_t *offs = dset_to_off (&test->wkld.u.dset, test->file_mode);
  if (!offs)
    return -1;
  /* pointer kept for freeing it later */
  off_t *first_off = offs;

  while (nbytes < (ssize_t) dset_size && nbytes >= 0)
    {
      if (fseek (fp, *offs, SEEK_SET) < 0)
	nbytes = -1;
      else
	{
	  nmemb = fread (buf + nbytes, sizeof(char), req_size, fp);
	  xferd = nmemb * sizeof(char);
	  if (xferd < req_size)
	    nbytes = -1;
	  else
	    nbytes += xferd;
	}
      offs++;
    }

  free (first_off);

  return nbytes;
} /* cstream_read_dset () */

int
cstream_close (iore_file_t *file)
{
  assert(file);
  return fclose (file->hdle.fptr);
} /* cstream_close () */

int
cstream_remove (iore_file_t file)
{
  return unlink (file.name);
} /* cstream_remove () */
