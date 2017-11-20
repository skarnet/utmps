/* ISC license. */

#include <string.h>
#include <utmps/utmpx.h>

void utmps_utmpx_pack (char *s, struct utmpx const *u)
{
  memcpy(s, u, sizeof(struct utmpx)) ;
}
