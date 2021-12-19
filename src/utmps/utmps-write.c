/* ISC license. */

#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <pwd.h>

#include <skalibs/sgetopt.h>
#include <skalibs/strerr2.h>
#include <skalibs/types.h>
#include <skalibs/tai.h>
#include <skalibs/fmtscan.h>

#include <utmps/config.h>
#include <utmps/utmps.h>
#include <utmps/utmpx.h>

#define USAGE "utmps-write [ -u ] [ -w ] [ -U utmpd-socket ] [ -W wtmpd-socket ] [ -t timeout ] [ -T timestamp ] [ -h host ] [ -i ip ] [ -l user ] [ -p pid ] id type line"
#define dieusage() strerr_dieusage(100, USAGE)

typedef struct uttype_s uttype, *uttype_ref ;
struct uttype_s
{
  char const *s ;
  short type ;
} ;

static uttype const uttypes[] =
{
  { "ACCOUNTING", 9 },
  { "BOOT_TIME", 2 },
  { "DEAD_PROCESS", 8 },
  { "EMPTY", 0 },
  { "INIT_PROCESS", 5 },
  { "LOGIN_PROCESS", 6 },
  { "NEW_TIME", 3 },
  { "OLD_TIME", 4 },
  { "RUN_LVL", 1 },
  { "USER_PROCESS", 7 }
} ;

static int uttype_cmp (void const *a, void const *b)
{
  char const *key = a ;
  uttype const *element = b ;
  return strcasecmp(key, element->s) ;
}

int main (int argc, char const *const *argv)
{
  char const *usocket = UTMPS_UTMPD_PATH ;
  char const *wsocket = UTMPS_WTMPD_PATH ;
  char const *stamp = 0 ;
  char const *user = 0 ;
  char const *ipstr = 0 ;
  unsigned int what = 0 ;
  tain tto ;
  struct utmpx b ;
  PROG = "utmps-write" ;
  memset(&b, 0, sizeof(struct utmpx)) ;
  b.ut_pid = -1 ;

  {
    unsigned int t = 0 ;
    subgetopt l = SUBGETOPT_ZERO ;
    for (;;)
    {
      int opt = subgetopt_r(argc, argv, "uwU:W:t:T:h:i:l:p:", &l) ;
      if (opt == -1) break ;
      switch (opt)
      {
        case 'u' : what |= 1 ; break ;
        case 'w' : what |= 2 ; break ;
        case 'U' : usocket = l.arg ; break ;
        case 'W' : wsocket = l.arg ; break ;
        case 't' : if (!uint0_scan(l.arg, &t)) dieusage() ; break ;
        case 'T' : stamp = l.arg ; break ;
        case 'h' : strncpy(b.ut_host, l.arg, UTMPS_UT_HOSTSIZE) ; break ;
        case 'i' : ipstr = l.arg ; break ;
        case 'l' : user = l.arg ; break ;
        case 'p' : if (!pid0_scan(l.arg, &b.ut_pid)) dieusage() ; break ;
        default : dieusage() ;
      }
    }
    argc -= l.ind ; argv += l.ind ;
    if (t) tain_from_millisecs(&tto, t) ;
    else tto = tain_infinite_relative ;
  }
  if (argc < 3) dieusage() ;
  if (!what) strerr_dief1x(100, "at least one of -u or -w must be used") ;

  {
    uttype const *p = bsearch(argv[1], uttypes, sizeof(uttypes) / sizeof(uttype), sizeof(uttype), &uttype_cmp) ;
    if (!p) strerr_dief2x(100, "invalid ut_type: ", argv[1]) ;
    b.ut_type = p->type ;
  }  
  strncpy(b.ut_id, argv[0], UTMPS_UT_IDSIZE) ;
  strncpy(b.ut_line, argv[2], UTMPS_UT_LINESIZE) ;
  if (ipstr)
  {
   /* ut_addr_v6 is in host byte order, don't ask */
    size_t len = ip6_scan(ipstr, (char *)b.ut_addr_v6) ;
    if (!len || ipstr[len])
    {
      len = ip4_scan(ipstr, (char *)b.ut_addr_v6) ;
      if (!len || ipstr[len])
        strerr_dief2x(100, "invalid IP address: ", ipstr) ;
    }
  }
  if (b.ut_pid == -1) b.ut_pid = getpid() ;
  if (user) strncpy(b.ut_user, user, UTMPS_UT_NAMESIZE) ;
  else
  {
    struct passwd *pw = getpwuid(getuid()) ;
    if (pw) strncpy(b.ut_user, pw->pw_name, UTMPS_UT_NAMESIZE) ;
  }
  tain_now_set_stopwatch_g() ;
  tain_add_g(&tto, &tto) ;
  {
    tain when ;
    if (stamp)
    {
      size_t n = timestamp_scan(stamp, &when) ;
      if (!n || stamp[n]) dieusage() ;
    }
    else when = STAMP ;
    if (!timeval_from_tain(&b.ut_tv, &when))
      strerr_diefu1sys(111, "timeval_from_tain") ;
  }

  if (what & 1)
  {
    utmps a = UTMPS_ZERO ;
    if (!utmps_start_g(&a, usocket, &tto))
      strerr_diefu2sys(111, "connect to utmpd at ", usocket) ;
    if (!utmps_putline_g(&a, &b, &tto))
      strerr_diefu1sys(111, "write utmp entry") ;
    utmps_end(&a) ;
  }

  if (what & 2)
  {
    if (!utmps_updwtmpx_g(wsocket, &b, &tto))
      strerr_diefu2sys(111, "write wtmp entry via wtmpd at ", wsocket) ;
  }

  return 0 ;
}
