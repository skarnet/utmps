/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

struct utmpx *getutxline (struct utmpx const *b)
{
  utmps_here_maybe_init() ;
  if (!utmps_getline(&utmps_here, b->ut_line, &utmps_utmpx_here, 0, 0)) return 0 ;
  return &utmps_utmpx_here ;
}
