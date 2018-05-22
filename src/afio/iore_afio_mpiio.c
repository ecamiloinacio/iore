/*
 * iore_afio_mpiio.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "iore_afio.h"
#include "iore_error.h"
#include "iore_util.h"
#include "iore_workload.h"
#include "iore_ctx.h"

/*** DEFINES *****************************************************************/

#define AFIO_MPIIO_FILE_VIEW_DATA_REP "native"

/*** PROTOTYPES **************************************************************/

int
mpiio_create (iore_file_t *, const iore_test_t *);
int
mpiio_open (iore_file_t *, const iore_test_t *);
ssize_t
mpiio_write_oset (iore_file_t, const void *, const off_t *, const iore_test_t *);
ssize_t
mpiio_read_oset (iore_file_t, void *, const off_t *, const iore_test_t *);
ssize_t
mpiio_write_dset (iore_file_t, const void *, const iore_test_t *);
ssize_t
mpiio_read_dset (iore_file_t, void *, const iore_test_t *);
int
mpiio_close (iore_file_t *);
int
mpiio_remove (iore_file_t);

static int
mpiio_set_file_view (MPI_File *, const iore_test_t *);
static int
mpiio_create_datatype (const iore_wkld_dset_t *, MPI_Datatype *);

/*** VARIABLES ***************************************************************/

const iore_afio_vtable_t afio_mpiio =
  { mpiio_create, mpiio_open, mpiio_write_oset, mpiio_read_oset,
      mpiio_write_dset, mpiio_read_dset, mpiio_close, mpiio_remove };

static const MPI_Datatype mpi_types[IORE_WKLD_DSET_DV_LENGTH] =
  { MPI_CHAR, MPI_INTEGER, MPI_LONG, MPI_LONG_LONG, MPI_FLOAT, MPI_DOUBLE };

/*** FUNCTIONS ***************************************************************/

int
mpiio_create (iore_file_t *file, const iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  MPI_File *fh = malloc (sizeof(MPI_File));
  assert(fh);
  int mode = MPI_MODE_CREATE | MPI_MODE_WRONLY;
  MPI_Comm comm =
      (test->file_mode == IORE_TEST_FMODE_NX1) ? ctx.comm : MPI_COMM_SELF;

  rerr = MPI_File_open (comm, file->name, mode, MPI_INFO_NULL, fh);
  if (rerr == MPI_SUCCESS)
    {
      file->hdle.fptr = fh;

      bool file_view = strtob (
	  dict_get (&test->afio.params, AFIO_PARAM_FILE_VIEW));
      if (test->wkld.type == IORE_WKLD_DATASET && file_view)
	{
	  rerr = (mpiio_set_file_view (fh, test) == MPI_SUCCESS) ?
	  IORE_SUCCESS :
								   IORE_FAILURE;
	}
    }
  else
    rerr = IORE_FAILURE;

  return rerr;
} /* mpiio_create () */

int
mpiio_open (iore_file_t *file, const iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  MPI_File *fh = malloc (sizeof(MPI_File));
  assert(fh);
  int mode = MPI_MODE_RDONLY;
  MPI_Comm comm =
      (test->file_mode == IORE_TEST_FMODE_NX1) ? ctx.comm : MPI_COMM_SELF;

  rerr = MPI_File_open (comm, file->name, mode, MPI_INFO_NULL, fh);
  if (rerr == MPI_SUCCESS)
    {
      file->hdle.fptr = fh;

      bool file_view = strtob (
	  dict_get (&test->afio.params, AFIO_PARAM_FILE_VIEW));
      if (test->wkld.type == IORE_WKLD_DATASET && file_view)
	{
	  rerr = (mpiio_set_file_view (fh, test) == MPI_SUCCESS) ?
	  IORE_SUCCESS :
								   IORE_FAILURE;
	}
    }
  else
    rerr = IORE_FAILURE;

  return rerr;
} /* mpiio_open () */

