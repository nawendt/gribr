#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_latlons(SEXP rgrib_fileHandle) {
  int err;
  FILE *file = NULL;
  grib_handle *h = NULL;
  grib_iterator *iter = NULL;

  file = R_ExternalPtrAddr(rgrib_fileHandle);
  h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
  if (err) {
    gerror("unable to open grib handle", err);
  }

  iter=grib_iterator_new(h, NO_ITER_FLAGS, &err);
  return R_NilValue;
}
