/*
 * iore_test.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_TEST_H_
#define INCLUDE_IORE_TEST_H_

#include "iore_test_types.h"

/*** PROTOTYPES **************************************************************/

iore_test_t *
test_init (iore_test_t *);
void
test_free (iore_test_t *);
int
test_exec (iore_test_t *, unsigned int, unsigned int, unsigned int);

#endif /* INCLUDE_IORE_TEST_H_ */
