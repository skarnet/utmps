/* ISC license. */

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <skalibs/unix-timed.h>
#include <utmps/utmpx.h>
#include <utmps/utmps.h>
#include "utmps-internal.h"

int utmps_getline (utmps *a, char const *line, struct utmpx *b, tain_t const *deadline, tain_t *stamp)
{
  ssize_t r ;
  char sbuf[1 + UTMPS_UT_LINESIZE] = "l" ;
  char rbuf[1 + sizeof(struct utmpx)] ;
  memset(sbuf + 1, 0, UTMPS_UT_LINESIZE) ;
  strncpy(sbuf + 1, line, UTMPS_UT_LINESIZE - 1) ;
  if (!ipc_timed_send(a->fd, sbuf, sizeof(sbuf), deadline, stamp)) return 0 ;
  r = ipc_timed_recv(a->fd, rbuf, sizeof(rbuf), 0, deadline, stamp) ;
  if (r < 0) return 0 ;
  if (!r) return (errno = EPIPE, 0) ;
  if (rbuf[0]) return (errno = rbuf[0], 0) ;
  utmps_utmpx_unpack(rbuf + 1, b) ;
  return 1 ;
}
