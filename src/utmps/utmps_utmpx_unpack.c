/* ISC license. */

#include <string.h>
#include <utmps/utmpx.h>

void utmps_utmpx_unpack (char const *s, struct utmpx *b)
{
  memcpy(b, s, sizeof(struct utmpx)) ;
  b->ut_user[UTMPS_UT_NAMESIZE - 1] = 0 ;
  b->ut_line[UTMPS_UT_LINESIZE - 1] = 0 ;
  b->ut_host[UTMPS_UT_HOSTSIZE - 1] = 0 ;
}
