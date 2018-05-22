/*
 * iore_parser_file_json_attr.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_PARSER_FILE_JSON_ATTR_H_
#define INCLUDE_IORE_PARSER_FILE_JSON_ATTR_H_

/*** DEFINES *****************************************************************/

#define JSON_ATTR_STEX "stats_exporter"
#define JSON_ATTR_STEX_REPORT_TYPE "report_type"
#define JSON_ATTR_STEX_DATA_FORMAT "data_format"
#define JSON_ATTR_STEX_EXPORT_DIR "export_dir"
#define JSON_ATTR_NUM_REPLICATIONS "num_replications"
#define JSON_ATTR_RUN_ORDER "run_order"
#define JSON_ATTR_RUNS "runs"
#define JSON_ATTR_RUN_NUM_REPETITIONS "num_repetitions"
#define JSON_ATTR_TEST "test"
#define JSON_ATTR_TEST_TYPE "type"
#define JSON_ATTR_TEST_WRITE_FLUSH "write_flush"
#define JSON_ATTR_TEST_WRITE_FLUSH_PER_REQ "write_flush_per_req"
#define JSON_ATTR_TEST_READ_REORDER_OFFSET "read_reorder_offset"
#define JSON_ATTR_TEST_INTRA_TEST_BARRIER "intra_test_barrier"
#define JSON_ATTR_TEST_INTER_TEST_DELAY_SECS "inter_test_delay_secs"
#define JSON_ATTR_TEST_FILE_MODE "file_mode"
#define JSON_ATTR_TEST_FILE_NAME "file_name"
#define JSON_ATTR_TEST_FILE_NAME_APPEND_SEQ_NUM "file_name_append_sequence_num"
#define JSON_ATTR_TEST_FILE_NAME_APPEND_TASK_ID "file_name_append_task_id"
#define JSON_ATTR_TEST_FILE_DIR_PER_TASK "file_dir_per_task"
#define JSON_ATTR_TEST_FILE_KEEP "file_keep"
#define JSON_ATTR_WKLD "workload"
#define JSON_ATTR_WKLD_NUM_TASKS "num_tasks"
#define JSON_ATTR_WKLD_OSET_AC_PATTERN "access_pattern"
#define JSON_ATTR_WKLD_OSET_DATA_SIZES "data_sizes"
#define JSON_ATTR_WKLD_OSET_REQ_SIZES "request_sizes"
#define JSON_ATTR_WKLD_OSET_DATA_SIZE_DISTRIB "data_size_distrib"
#define JSON_ATTR_WKLD_OSET_REQ_SIZE_DISTRIB "request_size_distrib"
#define JSON_ATTR_WKLD_DSET_NUM_VARS "num_vars"
#define JSON_ATTR_WKLD_DSET_VAR_TYPES "var_types"
#define JSON_ATTR_WKLD_DSET_CART_NUM_DIMS "num_dims"
#define JSON_ATTR_WKLD_DSET_CART_G_DIM_SIZES "dim_sizes"
#define JSON_ATTR_WKLD_DSET_CART_G_DIM_DIVS "dim_divs"
#define JSON_ATTR_AFIO "afio"
#define JSON_ATTR_AFSB "afsb"

#endif /* INCLUDE_IORE_PARSER_FILE_JSON_ATTR_H_ */
