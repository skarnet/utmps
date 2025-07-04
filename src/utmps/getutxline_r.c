/* ISC license. */

#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

int utmps_getutxline_r (struct utmpx const *a, struct utmpx *b, struct utmpx **p)
{
  if (!utmps_here_maybe_init()) return -1 ;
  if (!utmps_getline(&utmps_here, a->ut_line, b, 0, 0)) return -1 ;
  *p = b ;
  return 0 ;
}
