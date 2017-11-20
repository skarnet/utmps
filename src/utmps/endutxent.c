/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

void endutxent (void)
{
  utmps_end(&utmps_here) ;
}
