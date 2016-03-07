#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_get_values(SEXP rgrib_fileHandle) {
  int err;
  long nx,ny;
  R_len_t i;
  size_t values_length;
  SEXP rgrib_values, rgrib_output,rgrib_names;
  double *values, *p_rgrib_values, missingValue = 0;
  FILE *file = NULL;
  grib_handle *h = NULL;

  file = R_ExternalPtrAddr(rgrib_fileHandle);

  h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
  err = grib_get_long(h, "Ni", &nx);
  if (err) {
    gerror("unable to get x dim", err);
  }
  err = grib_get_long(h, "Nj", &ny);
  if (err) {
    gerror("unable to get y dim", err);
  }

  /* It will be easier to just deal with a vector instead
   * of trying to coerce to a "matrix" in c */
  values_length = nx*ny;
  rgrib_values = PROTECT(allocVector(REALSXP, values_length));
  values = malloc((values_length)*sizeof(double));
  err = grib_get_double_array(h, "values", values, &values_length);
  if (err) {
    gerror("unable to get values", err);
  }
  err = grib_get_double(h, "missingValue", &missingValue);
  if (err) {
    gerror("unable to get missing value", err);
  }

  /* Copy over the values to the R object;
     Use pointers for speed */
  p_rgrib_values = REAL(rgrib_values);
  for (i = 0; i < values_length; i++) {
    if (values[i] == missingValue) {
      p_rgrib_values[i] =  NA_REAL;
    } else {
      p_rgrib_values[i] =  values[i];
    }
  }
  free(values);

  /* Need to pass out nx, ny to be able to
   * create matrix in R wrapper */
  rgrib_output = PROTECT(allocVector(VECSXP, 3));
  SET_VECTOR_ELT(rgrib_output, 0, rgrib_values);
  SET_VECTOR_ELT(rgrib_output, 1, ScalarInteger(nx));
  SET_VECTOR_ELT(rgrib_output, 2, ScalarInteger(ny));

  /* Set names of output list for ease of handling
   * in the R wrapper function */
  rgrib_names = PROTECT(allocVector(STRSXP, 3));
  SET_STRING_ELT(rgrib_names, 0, mkChar("values"));
  SET_STRING_ELT(rgrib_names, 1, mkChar("nx"));
  SET_STRING_ELT(rgrib_names, 2, mkChar("ny"));
  namesgets(rgrib_output, rgrib_names);

  grib_handle_delete(h);
  UNPROTECT(3);
  return rgrib_output;
}
