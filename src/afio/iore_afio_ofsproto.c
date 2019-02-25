/*
 * iore_afio_ofsproto.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <fcntl.h>
#include <mpi.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
/* TODO: check if pvfs2.h and pvfs2-usrint.h is available */
#include <orange.h>
#include <pvfs2-hint.h>

#include "iore_afio.h"
#include "iore_error.h"
#include "iore_util.h"
#include "iore_workload.h"
#include "iore_ctx.h"

/*** PROTOTYPES **************************************************************/

int
ofsproto_create (iore_file_t *, const iore_test_t *);
int
ofsproto_open (iore_file_t *, const iore_test_t *);
ssize_t
ofsproto_write_oset (iore_file_t, const void *, const off_t *,
		     const iore_test_t *);
ssize_t
ofsproto_read_oset (iore_file_t, void *, const off_t *, const iore_test_t *);
ssize_t
ofsproto_write_dset (iore_file_t, const void *, const iore_test_t *);
ssize_t
ofsproto_read_dset (iore_file_t, void *, const iore_test_t *);
int
ofsproto_close (iore_file_t *);
int
ofsproto_remove (iore_file_t);

static int
ofsproto_create_nxn (iore_file_t *file, const iore_test_t *test, int oflag,
		     mode_t mode);

/*** VARIABLES ***************************************************************/

const iore_afio_vtable_t afio_ofsproto =
  { ofsproto_create, ofsproto_open, ofsproto_write_oset, ofsproto_read_oset,
      ofsproto_write_dset, ofsproto_read_dset, ofsproto_close, ofsproto_remove };

/*** FUNCTIONS ***************************************************************/

int
ofsproto_create (iore_file_t *file, const iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  int fd;
  int oflag = O_CREAT | O_WRONLY;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  if (test->file_mode == IORE_TEST_FMODE_NXN)
    {
      /**
       * Create the file with the OrangeFS API, closes it, then open it again
       * with the POSIX syscall. This is needed in order to other functions to
       * be able to use POSIX syscalls as well.
       */
      fd = ofsproto_create_nxn (file, test, oflag, mode);
      pvfs_close (fd);
    }

  fd = open (file->name, oflag, mode);

  if (fd >= 0)
    file->hdle.fint = fd;
  else
    rerr = fd;

  return rerr;
} /* ofsproto_create () */

int
ofsproto_open (iore_file_t *file, const iore_test_t *test)
{
  int rerr = IORE_SUCCESS;

  int fd;
  int oflag = O_RDONLY;

  fd = open (file->name, oflag);
  if (fd >= 0)
    file->hdle.fint = fd;
  else
    rerr = fd;

  return rerr;
} /* ofsproto_open () */

ssize_t
ofsproto_write_oset (iore_file_t file, const void *buf, const off_t *offs,
		     const iore_test_t *test)
{
  assert(buf);
  assert(offs);
  assert(test);

  ssize_t nbytes = 0;

  int fd = file.hdle.fint;
  size_t file_size = test->wkld.u.oset._file_size;
  size_t remaining = test->wkld.u.oset.my_data_size;
  size_t max_req_size = test->wkld.u.oset.my_req_size;
  size_t req_size;
  ssize_t xferd;

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
	  xferd = pwrite (fd, buf, req_size, *offs);
	  if (xferd < (ssize_t) req_size)
	    nbytes = -1;
	  else
	    {
	      nbytes += xferd;
	      if (test->write_flush_per_req)
		fsync (fd);
	    }
	  remaining -= req_size;
	  offs++;
	}
    }
  else /* not seek_rw_single_op */
    {
      while (remaining && nbytes >= 0)
	{
	  if (lseek (fd, *offs, SEEK_SET) < 0)
	    nbytes = -1;
	  else
	    {
	      req_size = (file_size - *offs);
	      if (req_size > max_req_size)
		req_size = max_req_size;
	      if (req_size > remaining)
		req_size = remaining;
	      xferd = write (fd, buf, req_size);
	      if (xferd < (ssize_t) req_size)
		nbytes = -1;
	      else
		{
		  nbytes += xferd;
		  if (test->write_flush_per_req)
		    fsync (fd);
		}
	    }
	  remaining -= req_size;
	  offs++;
	}
    }

  if (nbytes > 0 && test->write_flush)
    fsync (fd);

  return nbytes;
} /* ofsproto_write_oset () */

ssize_t
ofsproto_read_oset (iore_file_t file, void *buf, const off_t *offs,
		    const iore_test_t *test)
{
  assert(buf);
  assert(offs);
  assert(test);

  ssize_t nbytes = 0;

  int fd = file.hdle.fint;
  size_t file_size = test->wkld.u.oset._file_size;
  size_t remaining = test->wkld.u.oset.my_data_size;
  size_t max_req_size = test->wkld.u.oset.my_req_size;
  size_t req_size;
  ssize_t xferd;

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
	  xferd = pread (fd, buf, req_size, *offs);
	  if (xferd < (ssize_t) req_size)
	    nbytes = -1;
	  else
	    nbytes += xferd;

	  remaining -= req_size;
	  offs++;
	}
    }
  else /* not seek_rw_single_op */
    {
      while (remaining && nbytes >= 0)
	{
	  if (lseek (fd, *offs, SEEK_SET) < 0)
	    nbytes = -1;
	  else
	    {
	      req_size = (file_size - *offs);
	      if (req_size > max_req_size)
		req_size = max_req_size;
	      if (req_size > remaining)
		req_size = remaining;
	      xferd = read (fd, buf, req_size);
	      if (xferd < (ssize_t) req_size)
		nbytes = -1;
	      else
		nbytes += xferd;
	    }

	  remaining -= req_size;
	  offs++;
	}
    }

  return nbytes;
} /* ofsproto_read_oset () */

