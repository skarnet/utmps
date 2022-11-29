/* ISC license. */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>

#include <skalibs/posixishard.h>
#include <skalibs/types.h>
#include <skalibs/allreadwrite.h>
#include <skalibs/buffer.h>
#include <skalibs/strerr.h>
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

int main (int argc, char const *const *argv)
{
  struct utmpx b ;
  char const *file = "wtmp" ;
  char const *x ;
  tain deadline ;
  size_t w ;
  uid_t uid ;
  int fd ;
  char buf[sizeof(struct utmpx)] ;
  PROG = "utmps-wtmpd" ;

  x = ucspi_get("REMOTEEUID") ;
  if (!x) strerr_diefu1x(100, "get PROTO or IPCREMOTEEUID from environment") ;
  if (!uid0_scan(x, &uid)) strerr_dieinvalid(100, "IPCREMOTEEUID") ;
  if (ndelay_on(0) < 0) strerr_diefu1sys(111, "set stdin non-blocking") ;
  if (argc >= 2 && argv[1]) file = argv[1] ;
  tain_now_set_stopwatch_g() ;
  tain_ulong(&deadline, 30) ;
  tain_add_g(&deadline, &deadline) ;

  w = buffer_timed_get_g(buffer_0small, buf, 1, &deadline) ;
  if (!w) strerr_diefu1sys(111, "read from stdin") ;
  if (buf[0] != '+') { errno = EPROTO ; strerr_diefu1sys(111, "read command") ; }
  w = buffer_timed_get_g(buffer_0small, buf, sizeof(struct utmpx), &deadline) ; 
  if (w < sizeof(struct utmpx))
  {
    errno = EPIPE ;
    strerr_diefu1sys(111, "read from stdin") ;
  }
  utmps_utmpx_unpack(buf, &b) ;
  b.ut_user[UTMPS_UT_NAMESIZE - 1] = 0 ;
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
        strerr_diefu2x(1, "verify ut_user identity: ", "no such user") ;
      }
    }
    if (pw->pw_uid != uid)
    {
      answer(EPERM) ;
      strerr_diefu2x(1, "verify ut_user identity: ", "uid mismatch") ;
    }
  }
  
  fd = open_append(file) ;
  if (fd < 0)
  {
    answer(errno) ;
    strerr_diefu2sys(111, "open ", file) ;
  }
  if (fd_lock(fd, 1, 0) < 1)
  {
    answer(errno) ;
    strerr_diefu2sys(111, "lock ", file) ;
  }
  if (lseek(fd, 0, SEEK_END) < 0)
  {
    answer(errno) ;
    strerr_diefu2sys(111, "lseek on ", file) ;
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
    fd_unlock(fd) ;
    answer(e) ;
    errno = e ;
    strerr_diefu2sys(111, "append to ", file) ;
  }
  fsync(fd) ;
  fd_unlock(fd) ;
  answer(0) ;
  return 0 ;
}
