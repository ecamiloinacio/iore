/*
 * iore_trec_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_TREC_TYPES_H_
#define INCLUDE_IORE_TREC_TYPES_H_

#include "iore_test_types.h"

/*** DEFINES *****************************************************************/

#define TRECEXP2STR_FORMAT "iore_trec_exp_t (%p) { time = [ %s ], num_exp_rpl = %u, exp_repl = [ %s ] }"
#define TRECEXPREPL2STR_FORMAT "iore_trec_exp_repl_t (%p) { id = %u, time = [ %s ], num_run = %u, run = [ %s ] }"
#define TRECRUN2STR_FORMAT "iore_trec_run_t (%p) { id = %u, time = [ %s ], num_run_rept = %u, run_rept = [ %s ] }"
#define TRECRUNREPT2STR_FORMAT "iore_trec_run_rept_t (%p) { id = %u, time [ %s ], test = [ %s ] }"
#define TRECTEST2STR_FORMAT "iore_trec_test_t (%p) { type = '%s', task_time = [ %s ], task_data_size = %zu, test_time = [ %s ], test_data_size = %zu, committed = '%s' }"

/*** TYPES *******************************************************************/

typedef double iore_time_t;

enum iore_trec_event
{
  IORE_TREC_EVENT_START, IORE_TREC_EVENT_STOP, IORE_TREC_EVENT_LENGTH
};

enum iore_trec_action
{
  IORE_TREC_ACTION_CREATE_OPEN,
  IORE_TREC_ACTION_WRITE_READ,
  IORE_TREC_ACTION_CLOSE,
  IORE_TREC_ACTION_REMOVE,
  IORE_TREC_ACTION_LENGTH
};

typedef struct iore_trec_record_test
{
  enum iore_test_type type;

  iore_time_t task_time[IORE_TREC_ACTION_LENGTH][IORE_TREC_EVENT_LENGTH];
  size_t task_data_size;

  iore_time_t test_time[IORE_TREC_ACTION_LENGTH][IORE_TREC_EVENT_LENGTH];
  size_t test_data_size;

  bool committed; /* denotes all actions and events were recorded */
} iore_trec_test_t;

typedef struct iore_trec_record_run_rept
{
  unsigned int id;

  iore_time_t time[IORE_TREC_EVENT_LENGTH];

  iore_trec_test_t test[IORE_TEST_TYPE_LENGTH];
} iore_trec_run_rept_t;

typedef struct iore_trec_record_run
{
  unsigned int id;

  iore_time_t time[IORE_TREC_EVENT_LENGTH];

  unsigned int num_run_rept;
  iore_trec_run_rept_t *run_rept;
} iore_trec_run_t;

typedef struct iore_trec_record_exp_repl
{
  unsigned int id;

  iore_time_t time[IORE_TREC_EVENT_LENGTH];

  unsigned int num_run;
  iore_trec_run_t *run;
} iore_trec_exp_repl_t;

typedef struct iore_trec_record_exp
{
  iore_time_t time[IORE_TREC_EVENT_LENGTH];

  unsigned int num_exp_repl;
  iore_trec_exp_repl_t *exp_repl;
} iore_trec_exp_t;

typedef struct iore_trec_state
{
  iore_trec_exp_repl_t *trec_repl;
  iore_trec_run_t *trec_run;
  iore_trec_run_rept_t *trec_rept;
  iore_trec_test_t *trec_test;
  enum iore_trec_action test_action;
} iore_trec_st_t;

/*** PROTOTYPES **************************************************************/

char *
trecexp2str (const iore_trec_exp_t *);
char *
trecexprepl2str (const iore_trec_exp_repl_t *);
char *
trecrun2str (const iore_trec_run_t *);
char *
trecrunrept2str (const iore_trec_run_rept_t *);
char *
trectest2str (const iore_trec_test_t *);

/*** VARIABLES ***************************************************************/

extern const char * const trec_event_lbl[];
extern const char * const trec_action_lbl[];

#endif /* INCLUDE_IORE_TREC_TYPES_H_ */
