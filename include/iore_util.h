/*
 * iore_util.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_UTIL_H_
#define INCLUDE_IORE_UTIL_H_

#include <stdbool.h>
#include <stddef.h>

/*** DEFINES *****************************************************************/

/**
 * Byte size multipliers.
 */
#define KIBIBYTE (1ull << 10)
#define MEBIBYTE (1ull << 20)
#define GIBIBYTE (1ull << 30)
#define TEBIBYTE (1ull << 40)
#define PEBIBYTE (1ull << 50)

/*** PROTOTYPES **************************************************************/

bool
strtob (const char *);
char *
itoa (int);
char *
zutoa (size_t);
int
shuffle (void *, unsigned int, size_t, unsigned int);
char *
curtimestr ();
void
hrbytesd (double, char *, size_t);
char *
arru2str (const unsigned int *, int);
char *
arrzu2str (const size_t *, int);
char *
arrd2str (const double *, int);
char *
arrs2str (char **, int, size_t);
char *
arrlld2str (long long int *, int);
char *
strfmt (const char *fmt, ...);
char *
coallesce_str (const char **a, int len, char *sep);
char *
coallesce_uint (const unsigned int *a, int len, char *sep);

#endif /* INCLUDE_IORE_UTIL_H_ */
