/*
 * iore_error.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_ERROR_H_
#define INCLUDE_IORE_ERROR_H_

#include <errno.h>

/*** DEFINES *****************************************************************/

/**
 * Error handling macros
 */
#define iore_warn(reason) handle_warn (0, reason)
#define iore_warnf(format, ...) handle_warnf (0, format, __VA_ARGS__)
#define iore_warn_all(reason) handle_warn(1, reason)
#define iore_warnf_all(format, ...) handle_warnf (1, format, __VA_ARGS__)
#define iore_error(reason) handle_error (0, reason)
#define iore_errorf(format, ...) handle_errorf (0, format, __VA_ARGS__)
#define iore_error_all(reason) handle_error (1, reason)
#define iore_errorf_all(format, ...) handle_errorf (1, format, __VA_ARGS__)
#define iore_fatal(reason) handle_fatal (__FILE__, __LINE__, reason)
#define iore_fatalf(format, ...) handle_fatalf (__FILE__, __LINE__, \
						format, __VA_ARGS__)
#define iore_debug(msg) handle_debug (__FILE__, __LINE__, msg)
#define iore_debugf(format, ...) handle_debugf (__FILE__, __LINE__, format, \
						__VA_ARGS__)

/**
 * Error codes
 */
#define IORE_SUCCESS 0
#define IORE_FAILURE -1

/*** PROTOTYPES **************************************************************/

void
handle_warn (int, const char *);
void
handle_warnf (int, const char *, ...);
void
handle_error (int, const char *);
void
handle_errorf (int, const char *, ...);
void
handle_fatal (const char *, int, const char *);
void
handle_fatalf (const char *, int, const char *, ...);
void
handle_debug (const char *, int, const char *);
void
handle_debugf (const char *, int, const char *, ...);

#endif /* INCLUDE_IORE_ERROR_H_ */
