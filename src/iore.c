/*
 * iore.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#include "iore_ctx.h"
#include "iore_trec.h"
#include "iore_prng.h"
#include "iore_error.h"
#include "iore_parser.h"
#include "iore_stex.h"
#include "iore_experiment.h"
#include "iore_experiment_valid.h"

/*** PROTOTYPES **************************************************************/

static void
iore_init (int, char **);
static void
iore_finalize ();

/*** MAIN ********************************************************************/

int
main (int argc, char **argv)
{
  iore_init (argc, argv);
  if (ctx.task_id == IORE_MASTER_TASK)
    fputs ("IORE: The Distributed I/O Performance Evaluation Tool\n\n", stdout);

  int rerr = EXIT_SUCCESS;

  iore_exp_t exp =
    { };
  if (exp_init (&exp) != NULL)
    {
      if (parse_cmdline_opts (argc, argv, &exp))
	rerr = EXIT_FAILURE;
      else if (exp_valid (&exp))
	rerr = EXIT_FAILURE;
      else if (exp_exec (&exp))
	rerr = EXIT_FAILURE;
      else
	{
	  iore_trec_exp_t trec_exp = get_trec_exp ();
	  if (stex_export (trec_exp, exp.stex))
	    rerr = EXIT_FAILURE;
	}
      exp_free (&exp);
    }
  else
    iore_fatal("exp_init() failed.");

  iore_finalize ();

  if (rerr && ctx.task_id == IORE_MASTER_TASK)
    fputs ("\nExperiment not executed due to errors. Closing IORE.\n", stdout);

  exit (rerr);
} /* main () */

/*** FUNCTIONS ***************************************************************/

static void
iore_init (int argc, char **argv)
{
  MPI_Init (&argc, &argv);
  ctx_init ();
  trec_init ();
} /* iore_init () */

static void
iore_finalize ()
{
  trec_destroy ();
  MPI_Finalize ();
} /* iore_finalize () */
