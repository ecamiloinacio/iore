/*
 * test_iore_dict.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "iore_dict.h"

void
test_constructors ();
void
test_set_get_lookup_iterate ();
void
test_to_string ();

int
main ()
{
  fputs ("Initializing Dictionary test...\n", stdout);

  fputs ("Test #1: Constructors: ...", stdout);
  test_constructors ();
  fputs ("SUCCESS!\n", stdout);

  fputs ("Test #2: Set, Get, Lookup, Iterate: ...\n", stdout);
  test_set_get_lookup_iterate ();
  fputs ("SUCCESS!\n", stdout);

  fputs ("Test #3: To String: ...\n", stdout);
  test_to_string ();
  fputs ("SUCCESS!\n", stdout);

  fputs ("Finalizing Dictionary test.\n", stdout);
} /* main () */

void
test_constructors ()
{
  dict_t a = dict_new ();
  assert(a._capacity == 3);
  assert(a._length == 0);
  assert(a.entry);

  dict_t b = dict_newc (5);
  assert(b._capacity == 5);
  assert(b._length == 0);
  assert(b.entry);

  dict_t c;
  dict_init (&c);
  assert(c._capacity == 3);
  assert(c._length == 0);
  assert(c.entry);

  dict_t d;
  dict_initc (&d, 7);
  assert(d._capacity == 7);
  assert(d._length == 0);
  assert(d.entry);

  dict_t *e = malloc (sizeof(dict_t));
  dict_init (e);
  assert(e->_capacity == 3);
  assert(e->_length == 0);
  assert(e->entry);

  dict_t *f = malloc (sizeof(dict_t));
  dict_initc (f, 10);
  assert(f->_capacity == 10);
  assert(f->_length == 0);
  assert(f->entry);

  dict_free (&a);
  dict_free (&b);
  dict_free (&c);
  dict_free (&d);
  dict_free (e);
  free (e);
  dict_free (f);
  free (f);
} /* test_constructors () */

void
test_set_get_lookup_iterate ()
{
  dict_iter_t iter;
  const dict_entry_t *entry;

  dict_t a = dict_new ();
  dict_set (&a, "Key 1", "Val 1");
  dict_set (&a, "Key 2", "Val 2 Longer than Val 1");
  dict_set (&a, "Key 3", "Val 3 Even Longer than Val 1");
  dict_set (&a, "Key 4",
	    "Key/Val 4 requires the dictionary capacity to be increased");

  dict_t *b = malloc (sizeof(dict_t));
  dict_initc (b, dict_length (&a) + 10);
  dict_set (b, "Key 1", dict_get (&a, "Key 4"));
  dict_set (b, "Key 2", dict_get (&a, "Key 3"));
  dict_set (b, "Key 3", dict_get (&a, "Key 2"));
  dict_set (b, "Key 4", dict_get (&a, "Key 1"));

  fputs ("Dictionary a {\n", stdout);
  iter = dict_iter (&a);
  while ((entry = dict_iter_next (&iter)) != NULL)
    fprintf (stdout, "\t %s = %s\n", entry->key, entry->val);
  fputs ("}\n", stdout);

  fputs ("Dictionary b {\n", stdout);
  iter = dict_iter (b);
  while ((entry = dict_iter_next (&iter)))
    fprintf (stdout, "\t %s = %s\n", entry->key, entry->val);
  fputs ("}\n", stdout);
  fprintf (stdout, "Dictionary b %s the key = 'Key 2'\n",
	   (dict_has_key (b, "Key 2") ? "has" : "has not"));

  fputs ("Changing dictionary b:\n", stdout);
  fputs ("\t changing 'Key 3' value...\n", stdout);
  dict_set (b, "Key 3", "Val 3 has changed in dictionary b.");
  fputs ("\t removing 'Key 2'...\n", stdout);
  dict_remove (b, "Key 2");

  fputs ("Dictionary a {\n", stdout);
  iter = dict_iter (&a);
  while ((entry = dict_iter_next (&iter)))
    fprintf (stdout, "\t %s = %s\n", entry->key, entry->val);
  fputs ("}\n", stdout);

  fputs ("Dictionary b {\n", stdout);
  iter = dict_iter (b);
  while ((entry = dict_iter_next (&iter)))
    fprintf (stdout, "\t %s = %s\n", entry->key, entry->val);
  fputs ("}\n", stdout);
  fprintf (stdout, "Dictionary b %s the key = 'Key 2'\n",
	   (dict_has_key (b, "Key 2") ? "has" : "has not"));

  dict_free (&a);
  dict_free (b);
  free (b);
} /* test_set_get_lookup_iterate () */

void
test_to_string ()
{
  dict_t a = dict_new ();
  dict_set (&a, "Key 1", "Val 1");
  dict_set (&a, "Key 2", "Val 2 Longer than Val 1");
  dict_set (&a, "Key 3", "Val 3 Even Longer than Val 1");
  dict_set (&a, "Key 4", "Key/Val 4 requires the capacity to be increased");

  char *str = dict2str (&a);
  fprintf (stdout, "%s\n", str);

  dict_free (&a);
} /* test_to_string () */
