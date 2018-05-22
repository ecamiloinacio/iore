/*
 * iore_error.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

#include "iore_error.h"
#include "iore_ctx.h"

/*** DEFINES *****************************************************************/

#define WARN_MSG_TEMPLATE "[IORE Warn]: %s\n"
#define WARN_ALL_MSG_TEMPLATE "[IORE Warn]: Task %d: %s\n"
#define ERROR_MSG_TEMPLATE "[IORE Error]: %s\n"
#define ERROR_ALL_MSG_TEMPLATE "[IORE Error]: Task %d: %s\n"
#define FATAL_MSG_TEMPLATE "[IORE Fatal]: Task %d: %s (%s:%d)\n"
#define DEBUG_MSG_TEMPLATE "[IORE Debug]: Task %d: %s (%s:%d)\n"

/*** FUNCTIONS ***************************************************************/

void
handle_warn (int all, const char *reason)
{
  if (reason)
    {
      if (all)
	fprintf (stdout, WARN_ALL_MSG_TEMPLATE, ctx.task_id, reason);
      else if (!all && ctx.task_id == IORE_MASTER_TASK)
	fprintf (stdout, WARN_MSG_TEMPLATE, reason);
    }
} /* handle_warn () */

void
handle_warnf (int all, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);

  char *msg = NULL;

  int len = vsnprintf (msg, 0, format, ap) + 1;
  if (len > 0)
    {
      va_start(ap, format);
      msg = malloc (len);
      assert(msg);
      vsnprintf (msg, len, format, ap);
    }

  va_end(ap);

  handle_warn (all, msg);

  free (msg);
} /* handle_warnf () */

void
handle_error (int all, const char *reason)
{
  if (reason)
    {
      if (all)
	fprintf (stderr, ERROR_ALL_MSG_TEMPLATE, ctx.task_id, reason);
      else if (!all && ctx.task_id == IORE_MASTER_TASK)
	fprintf (stderr, ERROR_MSG_TEMPLATE, reason);
    }
} /* handle_error () */

void
handle_errorf (int all, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);

  char *msg = NULL;

  int len = vsnprintf (msg, 0, format, ap) + 1;
  if (len > 0)
    {
      va_start(ap, format);
      msg = malloc (len);
      assert(msg);
      vsnprintf (msg, len, format, ap);
    }

  va_end(ap);

  handle_error (all, msg);

  free (msg);
} /* handle_errorf () */

void
handle_fatal (const char *file, int line, const char *reason)
{
  if (reason)
    fprintf (stderr, FATAL_MSG_TEMPLATE, ctx.task_id, reason, file, line);

  MPI_Abort (ctx.comm, IORE_FAILURE);
} /* handle_fatal () */

void
handle_fatalf (const char *file, int line, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);

  char *msg = NULL;

  int len = vsnprintf (msg, 0, format, ap) + 1;
  if (len > 0)
    {
      va_start(ap, format);
      msg = malloc (len);
      assert(msg);
      vsnprintf (msg, len, format, ap);
    }

  va_end(ap);

  handle_fatal (file, line, msg);

  free (msg);
} /* handle_fatalf () */

void
handle_debug (const char *file, int line, const char *reason)
{
  if (reason)
    fprintf (stderr, DEBUG_MSG_TEMPLATE, ctx.task_id, reason, file, line);
} /* handle_debug () */

void
handle_debugf (const char *file, int line, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);

  char *msg = NULL;

  int len = vsnprintf (msg, 0, format, ap) + 1;
  if (len > 0)
    {
      va_start(ap, format);
      msg = malloc (len);
      assert(msg);
      vsnprintf (msg, len, format, ap);
    }

  va_end(ap);

  handle_debug (file, line, msg);

  free (msg);
} /* handle_debugf ()*/
