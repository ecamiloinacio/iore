/*
 * iore_test_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_TEST_TYPES_H_
#define INCLUDE_IORE_TEST_TYPES_H_

#include <stdbool.h>

#include "iore_afio_types.h"
#include "iore_afsb_types.h"
#include "iore_workload_types.h"

/*** DEFINES *****************************************************************/

#define TEST2STR_FORMAT "iore_test_t (%p) { type = { write = '%s', read = '%s' }, write_flush = '%s', write_flush_per_req = '%s', read_reorder_offset = %d, intra_test_barrier = '%s', inter_test_delay_secs = %d, file_mode = '%s', file_name = '%s', file_name_append_sequence_num = '%s', file_name_append_task_id = '%s', file_dir_per_task = '%s', file_keep = '%s', wkld = %s, afio = %s, afsb = %s }"

/*** TYPES *******************************************************************/

enum iore_test_file_mode
{
  IORE_TEST_FMODE_NX1, /* single file shared by all tasks */
  IORE_TEST_FMODE_NXN, /* one independent file for each task */
  IORE_TEST_FMODE_LENGTH
};

enum iore_test_type
{
  IORE_TEST_TYPE_WRITE, IORE_TEST_TYPE_READ, IORE_TEST_TYPE_LENGTH
};

typedef struct iore_test_type_flags
{
  bool write;
  bool read;
} iore_test_type_flags_t;

typedef struct iore_test
{
  iore_test_type_flags_t type;
  bool write_flush;
  bool write_flush_per_req;
  unsigned int read_reorder_offset;
  bool intra_test_barrier;
  unsigned int inter_test_delay_secs;

  enum iore_test_file_mode file_mode;
  char *file_name;
  bool file_name_append_sequence_num;
  bool file_name_append_task_id;
  bool file_dir_per_task;
  bool file_keep;

  iore_wkld_t wkld;
  iore_afio_t afio;
  iore_afsb_t *afsb;
} iore_test_t;

/*** PROTOTYPES **************************************************************/

char *
test2str (const iore_test_t *);

/*** VARIABLES ***************************************************************/

extern const char *test_type_lbl[];
extern const char *test_file_mode_lbl[];

#endif /* INCLUDE_IORE_TEST_TYPES_H_ */
