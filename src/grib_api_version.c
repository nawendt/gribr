#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_api_version(void) {
  long version;
  long major;
  long minor;
  long revision;
  char string[50];
  SEXP gribr_version;

  version = codes_get_api_version();
  major = version / 10000;
  minor = (version - major * 10000) / 100;
  revision = (version - major * 10000) - (minor * 100);
  sprintf(string, "%ld.%ld.%ld", major, minor, revision);

  gribr_version = PROTECT(mkString(string));

  UNPROTECT(1);
  return gribr_version;
}
