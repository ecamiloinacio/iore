/*
 * iore_util.c
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>

#include "iore_util.h"
#include "iore_prng.h"
#include "iore_error.h"

/*** DEFINES *****************************************************************/

#define CURTIMESTR_MAX_SIZE 25

/*** PROTOTYPES ***************************************************************/

static char *
vstrfmt (const char *fmt, va_list ap);

/*** FUNCTIONS ***************************************************************/

bool
strtob (const char *str)
{
  bool b = (str && strcasecmp (str, "true") == 0);
  return b;
} /* strtob () */

char *
itoa (int i)
{
  char *a = NULL;
  int nbytes = snprintf(a, 0, "%d", i);
  if (nbytes > 0)
    {
      a = malloc (nbytes + 1);
      assert(a);
      snprintf(a, nbytes + 1, "%d", i);
    }

  return a;
} /* itoa () */

char *
zutoa (size_t zu)
{
  char *a = NULL;
  int nbytes = snprintf(a, 0, "%zu", zu);
  if (nbytes > 0)
    {
      a = malloc (nbytes + 1);
      assert(a);
      snprintf(a, nbytes + 1, "%zu", zu);
    }

  return a;
} /* zutoa () */

int
shuffle (void *ptr, unsigned int n, size_t size, unsigned int seed)
{
  int rerr = IORE_SUCCESS;

  void *swp = malloc (size);
  assert(swp);

  iore_prng_dist_t dist;
  dist.type = IORE_PRNG_UNIF;
  dict_initc (&dist.params, 2);
  dict_set (&dist.params, PRNG_PARAM_MIN, "0");
  dict_set (&dist.params, PRNG_PARAM_MAX, itoa ((int) n));
  iore_prng_t *prng = prng_new_seed (&dist, seed);
  dict_free (&dist.params);

  if (prng)
    {
      unsigned int i, j;
      for (i = 0; i < n; i++)
	{
	  j = prng_next_uint (prng);
	  if (j != i)
	    {
	      memcpy(swp, ptr + j * size, size);
	      memcpy(ptr + j * size, ptr + i * size, size);
	      memcpy(ptr + i * size, swp, size);
	    }
	}
      free (prng);
    }
  else
    {
      rerr = IORE_FAILURE;
      iore_fatal("PRNG failed.");
    }

  free (swp);

  return rerr;
} /* shuffle () */

char *
curtimestr ()
{
  char *curtimestr = malloc (CURTIMESTR_MAX_SIZE);
  assert(curtimestr);

  time_t t = time (NULL);
  if (t == -1)
    iore_fatal("time() failed.");

  struct tm *tm = localtime (&t);
  if (tm == NULL)
    iore_fatal("locatime() failed.");

  if (strftime (curtimestr, CURTIMESTR_MAX_SIZE, "%c", tm) == 0)
    iore_fatal("strftime() failed.");

  return curtimestr;
} /* curtimestr () */

void
hrbytesd (double bytes, char *hr, size_t len)
{
  assert(hr);

  unsigned long long div = 1;
  char *unit = "B/s";
  if (bytes >= PEBIBYTE)
    {
      div = PEBIBYTE;
      unit = "PiB/s";
    }
  else if (bytes >= TEBIBYTE)
    {
      div = TEBIBYTE;
      unit = "TiB/s";
    }
  else if (bytes >= GIBIBYTE)
    {
      div = GIBIBYTE;
      unit = "GiB/s";
    }
  else if (bytes >= MEBIBYTE)
    {
      div = MEBIBYTE;
      unit = "MiB/s";
    }
  else if (bytes >= KIBIBYTE)
    {
      div = KIBIBYTE;
      unit = "KiB/s";
    }
  snprintf(hr, len, "%7.3f %s", bytes / div, unit);
} /* hrbytesd () */

