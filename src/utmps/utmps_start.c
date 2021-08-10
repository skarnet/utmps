/* ISC license. */

#include <skalibs/djbunix.h>
#include <skalibs/socket.h>
#include <utmps/utmps.h>

int utmps_start (utmps *a, char const *path, tain const *deadline, tain *stamp)
{
  int fd = ipc_stream_nbcoe() ;
  if (fd < 0) return 0 ;
  if (!ipc_timed_connect(fd, path, deadline, stamp))
  {
    fd_close(fd) ;
    return 0 ;
  }
  a->fd = fd ;
  return 1 ;
}
