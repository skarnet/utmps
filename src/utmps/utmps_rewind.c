/* ISC license. */

#include <sys/types.h>
#include <errno.h>
#include <skalibs/unix-timed.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

int utmps_rewind (utmps *a, tain const *deadline, tain *stamp)
{
  ssize_t r ;
  char c ;
  if (!ipc_timed_send(a->fd, "r", 1, deadline, stamp)) return 0 ;
  r = ipc_timed_recv(a->fd, &c, 1, 0, deadline, stamp) ;
  if (r < 0) return 0 ;
  if (!r) return (errno = EPIPE, 0) ;
  if (c) return (errno = c, 0) ;
  return 1 ;
}
