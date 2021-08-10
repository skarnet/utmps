/* ISC license. */

#ifndef UTMPS_H
#define UTMPS_H

#include <skalibs/tai.h>
#include <utmps/utmpx.h>

typedef struct utmps_s utmps, *utmps_ref ;
struct utmps_s
{
  int fd ;
} ;
#define UTMPS_ZERO { .fd = -1 }

extern int utmps_start (utmps *, char const *, tain const *, tain *) ;
#define utmps_start_g(a, s, deadline) utmps_start(a, s, (deadline), &STAMP)

extern void utmps_end (utmps *) ;

extern int utmps_rewind (utmps *, tain const *, tain *) ;
#define utmps_rewind_g (a, deadline) utmps_rewind(a, (deadline), &STAMP)
extern int utmps_getent (utmps *, struct utmpx *, tain const *, tain *) ;
#define utmps_getent_g (a, b, deadline) utmps_getent(a, b, (deadline), &STAMP)
extern int utmps_getid (utmps *, unsigned short, char const *, struct utmpx *, tain const *, tain *) ;
#define utmps_getid_g(a, type, id, b, deadline) utmps_getid(a, type, id, b, (deadline), &STAMP)
extern int utmps_getline (utmps *, char const *, struct utmpx *, tain const *, tain *) ;
#define utmps_getline_g(a, line, b, deadline) utmps_getline(a, line, b, (deadline), &STAMP)
extern int utmps_putline (utmps *, struct utmpx const *, tain const *, tain *) ;
#define utmps_putline_g(a, entry, deadline) utmps_putline(a, entry, (deadine), &STAMP)

extern int utmps_updwtmpx (char const *, struct utmpx const *, tain const *, tain *) ;
#define utmps_updwtmpx_g(file, b, deadline) utmps_updwtmpx(file, b, (deadline), &STAMP)

#endif
