/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

struct utmpx *getutxent (void)
{
  utmps_here_maybe_init() ;
  if (!utmps_getent(&utmps_here, &utmps_utmpx_here, 0, 0)) return 0 ;
  return &utmps_utmpx_here ;
}
