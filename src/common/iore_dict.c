/*
 * iore_dict.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iore_dict.h"
#include "iore_util.h"

/*** DEFINES *****************************************************************/

#define DICT_DEFAULT_CAPACITY 3

#define DICTENTRY2STR_FORMAT "'%s' = '%s'"

/*** PROTOTYPES **************************************************************/

static int
dict_indexof (const dict_t *, const char *);
static void
dict_ensure_capacity (dict_t *, unsigned int);
static char *
dictentry2str (const dict_entry_t *);
static char *
arrentry2str (const dict_entry_t *, int);

/*** FUNCTIONS ***************************************************************/

dict_t
dict_new ()
{
  return dict_newc (DICT_DEFAULT_CAPACITY);
} /* dict_new () */

dict_t
dict_newc (unsigned int capacity)
{
  dict_t dict =
    { };
  if (capacity > 0)
    {
      dict._length = 0;
      dict._capacity = capacity;
      dict.entry = calloc (capacity, sizeof(dict_entry_t));
      assert(dict.entry);
    }

  return dict;
} /* dict_newc () */

dict_t *
dict_init (dict_t *this)
{
  return dict_initc (this, DICT_DEFAULT_CAPACITY);
} /* dict_init () */

dict_t *
dict_initc (dict_t *this, unsigned int capacity)
{
  if (this && capacity > 0)
    {
      this->_length = 0;
      this->_capacity = capacity;
      this->entry = calloc (capacity, sizeof(dict_entry_t));
      assert(this->entry);
    }

  return this;
} /* dict_initc () */

void
dict_free (dict_t *this)
{
  if (this)
    {
      unsigned int i;
      for (i = 0; i < this->_length; i++)
	{
	  free (this->entry[i].key);
	  this->entry[i].key = NULL;
	  free (this->entry[i].val);
	  this->entry[i].val = NULL;
	}
      free (this->entry);
      this->entry = NULL;
    }
} /* dict_free () */

void
dict_set (dict_t *this, const char *key, const char *val)
{
  if (this && key && strlen (key) > 0)
    {
      int idx = dict_indexof (this, key);
      if (idx >= 0) /* key exists in the dictionary */
	{
	  dict_entry_t *entry = &this->entry[idx];

	  if (entry->val)
	    free (entry->val);
	  size_t size = strlen (val) + 1; /* +1 for '\0' */
	  entry->val = malloc (size);
	  assert(entry->val);
	  strncpy (entry->val, val, size);
	}
      else /* key does not exist */
	{
	  dict_ensure_capacity (this, this->_length + 1);

	  dict_entry_t *entry = &this->entry[this->_length];

	  size_t size = strlen (key) + 1; /* +1 for '\0' */
	  entry->key = malloc (size);
	  assert(entry->key);
	  strncpy (entry->key, key, size);

	  size = strlen (val) + 1; /* +1 for '\0' */
	  entry->val = malloc (size);
	  assert(entry->val);
	  strncpy (entry->val, val, size);

	  this->_length++;
	}
    }
} /* dict_set () */

/**
 * Returns a NULL pointer in case of failure.
 */
char *
dict_get (const dict_t *this, const char *key)
{
  char *val = NULL;

  if (this && key && strlen (key) > 0)
    {
      int idx = dict_indexof (this, key);
      if (idx >= 0)
	{
	  size_t size = strlen (this->entry[idx].val) + 1; /* +1 for '\0' */
	  val = malloc (size);
	  strncpy (val, this->entry[idx].val, size);
	}
    }

  return val;
} /* dict_get () */

bool
dict_has_key (const dict_t *this, const char *key)
{
  return (dict_indexof (this, key) >= 0);
} /* dict_has_key () */

void
dict_remove (dict_t *this, const char *key)
{
  if (this && key)
    {
      int idx = dict_indexof (this, key);
      if (idx >= 0)
	{
	  free (this->entry[idx].key);
	  free (this->entry[idx].val);
	  while ((unsigned int) idx < this->_length - 1)
	    {
	      this->entry[idx].key = this->entry[idx + 1].key;
	      this->entry[idx].val = this->entry[idx + 1].val;
	      idx++;
	    }
	  this->entry[idx].key = NULL;
	  this->entry[idx].val = NULL;
	  this->_length--;
	}
    }
} /* dict_remove () */

dict_iter_t
dict_iter (const dict_t *this)
{
  dict_iter_t iter;
  iter._index = 0;
  iter.dict = this;

  return iter;
} /* dict_iter () */

/**
 * Returns a NULL pointer in case of failure.
 */
const dict_entry_t *
dict_iter_next (dict_iter_t *iter)
{
  const dict_entry_t *entry = NULL;

  if (iter->dict && iter->_index < iter->dict->_length)
    entry = &iter->dict->entry[iter->_index++];

  return entry;
} /* dict_iter_next () */

unsigned int
dict_length (const dict_t *this)
{
  return this->_length;
} /* dict_length () */

char *
dict2str (const dict_t *dict)
{
  char *str = NULL;

  if (dict)
    {
      char *entries = arrentry2str (dict->entry, dict->_length);

      int len = snprintf (str, 0, DICT2STR_FORMAT, dict, dict->_length,
			  dict->_capacity, entries) + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, DICT2STR_FORMAT, dict, dict->_length,
		    dict->_capacity, entries);
	}
    }

  return str;
} /* dict2str () */

/**
 * Returns -1 if the key does not exist in the dictionary.
 */
static int
dict_indexof (const dict_t *this, const char *key)
{
  int idx = -1;

  if (this && key)
    {
      unsigned int i;
      for (i = 0; i < this->_length; i++)
	if (strcmp (key, this->entry[i].key) == 0)
	  idx = i;
    }

  return idx;
} /* dict_indexof () */

/**
 * Implemented as the ensureCapacity method of the ArrayList class in the
 * Java Sun JDK6.
 */
static void
dict_ensure_capacity (dict_t *this, unsigned int capacity)
{
  if (this)
    {
      unsigned int old_capacity = this->_capacity;
      if (capacity > old_capacity)
	{
	  unsigned int new_capacity = (old_capacity * 3) / 2 + 1;
	  if (new_capacity < capacity)
	    new_capacity = capacity;

	  dict_entry_t *old_entry = this->entry;
	  dict_entry_t *new_entry = calloc (new_capacity, sizeof(dict_entry_t));
	  assert(new_entry);
	  memcpy (new_entry, old_entry, this->_length * sizeof(dict_entry_t));
	  this->entry = new_entry;

	  free (old_entry);

	  this->_capacity = new_capacity;
	}
    }
} /* dict_ensure_capacity () */

static char *
dictentry2str (const dict_entry_t *entry)
{
  char *str = NULL;

  if (entry)
    {
      int len = snprintf (str, 0, DICTENTRY2STR_FORMAT, entry->key, entry->val)
	  + 1;
      if (len > 0)
	{
	  str = malloc (len);
	  assert(str);
	  snprintf (str, len, DICTENTRY2STR_FORMAT, entry->key, entry->val);
	}
    }

  return str;
} /* dictentry2str () */

static char *
arrentry2str (const dict_entry_t *entries, int len)
{
  char *str = NULL;

  if (entries && len > 0)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_l[i] = dictentry2str (&entries[i]);
	  swp_len = strlen (swp_l[i]);
	  str_len += swp_len;
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrentry2str () */
