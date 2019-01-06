/* ISC license. */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <skalibs/types.h>
#include <skalibs/error.h>
#include <skalibs/allreadwrite.h>
#include <skalibs/buffer.h>
#include <skalibs/strerr2.h>
#include <skalibs/env.h>
#include <skalibs/tai.h>
#include <skalibs/djbunix.h>
#include <skalibs/unix-timed.h>
#include <utmps/utmpx.h>
#include "utmps-internal.h"

static void answer (int e)
{
  unsigned char c = e ;
  write(1, (char *)&c, 1) ;
}

int main (void)
{
  struct utmpx b ;
  char const *x ;
  tain_t deadline ;
  size_t w ;
  uid_t uid ;
  int fd ;
  char buf[sizeof(struct utmpx)] ;
  PROG = "utmps-wtmpd" ;

  x = ucspi_get("REMOTEEUID") ;
  if (!x) strerr_diefu1x(100, "get $IPCREMOTEEUID from environment") ;
  if (!uid0_scan(x, &uid)) strerr_dieinvalid(100, "IPCREMOTEEUID") ;
  if (ndelay_on(0) < 0) strerr_diefu1sys(111, "set stdin non-blocking") ;
  tain_now_g() ;
  tain_ulong(&deadline, 30) ;
  tain_add_g(&deadline, &deadline) ;

  w = buffer_timed_get_g(buffer_0small, buf, 1, &deadline) ;
  if (!w) strerr_diefu1sys(111, "read from stdin") ;
  if (buf[0] != '+') { errno = EPROTO ; strerr_diefu1sys(111, "read command") ; }
  w = buffer_timed_get_g(buffer_0small, buf, sizeof(struct utmpx), &deadline) ; 
  if (w < sizeof(struct utmpx)) strerr_diefu1sys(111, "read from stdin") ;
  utmps_utmpx_unpack(buf, &b) ;
  if (uid)
  {
    struct passwd *pw ;
    errno = 0 ;
    pw = getpwnam(b.ut_user) ;
    if (!pw)
    {
      if (errno)
      {
        answer(errno) ;
        strerr_diefu1sys(111, "read user database") ;
      }
      else
      {
        answer(EPERM) ;
        strerr_diefu2x(1, "verify ut_user identity", ": no such user") ;
      }
    }
    if (pw->pw_uid != uid)
    {
      answer(EPERM) ;
      strerr_diefu2x(1, "verify ut_user identity", ": uid mismatch") ;
    }
  }
  
  fd = open_append("wtmp") ;
  if (fd < 0)
  {
    answer(errno) ;
    strerr_diefu1sys(111, "open wtmp") ;
  }
  if (lock_ex(fd) < 0)
  {
    answer(errno) ;
    strerr_diefu1sys(111, "open wtmp") ;
  }
  if (lseek(fd, 0, SEEK_END) < 0)
  {
    answer(errno) ;
    strerr_diefu1sys(111, "lseek on wtmp") ;
  }
  w = allwrite(fd, buf, sizeof(struct utmpx)) ;
  if (w < sizeof(struct utmpx))
  {
    int e = errno ;
    if (w)
    {
      struct stat st ;
      if (!fstat(fd, &st)) ftruncate(fd, st.st_size - w) ;
    }
    answer(e) ;
    strerr_diefu1sys(111, "append to wtmp") ;
  }
  fsync(fd) ;
  answer(0) ;
  return 0 ;
}
