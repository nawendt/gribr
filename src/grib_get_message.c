#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_get_message(SEXP rgrib_fileHandle) {
  int err;
  int keyType;
  int filter;
  long nx, ny;
  int *p_rgrib_INT_keyValue = NULL;
  R_len_t valuesLength;
  R_len_t n;
  size_t keyLength;
  double lat;
  double lon;
  double value;
  double missingValue;
  double *p_rgrib_lat = NULL;
  double *p_rgrib_lon = NULL;
  double *p_rgrib_values = NULL;
  double *p_rgrib_REAL_keyValue = NULL;
  const char *nameSpace = NULL;
  const char *keyName = NULL;
  const char *p_rgrib_STR_keyValue = NULL;
  FILE *file = NULL;
  grib_handle *h = NULL;
  grib_iterator *iter = NULL;
  grib_keys_iterator *keyIter = NULL;
  SEXP rgrib_grib_message;
  SEXP rgrib_lat;
  SEXP rgrib_lon;
  SEXP rgrib_bitmap;
  SEXP rgrib_values;
  SEXP rgrib_names;
  SEXP rgrib_keys;
  SEXP rgrib_STR_keyValue;
  SEXP rgrib_REAL_keyValue;
  SEXP rgrib_INT_keyValue;
  void *keyValue = NULL;

  file = R_ExternalPtrAddr(rgrib_fileHandle);
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
  rgrib_lat = PROTECT(allocMatrix(REALSXP, nx, ny));
  rgrib_lon = PROTECT(allocMatrix(REALSXP, nx, ny));
  rgrib_values = PROTECT(allocMatrix(REALSXP, nx, ny));

  p_rgrib_lat = REAL(rgrib_lat);
  p_rgrib_lon = REAL(rgrib_lon);
  p_rgrib_values = REAL(rgrib_values);

  n = 0;
  while(grib_iterator_next(iter,&lat,&lon,&value) && n < valuesLength) {
    p_rgrib_lat[n] = lat;
    p_rgrib_lon[n] = lon;
    if (value == missingValue) {
      p_rgrib_values[n] = NA_REAL;
    } else {
      p_rgrib_values[n] = value;
    }
    n++;
  }

  grib_iterator_delete(iter);

  rgrib_grib_message = PROTECT(allocVector(VECSXP, 4));
  SET_VECTOR_ELT(rgrib_grib_message, 0, rgrib_values);
  SET_VECTOR_ELT(rgrib_grib_message, 1, rgrib_lat);
  SET_VECTOR_ELT(rgrib_grib_message, 2, rgrib_lon);

  rgrib_names = PROTECT(allocVector(STRSXP,3));
  SET_STRING_ELT(rgrib_names, 0, mkChar("values"));
  SET_STRING_ELT(rgrib_names, 1, mkChar("lat"));
  SET_STRING_ELT(rgrib_names, 2, mkChar("lon"));
  namesgets(rgrib_grib_message, rgrib_names);

  /* Work on getting the keys in here, too */

  keyIter = grib_keys_iterator_new(h, NO_FILTER, NULL_NAMESPACE);
  if (keyIter == NULL) {
    error("%s(%d): unable to create key iterator", __FILE__, __LINE__);
  }

  /* This seems like the only way to get the keys
   * count to then allocate a list to contain them.
   * Will monitor for a better solution. */
  n = 0;
  while(grib_keys_iterator_next(keyIter)) {
    n++;
  }
  err = grib_keys_iterator_rewind(keyIter);
  if (err) {
    gerror("unable to rewind keys iterator", err);
  }

  /* Now, allocate R list/component vectors and
   * iterate over keys again.*/
  rgrib_keys = PROTECT(allocVector(VECSXP, n));
  rgrib_INT_keyValue = PROTECT(allocVector(INTSXP, 1));
  rgrib_REAL_keyValue = PROTECT(allocVector(REALSXP, 1));
  rgrib_STR_keyValue = PROTECT(allocVector(STRSXP, 1));

  /* Get pointers to the components for speed */
  p_rgrib_INT_keyValue = INTEGER(rgrib_INT_keyValue);
  p_rgrib_REAL_keyValue = REAL(rgrib_REAL_keyValue);
  p_rgrib_STR_keyValue = CHAR(STRING_ELT(rgrib_STR_keyValue, 0));

  n = 0;
  while(grib_keys_iterator_next(keyIter)) {
    keyLength = MAX_VAL_LEN;
    const char *keyName = grib_keys_iterator_get_name(keyIter);
    err = grib_get_native_type(h, keyName, &keyType);
    if (err) {
      gerror("unable to get native key type", err);
    }

    switch(keyType){
    // THE ERROR IS OCCURRING WITH STRINGS ******************************************
    case GRIB_TYPE_STRING:
      bzero(keyValue, keyLength);
      setAttrib(rgrib_STR_keyValue, R_NamesSymbol, mkString(keyName));
      keyValue = malloc(keyLength*sizeof(char));
      if (keyValue == NULL) {
        error("%s(%d): unable to allocate keyValue string for %s\n", __FILE__, __LINE__, keyName);
      }

      err = grib_get_string(h, keyName, keyValue, &keyLength);
      if (err) {
        warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        p_rgrib_STR_keyValue = CHAR(NA_STRING);
        //SET_STRING_ELT(rgrib_STR_keyValue, 0, NA_STRING);
        SET_VECTOR_ELT(rgrib_keys, n++, rgrib_STR_keyValue);
      } else {
        p_rgrib_STR_keyValue = keyValue;
        //SET_STRING_ELT(rgrib_STR_keyValue, 0, mkChar((char*)keyValue));
        SET_VECTOR_ELT(rgrib_keys, n++, rgrib_STR_keyValue);
      }

      free(keyValue);
      keyValue = NULL;
      break;

    case GRIB_TYPE_DOUBLE:
      setAttrib(rgrib_REAL_keyValue, R_NamesSymbol, mkString(keyName));
      keyValue = malloc(sizeof(double));
      if (keyValue == NULL) {
        error("%s(%d): unable to allocate keyValue double for %s\n", __FILE__, __LINE__, keyName);
      }

      err = grib_get_double(h, keyName, keyValue);
      if (err) {
        warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        p_rgrib_REAL_keyValue = &R_NaReal;
        SET_VECTOR_ELT(rgrib_keys, n++, rgrib_REAL_keyValue);
      } else {
        p_rgrib_REAL_keyValue = keyValue;
        SET_VECTOR_ELT(rgrib_keys, n++, rgrib_REAL_keyValue);
      }

      free(keyValue);
      keyValue = NULL;
      break;

    case GRIB_TYPE_LONG:
      setAttrib(rgrib_INT_keyValue, R_NamesSymbol, mkString(keyName));
      keyValue = malloc(sizeof(int));
      if (keyValue == NULL) {
        error("%s(%d): unable to allocate keyValue long for %s\n", __FILE__, __LINE__, keyName);
      }

      err = grib_get_long(h, keyName, keyValue);
      if (err) {
        warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        p_rgrib_INT_keyValue = &R_NaInt;
        SET_VECTOR_ELT(rgrib_keys, n++, rgrib_INT_keyValue);
      } else {
        p_rgrib_INT_keyValue = keyValue;
        SET_VECTOR_ELT(rgrib_keys, n++, rgrib_INT_keyValue);
      }

      free(keyValue);
      keyValue = NULL;
      break;

    default:
      setAttrib(rgrib_INT_keyValue, R_NamesSymbol, mkString(keyName));
      p_rgrib_INT_keyValue = &R_NaInt;
      SET_VECTOR_ELT(rgrib_keys, n++, rgrib_INT_keyValue);
    }
  }

  grib_handle_delete(h);
  grib_keys_iterator_delete(keyIter);

  setAttrib(rgrib_INT_keyValue, R_NamesSymbol, mkString("keys"));
  SET_VECTOR_ELT(rgrib_grib_message, 3, rgrib_keys);

  UNPROTECT(9);
  return rgrib_grib_message;
}
