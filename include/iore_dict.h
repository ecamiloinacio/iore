/*
 * iore_dict.h
 *
 * Simple dictionary data structure for generic parameters in IORE.
 *
 * Since each dictionary is expected to hold a small number of entries
 * (i.e., key/value pairs), it is implemented in a way that lookups are
 * performed sequentialy (O(n)).
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_DICT_H_
#define INCLUDE_IORE_DICT_H_

#include <stdbool.h>

/*** DEFINES *****************************************************************/

#define DICT2STR_FORMAT "dict_t (%p) { _length = %u, _capacity = %u, entries = { %s } }"

/*** TYPES *******************************************************************/

typedef struct dict_entry
{
  char *key;
  char *val;
} dict_entry_t;

typedef struct dict
{
  dict_entry_t *entry;

  unsigned int _length;
  unsigned int _capacity;
} dict_t;

typedef struct dict_iter
{
  const dict_t *dict;

  unsigned int _index;
} dict_iter_t;

/*** PROTOTYPES **************************************************************/

dict_t
dict_new ();
dict_t
dict_newc (unsigned int);
dict_t *
dict_init (dict_t *);
dict_t *
dict_initc (dict_t *, unsigned int);
void
dict_free (dict_t *);
void
dict_set (dict_t *, const char *, const char *);
char *
dict_get (const dict_t *, const char *);
bool
dict_has_key (const dict_t *, const char *);
void
dict_remove (dict_t *, const char *);
dict_iter_t
dict_iter (const dict_t *);
const dict_entry_t *
dict_iter_next (dict_iter_t *);
unsigned int
dict_length (const dict_t *);
char *
dict2str (const dict_t *);

#endif /* INCLUDE_IORE_DICT_H_ */
