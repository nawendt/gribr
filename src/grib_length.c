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
    error("%s(%d): unable to count messages; GRIB ERROR %3d",__FILE__,__LINE__,err);
  } else {
    return ScalarInteger(n);
  }
}
