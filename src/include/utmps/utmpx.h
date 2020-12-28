/* ISC license. */

#ifndef UTMPS_UTMPX_H
#define UTMPS_UTMPX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stdint.h>
#include <sys/time.h>

#define UTMPS_UT_LINESIZE 32
#define UTMPS_UT_NAMESIZE 32
#define UTMPS_UT_HOSTSIZE 256
#define UTMPS_UT_IDSIZE 4

struct exit_status
{
  short e_termination ;
  short e_exit ;
} ;

struct utmpx
{
  short ut_type ;
  pid_t ut_pid ;
  char ut_line[UTMPS_UT_LINESIZE] ;
  char ut_id[UTMPS_UT_IDSIZE] ;
  char ut_user[UTMPS_UT_NAMESIZE] ;

  char ut_host[UTMPS_UT_HOSTSIZE] ;
  struct exit_status ut_exit ;
  pid_t ut_session ;

  struct timeval ut_tv ;

  uint32_t ut_addr_v6[4] ;
  char _dummy[20] ;
} ;

#define EMPTY 0
#define BOOT_TIME 2
#define OLD_TIME 4
#define NEW_TIME 3
#define USER_PROCESS 7
#define INIT_PROCESS 5
#define LOGIN_PROCESS 6
#define DEAD_PROCESS 8

#define RUN_LVL 1
#define ACCOUNTING 9

extern void endutxent (void) ;
extern void setutxent (void) ;
extern struct utmpx *getutxent (void) ;
extern struct utmpx *getutxid (struct utmpx const *) ;
extern struct utmpx *getutxline (struct utmpx const *) ;
extern struct utmpx *pututxline (struct utmpx const *) ;

extern int utmpxname (char const *) ;
extern void updwtmpx (char const *, struct utmpx const *) ;
extern void logwtmp (char const *, char const *, char const *) ;

#define UT_LINESIZE UTMPS_UT_LINESIZE
#define UT_NAMESIZE UTMPS_UT_NAMESIZE
#define UT_HOSTSIZE UTMPS_UT_HOSTSIZE

/* Unused, but some packages (e.g. procps-ng) require this macro to be present */
#define UTMPX_FILE "/run/utmps/utmp"

/* More old GNU/crap compatibility */
#define ut_name ut_user
#define ut_xtime ut_tv.tv_sec
#define ut_addr ut_addr_v6[0]
#ifndef _NO_UT_TIME
# define ut_time ut_tv.tv_sec
#endif

#ifdef __cplusplus
}
#endif

#endif
