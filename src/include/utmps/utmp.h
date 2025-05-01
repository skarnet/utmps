/* ISC license. */

#ifndef UTMPS_UTMP_H
#define UTMPS_UTMP_H

#include <utmps/utmpx.h>

/* Unused, but some packages require these macros to be present */
#define UTMP_FILE UTMPX_FILE
#define WTMP_FILE "/var/log/wtmp"
#define UTMP_FILENAME UTMP_FILE
#define WTMP_FILENAME WTMP_FILE
#define _PATH_UTMP UTMP_FILE
#define _PATH_WTMP WTMP_FILE

struct lastlog
{
  time_t ll_time ;
  char ll_line[UTMPS_UT_LINESIZE] ;
  char ll_host[UTMPS_UT_HOSTSIZE] ;
} ;

#define utmp utmpx

#define endutent endutxent
#define setutent setutxent
#define getutent getutxent
#define getutid getutxid
#define getutline getutxline
#define pututline pututxline
#define utmpname utmpxname
#define updwtmp updwtmpx

#ifdef _DEFAULT_SOURCE
#define getutent_r utmps_getutxent_r
#define getutid_r utmps_getutxid_r
#define getutline_r utmps_getutxline_r
#endif

#endif
