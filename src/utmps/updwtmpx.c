/* ISC license. */

#include <utmps/config.h>
#include <utmps/utmps.h>

void updwtmpx (char const *file, struct utmpx const *b)
{
  (void)file ;
  utmps_updwtmpx(UTMPS_WTMPD_PATH, b, 0, 0) ;
}
