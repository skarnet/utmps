/* ISC license. */

#ifndef UTMPS_INTERNAL_H
#define UTMPS_INTERNAL_H

#include <utmps/utmpx.h>
#include <utmps/utmps.h>

extern struct utmpx utmps_utmpx_here ;
extern utmps utmps_here ;
extern void utmps_here_maybe_init (void) ;

extern void utmps_utmpx_pack (char *, struct utmpx const *) ;
extern void utmps_utmpx_unpack (char const *, struct utmpx *) ;

#endif