ssize_t
ofsproto_write_dset (iore_file_t file, const void *buf, const iore_test_t *test)
{
  assert(buf);
  assert(test);

  ssize_t nbytes = 0;

  int fd = file.hdle.fint;
  size_t dset_size = test->wkld.u.dset.my_size;
  size_t req_size;
  ssize_t xferd;

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

  bool seek_rw_single_op = strtob (
      dict_get (&test->afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP));
  if (seek_rw_single_op)
    {
      while (nbytes < (ssize_t) dset_size && nbytes >= 0)
	{
	  xferd = pwrite (fd, buf + nbytes, req_size, *offs);
	  if (xferd < (ssize_t) req_size)
	    nbytes = -1;
	  else
	    {
	      nbytes += xferd;
	      if (test->write_flush_per_req)
		fsync (fd);
	    }
	  offs++;
	}
    }
  else /* not seek_rw_single_op */
    {
      while (nbytes < (ssize_t) dset_size && nbytes >= 0)
	{
	  if (lseek (fd, *offs, SEEK_SET) < 0)
	    nbytes = -1;
	  else
	    {
	      xferd = write (fd, buf + nbytes, req_size);
	      if (xferd < (ssize_t) req_size)
		nbytes = -1;
	      else
		{
		  nbytes += xferd;
		  if (test->write_flush_per_req)
		    fsync (fd);
		}
	    }
	  offs++;
	}
    }

  if (nbytes > 0 && test->write_flush)
    fsync (fd);

  free (first_off);

  return nbytes;
} /* ofsproto_write_dset () */

ssize_t
ofsproto_read_dset (iore_file_t file, void *buf, const iore_test_t *test)
{
  assert(buf);
  assert(test);

  ssize_t nbytes = 0;

  int fd = file.hdle.fint;
  size_t dset_size = test->wkld.u.dset.my_size;
  size_t req_size;
  ssize_t xferd;

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

  bool seek_rw_single_op = strtob (
      dict_get (&test->afio.params, AFIO_PARAM_SEEK_RW_SINGLE_OP));
  if (seek_rw_single_op)
    {
      while (nbytes < (ssize_t) dset_size && nbytes >= 0)
	{
	  xferd = pread (fd, buf + nbytes, req_size, *offs);
	  if (xferd < (ssize_t) req_size)
	    nbytes = -1;
	  else
	    nbytes += xferd;

	  offs++;
	}
    }
  else /* not seek_rw_single_op */
    {
      while (nbytes < (ssize_t) dset_size && nbytes >= 0)
	{
	  if (lseek (fd, *offs, SEEK_SET) < 0)
	    nbytes = -1;
	  else
	    {
	      xferd = read (fd, buf + nbytes, req_size);
	      if (xferd < (ssize_t) req_size)
		nbytes = -1;
	      else
		nbytes += xferd;
	    }
	  offs++;
	}
    }

  free (first_off);

  return nbytes;
} /* ofsproto_read_dset () */

int
ofsproto_close (iore_file_t *file)
{
  assert(file);

  return close (file->hdle.fint);
} /* ofsproto_close () */

int
ofsproto_remove (iore_file_t file)
{
  return unlink (file.name);
} /* ofsproto_remove () */

/*** STATIC FUNCTIONS *********************************************************/

static int
ofsproto_create_nxn (iore_file_t *file, const iore_test_t *test, int oflag,
		     mode_t mode)
{
  int nosts, first_ost, stride, fd;
  int stripe_off;
  PVFS_hint hint = NULL;
  int layout = PVFS_SYS_LAYOUT_LIST;
  int num_dfiles;
  char *serverlist;
  unsigned int *u_serverlist;

  num_dfiles = atoi (dict_get (&test->afio.params, AFIO_PARAM_STRIPE_WIDTH));
  /* TODO: use OrangeFS API to get the number of data servers (?) */
  nosts = atoi (getenv ("OFS_NUM_DSERV"));
  if (ctx.task_id == IORE_MASTER_TASK)
    {
      /* TODO: replace RNG */
      first_ost = rand () % nosts;
    }
  MPI_Bcast (&first_ost, 1, MPI_INT, 0, ctx.comm);
  stride = nosts / test->wkld.num_tasks;
  if (stride < 1)
    stride = 1;
  stripe_off = (first_ost + ctx.task_id * stride) % nosts;

  u_serverlist = calloc (num_dfiles + 1, sizeof(unsigned int));
  assert(u_serverlist);
  u_serverlist[0] = num_dfiles;
  u_serverlist[1] = stripe_off;
  /* TODO: implement loop for num_dfiles > 1 */
  serverlist = coallesce_uint (u_serverlist, num_dfiles + 1, ":");
  free (u_serverlist);

  PVFS_hint_add (&hint, PVFS_HINT_LAYOUT_NAME, sizeof(layout), &layout);
  PVFS_hint_add (&hint, PVFS_HINT_DFILE_COUNT_NAME, sizeof(num_dfiles),
		 &num_dfiles);
  PVFS_hint_add (&hint, PVFS_HINT_SERVERLIST_NAME, strlen (serverlist),
		 serverlist);

  oflag |= O_HINTS;

  fd = pvfs_open (file->name, oflag, mode, hint);

  return fd;
} /* ofsproto_create_nxn () */
