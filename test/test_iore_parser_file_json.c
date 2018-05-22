/*
 * test_iore_parser_file_json.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iore_parser_file.h"
#include "iore_ctx.h"
#include "iore_error.h"
#include "iore_experiment.h"

#define MAX_PROCESSES 2

int
test_parse (char *);

int
main (int argc, char **argv)
{
  fputs ("Initializing JSON parser test...\n", stdout);
  MPI_Init (&argc, &argv);
  ctx_init ();

  if (argc == 2)
    {
      int rerr = test_parse (argv[1]);
      fprintf (stdout, "[Task %d] Test parse:...%s!\n", ctx.task_id,
	       (rerr ? "FAILED" : "SUCCESS"));
    }
  else
    fputs ("Missign experiment definition file.\n", stderr);

  MPI_Finalize ();
  fputs ("Finalizing JSON parser test.\n", stdout);
} /* main () */

int
test_parse (char *file)
{
  int rerr = IORE_SUCCESS;

  iore_exp_t exp =
    { };
  if (exp_init (&exp) != NULL)
    {
      if (!parse (file, &exp))
	rerr = IORE_FAILURE;
    }
  else
    rerr = IORE_FAILURE;

  return rerr;
} /* test_parse () */
