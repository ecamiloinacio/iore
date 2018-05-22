/*
 * iore_parser_file.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iore_parser_file.h"
#include "iore_error.h"
#include "iore_file.h"

/*** VARIABLES ***************************************************************/

extern const iore_prsr_vtable_t prsr_json;
static const iore_prsr_vtable_t *prsr_pool[] =
  { &prsr_json, NULL };

/*** FUNCTIONS ***************************************************************/

iore_exp_t *
parse (char *path, iore_exp_t *exp)
{
  assert(exp);

  if (!path || !file_exists (path) || !file_is_regular (path)
      || !file_is_readable (path))
    return NULL;

  char *ext = file_get_extension (path);
  if (!ext)
    {
      iore_errorf("File without extension '%s'.", path);
      return NULL;
    }

  const iore_prsr_vtable_t *backend = NULL;
  const iore_prsr_vtable_t *iter = prsr_pool[0];
  while (iter && !backend)
    {
      if (strcmp (ext, iter->extension) == 0)
	backend = iter;

      iter++;
    }

  if (!backend)
    {
      iore_errorf("Unsupported file type '%s'.", ext);
      return NULL;
    }

  if (backend->parse (path, exp))
    return NULL;

  return exp;
} /* parse () */
