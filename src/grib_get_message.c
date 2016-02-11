#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP R_grib_get_message(SEXP R_fileHandle) {
  int err;
  int keyType;
  int filter;
  long int nx, ny;
  int *p_R_INT_keyValue = NULL;
  R_len_t valuesLength;
  R_len_t n;
  size_t keyLength;
  double lat;
  double lon;
  double value;
  double missingValue;
  double *p_R_lat = NULL;
  double *p_R_lon = NULL;
  double *p_R_values = NULL;
  double *p_R_REAL_keyValue = NULL;
  const char *nameSpace = NULL;
  const char *keyName = NULL;
  const char *p_R_STR_keyValue = NULL;
  FILE *file = NULL;
  grib_handle *h = NULL;
  grib_iterator *iter = NULL;
  grib_keys_iterator *keyIter = NULL;
  SEXP R_grib_message;
  SEXP R_lat;
  SEXP R_lon;
  SEXP R_bitmap;
  SEXP R_values;
  SEXP R_names;
  SEXP R_keys;
  SEXP R_STR_keyValue;
  SEXP R_REAL_keyValue;
  SEXP R_INT_keyValue;
  void *keyValue = NULL;

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
  R_keys = PROTECT(allocVector(VECSXP, n));
  R_INT_keyValue = allocVector(INTSXP, 1);
  R_REAL_keyValue = allocVector(REALSXP, 1);
  R_STR_keyValue = allocVector(STRSXP, 1);

  /* Get pointers to the components for speed */
  p_R_INT_keyValue = INTEGER(R_INT_keyValue);
  p_R_REAL_keyValue = REAL(R_REAL_keyValue);
  p_R_STR_keyValue = CHAR(STRING_ELT(R_STR_keyValue, 0));

  n = 0;
  while(grib_keys_iterator_next(keyIter)) {
    keyLength = MAX_VAL_LEN;
    const char *keyName = grib_keys_iterator_get_name(keyIter);
    err = grib_get_native_type(h, keyName, &keyType);
    if (err) {
      gerror("unable to get native key type", err);
    }
    switch(keyType){
    case GRIB_TYPE_STRING:
      bzero(keyValue, keyLength);
      setAttrib(R_STR_keyValue, R_NamesSymbol, mkString(keyName));
      keyValue = malloc(keyLength*sizeof(char));
      if (keyValue == NULL) {
        error("%s(%d): unable to allocate keyValue string for %s\n", __FILE__, __LINE__, keyName);
      }
      err = grib_get_string(h, keyName, (char*)keyValue, &keyLength);
      if (err) {
        warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        p_R_STR_keyValue = CHAR(NA_STRING);
        //SET_STRING_ELT(R_STR_keyValue, 0, NA_STRING);
        SET_VECTOR_ELT(R_keys, n++, R_STR_keyValue);
      } else {
        p_R_STR_keyValue = (char*)keyValue;
        //SET_STRING_ELT(R_STR_keyValue, 0, mkChar((char*)keyValue));
        SET_VECTOR_ELT(R_keys, n++, R_STR_keyValue);
      }
      free(keyValue);
      keyValue = NULL;
      break;
    case GRIB_TYPE_DOUBLE:
      R_keyValue = allocVector(REALSXP, 1);
      setAttrib(R_keyValue, R_NamesSymbol, mkString(keyName));
      keyValue = malloc(sizeof(double));
      if (keyValue == NULL) {
        error("%s(%d): unable to allocate keyValue double for %s\n", __FILE__, __LINE__, keyName);
      }
      err = grib_get_string(h, keyName, (double*)keyValue);
      if (err) {
        warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        SET_STRING_ELT(R_keyValue, 0, REAL_NA);
        SET_VECTOR_ELT(R_keys, n++, R_keyValue);
      } else {
        SET_STRING_ELT(R_keyValue, 0, mkChar((double*)keyValue));
        SET_VECTOR_ELT(R_keys, n++, R_keyValue);
      }
      free(keyValue);
      keyValue = NULL;
      break;
    case GRIB_TYPE_LONG:
      setAttrib(R_keyValue, R_NamesSymbol, mkString(keyName));
      keyValue = malloc(sizeof(long));
      if (keyValue == NULL) {
        error("%s(%d): unable to allocate keyValue long for %s\n", __FILE__, __LINE__, keyName);
      }
      err = grib_get_string(h, keyName, (long*)keyValue);
      if (err) {
        warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        SET_STRING_ELT(R_keyValue, 0, NA_REAL);
        SET_VECTOR_ELT(R_keys, n++, R_keyValue);
      } else {
        SET_STRING_ELT(R_keyValue, 0, mkChar((long*)keyValue));
        SET_VECTOR_ELT(R_keys, n++, R_keyValue);
      }
      free(keyValue);
      keyValue = NULL;
      break;
    default:
      return NULL_USER_OBJECT;
    }

  }

  UNPROTECT(6);
  return R_grib_message;
}
