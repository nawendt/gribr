#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_get_message(SEXP rgrib_fileHandle) {
  int err;
  int keyType;
  long nx, ny;
  long keyValue_i;
  R_len_t valuesLength;
  R_len_t n;
  size_t keyLength;
  R_len_t totalKeys;
  //size_t byteLength[MAX_BYTE_LENGTH];
  double lat;
  double lon;
  double value;
  double missingValue;
  double keyValue_d;
  double *p_rgrib_lat = NULL;
  double *p_rgrib_lon = NULL;
  double *p_rgrib_values = NULL;
  const char *keyName = NULL;
  char keyValue_c[MAX_VAL_LEN];
  FILE *file = NULL;
  grib_handle *h = NULL;
  grib_iterator *iter = NULL;
  grib_keys_iterator *keyIter = NULL;
  SEXP rgrib_grib_message;
  SEXP rgrib_lat;
  SEXP rgrib_lon;
  //SEXP rgrib_bitmap;
  SEXP rgrib_values;
  SEXP rgrib_slots;
  SEXP rgrib_keys;
  SEXP rgrib_keyNames;

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

  rgrib_slots = PROTECT(allocVector(STRSXP,4));
  SET_STRING_ELT(rgrib_slots, 0, mkChar("values"));
  SET_STRING_ELT(rgrib_slots, 1, mkChar("lat"));
  SET_STRING_ELT(rgrib_slots, 2, mkChar("lon"));
  SET_STRING_ELT(rgrib_slots, 3, mkChar("keys"));
  namesgets(rgrib_grib_message, rgrib_slots);

  keyIter = grib_keys_iterator_new(h, GRIB_KEYS_ITERATOR_ALL_KEYS, NULL_NAMESPACE);
  if (keyIter == NULL) {
    error("%s(%d): unable to create key iterator", __FILE__, __LINE__);
  }

  /* This seems like the only way to get the keys
   * count to then allocate a list to contain them.
   * Will monitor for a better solution. */
  totalKeys = 0;
  while(grib_keys_iterator_next(keyIter)) {
    totalKeys++;
  }
  err = grib_keys_iterator_rewind(keyIter);
  if (err) {
    gerror("unable to rewind keys iterator", err);
  }

  /* Now, allocate R list/component vectors w/ names and
   * iterate over keys again. */
  rgrib_keys = PROTECT(allocVector(VECSXP, totalKeys));
  rgrib_keyNames = PROTECT(allocVector(STRSXP, totalKeys));

  n = 0;
  while(grib_keys_iterator_next(keyIter) && n < totalKeys) {

    /* In this section we make use of the void pointer
     * 'keyValue' to capture the key. This is only cast where
     * necessary as some functions expect a pointer and
     * safely cast this variable correctly. Functions expecting
     * non-pointers of the same type need a cast */

    R_CheckUserInterrupt();
    keyLength = MAX_VAL_LEN;
    keyName = grib_keys_iterator_get_name(keyIter);
    err = grib_get_native_type(h, keyName, &keyType);
    if (err) {
      gerror("unable to get native key type", err);
    }
    SET_STRING_ELT(rgrib_keyNames, n, mkChar(keyName));

    switch(keyType){
    case GRIB_TYPE_STRING:
      bzero(keyValue_c, keyLength);
      err = grib_get_string(h, keyName, keyValue_c, &keyLength);
      if (err) {
        //warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        SET_VECTOR_ELT(rgrib_keys, n++, ScalarString(NA_STRING));
      } else {
        SET_VECTOR_ELT(rgrib_keys, n++, mkString(keyValue_c));
      }
      break;

    case GRIB_TYPE_DOUBLE:
      err = grib_get_double(h, keyName, &keyValue_d);
      if (err) {
        //warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        SET_VECTOR_ELT(rgrib_keys, n++, ScalarReal(NA_REAL));
      } else {
        SET_VECTOR_ELT(rgrib_keys, n++, ScalarReal(keyValue_d));
      }
      break;

    case GRIB_TYPE_LONG:
      err = grib_get_long(h, keyName, &keyValue_i);
      if (err) {
        //warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        SET_VECTOR_ELT(rgrib_keys, n++, ScalarInteger(NA_INTEGER));
      } else {
        SET_VECTOR_ELT(rgrib_keys, n++, ScalarInteger(keyValue_i));
      }
      break;

    default:
      SET_VECTOR_ELT(rgrib_keys, n++, ScalarInteger(NA_INTEGER));
    }
  }

  namesgets(rgrib_keys, rgrib_keyNames);
  SET_VECTOR_ELT(rgrib_grib_message, 3, rgrib_keys);

  grib_keys_iterator_delete(keyIter);
  grib_handle_delete(h);

  UNPROTECT(7);
  return rgrib_grib_message;
}
