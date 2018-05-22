/*
 * iore_parser.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#include "iore_parser.h"
#include "iore_error.h"
#include "iore_ctx.h"
#include "iore_file.h"
#include "iore_parser_file.h"

/*** DEFINES *****************************************************************/

#define PARSER_OPTS "hvf:"
#define PARSER_OPT_HELP "-h"

/*** PROTOTYPES **************************************************************/

static void
parse_cmdline_preempt_opts (int, char **);

static void
usage (char **);

/*** FUNCTIONS ***************************************************************/

int
parse_cmdline_opts (int argc, char **argv, iore_exp_t *exp)
{
  assert(argv);
  assert(exp);

  int rerr = IORE_SUCCESS;

  parse_cmdline_preempt_opts (argc, argv);

  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, PARSER_OPTS)) != -1)
    {
      switch (c)
	{
	case 'h':
	  /* preemptive option; should be ignored at this point */
	  break;
	case 'v':
	  ctx.verb_lvl++;
	  break;
	case 'f':
	  if (!parse (optarg, exp))
	    {
	      iore_fatalf("Unable to parse file '%s'.", optarg);
	      rerr = IORE_FAILURE;
	    }
	  break;
	case '?':
	  rerr = IORE_FAILURE;
	  if (ctx.task_id == IORE_MASTER_TASK)
	    {
	      if (optopt == 'f')
		iore_errorf("Option -%c requires the experiment definition "
			    "file as argument.",
			    optopt);
	      else if (isprint(optopt))
		iore_errorf("Unknown option -%c.", optopt);
	      else
		iore_errorf("Unknown option character '\\x%x'.", optopt);
	    }
	  break;
	default:
	  iore_fatal("Parsing failed.");
	  break;
	}
    } /* end of option loop */

  return rerr;
} /* parse_cmdline_opts () */

/**
 * In case of a preemtive option, the respective function exists the program.
 */
static void
parse_cmdline_preempt_opts (int argc, char **argv)
{
  int i;
  for (i = 1; i < argc; i++)
    if (strcmp (argv[i], PARSER_OPT_HELP) == 0)
      if (ctx.task_id == IORE_MASTER_TASK)
	usage (argv);
} /* parse_cmdline_preempt_opts () */

static void
usage (char **argv)
{
  if (ctx.task_id == IORE_MASTER_TASK)
    {
      fprintf (stderr, "usage: %s [OPTIONS]\n\n", *argv);
      fputs ("OPTIONS:\n", stderr);
      fputs ("  -f FILE : experiment description file\n", stderr);
      fputs ("  -h      : displays this help message\n", stderr);
      fputs ("  -v      : verbose output\n", stderr);
      fflush (stderr);
    }

  exit (EXIT_FAILURE);
} /* usage () */
