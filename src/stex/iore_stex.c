/*
 * iore_stex.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <assert.h>

#include "iore_stex.h"
#include "iore_error.h"

/*** VARIABLES ***************************************************************/

extern const iore_stex_vtable_t stex_csv;
static const iore_stex_vtable_t *stex_pool[IORE_STEX_FORMAT_LENGTH] =
  { &stex_csv };

/*** FUNCTIONS ***************************************************************/

iore_stex_t *
stex_init (iore_stex_t *this)
{
  assert(this);

  this->report_type.task = false;
  this->report_type.test = false;
  this->data_format = IORE_STEX_FORMAT_LENGTH;
  this->export_dir = NULL;

  return this;
} /* stex_init () */

void
stex_free (iore_stex_t *this)
{
  if (this)
    {
      if (this->export_dir)
	{
	  free (this->export_dir);
	  this->export_dir = NULL;
	}
    }
} /* stex_free () */

int
stex_export (const iore_trec_exp_t trec_exp, const iore_stex_t stex)
{
  int rerr = IORE_SUCCESS;

  const enum iore_stex_format data_format = stex.data_format;
  const iore_stex_vtable_t *stex_backend;
  if (data_format < IORE_STEX_FORMAT_LENGTH)
    {
      stex_backend = stex_pool[data_format];
      if (stex.report_type.task)
	rerr |= stex_backend->export_task (trec_exp, stex.export_dir);

      if (stex.report_type.test)
	rerr |= stex_backend->export_test (trec_exp, stex.export_dir);
    }
  else if (data_format > IORE_STEX_FORMAT_LENGTH)
    {
      iore_error("Unsupported exporter format.");
      rerr = IORE_FAILURE;
    }

  return rerr;
} /* stex_export () */