ssize_t
mpiio_write_oset (iore_file_t file, const void *buf, const off_t *offs,
		  const iore_test_t *test)
{
  assert(buf);
  assert(offs);
  assert(test);

  ssize_t nbytes = 0;

  MPI_File *fh = (MPI_File *) file.hdle.fptr;
  size_t file_size = test->wkld.u.oset._file_size;
  size_t remaining = test->wkld.u.oset.my_data_size;
  size_t max_req_size = test->wkld.u.oset.my_req_size;
  size_t req_size;
  int rerr = MPI_SUCCESS;

  int
  (*mpi_write) (MPI_File, const void *, int, MPI_Datatype, MPI_Status *);
  int
  (*mpi_write_at) (MPI_File, MPI_Offset, const void *, int, MPI_Datatype,
		   MPI_Status *);
  bool collective_io = strtob (
      dict_get (&test->afio.params, AFIO_PARAM_COLLECTIVE_IO));
  if (collective_io)
    {
      mpi_write = MPI_File_write_all;
      mpi_write_at = MPI_File_write_at_all;
    }
  else /* not collective_io */
    {
      mpi_write = MPI_File_write;
      mpi_write_at = MPI_File_write_at;
    }

  bool seek_rw_single_op = strtob (
      dict_get (&test->afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP));
  if (seek_rw_single_op)
    {
      while (remaining && nbytes >= 0)
	{
	  req_size = (file_size - *offs);
	  if (req_size > max_req_size)
	    req_size = max_req_size;
	  if (req_size > remaining)
	    req_size = remaining;
	  rerr = mpi_write_at (*fh, *offs, buf, req_size, MPI_BYTE,
	  MPI_STATUS_IGNORE);
	  if (rerr != MPI_SUCCESS)
	    nbytes = -1;
	  else
	    {
	      nbytes += req_size * sizeof(char);
	      if (test->write_flush_per_req)
		MPI_File_sync (*fh);
	    }
	  remaining -= req_size;
	  offs++;
	}
    }
  else /* not seek_rw_single_op */
    {
      while (remaining && nbytes >= 0)
	{
	  req_size = (file_size - *offs);
	  if (req_size > max_req_size)
	    req_size = max_req_size;
	  if (req_size > remaining)
	    req_size = remaining;
	  rerr = MPI_File_seek (*fh, *offs, MPI_SEEK_SET);
	  if (rerr != MPI_SUCCESS)
	    nbytes = -1;
	  else
	    {
	      rerr = mpi_write (*fh, buf, req_size, MPI_BYTE,
	      MPI_STATUS_IGNORE);
	      if (rerr != MPI_SUCCESS)
		nbytes = -1;
	      else
		{
		  nbytes += req_size * sizeof(char);
		  if (test->write_flush_per_req)
		    MPI_File_sync (*fh);
		}
	    }
	  remaining -= req_size;
	  offs++;
	}
    }

  if (nbytes > 0 && test->write_flush)
    MPI_File_sync (*fh);

  return nbytes;
} /* mpiio_write_oset () */

