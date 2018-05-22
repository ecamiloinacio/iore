/*
 * iore_afio_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_AFIO_TYPES_H_
#define INCLUDE_IORE_AFIO_TYPES_H_

#include "iore_dict.h"

/*** DEFINES *****************************************************************/

#define AFIO_PARAM_SEEK_RW_SINGLE_OP "seek_rw_single_op"
#define AFIO_PARAM_COLLECTIVE_IO "collective_io"
#define AFIO_PARAM_FILE_VIEW "file_view"

#define AFIO2STR_FORMAT "iore_afio_t (%p) { type = '%s', params = %s }"

/*** TYPES *******************************************************************/

enum iore_afio_type
{
  IORE_AFIO_POSIX, /* POSIX I/O system calls */
  IORE_AFIO_CSTREAM, /* C standard stream-based I/O functions */
#ifdef WITH_MPIIO_AFIO
  IORE_AFIO_MPIIO, /* MPI-IO functions */
#endif
  IORE_AFIO_LENGTH
};

typedef struct iore_afio
{
  enum iore_afio_type type;
  dict_t params;
} iore_afio_t;

/*** PROTOTYPES **************************************************************/

char *
afio2str (const iore_afio_t *);

/*** VARIABLES ***************************************************************/

extern const char * const afio_lbl[];

#endif /* INCLUDE_IORE_AFIO_TYPES_H_ */
