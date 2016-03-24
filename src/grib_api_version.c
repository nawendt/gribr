#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_api_version(void) {
  long version;
  char string[MAX_VAL_LEN];
  SEXP rgrib_version;

  version = grib_get_api_version();
  sprintf(string, "%ld", version);

  rgrib_version = PROTECT(mkString(string));

  UNPROTECT(1);
  return rgrib_version;
}
