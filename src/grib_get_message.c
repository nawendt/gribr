#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP R_grib_get_message(SEXP R_fileHandle) {
  int err;
  long int nx, ny;
  R_len_t valuesLength, n;
  double lat, lon, value, missingValue = 0;
  double *p_R_lat, *p_R_lon, *p_R_values;
  FILE *file = NULL;
  grib_handle *h = NULL;
  grib_iterator *iter = NULL;
  grib_keys_iterator *keyIter = NULL;
  SEXP R_grib_message, R_lat, R_lon, R_bitmap, R_values;
  SEXP R_names;

  file = R_ExternalPtrAddr(R_fileHandle);
  h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
  if (err) {
    gerror("unable to open grib handle", err);
  }

  iter = grib_iterator_new(h, NO_ITER_FLAGS, &err);
  if (err) {
    gerror("unable to create new iterator", err);
  }

  err = grib_get_long(h, "Ni", &nx);
  if (err) {
    gerror("unable to get x dim", err);
  }

  err = grib_get_long(h, "Nj", &ny);
  if (err) {
    gerror("unable to get y dim", err);
  }

  err = grib_get_double(h,"missingValue",&missingValue);
  if (err) {
    gerror("unable to get missing value", err);
  }

  valuesLength = nx*ny;
  R_lat = PROTECT(allocMatrix(REALSXP, nx, ny));
  R_lon = PROTECT(allocMatrix(REALSXP, nx, ny));
  R_values = PROTECT(allocMatrix(REALSXP, nx, ny));

  p_R_lat = REAL(R_lat);
  p_R_lon = REAL(R_lon);
  p_R_values = REAL(R_values);

  n = 0;
  while(grib_iterator_next(iter,&lat,&lon,&value) && n < valuesLength) {
    p_R_lat[n] = lat;
    p_R_lon[n] = lon;
    if (value == missingValue) {
      p_R_values[n] = NA_REAL;
    } else {
      p_R_values[n] = value;
    }
    n++;
  }

  grib_iterator_delete(iter);
  grib_handle_delete(h);

  R_grib_message = PROTECT(allocVector(VECSXP, 3));
  SET_VECTOR_ELT(R_grib_message, 0, R_values);
  SET_VECTOR_ELT(R_grib_message, 1, R_lat);
  SET_VECTOR_ELT(R_grib_message, 2, R_lon);

  R_names = PROTECT(allocVector(STRSXP,3));
  SET_STRING_ELT(R_names, 0, mkChar("values"));
  SET_STRING_ELT(R_names, 1, mkChar("lat"));
  SET_STRING_ELT(R_names, 2, mkChar("lon"));
  namesgets(R_grib_message, R_names);

  /* Work on getting the keys in here, too */

  UNPROTECT(5);
  return R_grib_message;
}
