/* ISC license. */

#include <unistd.h>
#include <string.h>
#include <skalibs/tai.h>
#include <utmps/utmpx.h>

void logwtmp (char const *line, char const *name, char const *host)
{
  struct utmpx b ;
  memset(&b, 0, sizeof(struct utmpx)) ;
  strncpy(b.ut_line, line, UTMPS_UT_LINESIZE - 1) ;
  strncpy(b.ut_user, name, UTMPS_UT_NAMESIZE - 1) ;
  strncpy(b.ut_host, host, UTMPS_UT_HOSTSIZE - 1) ;
  b.ut_pid = getpid() ;
  {
    tain now ;
    tain_wallclock_read(&now) ;
    timeval_from_tain(&b.ut_tv, &now) ;
  }
  updwtmpx("", &b) ;
}
