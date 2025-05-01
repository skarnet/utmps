/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

int utmps_getutxid_r (struct utmpx const *a, struct utmpx *b, struct utmpx **p)
{
  if (!utmps_here_maybe_init()) return -1 ;
  if (!utmps_getid(&utmps_here, (unsigned short)a->ut_type, a->ut_id, b, 0, 0)) return -1 ;
  *p = b ;
  return 0 ;
}
