/*
 * iore_stex_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_STEX_TYPES_H_
#define INCLUDE_IORE_STEX_TYPES_H_

#include <stdbool.h>

/*** DEFINES *****************************************************************/

#define STEX2STR_FORMAT "iore_stex_t (%p) { report_type = { task = '%s', test = '%s' }, data_format = '%s', export_dir = '%s' }"

/*** TYPES *******************************************************************/

enum iore_stex_format
{
  IORE_STEX_FORMAT_CSV, /* comma-separated values format */
  IORE_STEX_FORMAT_LENGTH
};

enum iore_stex_report_type
{
  IORE_STEX_REPORT_TASK, /* statistics per task */
  IORE_STEX_REPORT_TEST, /* statistics per write/read test */
  IORE_STEX_REPORT_LENGTH
};

typedef struct iore_stex_report_type_flags
{
  bool task; /* statistics per task */
  bool test; /* statistics per write/read test */
} iore_stex_report_type_flags_t;

typedef struct iore_stex
{
  iore_stex_report_type_flags_t report_type;
  enum iore_stex_format data_format;
  char *export_dir;
} iore_stex_t;

/*** PROTOTYPES **************************************************************/

char *
stex2str (const iore_stex_t *);

/*** VARIABLES ***************************************************************/

extern const char * const stex_format_lbl[];
extern const char * const stex_report_lbl[];

#endif /* INCLUDE_IORE_STEX_TYPES_H_ */
