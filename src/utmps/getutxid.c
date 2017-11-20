/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

struct utmpx *getutxid (struct utmpx const *b)
{
  utmps_here_maybe_init() ;
  if (!utmps_getid(&utmps_here, (unsigned short)b->ut_type, b->ut_id, &utmps_utmpx_here, 0, 0)) return 0 ;
  return &utmps_utmpx_here ;
}
