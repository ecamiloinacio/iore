/*
 * iore_ctx.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_CTX_H_
#define INCLUDE_IORE_CTX_H_

#include <mpi.h>

/*** DEFINES *****************************************************************/

#define IORE_MASTER_TASK 0
#define VERB_LVL_NORMAL 0
#define VERB_LVL_VERBOSE 1

/*** TYPES *******************************************************************/

typedef struct iore_ctx
{
  int num_procs;
  int task_id;
  MPI_Comm comm;

  int verb_lvl;
} iore_ctx_t;

/*** VARIABLES ***************************************************************/

extern iore_ctx_t ctx;

/*** PROTOTYPES **************************************************************/

void
ctx_init ();

#endif /* INCLUDE_IORE_CTX_H_ */
