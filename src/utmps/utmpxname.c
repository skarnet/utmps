/* ISC license. */

#include <errno.h>
#include <utmps/utmpx.h>

 /*
     utmps does not support changing the utmp file: this file is
    hardcoded as $wd/utmp, $wd being the working directory of
    the utmps-utmpd daemon.

     Software that needs to change the utmp file should instead
    run its own instance of utmps-utmpd to access the alternate
    file the software wants, and connect to that instance.

     A stub is still provided so software can *build* and work
    in the cases where it doesn't try accessing a non-default
    utmp file.
 */

int utmpxname (char const *file)
{
  (void)file ;
  return (errno = ENOSYS, -1) ;
}
