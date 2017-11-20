/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

void setutxent (void)
{
  utmps_here_maybe_init() ;
  utmps_rewind(&utmps_here, 0, 0) ;
}