ssize_t
mpiio_read_oset (iore_file_t file, void *buf, const off_t *offs,
		 const iore_test_t *test)
{
  ssize_t nbytes = 0;

  MPI_File *fh = (MPI_File *) file.hdle.fptr;
  size_t file_size = test->wkld.u.oset._file_size;
  size_t remaining = test->wkld.u.oset.my_data_size;
  size_t max_req_size = test->wkld.u.oset.my_req_size;
  size_t req_size;
  int rerr = MPI_SUCCESS;

  int
  (*mpi_read) (MPI_File, void *, int, MPI_Datatype, MPI_Status *);
  int
  (*mpi_read_at) (MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
  bool collective_io = strtob (
      dict_get (&test->afio.params, AFIO_PARAM_COLLECTIVE_IO));
  if (collective_io)
    {
      mpi_read = MPI_File_read_all;
      mpi_read_at = MPI_File_read_at_all;
    }
  else /* not collective_io */
    {
      mpi_read = MPI_File_read;
      mpi_read_at = MPI_File_read_at;
    }

  bool seek_rw_single_op = strtob (
      dict_get (&test->afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP));
  if (seek_rw_single_op)
    {
      while (remaining && nbytes >= 0)
	{
	  req_size = (file_size - *offs);
	  if (req_size > max_req_size)
	    req_size = max_req_size;
	  if (req_size > remaining)
	    req_size = remaining;
	  rerr = mpi_read_at (*fh, *offs, buf, req_size, MPI_BYTE,
	  MPI_STATUS_IGNORE);
	  if (rerr != MPI_SUCCESS)
	    nbytes = -1;
	  else
	    nbytes += req_size * sizeof(char);

	  remaining -= req_size;
	  offs++;
	}
    }
  else /* not seek_rw_single_op */
    {
      while (remaining && nbytes >= 0)
	{
	  req_size = (file_size - *offs);
	  if (req_size > max_req_size)
	    req_size = max_req_size;
	  if (req_size > remaining)
	    req_size = remaining;
	  rerr = MPI_File_seek (*fh, *offs, MPI_SEEK_SET);
	  if (rerr != MPI_SUCCESS)
	    nbytes = -1;
	  else
	    {
	      rerr = mpi_read (*fh, buf, req_size, MPI_BYTE,
	      MPI_STATUS_IGNORE);
	      if (rerr != MPI_SUCCESS)
		nbytes = -1;
	      else
		nbytes += req_size * sizeof(char);
	    }
	  remaining -= req_size;
	  offs++;
	}
    }

  return nbytes;
} /* mpiio_read_oset () */

ssize_t
mpiio_write_dset (iore_file_t file, const void *buf, const iore_test_t *test)
{
  assert(buf);
  assert(test);

  ssize_t nbytes = 0;

  MPI_File *fh = (MPI_File *) file.hdle.fptr;
  size_t dset_size = test->wkld.u.dset.my_size;
  size_t req_size;
  int rerr = 0;

  int
  (*mpi_write) (MPI_File, const void *, int, MPI_Datatype, MPI_Status *);
  int
  (*mpi_write_at) (MPI_File, MPI_Offset, const void *, int, MPI_Datatype,
		   MPI_Status *);
  bool collective_io = strtob (
      dict_get (&test->afio.params, AFIO_PARAM_COLLECTIVE_IO));
  if (collective_io)
    {
      mpi_write = MPI_File_write_all;
      mpi_write_at = MPI_File_write_at_all;
    }
  else /* not collective_io */
    {
      mpi_write = MPI_File_write;
      mpi_write_at = MPI_File_write_at;
    }

  bool file_view = strtob (dict_get (&test->afio.params, AFIO_PARAM_FILE_VIEW));
  if (file_view)
    {
      rerr = mpi_write (*fh, buf, dset_size, MPI_CHAR, MPI_STATUS_IGNORE);
      if (rerr != MPI_SUCCESS)
	nbytes = -1;
      else
	{
	  nbytes = dset_size;
	}
    }
  else /* not file_view */
    {
      off_t *offs = dset_to_off (&test->wkld.u.dset, test->file_mode);
      if (!offs)
	return -1;
      /* pointer kept for freeing it later */
      off_t *first_off = offs;

      if (test->wkld.u.dset.type == IORE_WKLD_DSET_CARTESIAN)
	{
	  req_size = (test->wkld.u.dset._vars_size
	      * test->wkld.u.dset.u.cart.my_dim_sizes[0]);
	}
      else /* unsupported dataset type */
	{
	  return -1;
	}

      bool seek_rw_single_op = strtob (
	  dict_get (&test->afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP));
      if (seek_rw_single_op)
	{
	  while (nbytes < (ssize_t) dset_size && nbytes >= 0)
	    {
	      rerr = mpi_write_at (*fh, *offs, buf + nbytes, req_size, MPI_BYTE,
	      MPI_STATUS_IGNORE);
	      if (rerr != MPI_SUCCESS)
		nbytes = -1;
	      else
		{
		  nbytes += req_size * sizeof(char);
		  if (test->write_flush_per_req)
		    MPI_File_sync (*fh);
		}
	      offs++;
	    }
	}
      else /* not seek_rw_single_op */
	{
	  while (nbytes < (ssize_t) dset_size && nbytes >= 0)
	    {
	      rerr = MPI_File_seek (*fh, *offs, MPI_SEEK_SET);
	      if (rerr != MPI_SUCCESS)
		nbytes = -1;
	      else
		{
		  rerr = mpi_write (*fh, buf, req_size, MPI_BYTE,
		  MPI_STATUS_IGNORE);
		  if (rerr != MPI_SUCCESS)
		    nbytes = -1;
		  else
		    {
		      nbytes += req_size * sizeof(char);
		      if (test->write_flush_per_req)
			MPI_File_sync (*fh);
		    }
		}
	      offs++;
	    }
	}

      free (first_off);
    }

  if (nbytes > 0 && test->write_flush)
    MPI_File_sync (*fh);

  return nbytes;
} /* mpiio_write_dset () */

ssize_t
mpiio_read_dset (iore_file_t file, void *buf, const iore_test_t *test)
{
  assert(buf);
  assert(test);

  ssize_t nbytes = 0;

  MPI_File *fh = (MPI_File *) file.hdle.fptr;
  size_t dset_size = test->wkld.u.dset.my_size;
  size_t req_size;
  int rerr = 0;

  int
  (*mpi_read) (MPI_File, void *, int, MPI_Datatype, MPI_Status *);
  int
  (*mpi_read_at) (MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
  bool collective_io = strtob (
      dict_get (&test->afio.params, AFIO_PARAM_COLLECTIVE_IO));
  if (collective_io)
    {
      mpi_read = MPI_File_read_all;
      mpi_read_at = MPI_File_read_at_all;
    }
  else /* not collective_io */
    {
      mpi_read = MPI_File_read;
      mpi_read_at = MPI_File_read_at;
    }

  bool file_view = strtob (dict_get (&test->afio.params, AFIO_PARAM_FILE_VIEW));
  if (file_view)
    {
      rerr = mpi_read (*fh, buf, dset_size, MPI_CHAR, MPI_STATUS_IGNORE);
      if (rerr != MPI_SUCCESS)
	nbytes = -1;
      else
	nbytes = dset_size;
    }
  else /* not file_view */
    {
      off_t *offs = dset_to_off (&test->wkld.u.dset, test->file_mode);
      if (!offs)
	return -1;
      /* pointer kept for freeing it later */
      off_t *first_off = offs;

      if (test->wkld.u.dset.type == IORE_WKLD_DSET_CARTESIAN)
	{
	  req_size = (test->wkld.u.dset._vars_size
	      * test->wkld.u.dset.u.cart.my_dim_sizes[0]);
	}
      else /* unsupported dataset type */
	{
	  return -1;
	}

      bool seek_rw_single_op = strtob (
	  dict_get (&test->afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP));
      if (seek_rw_single_op)
	{
	  while (nbytes < (ssize_t) dset_size && nbytes >= 0)
	    {
	      rerr = mpi_read_at (*fh, *offs, buf + nbytes, req_size, MPI_BYTE,
	      MPI_STATUS_IGNORE);
	      if (rerr != MPI_SUCCESS)
		nbytes = -1;
	      else
		nbytes += req_size * sizeof(char);

	      offs++;
	    }
	}
      else /* not seek_rw_single_op */
	{
	  while (nbytes < (ssize_t) dset_size && nbytes >= 0)
	    {
	      rerr = MPI_File_seek (*fh, *offs, MPI_SEEK_SET);
	      if (rerr != MPI_SUCCESS)
		nbytes = -1;
	      else
		{
		  rerr = mpi_read (*fh, buf, req_size, MPI_BYTE,
		  MPI_STATUS_IGNORE);
		  if (rerr != MPI_SUCCESS)
		    nbytes = -1;
		  else
		    nbytes += req_size * sizeof(char);
		}
	      offs++;
	    }
	}

      free (first_off);
    }

  return nbytes;
} /* mpiio_read_dset () */

int
mpiio_close (iore_file_t *file)
{
  assert(file);

  int rerr =
      (MPI_File_close ((MPI_File *) file->hdle.fptr) == MPI_SUCCESS) ?
	  IORE_SUCCESS : IORE_FAILURE;
  return rerr;
} /* mpiio_close () */

int
mpiio_remove (iore_file_t file)
{
  int rerr =
      (MPI_File_delete (file.name, MPI_INFO_NULL) == MPI_SUCCESS) ?
	  IORE_SUCCESS :
	  IORE_FAILURE;
  return rerr;
} /* mpiio_remove () */

static int
mpiio_set_file_view (MPI_File *fh, const iore_test_t *test)
{
  int rerr = MPI_SUCCESS;

  const iore_wkld_dset_t *dset = &test->wkld.u.dset;
  MPI_Datatype datatype;
  rerr = mpiio_create_datatype (dset, &datatype);
  if (rerr == MPI_SUCCESS)
    {
      if (dset->type == IORE_WKLD_DSET_CARTESIAN)
	{
	  const iore_wkld_dset_cart_t *cart = &dset->u.cart;
	  MPI_Datatype file_type;
	  rerr = MPI_Type_create_subarray (cart->num_dims,
					   (const int *) cart->g_dim_sizes,
					   (const int *) cart->my_dim_sizes,
					   (const int *) cart->my_start_coord,
					   MPI_ORDER_C,
					   datatype, &file_type);
	  if (rerr == MPI_SUCCESS)
	    {
	      rerr = MPI_Type_commit (&file_type);
	      if (rerr == MPI_SUCCESS)
		{
		  rerr = MPI_File_set_view (*fh, 0, datatype, file_type,
		  AFIO_MPIIO_FILE_VIEW_DATA_REP,
					    MPI_INFO_NULL);
		}
	    }
	}
      else /* unsupported dataset type */
	{
	  rerr = IORE_FAILURE;
	}
    }

  return rerr;
} /* mpiio_set_file_view () */

static int
mpiio_create_datatype (const iore_wkld_dset_t *dset, MPI_Datatype *datatype)
{
  assert(dset);
  assert(datatype);

  int rerr = MPI_SUCCESS;

  unsigned int num_vars = dset->num_vars;
  if (num_vars == 1)
    datatype[0] = mpi_types[dset->var_types[0]];
  else /* num_vars > 1 */
    {
      int *blk_len = malloc (num_vars * sizeof(int));
      assert(blk_len);
      MPI_Aint *displ = malloc (num_vars * sizeof(MPI_Aint));
      assert(displ);
      MPI_Datatype *var_types = malloc (num_vars * sizeof(MPI_Datatype));
      assert(var_types);

      int var_size;
      unsigned int size = 0;

      unsigned int i;
      for (i = 0; i < num_vars && rerr == MPI_SUCCESS; i++)
	{
	  blk_len[i] = 1;
	  displ[i] = size;
	  var_types[i] = mpi_types[dset->var_types[i]];

	  rerr = MPI_Type_size (var_types[i], &var_size);
	  size += var_size;
	}

      if (rerr == MPI_SUCCESS)
	{
	  rerr = MPI_Type_create_struct (num_vars, blk_len, displ, var_types,
					 datatype);
	  if (rerr == MPI_SUCCESS)
	    rerr = MPI_Type_commit (datatype);
	}

      free (blk_len);
      free (displ);
      free (var_types);
    }

  return rerr;
} /* mpiio_create_datatype () */
