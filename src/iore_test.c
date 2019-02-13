/*
 * iore_test.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <assert.h>
#include <libgen.h>
#include <sys/stat.h>
#include <errno.h>

/* TODO: do not merge into MASTER branch */
#include <lustre/lustreapi.h>

#include "iore_test_types.h"
#include "iore_workload.h"
#include "iore_afio.h"
#include "iore_afsb.h"
#include "iore_error.h"
#include "iore_trec.h"
#include "iore_ctx.h"
#include "iore_util.h"
#include "iore_prng.h"

/*** DEFINES *****************************************************************/

#define TEST_FILE_NAME_DFLT "./testfile.iore"

/*** PROTOTYPES **************************************************************/

static int
test_oset_exec (iore_test_t *, unsigned int, unsigned int, unsigned int);
static int
test_oset_type_prep (iore_test_t *, enum iore_test_type, unsigned int,
		     unsigned int, unsigned int, iore_file_t *, char **,
		     off_t **);
static int
test_oset_type_post (iore_test_t *, iore_file_t *, char *, off_t *);
static int
test_oset_write_exec (iore_test_t *, iore_file_t, const char *, const off_t *);
static int
test_oset_read_exec (iore_test_t *, iore_file_t, char *, const off_t *);

static int
test_dset_exec (iore_test_t *, unsigned int, unsigned int, unsigned int);
static int
test_dset_type_prep (iore_test_t *, enum iore_test_type, unsigned int,
		     unsigned int, unsigned int, iore_file_t *, char **);
static int
test_dset_type_post (iore_test_t *, iore_file_t *, char *);
static int
test_dset_cart_type_prep (iore_wkld_dset_t *, int);
static int
test_dset_cart_type_post (iore_wkld_dset_cart_t *);
static int
test_dset_write_exec (iore_test_t *, iore_file_t, const char *);
static int
test_dset_read_exec (iore_test_t *, iore_file_t, char *);

static int
test_file_prep (iore_test_t *, unsigned int, unsigned int, unsigned int, int,
		iore_file_t *);
static int
test_file_post (iore_test_t *, iore_file_t *);
static int
test_buf_prep (size_t, enum iore_test_type, char **);
static int
test_offs_prep (iore_test_t *, int, off_t **);
static int
test_offs_nxn_prep (iore_wkld_t *, int, off_t *);
static int
test_offs_nx1_seq_prep (iore_wkld_t *, int, int, off_t *);
static int
test_offs_nx1_rnd_prep (iore_wkld_t *, int, int, off_t *);
static int
test_sizes_prep (iore_prng_dist_t *, unsigned int, size_t **);

static void
test_show_summary (iore_trec_test_t *);

/*** FUNCTIONS ***************************************************************/

iore_test_t *
test_init (iore_test_t *this)
{
  assert(this);

  this->type.write = true;
  this->type.read = true;
  this->write_flush = false;
  this->write_flush_per_req = false;
  this->read_reorder_offset = 0;
  this->intra_test_barrier = false;
  this->inter_test_delay_secs = 0;
  this->file_mode = IORE_TEST_FMODE_NX1;
  this->file_name = strdup (TEST_FILE_NAME_DFLT);
  assert(this->file_name);
  this->file_name_append_sequence_num = false;
  this->file_name_append_task_id = false;
  this->file_dir_per_task = false;
  this->file_keep = false;

  wkld_init (&this->wkld);
  afio_init (&this->afio);
  this->afsb = NULL;

  return this;
} /* test_init () */

void
test_free (iore_test_t *this)
{
  if (this)
    {
      if (this->file_name)
	{
	  free (this->file_name);
	  this->file_name = NULL;
	}

      wkld_free (&this->wkld);
      afio_free (&this->afio);
      afsb_free (this->afsb);
    }
} /* test_free () */

