/* ISC license. */

#include <skalibs/djbunix.h>
#include <utmps/utmps.h>

void utmps_end (utmps *a)
{
  fd_close(a->fd) ;
  a->fd = -1 ;
}
