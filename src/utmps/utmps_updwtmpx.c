/* ISC license. */

#include <sys/types.h>
#include <errno.h>
#include <skalibs/unix-timed.h>
#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

int utmps_updwtmpx (char const *path, struct utmpx const *b, tain_t const *deadline, tain_t *stamp)
{
  utmps a = UTMPS_ZERO ;
  ssize_t r ;
  char buf[1 + sizeof(struct utmpx)] = "+" ;
  if (!utmps_start(&a, path, deadline, stamp)) return 0 ;
  utmps_utmpx_pack(buf + 1, b) ;
  if (!ipc_timed_send(a.fd, buf, 1 + sizeof(struct utmpx), deadline, stamp)) goto err ;
  r = ipc_timed_recv(a.fd, buf, 1, 0, deadline, stamp) ;
  if (r < 0) goto err ;
  if (!r) { errno = EPIPE ; goto err ; }
  if (buf[0]) { errno = buf[0] ; goto err ; }
  utmps_end(&a) ;
  return 1 ;

 err :
  utmps_end(&a) ;
  return 0 ;
}
