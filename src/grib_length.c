#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP R_grib_length(SEXP R_fileHandle) {

  int err, n;
  FILE *file = NULL;
  file = R_ExternalPtrAddr(R_fileHandle);

  err = grib_count_in_file(DEFAULT_CONTEXT,file,&n);
  file = NULL;
  if (err) {
    gerror("unable to count messages", err);
  } else {
    return ScalarInteger(n);
  }
}
