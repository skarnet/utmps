/* ISC license. */

#include <string.h>
#include <utmps/utmpx.h>

void utmps_utmpx_unpack (char const *s, struct utmpx *b)
{
  memcpy(b, s, sizeof(struct utmpx)) ;
}
