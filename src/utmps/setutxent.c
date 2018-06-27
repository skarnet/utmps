/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

void setutxent (void)
{
  if (!utmps_here_maybe_init()) return ;
  utmps_rewind(&utmps_here, 0, 0) ;
}