int
test_exec (iore_test_t *this, unsigned int rpl_id, unsigned int run_id,
	   unsigned int rpt_id)
{
  assert(this);

  int rerr = IORE_SUCCESS;

  if (this->wkld.type == IORE_WKLD_OFFSET)
    rerr = test_oset_exec (this, rpl_id, run_id, rpt_id);
  else if (this->wkld.type == IORE_WKLD_DATASET)
    rerr = test_dset_exec (this, rpl_id, run_id, rpt_id);
  else
    {
      iore_error("Unsupported workload type");
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* test_exec () */

static int
test_oset_exec (iore_test_t *test, unsigned int rpl_id, unsigned int run_id,
		unsigned int rpt_id)
{
  iore_file_t file;
  char *buf;
  off_t *offs;
  iore_trec_test_t *trec_test;

  if (test->type.write)
    {
      if (test_oset_type_prep (test, IORE_TEST_TYPE_WRITE, rpl_id, run_id,
			       rpt_id, &file, &buf, &offs))
	return IORE_FAILURE;
      if (test_oset_write_exec (test, file, buf, offs))
	return IORE_FAILURE;
      else
	{
	  trec_test = trec_test_commit ();
	  test_show_summary (trec_test);
	}
      if (test_oset_type_post (test, &file, buf, offs))
	return IORE_FAILURE;

      if (test->type.read)
	sleep (test->inter_test_delay_secs);
    }

  if (test->type.read)
    {
      if (test_oset_type_prep (test, IORE_TEST_TYPE_READ, rpl_id, run_id,
			       rpt_id, &file, &buf, &offs))
	return IORE_FAILURE;
      if (test_oset_read_exec (test, file, buf, offs))
	return IORE_FAILURE;
      else
	{
	  trec_test = trec_test_commit ();
	  test_show_summary (trec_test);
	}
      if (test_oset_type_post (test, &file, buf, offs))
	return IORE_FAILURE;
    }

  return IORE_SUCCESS;
} /* test_oset_exec () */

static int
test_oset_type_prep (iore_test_t *test, enum iore_test_type type,
		     unsigned int rpl_id, unsigned int run_id,
		     unsigned int rpt_id, iore_file_t *file, char **buf,
		     off_t **offs)
{
  /* in read tests, tasks can perform as a different 'id' */
  int task_id =
      (type == IORE_TEST_TYPE_WRITE) ?
	  (unsigned int) ctx.task_id :
	  (ctx.task_id + test->read_reorder_offset) % test->wkld.num_tasks;

  /* prepares the file */
  if (test_file_prep (test, rpl_id, run_id, rpt_id, task_id, file))
    return IORE_FAILURE;

  // TODO: invoke AFSB

  /* checks/generates data sizes */
  if (test->wkld.u.oset._data_sizes_len == 0)
    {
      if (test->wkld.u.oset.data_size_distrib != NULL)
	{
	  if (test_sizes_prep (test->wkld.u.oset.data_size_distrib,
			       test->wkld.num_tasks,
			       &test->wkld.u.oset.data_sizes))
	    return IORE_FAILURE;
	  else
	    test->wkld.u.oset._data_sizes_len = test->wkld.num_tasks;
	}
      else
	return IORE_FAILURE;
    }

  /* checks/generates request sizes */
  if (test->wkld.u.oset._req_sizes_len == 0)
    {
      if (test->wkld.u.oset.req_size_distrib != NULL)
	{
	  if (test_sizes_prep (test->wkld.u.oset.req_size_distrib,
			       test->wkld.num_tasks,
			       &test->wkld.u.oset.req_sizes))
	    return IORE_FAILURE;
	  else
	    test->wkld.u.oset._req_sizes_len = test->wkld.num_tasks;
	}
      else
	return IORE_FAILURE;
    }

  /* sets task-specific parameters */
  test->wkld.u.oset.my_data_size = test->wkld.u.oset.data_sizes[task_id
      % test->wkld.u.oset._data_sizes_len];
  test->wkld.u.oset.my_req_size = test->wkld.u.oset.req_sizes[task_id
      % test->wkld.u.oset._req_sizes_len];
  if (test->file_mode == IORE_TEST_FMODE_NXN)
    test->wkld.u.oset._file_size = test->wkld.u.oset.my_data_size;
  else if (test->file_mode == IORE_TEST_FMODE_NX1)
    {
      size_t file_size = 0;
      size_t *data_sizes = test->wkld.u.oset.data_sizes;
      unsigned int num_tasks = test->wkld.num_tasks;
      unsigned int i;
      for (i = 0; i < num_tasks; i++)
	file_size += data_sizes[i % test->wkld.u.oset._data_sizes_len];
      test->wkld.u.oset._file_size = file_size;
    }
  else
    test->wkld.u.oset._file_size = 0;

  /* prepares the buffer */
  if (test_buf_prep (test->wkld.u.oset.my_req_size, type, buf))
    return IORE_FAILURE;

  /* prepares the list of offsets */
  if (test_offs_prep (test, task_id, offs))
    return IORE_FAILURE;

  return IORE_SUCCESS;
} /* test_oset_type_prep () */

static int
test_oset_type_post (iore_test_t *test, iore_file_t *file, char *buf,
		     off_t *offs)
{
  int rerr = IORE_SUCCESS;

  rerr = test_file_post (test, file);
  free (buf);
  free (offs);

  return rerr;
} /* test_oset_type_post () */

static int
test_oset_write_exec (iore_test_t *test, iore_file_t file, const char *buf,
		      const off_t *offs)
{
  int rerr = IORE_SUCCESS;

  const iore_afio_vtable_t *afio = afio_pool[test->afio.type];
  bool intra_test_barrier = test->intra_test_barrier;
  ssize_t nbytes = 0;

  /**
   * Temporary code for load balancing analysis on Lustre OSTs.
   *
   * TODO: do not merge into MASTER branch.
   */
  struct lov_user_md *lum = NULL;
  int *l_load_counter, *g_load_counter;
  int i;
  int nosts;
  char mntdir[1024], fsname[1024];

  llapi_search_mounts (file.name, 0, mntdir, fsname);
  llapi_get_obd_count (mntdir, &nosts, 0);

  l_load_counter = calloc (nosts, sizeof(int));
  g_load_counter = calloc (nosts, sizeof(int));

  lum = malloc (sizeof(struct lov_user_md));
  /**
   *  END of temporary code
   *
   *  TODO: do not merge into MASTER branch.
   */

  MPI_Barrier (ctx.comm);

  /* creates */
  trec_test_start (IORE_TEST_TYPE_WRITE, IORE_TREC_ACTION_CREATE_OPEN);
  rerr = afio->create (&file, test);
  trec_test_stop (0);
  if (rerr)
    iore_fatalf("Failed creating file '%s'", file.name);
  else
    {
      if (intra_test_barrier)
	MPI_Barrier (ctx.comm);

      /* writes */
      trec_test_start (IORE_TEST_TYPE_WRITE, IORE_TREC_ACTION_WRITE_READ);
      nbytes = afio->write_oset (file, buf, offs, test);
      trec_test_stop (nbytes);
      rerr = (nbytes < 0)
	  || (((unsigned int) nbytes) != test->wkld.u.oset.my_data_size);
      if (rerr)
	iore_fatalf("Failed writing to file '%s'", file.name);
      else
	{
	  if (intra_test_barrier)
	    MPI_Barrier (ctx.comm);

	  /* closes */
	  trec_test_start (IORE_TEST_TYPE_WRITE, IORE_TREC_ACTION_CLOSE);
	  rerr = afio->close (&file);
	  trec_test_stop (0);
	  if (rerr)
	    iore_fatalf("Failed closing file '%s'", file.name);
	  else
	    {
	      /**
	       * Temporary code for load balancing analysis on Lustre OSTs.
	       *
	       * TODO: do not merge into MASTER branch.
	       */
	      llapi_file_get_stripe (file.name, lum);
	      for (i = 0; i < lum->lmm_stripe_count; i++)
		l_load_counter[lum->lmm_objects[i].l_ost_idx]++;
	      /**
	       *  END of temporary code
	       *
	       *  TODO: do not merge into MASTER branch.
	       */

	      if (!test->type.read && !test->file_keep)
		{
		  MPI_Barrier (ctx.comm);

		  trec_test_start (IORE_TEST_TYPE_WRITE,
				   IORE_TREC_ACTION_REMOVE);
		  /* removes */
		  if ((test->file_mode == IORE_TEST_FMODE_NXN)
		      || ((test->file_mode == IORE_TEST_FMODE_NX1)
			  && (ctx.task_id == IORE_MASTER_TASK)))
		    {

		      rerr = afio->remove (file);
		    }
		  trec_test_stop (0);
		}
	    } /* end of remove block */
	} /* end of close block */
    } /* end of write block */

  MPI_Barrier (ctx.comm);

  /**
   * Temporary code for load balancing analysis on Lustre OSTs.
   *
   * TODO: do not merge into MASTER branch.
   */
  MPI_Reduce (l_load_counter, g_load_counter, nosts, MPI_INT, MPI_SUM,
	      (IORE_MASTER_TASK), ctx.comm);
  if (ctx.task_id == IORE_MASTER_TASK)
    {
      puts("ost_idx,stripe_count");
      for (i = 0; i < nosts; i++)
	printf ("%d,%d\n", i, g_load_counter[i]);
    }

  free (lum);
  free (l_load_counter);
  free (g_load_counter);
  /**
   *  END of temporary code
   *
   *  TODO: do not merge into MASTER branch.
   */

  return rerr;
} /* test_oset_write_exec () */

static int
test_oset_read_exec (iore_test_t *test, iore_file_t file, char *buf,
		     const off_t *offs)
{
  int rerr = IORE_SUCCESS;

  const iore_afio_vtable_t *afio = afio_pool[test->afio.type];
  bool intra_test_barrier = test->intra_test_barrier;
  ssize_t nbytes = 0;

  MPI_Barrier (ctx.comm);

  /* opens */
  trec_test_start (IORE_TEST_TYPE_READ, IORE_TREC_ACTION_CREATE_OPEN);
  rerr = afio->open (&file, test);
  trec_test_stop (0);
  if (rerr)
    iore_fatalf("Failed opening file '%s'", file.name);
  else
    {
      if (intra_test_barrier)
	MPI_Barrier (ctx.comm);

      /* reads */
      trec_test_start (IORE_TEST_TYPE_READ, IORE_TREC_ACTION_WRITE_READ);
      nbytes = afio->read_oset (file, buf, offs, test);
      trec_test_stop (nbytes);
      rerr = (nbytes < 0)
	  || (((unsigned int) nbytes) != test->wkld.u.oset.my_data_size);
      if (rerr)
	iore_fatalf("Failed reading from file '%s'", file.name);
      else
	{
	  if (intra_test_barrier)
	    MPI_Barrier (ctx.comm);

	  /* closes */
	  trec_test_start (IORE_TEST_TYPE_READ, IORE_TREC_ACTION_CLOSE);
	  rerr = afio->close (&file);
	  trec_test_stop (0);
	  if (rerr)
	    iore_fatalf("Failed closing file '%s'", file.name);
	  else
	    {
	      if (!test->file_keep)
		{
		  MPI_Barrier (ctx.comm);

		  trec_test_start (IORE_TEST_TYPE_READ,
				   IORE_TREC_ACTION_REMOVE);
		  /* removes */
		  if ((test->file_mode == IORE_TEST_FMODE_NXN)
		      || ((test->file_mode == IORE_TEST_FMODE_NX1)
			  && (ctx.task_id == IORE_MASTER_TASK)))
		    {

		      rerr = afio->remove (file);
		    }
		  trec_test_stop (0);
		}
	    } /* end of remove block */
	} /* end of close block */
    } /* end of read block */

  MPI_Barrier (ctx.comm);

  return rerr;
} /* test_oset_read_exec () */

static int
test_dset_exec (iore_test_t *test, unsigned int rpl_id, unsigned int run_id,
		unsigned int rpt_id)
{
  iore_file_t file;
  char *buf;

  iore_trec_test_t *trec_test;
  if (test->type.write)
    {
      if (test_dset_type_prep (test, IORE_TEST_TYPE_WRITE, rpl_id, run_id,
			       rpt_id, &file, &buf))
	return IORE_FAILURE;
      if (test_dset_write_exec (test, file, buf))
	return IORE_FAILURE;
      else
	{
	  trec_test = trec_test_commit ();
	  test_show_summary (trec_test);
	}
      if (test_dset_type_post (test, &file, buf))
	return IORE_FAILURE;
    }

  if (test->type.read)
    {
      if (test_dset_type_prep (test, IORE_TEST_TYPE_READ, rpl_id, run_id,
			       rpt_id, &file, &buf))
	return IORE_FAILURE;
      if (test_dset_read_exec (test, file, buf))
	return IORE_FAILURE;
      else
	{
	  trec_test = trec_test_commit ();
	  test_show_summary (trec_test);
	}
      if (test_dset_type_post (test, &file, buf))
	return IORE_FAILURE;
    }

  return IORE_SUCCESS;
} /* test_dset_exec () */

static int
test_dset_type_prep (iore_test_t *test, enum iore_test_type type,
		     unsigned int rpl_id, unsigned int run_id,
		     unsigned int rpt_id, iore_file_t *file, char **buf)
{
  /* in read tests, tasks can perform as a different 'id' */
  int task_id =
      (type == IORE_TEST_TYPE_WRITE) ?
	  (unsigned int) ctx.task_id :
	  (ctx.task_id + test->read_reorder_offset) % test->wkld.num_tasks;

  /* prepares the file */
  if (test_file_prep (test, rpl_id, run_id, rpt_id, task_id, file))
    return IORE_FAILURE;

  // TODO: invoke AFSB

  /* sets task-specific parameters */
  if (test->wkld.u.dset.type == IORE_WKLD_DSET_CARTESIAN)
    if (test_dset_cart_type_prep (&test->wkld.u.dset, task_id))
      return IORE_FAILURE;

  /* prepares the buffer */
  if (test_buf_prep (test->wkld.u.dset.my_size, type, buf))
    return IORE_FAILURE;

  return IORE_SUCCESS;
} /* test_dset_type_prep () */

static int
test_dset_type_post (iore_test_t *test, iore_file_t *file, char *buf)
{
  int rerr = IORE_SUCCESS;

  if (test->wkld.u.dset.type == IORE_WKLD_DSET_CARTESIAN)
    rerr = test_dset_cart_type_post (&test->wkld.u.dset.u.cart);
  rerr |= test_file_post (test, file);
  free (buf);

  return rerr;
} /* test_dset_type_post () */

static int
test_dset_cart_type_prep (iore_wkld_dset_t *dset, int task_id)
{
  unsigned int num_dims = dset->u.cart.num_dims;
  size_t size = num_dims * sizeof(unsigned int);
  dset->u.cart.my_dim_sizes = malloc (size);
  assert(dset->u.cart.my_dim_sizes);
  dset->u.cart.my_start_coord = malloc (size);
  assert(dset->u.cart.my_start_coord);

  size_t dset_size = dset->_vars_size;
  int fac = 1;
  unsigned int i;
  for (i = 0; i < num_dims; i++)
    {
      /* maximum size of the ith dimension in any task */
      int max_dim_size_task = (dset->u.cart.g_dim_sizes[i]
	  + dset->u.cart.g_dim_divs[i] - 1) / dset->u.cart.g_dim_divs[i];

      int j = i - 1;
      if (j >= 0)
	fac *= dset->u.cart.g_dim_divs[j];

      dset->u.cart.my_start_coord[i] = ((task_id / fac)
	  % dset->u.cart.g_dim_divs[i]) * max_dim_size_task;

      int distance = dset->u.cart.g_dim_sizes[i]
	  - dset->u.cart.my_start_coord[i];
      dset->u.cart.my_dim_sizes[i] =
	  (distance < max_dim_size_task) ? distance : max_dim_size_task;

      dset_size *= dset->u.cart.my_dim_sizes[i];
    }

  dset->my_size = dset_size;

  return IORE_SUCCESS;
} /* test_dset_cart_type_prep () */

static int
test_dset_cart_type_post (iore_wkld_dset_cart_t *cart)
{
  if (cart->my_dim_sizes)
    {
      free (cart->my_dim_sizes);
      cart->my_dim_sizes = NULL;
    }

  if (cart->my_start_coord)
    {
      free (cart->my_start_coord);
      cart->my_start_coord = NULL;
    }

  return IORE_SUCCESS;
} /* test_dset_cart_type_post () */

static int
test_dset_write_exec (iore_test_t *test, iore_file_t file, const char *buf)
{
  int rerr = IORE_SUCCESS;

  const iore_afio_vtable_t *afio = afio_pool[test->afio.type];
  bool intra_test_barrier = test->intra_test_barrier;
  ssize_t nbytes = 0;

  MPI_Barrier (ctx.comm);

  /* creates */
  trec_test_start (IORE_TEST_TYPE_WRITE, IORE_TREC_ACTION_CREATE_OPEN);
  rerr = afio->create (&file, test);
  trec_test_stop (0);
  if (rerr)
    iore_fatalf("Failed creating file '%s'", file.name);
  else
    {
      if (intra_test_barrier)
	MPI_Barrier (ctx.comm);

      /* writes */
      trec_test_start (IORE_TEST_TYPE_WRITE, IORE_TREC_ACTION_WRITE_READ);
      nbytes = afio->write_dset (file, buf, test);
      trec_test_stop (nbytes);
      rerr = (nbytes < 0)
	  || (((unsigned int) nbytes) != test->wkld.u.dset.my_size);
      if (rerr)
	iore_fatalf("Failed writing to file '%s'", file.name);
      else
	{
	  if (intra_test_barrier)
	    MPI_Barrier (ctx.comm);

	  /* closes */
	  trec_test_start (IORE_TEST_TYPE_WRITE, IORE_TREC_ACTION_CLOSE);
	  rerr = afio->close (&file);
	  trec_test_stop (0);
	  if (rerr)
	    iore_fatalf("Failed closing file '%s'", file.name);
	  else
	    {
	      if (!test->type.read && !test->file_keep)
		{
		  MPI_Barrier (ctx.comm);

		  trec_test_start (IORE_TEST_TYPE_WRITE,
				   IORE_TREC_ACTION_REMOVE);
		  /* removes */
		  if ((test->file_mode == IORE_TEST_FMODE_NXN)
		      || ((test->file_mode == IORE_TEST_FMODE_NX1)
			  && (ctx.task_id == IORE_MASTER_TASK)))
		    {

		      rerr = afio->remove (file);
		    }
		  trec_test_stop (0);
		}
	    } /* end of remove block */
	} /* end of close block */
    } /* end of write block */

  MPI_Barrier (ctx.comm);

  return rerr;
} /* test_dset_write_exec () */

static int
test_dset_read_exec (iore_test_t *test, iore_file_t file, char *buf)
{
  int rerr = IORE_SUCCESS;

  const iore_afio_vtable_t *afio = afio_pool[test->afio.type];
  bool intra_test_barrier = test->intra_test_barrier;
  ssize_t nbytes = 0;

  MPI_Barrier (ctx.comm);

  /* opens */
  trec_test_start (IORE_TEST_TYPE_READ, IORE_TREC_ACTION_CREATE_OPEN);
  rerr = afio->open (&file, test);
  trec_test_stop (0);
  if (rerr)
    iore_fatalf("Failed opening file '%s'", file.name);
  else
    {
      if (intra_test_barrier)
	MPI_Barrier (ctx.comm);

      /* reads */
      trec_test_start (IORE_TEST_TYPE_READ, IORE_TREC_ACTION_WRITE_READ);
      nbytes = afio->read_dset (file, buf, test);
      trec_test_stop (nbytes);
      rerr = (nbytes < 0)
	  || (((unsigned int) nbytes) != test->wkld.u.dset.my_size);
      if (rerr)
	iore_fatalf("Failed reading from file '%s'", file.name);
      else
	{
	  if (intra_test_barrier)
	    MPI_Barrier (ctx.comm);

	  /* closes */
	  trec_test_start (IORE_TEST_TYPE_READ, IORE_TREC_ACTION_CLOSE);
	  rerr = afio->close (&file);
	  trec_test_stop (0);
	  if (rerr)
	    iore_fatalf("Failed closing file '%s'", file.name);
	  else
	    {
	      if (!test->file_keep)
		{
		  MPI_Barrier (ctx.comm);

		  trec_test_start (IORE_TEST_TYPE_READ,
				   IORE_TREC_ACTION_REMOVE);
		  /* removes */
		  if ((test->file_mode == IORE_TEST_FMODE_NXN)
		      || ((test->file_mode == IORE_TEST_FMODE_NX1)
			  && (ctx.task_id == IORE_MASTER_TASK)))
		    {

		      rerr = afio->remove (file);
		    }
		  trec_test_stop (0);
		}
	    } /* end of remove block */
	} /* end of close block */
    } /* end of read block */

  MPI_Barrier (ctx.comm);

  return rerr;
} /* test_dset_read_exec () */

static int
test_file_prep (iore_test_t *test, unsigned int rpl_id, unsigned int run_id,
		unsigned int rpt_id, int task_id, iore_file_t *file)
{
  char *file_name = malloc (strlen (test->file_name) + 1);
  assert(file_name);
  strncpy(file_name, test->file_name, strlen (test->file_name) + 1);
  char *base_name = basename (file_name);
  char *dir_name = dirname (file_name);

  char *swp;
  int nbytes;
  /* appends sequence numbers */
  if (test->file_name_append_sequence_num)
    {
      swp = malloc (PATH_MAX);
      assert(swp);
      nbytes = snprintf(swp, PATH_MAX, "%s.%d.%d.%d", base_name, rpl_id, run_id,
			rpt_id);
      if (nbytes == -1 || nbytes > PATH_MAX)
	{
	  iore_error_all("Failed appending sequence numbers to file name.");
	  free (swp);
	  return IORE_FAILURE;
	}
      base_name = swp;
    }

  /* appends task id */
  if (test->file_name_append_task_id || test->file_mode == IORE_TEST_FMODE_NXN)
    {
      swp = malloc (PATH_MAX);
      assert(swp);
      nbytes = snprintf(swp, PATH_MAX, "%s.%d", base_name, task_id);
      if (nbytes == -1 || nbytes > PATH_MAX)
	{
	  iore_error_all("Failed appending the task id to file name.");
	  free (swp);
	  return IORE_FAILURE;
	}
      base_name = swp;
    }

  /* creates a directory per task */
  if (test->file_dir_per_task && test->file_mode == IORE_TEST_FMODE_NXN)
    {
      swp = malloc (PATH_MAX);
      assert(swp);
      nbytes = snprintf(swp, PATH_MAX, "%s/task.%d", dir_name, task_id);
      if (nbytes == -1 || nbytes > PATH_MAX)
	{
	  iore_errorf_all("Failed appeding 'task.%d' to file parent dir.",
			  task_id);
	  free (swp);
	  return IORE_FAILURE;
	}
      dir_name = swp;

      /* checks if the directory exists */
      if (mkdir (dir_name, S_IRWXU) && errno != EEXIST)
	{
	  iore_errorf_all("Failed creating directory '%s'.", dir_name);
	  free (swp);
	  return IORE_FAILURE;
	}
    }

  /* composes the complete file name */
  swp = malloc (PATH_MAX);
  assert(swp);
  nbytes = snprintf(swp, PATH_MAX, "%s/%s", dir_name, base_name);
  if (nbytes == -1 || nbytes > PATH_MAX)
    {
      iore_error_all("Failed composing file name.");
      free (swp);
      return IORE_FAILURE;
    }
  file->name = swp;

  return IORE_SUCCESS;
} /* test_file_prep () */

static int
test_file_post (iore_test_t *test, iore_file_t *file)
{
  int rerr = IORE_SUCCESS;

  if (!test->file_keep && test->file_dir_per_task
      && test->file_mode == IORE_TEST_FMODE_NXN)
    {
      char *file_name = strdup (file->name);
      char *dir_name = dirname (file_name);
      if (rmdir (dir_name))
	{
	  iore_errorf_all("Failed removing directory '%s'.", dir_name);
	  free (file_name);
	  rerr = IORE_FAILURE;
	}
    }

  free (file->name);
  file->name = NULL;

  return rerr;
} /* test_file_post () */

// TODO: fill with random data
static int
test_buf_prep (size_t size, enum iore_test_type type, char **buf)
{
  *buf = malloc (size);
  assert(*buf);

  if (type == IORE_TEST_TYPE_WRITE)
    {
      unsigned int i;
      for (i = 0; i < size; i++)
	(*buf)[i] = (ctx.task_id % 26) + 'a';
    }

  return IORE_SUCCESS;
} /* test_buf_prep () */

static int
test_offs_prep (iore_test_t *test, int task_id, off_t **offs)
{
  int rerr = IORE_SUCCESS;

  iore_wkld_oset_t *oset = &test->wkld.u.oset;

  int num_reqs = (oset->my_data_size + oset->my_req_size - 1)
      / oset->my_req_size;
  *offs = malloc (num_reqs * sizeof(off_t));
  assert(*offs);

  if (test->file_mode == IORE_TEST_FMODE_NXN)
    rerr = test_offs_nxn_prep (&test->wkld, num_reqs, *offs);
  else if ((test->file_mode == IORE_TEST_FMODE_NX1)
      && (oset->ac_pattern == IORE_WKLD_OSET_AP_SEQUENTIAL))
    rerr = test_offs_nx1_seq_prep (&test->wkld, num_reqs, task_id, *offs);
  else if ((test->file_mode == IORE_TEST_FMODE_NX1)
      && (oset->ac_pattern == IORE_WKLD_OSET_AP_RANDOM))
    rerr = test_offs_nx1_rnd_prep (&test->wkld, num_reqs, task_id, *offs);

  if (rerr)
    {
      free (*offs);
      *offs = NULL;
    }

  return rerr;
} /* test_offs_prep () */

static int
test_offs_nxn_prep (iore_wkld_t *wkld, int num_reqs, off_t *offs)
{
  int rerr = IORE_SUCCESS;

  int i;
  for (i = 0; i < num_reqs; i++)
    offs[i] = i * wkld->u.oset.my_req_size;

  if (wkld->u.oset.ac_pattern == IORE_WKLD_OSET_AP_RANDOM)
    {
      unsigned int seed;
      if (ctx.task_id == IORE_MASTER_TASK)
	seed = prng_gen_seed ();
      MPI_Bcast (&seed, 1, MPI_UNSIGNED, IORE_MASTER_TASK, ctx.comm);
      rerr = shuffle (offs, num_reqs, sizeof(off_t), seed);
    }

  return rerr;
} /* test_offs_nxn_prep () */

static int
test_offs_nx1_seq_prep (iore_wkld_t *wkld, int num_reqs, int task_id,
			off_t *offs)
{
  offs[0] = 0;
  int i;
  for (i = 0; i < task_id; i++)
    offs[0] += wkld->u.oset.data_sizes[i % wkld->u.oset._data_sizes_len];

  for (i = 1; i < num_reqs; i++)
    offs[i] = i * wkld->u.oset.my_req_size + offs[0];

  return IORE_SUCCESS;
} /* test_offs_nx1_seq_prep () */

/**
 * The process is guided by increasing offset order. A task id is
 * randomly generated for the first offset. If the id is of the running
 * task, the offset is added to the list. The offset is incremented by
 * the request size of the generated task and the same amount is
 * decremented from the remaining data of the respective task.
 * The process continues until all offsets of the running task are
 * identified.
 */
static int
test_offs_nx1_rnd_prep (iore_wkld_t *wkld, int num_reqs, int task_id,
			off_t *offs)
{
  int rerr = IORE_SUCCESS;

  iore_wkld_oset_t *oset = &wkld->u.oset;

  unsigned int l_tid_len = wkld->num_tasks;
  int *l_tid = malloc (l_tid_len * sizeof(int));
  assert(l_tid);
  size_t *l_remain = malloc (l_tid_len * sizeof(size_t));
  assert(l_remain);
  size_t *l_req_sz = malloc (l_tid_len * sizeof(size_t));
  assert(l_req_sz);

  unsigned int i;
  for (i = 0; i < l_tid_len; i++)
    {
      l_tid[i] = i;
      l_remain[i] = oset->data_sizes[i % oset->_data_sizes_len];
      l_req_sz[i] = oset->req_sizes[i % oset->_req_sizes_len];
    }

  iore_prng_dist_t dist;
  dist.type = IORE_PRNG_UNIF;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MIN, "0");
  dict_set (&dist.params, PRNG_PARAM_MAX, "1");
  iore_prng_t *prng = prng_new_sync (&dist);
  dict_free (&dist.params);

  if (prng)
    {
      off_t off = 0;
      int tid;
      int c = 0;
      size_t req_sz;
      while (c < num_reqs)
	{
	  i = prng_next_uint (prng); /* [0,1) */
	  i *= l_tid_len;

	  tid = l_tid[i];
	  if (tid == task_id)
	    offs[c++] = off;

	  req_sz =
	      (l_req_sz[tid] < l_remain[tid]) ? l_req_sz[tid] : l_remain[tid];
	  off += req_sz;
	  l_remain[tid] -= req_sz;

	  if (l_remain[tid] <= 0)
	    {
	      l_tid_len--;
	      l_tid[i] = l_tid[l_tid_len];
	    }
	}

      free (prng);
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  free (l_tid);
  free (l_remain);
  free (l_req_sz);

  return rerr;
} /* test_offs_nx1_rnd_prep () */

static int
test_sizes_prep (iore_prng_dist_t *dist, unsigned int len, size_t **l_sizes)
{
  assert(dist);

  int rerr = IORE_SUCCESS;

  *l_sizes = malloc (len * sizeof(size_t));
  assert(*l_sizes);

  iore_prng_t *prng = prng_new_sync (dist);
  if (prng)
    {
      unsigned int i;
      for (i = 0; i < len; i++)
	(*l_sizes)[i] = prng_next_size (prng);

      free (prng);
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  return rerr;
} /* test_sizes_prep () */

static void
test_show_summary (iore_trec_test_t *trec_test)
{
  assert(trec_test);

  iore_time_t min_time[IORE_TREC_ACTION_LENGTH];
  iore_time_t max_time[IORE_TREC_ACTION_LENGTH];
  size_t agg_data_size = 0;
  int i_action;
  for (i_action = 0; i_action < IORE_TREC_ACTION_LENGTH; i_action++)
    {
      MPI_Reduce (&trec_test->task_time[i_action][IORE_TREC_EVENT_START],
		  &min_time[i_action], 1, MPI_DOUBLE, MPI_MIN, IORE_MASTER_TASK,
		  ctx.comm);
      MPI_Reduce (&trec_test->task_time[i_action][IORE_TREC_EVENT_STOP],
		  &max_time[i_action], 1, MPI_DOUBLE, MPI_MAX, IORE_MASTER_TASK,
		  ctx.comm);
      if (i_action == IORE_TREC_ACTION_WRITE_READ)
	{
	  MPI_Reduce (&trec_test->task_data_size, &agg_data_size, 1, MPI_AINT,
	  MPI_SUM,
		      IORE_MASTER_TASK, ctx.comm);
	}
    }

  iore_time_t create_open = (max_time[IORE_TREC_ACTION_CREATE_OPEN]
      - min_time[IORE_TREC_ACTION_CREATE_OPEN]);
  iore_time_t write_read = (max_time[IORE_TREC_ACTION_WRITE_READ]
      - min_time[IORE_TREC_ACTION_WRITE_READ]);
  iore_time_t close = (max_time[IORE_TREC_ACTION_CLOSE]
      - min_time[IORE_TREC_ACTION_CLOSE]);
  iore_time_t remove = (max_time[IORE_TREC_ACTION_REMOVE]
      - min_time[IORE_TREC_ACTION_REMOVE]);
  iore_time_t total = ((
      remove > 0 ?
	  max_time[IORE_TREC_ACTION_REMOVE] : max_time[IORE_TREC_ACTION_CLOSE])
      - min_time[IORE_TREC_ACTION_CREATE_OPEN]);
  double tput_bs = (agg_data_size / total);
  char tput[15];
  hrbytesd (tput_bs, tput, 15);

  if (ctx.task_id == IORE_MASTER_TASK)
    {
      fprintf (stdout, "%-5s %10.4f %10.4f %10.4f %10.4f %10.4f %15s\n",
	       test_type_lbl[trec_test->type], create_open, write_read, close,
	       remove, total, tput);
    }
} /* test_show_summary () */
