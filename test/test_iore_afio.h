/*
 * test_iore_afio.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef TEST_TEST_IORE_AFIO_H_
#define TEST_TEST_IORE_AFIO_H_

#include "iore_test_types.h"
#include "iore_file.h"
#include "iore_afio.h"

/**
 * ATTENTION: test designed for at most two processes.
 */
#define MAX_PROCESSES 2
#define TEST_FILE_NAME "/tmp/testfile"
#define BASE_DATA_SIZE 1024
#define BASE_REQ_SIZE 384
#define NUM_VARS 5
#define NUM_DIMS 3

iore_test_t
get_sample_oset_test ();
iore_test_t
get_sample_dset_test ();
int
fill_buffer (char **buf, size_t size);
int
fill_offsets (off_t **, size_t, size_t, enum iore_test_file_mode);
int
test_wr_oset (const iore_afio_vtable_t *, iore_file_t, iore_test_t);
int
test_wr_dset (const iore_afio_vtable_t *, iore_file_t, iore_test_t);

#endif /* TEST_TEST_IORE_AFIO_H_ */
