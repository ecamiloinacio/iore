/*
 * iore_run_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_RUN_TYPES_H_
#define INCLUDE_IORE_RUN_TYPES_H_

#include "iore_test_types.h"

/*** DEFINES *****************************************************************/

#define RUN2STR_FORMAT "iore_run_t (%p) { id = %u, num_repetitions = %u, test = %s }"

/*** TYPES *******************************************************************/

typedef struct iore_run
{
  unsigned int id;

  unsigned int num_repetitions;
  iore_test_t test;
} iore_run_t;

/*** PROTOTYPES **************************************************************/

char *
run2str (const iore_run_t *);
char *
runs2str (const iore_run_t *, int);

#endif /* INCLUDE_IORE_RUN_TYPES_H_ */
