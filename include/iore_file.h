/*
 * iore_file.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_FILE_H_
#define INCLUDE_IORE_FILE_H_

/*** DEFINES *****************************************************************/

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define FILE2STR_FORMAT "iore_file_t (%p) { name = '%s', fint = %d, fptr = %p }"

/*** TYPES *******************************************************************/

typedef struct iore_file
{
  char *name;
  union
  {
    int fint;
    void *fptr;
  } hdle;
} iore_file_t;

/*** PROTOTYPES **************************************************************/

char *
file_get_extension (char *);
bool
file_is_dir (const char *);
bool
file_is_regular (const char *);
bool
file_exists (const char *);
bool
file_is_readable (const char *);
char *
file2str (const iore_file_t *);

#endif /* INCLUDE_IORE_FILE_H_ */
