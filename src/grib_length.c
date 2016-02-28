#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_length(SEXP rgrib_fileHandle) {
  int err;
  R_len_t n_on;
  R_len_t n_off;
  grib_handle *h = NULL;
  FILE *file = NULL;
  file = R_ExternalPtrAddr(rgrib_fileHandle);

  err = grib_count_in_file(DEFAULT_CONTEXT, file, &n_off);
  if (err) {
    gerror("unable to count grib messages", err);
  }

  grib_multi_support_on(DEFAULT_CONTEXT);
  err = grib_count_in_file(DEFAULT_CONTEXT, file, &n_on);
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    n_on++;
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  /* This rewind is important as neglecting to do so will
   * leave the file handle in a unusable state and cause
   * R to crash */
  if (fseek(file, 0, SEEK_SET)) {
    error("rGRIB: unable to rewind file");
  }

  grib_handle_delete(h);

  if (n_on != n_off) {
    return ScalarInteger(n_on);
  }

  return ScalarInteger(n_off);
}
