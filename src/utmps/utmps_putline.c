/* ISC license. */

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <skalibs/unix-timed.h>
#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

int utmps_putline (utmps *a, struct utmpx const *b, tain const *deadline, tain *stamp)
{
  ssize_t r ;
  char buf[1 + sizeof(struct utmpx)] = "E" ;
  utmps_utmpx_pack(buf + 1, b) ;
  if (!ipc_timed_send(a->fd, buf, sizeof(buf), deadline, stamp)) return 0 ;
  r = ipc_timed_recv(a->fd, buf, 1, 0, deadline, stamp) ;
  if (r < 0) return 0 ;
  if (!r) return (errno = EPIPE, 0) ;
  if (buf[0]) return (errno = (unsigned char)buf[0], 0) ;
  return 1 ;
}
