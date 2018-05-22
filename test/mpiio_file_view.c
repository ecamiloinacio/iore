#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* matrix (8 x 16) divided by 4 processes */
#define NUM_DIMS 2
#define G_X_SIZE 8
#define G_Y_SIZE 16
#define NUM_PROCS 4
#define FILE_NAME "./test_mpi_fview.txt"

int
main (int argc, char **argv)
{
  MPI_Init (&argc, &argv);

  int rank, nprocs;
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  if (nprocs != NUM_PROCS)
    {
      fputs ("Must have 4 MPI ranks.\n", stderr);
      MPI_Abort (MPI_COMM_WORLD, -1);
    }

  /* matrix (8 x 16) */
  int g_sizes[NUM_DIMS] =
    { G_X_SIZE, G_Y_SIZE };

  /* sub-matrix (4 x 8) */
  int l_sizes[NUM_DIMS] =
    { (G_X_SIZE / 2), (G_Y_SIZE / 2) };
  int l_coord[NUM_DIMS] =
    { ((rank % 2) * l_sizes[0]), (((rank / 2) % 2) * l_sizes[1]) };
  size_t size = l_sizes[0] * l_sizes[1] * sizeof(char);
  char *buf = malloc (size);
  assert(buf);
  int i;
  for (i = 0; i < size; i++)
    buf[i] = (rank % 26) + 'a';

  /* Open file */
  MPI_File fh;
  if (MPI_File_open (MPI_COMM_WORLD, FILE_NAME,
  MPI_MODE_CREATE | MPI_MODE_WRONLY,
		     MPI_INFO_NULL, &fh))
    {
      fputs ("Open failed\n", stderr);
      MPI_Abort (MPI_COMM_WORLD, -1);
    }

  /* MPI file type */
  MPI_Datatype file_type;
  if (MPI_Type_create_subarray (NUM_DIMS, g_sizes, l_sizes, l_coord,
  MPI_ORDER_C,
				MPI_CHAR, &file_type))
    {
      fputs ("Create subarray failed\n", stderr);
      MPI_Abort (MPI_COMM_WORLD, -1);
    }
  MPI_Type_commit (&file_type);
  if (MPI_File_set_view (fh, 0, MPI_CHAR, file_type, "native", MPI_INFO_NULL))
    {
      fputs ("Set view failed\n", stderr);
      MPI_Abort (MPI_COMM_WORLD, -1);
    }

  /* write sub-matrix to file */
  if (MPI_File_write (fh, buf, size, MPI_CHAR, MPI_STATUS_IGNORE))
    {
      fputs ("Write failed\n", stderr);
      MPI_Abort (MPI_COMM_WORLD, -1);
    }

  /* close file */
  MPI_File_close (&fh);

  MPI_Finalize ();

  free (buf);

  exit (EXIT_SUCCESS);
} /* main () */

