#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_api_version(void) {
  long version;
  char string[MAX_VAL_LEN];
  SEXP gribr_version;

  version = grib_get_api_version();
  sprintf(string, "%ld", version);

  gribr_version = PROTECT(mkString(string));

  UNPROTECT(1);
  return gribr_version;
}