char *
arru2str (const unsigned int *arru, int len)
{
  char *str = NULL;

  if (arru)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_len = snprintf(swp_l[i], 0, "%u", arru[i]);
	  if (swp_len > 0)
	    {
	      swp_l[i] = malloc (swp_len + 1);
	      assert(swp_l[i]);
	      snprintf(swp_l[i], swp_len + 1, "%u", arru[i]);
	      str_len += swp_len;
	    }
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arru2str () */

char *
arrzu2str (const size_t *arrzu, int len)
{
  char *str = NULL;

  if (arrzu)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_l[i] = zutoa (arrzu[i]);
	  swp_len = strlen (swp_l[i]);
	  str_len += swp_len;
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrzu2str () */

char *
arrd2str (const double *arrd, int len)
{
  char *str = NULL;

  if (arrd)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_len = snprintf(swp_l[i], 0, "%lf", arrd[i]);
	  if (swp_len > 0)
	    {
	      swp_l[i] = malloc (swp_len + 1);
	      assert(swp_l[i]);
	      snprintf(swp_l[i], swp_len + 1, "%lf", arrd[i]);
	      str_len += swp_len;
	    }
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrd2str () */

char *
arrs2str (char **arrs, int len, size_t str_len)
{
  char *str = NULL;

  if (arrs)
    {
      str_len += 1 + ((len - 1) * 2);
      str = calloc (1, str_len);
      assert(str);
      char *str_ptr = str;
      int i;
      for (i = 0; i < len; i++)
	{
	  if (i > 0)
	    {
	      *str_ptr = ',';
	      str_ptr++;
	      *str_ptr = ' ';
	      str_ptr++;
	    }

	  size_t s_len = strlen (arrs[i]);
	  strncpy(str_ptr, arrs[i], s_len);
	  str_ptr += s_len;
	}
    }

  return str;
} /* arrs2str () */

char *
arrlld2str (long long int *arrlld, int len)
{
  char *str = NULL;

  if (arrlld)
    {
      size_t str_len = 0;

      char **swp_l = malloc (len * sizeof(char *));
      assert(swp_l);
      size_t swp_len;
      int i;
      for (i = 0; i < len; i++)
	{
	  swp_len = snprintf(swp_l[i], 0, "%lld", arrlld[i]);
	  if (swp_len > 0)
	    {
	      swp_l[i] = malloc (swp_len + 1);
	      assert(swp_l[i]);
	      snprintf(swp_l[i], swp_len + 1, "%lld", arrlld[i]);
	      str_len += swp_len;
	    }
	}

      str = arrs2str (swp_l, len, str_len);
      for (i = 0; i < len; i++)
	free (swp_l[i]);
      free (swp_l);
    }

  return str;
} /* arrlld2str () */

char *
strfmt (const char *fmt, ...)
{
  va_list ap;
  char *str;

  va_start (ap, fmt);
  str = vstrfmt (fmt, ap);
  va_end (ap);

  return str;
} /* strfmt () */

char *
coallesce_str (const char **a, int len, char *sep)
{
  char *str = NULL;
  int ttl_len, sep_len, i;

  if (a && len > 0 && sep)
    {
      ttl_len = 0;
      sep_len = strlen (sep);

      for (i = 0; i < len; i++)
	ttl_len += strlen (a[i]);
      ttl_len += (len - 1) * sep_len; /* for one separator between every two elements */
      ttl_len++; /* for the terminating NUL character */

      str = malloc (ttl_len);
      assert(str);

      str = stpncpy(str, a[0], strlen (a[0]));
      for (i = 1; i < len; i++)
	{
	  str = stpncpy(str, sep, sep_len);
	  str = stpncpy(str, a[0], strlen (a[0]));
	}
    }

  return &str[0];
} /* coallesce_str () */

char *
coallesce_uint (const unsigned int *a, int len, char *sep)
{
  char *str = NULL;
  char **str_a;
  int i;

  if (a && len > 0 && sep)
    {
      str_a = malloc (len * sizeof(char *));
      assert(str_a);
      for (i = 0; i < len; i++)
	str_a[i] = strfmt ("%u", a[i]);

      str = coallesce_str (str_a, len, sep);

      for (i = 0; i < len; i++)
	free (str_a[i]);
      free (str_a);
    }

  return str;
} /* coallesce_uint ()

/*** STATIC FUNCTIONS *********************************************************/

static char *
vstrfmt (const char *fmt, va_list ap)
{
  va_list copy;
  char *str;
  int len;

  va_copy(copy, ap);
  len = vsnprintf(str, 0, fmt, copy);
  va_end(copy);
  if (len > 0)
    {
      len++;
      str = pioss_malloc (len);
      vsnprintf(str, len, fmt, ap);
    }

  return str;
} /* vstrfmt () */
