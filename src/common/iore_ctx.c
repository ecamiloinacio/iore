/*
 * iore_ctx.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include "iore_ctx.h"

/*** VARIABLES ***************************************************************/

iore_ctx_t ctx =
  { };

/*** FUNCTIONS ***************************************************************/

void
ctx_init ()
{
  ctx.comm = MPI_COMM_WORLD;

  MPI_Comm_size (MPI_COMM_WORLD, &ctx.num_procs);
  MPI_Comm_rank (MPI_COMM_WORLD, &ctx.task_id);

  ctx.verb_lvl = VERB_LVL_NORMAL;
} /* ctx_init () */
