/* ISC license. */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <skalibs/types.h>
#include <skalibs/env.h>
#include <skalibs/error.h>
#include <skalibs/allreadwrite.h>
#include <skalibs/buffer.h>
#include <skalibs/strerr2.h>
#include <skalibs/tai.h>
#include <skalibs/djbunix.h>
#include <skalibs/unix-timed.h>
#include <utmps/utmpx.h>
#include "utmps-internal.h"

static int fd = -1 ;

static void get0 (char *s, size_t n)
{
  tain_t deadline ;
  tain_ulong(&deadline, 30) ;
  tain_add_g(&deadline, &deadline) ;
  if (buffer_timed_get_g(buffer_0small, s, n, &deadline) < n)
    strerr_diefu1sys(111, "read from stdin") ;
}

static void flush1 (void)
{
  tain_t deadline ;
  tain_ulong(&deadline, 30) ;
  tain_add_g(&deadline, &deadline) ;
  if (!buffer_timed_flush_g(buffer_1small, &deadline))
    strerr_diefu1sys(111, "write to stdout") ;
}

static void answer (int e)
{
  unsigned char c = e ;
  buffer_putnoflush(buffer_1small, (char *)&c, 1) ;
  flush1() ;
}

static void maybe_open (void)
{
  if (fd < 0)
  {
    mode_t m = umask(0) ;
    fd = open("utmp", O_RDWR | O_CREAT, 0644) ;
    umask(m) ;
    if (fd < 0)
    {
      int e = errno ;
      answer(e) ;
      errno = e ;
      strerr_diefu1sys(111, "open utmp file") ;
    }
  }
}

static int read_utmp_entry (char *s)
{
  ssize_t r ;
  int e ;
  if (lock_sh(fd) < 0) { e = errno ; goto err ; }
  r = read(fd, s, sizeof(struct utmpx)) ;
  lock_unx(fd) ;
  if (r < 0) { e = errno ; goto err ; }
  if (!r) return 0 ;
  if (r < sizeof(struct utmpx)) { e = EPIPE ; goto err ; }
  return 1 ;
 err:
  unlink("utmp") ;
  answer(e) ;
  errno = e ;
  strerr_diefu1sys(111, "read utmp file") ;
}

static int idmatch (unsigned short type, char const *id, struct utmpx const *b)
{
  if (type == BOOT_TIME || type == OLD_TIME || type == NEW_TIME)
  {
    if (type == (unsigned short)b->ut_type) return 1 ;
  }
  else if (type == INIT_PROCESS || type == LOGIN_PROCESS || type == USER_PROCESS || type == DEAD_PROCESS)
  {
    if ((b->ut_type == INIT_PROCESS || b->ut_type == LOGIN_PROCESS || b->ut_type == USER_PROCESS || b->ut_type == DEAD_PROCESS)
      && !strncmp(id, b->ut_id, UTMPS_UT_IDSIZE - 1)) return 1 ;
  }
  return 0 ;
}

static void do_getent (void)
{
  struct utmpx b ;
  char buf[1 + sizeof(struct utmpx)] = "" ;
  maybe_open() ;
  if (!read_utmp_entry(buf+1)) { answer(ESRCH) ; return ; }
  utmps_utmpx_unpack(buf+1, &b) ;
  utmps_utmpx_pack(buf+1, &b) ;
  buffer_putnoflush(buffer_1small, buf, 1 + sizeof(struct utmpx)) ;
  flush1() ;
}

static void do_getid (void)
{
  unsigned short type ;
  char rbuf[USHORT_PACK + UTMPS_UT_IDSIZE] ;
  char sbuf[1 + sizeof(struct utmpx)] = "" ;
  get0(rbuf, USHORT_PACK + UTMPS_UT_IDSIZE) ;
  ushort_unpack_big(rbuf, &type) ;
  rbuf[USHORT_PACK + UTMPS_UT_IDSIZE - 1] = 0 ;
  maybe_open() ;
  for (;;)
  {
    struct utmpx b ;
    if (!read_utmp_entry(sbuf+1)) { answer(ESRCH) ; return ; }
    utmps_utmpx_unpack(sbuf+1, &b) ;
    if (idmatch(type, rbuf + USHORT_PACK, &b)) break ;
  }
  buffer_putnoflush(buffer_1small, sbuf, 1 + sizeof(struct utmpx)) ;
  flush1() ;
}

