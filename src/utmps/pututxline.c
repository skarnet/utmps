/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

struct utmpx *pututxline (struct utmpx const *b)
{
  static struct utmpx here ; /* POSIX says we can't use utmps_utmpx_here */
  if (!utmps_here_maybe_init()) return 0 ;
  if (!utmps_putline(&utmps_here, b, 0, 0)) return 0 ;
  here = *b ;
  return &here ;
}
