/*
 * iore_file.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <libgen.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "iore_file.h"
#include "iore_error.h"

/*** FUNCTIONS ***************************************************************/

/**
 * Returns NULL in case of failure.
 */
char *
file_get_extension (char *path)
{
  char *extension = NULL;

  char *base_name = basename (path);
  extension = strrchr (base_name, '.');
  if (extension == base_name)
    extension = NULL; /* file without extension */
  else
    extension++; /* escape the '.' */

  return extension;
} /* file_get_extension () */

bool
file_is_dir (const char *path)
{
  bool is_dir = false;

  struct stat st;
  if (stat (path, &st))
    iore_fatalf("Stat failed: '%s'.", path);
  else
    is_dir = S_ISDIR(st.st_mode);

  return is_dir;
} /* file_is_dir () */

bool
file_is_regular (const char *path)
{
  bool is_regular = false;

  struct stat st;
  if (stat (path, &st))
    iore_fatalf("Stat failed: '%s'.", path);
  else
    is_regular = S_ISREG(st.st_mode);

  return is_regular;
} /* file_is_regular () */

bool
file_exists (const char *path)
{
  if (!path || strlen (path) == 0)
    {
      iore_error("Invalid argument.");
      return false;
    }

  if (access (path, F_OK) != 0)
    {
      if (errno == ENOENT)
	iore_errorf_all("File '%s' does not exist.", path);
      else if (errno == EACCES)
	iore_errorf_all("File '%s' is not accessible.", path);
      else
	iore_error_all("Invalid argument.");
      return false;
    }

  return true;
} /* file_exists () */

bool
file_is_readable (const char *path)
{
  if (!path || strlen (path) == 0)
    {
      iore_error("Invalid argument.");
      return false;
    }

  if (access (path, R_OK) != 0)
    {
      iore_errorf_all("File '%s' is not readable (access denied).", path);
      return false;
    }

  return true;
} /* file_is_readable () */

char *
file2str (const iore_file_t *file)
{
  char *str = NULL;

  if (file)
    {
      int len = snprintf (str, 0, FILE2STR_FORMAT, file, file->name,
			  file->hdle.fint, file->hdle.fptr) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, FILE2STR_FORMAT, file, file->name, file->hdle.fint,
		    file->hdle.fptr);
	}
    }

  return str;
} /* file2str () */
