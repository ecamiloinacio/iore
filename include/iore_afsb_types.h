/*
 * iore_afsb_types.h
 *
 * Author: Camilo <eduardo.camilo@posgrad.ufsc.br>
 */

#ifndef INCLUDE_IORE_AFSB_TYPES_H_
#define INCLUDE_IORE_AFSB_TYPES_H_

#include "iore_dict.h"

/*** DEFINES *****************************************************************/

#define AFSB_PARAM_STRIPE_SIZE "stripe_size"
#define AFSB_PARAM_STRIPE_WIDTH "stripe_width"

#define AFSB2STR_FORMAT "iore_afsb_t (%p) { type = '%s', params = %s }"

/*** TYPES *******************************************************************/

enum iore_afsb_type
{
  IORE_AFSB_UNSET, /* no AFSB configuration on tests */
#ifdef WITH_ORANGEFS_AFSB
  IORE_AFSB_ORANGEFS, /* PVFS2/OrangeFS */
#endif
#ifdef WITH_LUSTRE_AFSB
  IORE_AFSB_LUSTRE, /* Lustre */
#endif
  IORE_AFSB_LENGTH
};

typedef struct iore_afsb
{
  enum iore_afsb_type type;
  dict_t params;
} iore_afsb_t;

/*** PROTOTYPES **************************************************************/

char *
afsb2str (const iore_afsb_t *);

/*** VARIABLES ***************************************************************/

extern const char * const afsb_lbl[];

#endif /* INCLUDE_IORE_AFSB_TYPES_H_ */
