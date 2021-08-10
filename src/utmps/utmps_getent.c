/* ISC license. */

#include <sys/types.h>
#include <errno.h>
#include <skalibs/unix-timed.h>
#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

int utmps_getent (utmps *a, struct utmpx *b, tain const *deadline, tain *stamp)
{
  ssize_t r ;
  char buf[1 + sizeof(struct utmpx)] ;
  if (!ipc_timed_send(a->fd, "e", 1, deadline, stamp)) return 0 ;
  r = ipc_timed_recv(a->fd, buf, sizeof(buf), 0, deadline, stamp) ;
  if (r < 0) return 0 ;
  if (!r) return (errno = EPIPE, 0) ;
  if (buf[0]) return (errno = buf[0], 0) ;
  utmps_utmpx_unpack(buf + 1, b) ;
  return 1 ;
}
