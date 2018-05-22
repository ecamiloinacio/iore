/*
 * iore_parser_file.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_PARSER_FILE_H_
#define INCLUDE_IORE_PARSER_FILE_H_

#include "iore_experiment_types.h"

/*** TYPES *******************************************************************/

typedef struct iore_prsr_vtable
{
  char extension[4];
  int
  (*parse) (const char *, iore_exp_t *);
} iore_prsr_vtable_t;

/*** PROTOTYPES **************************************************************/

iore_exp_t *
parse (char *, iore_exp_t *);

#endif /* INCLUDE_IORE_PARSER_FILE_H_ */
