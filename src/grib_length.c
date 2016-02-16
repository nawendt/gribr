#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_length(SEXP rgrib_fileHandle) {
  int err;
  R_len_t n;
  R_len_t n_on;
  R_len_t n_off;
  grib_context *c = NULL;
  FILE *file = NULL;
  file = R_ExternalPtrAddr(rgrib_fileHandle);

  c = grib_context_get_default();

  grib_multi_support_on(c);
  err = grib_count_in_file(c, file, &n_on);
  if (err) {
    gerror("unable to count messages", err);
  }

  grib_multi_support_off(c);
  err = grib_count_in_file(c, file, &n_off);
  if (err) {
    gerror("unable to count messages", err);
  }

  if (n_on == n_off) {
    n = n_off;
  } else {
    n = n_on;
  }

  return ScalarInteger(n);
}