static void do_getline (void)
{
  char rbuf[UTMPS_UT_LINESIZE] ;
  char sbuf[1 + sizeof(struct utmpx)] = "" ;
  get0(rbuf, UTMPS_UT_LINESIZE) ;
  rbuf[UTMPS_UT_LINESIZE - 1] = 0 ;
  maybe_open() ;
  for (;;)
  {
    struct utmpx b ;
    if (!read_utmp_entry(sbuf+1)) { answer(ESRCH) ; return ; }
    utmps_utmpx_unpack(sbuf+1, &b) ;
    if ((b.ut_type == LOGIN_PROCESS || b.ut_type == USER_PROCESS)
      && !strncmp(rbuf, b.ut_line, UTMPS_UT_LINESIZE - 1)) break ;
  }
  buffer_putnoflush(buffer_1small, sbuf, 1 + sizeof(struct utmpx)) ;
  flush1() ;
}

static void do_putline (uid_t uid)
{
  struct utmpx u ;
  char buf[sizeof(struct utmpx)] ;
  get0(buf, sizeof(struct utmpx)) ;
  if (uid) { answer(EPERM) ; return ; }
  utmps_utmpx_unpack(buf, &u) ;
  maybe_open() ;
  for (;;)
  {
    struct utmpx b ;
    char tmp[sizeof(struct utmpx)] ;
    if (!read_utmp_entry(tmp)) break ;
    utmps_utmpx_unpack(tmp, &b) ;
    if (idmatch(u.ut_type, u.ut_id, &b) && !strncmp(u.ut_line, b.ut_line, UTMPS_UT_LINESIZE - 1))
    {
      if (lseek(fd, -(off_t)sizeof(struct utmpx), SEEK_CUR) < 0)
      {
        answer(errno) ;
        return ;
      }
      break ;
    }
  }
  utmps_utmpx_pack(buf, &u) ;
  if (lock_ex(fd) < 0) { answer(errno) ; return ; }
  if (allwrite(fd, buf, sizeof(struct utmpx)) < sizeof(struct utmpx))
  {
    int e = errno ;
    answer(e) ;
    errno = e ;
    strerr_diefu1sys(111, "write to utmp") ;
  }
  fsync(fd) ;
  lock_unx(fd) ;
  answer(0) ;
}

static void do_rewind (void)
{
  maybe_open() ;
  if (lseek(fd, 0, SEEK_SET) < 0) { answer(errno) ; return ; }
  answer(0) ;
}

int main (void)
{
  uid_t uid ;
  char const *x ;
  PROG = "utmps-utmpd" ;
  x = ucspi_get("REMOTEEUID") ;
  if (!x) strerr_diefu1x(100, "get $IPCREMOTEEUID from environment") ;
  if (!uid0_scan(x, &uid)) strerr_dieinvalid(100, "IPCREMOTEEUID") ;
  if (ndelay_on(0) < 0) strerr_diefu1sys(111, "set stdin non-blocking") ;
  tain_now_g() ;

  for (;;)
  {
    tain_t deadline ;
    char c ;
    tain_add_g(&deadline, &tain_infinite_relative) ;
    if (!buffer_timed_get_g(buffer_0small, &c, 1, &deadline)) break ;
    switch (c)
    {
      case 'e' : do_getent() ; break ;
      case 'i' : do_getid() ; break ;
      case 'l' : do_getline() ; break ;
      case 'E' : do_putline(uid) ; break ;
      case 'r' : do_rewind() ; break ;
      default :
        errno = EPROTO ;
        strerr_diefu1sys(1, "interpret stdin") ;
    }
  }
  return 0 ;
}
