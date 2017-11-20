/* ISC license. */

#include <utmps/config.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

void utmps_here_maybe_init (void)
{
  if (utmps_here.fd < 0)
    utmps_start(&utmps_here, UTMPS_UTMPD_PATH, 0, 0) ;
}
