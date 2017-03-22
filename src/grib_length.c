#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_length(SEXP gribr_fileHandle) {
  int err;
  R_len_t n_on;
  R_len_t n_off;
  codes_handle *h = NULL;
  FILE *file = NULL;
  file = R_ExternalPtrAddr(gribr_fileHandle);

  err = codes_count_in_file(DEFAULT_CONTEXT, file, &n_off);
  if (err) {
    gerror("unable to count grib messages", err);
  }

  codes_grib_multi_support_on(DEFAULT_CONTEXT);
  n_on = 0;
  while((h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    n_on++;
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  /* This rewind is important as neglecting to do so will
   * leave the file handle in a unusable state and cause
   * R to crash */
  grewind(file);

  codes_handle_delete(h);

  if (n_on != n_off) {
    return ScalarInteger(n_on);
  }

  return ScalarInteger(n_off);
}
