/* ISC license. */

#include <utmps/config.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

int utmps_here_maybe_init (void)
{
  return utmps_here.fd >= 0 || utmps_start(&utmps_here, UTMPS_UTMPD_PATH, 0, 0) ;
}
