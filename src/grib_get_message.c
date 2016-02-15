#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_get_message(SEXP rgrib_fileHandle, SEXP rgrib_filter, SEXP rgrib_nameSpace) {
  int err;
  int filter;
  int keyType;
  long nx, ny;
  long keyValue_i;
  long bitmapPresent;
  long *bitmap;
  int *p_rgrib_bitmap;
  R_len_t valuesLength;
  R_len_t n;
  R_len_t i;
  R_len_t totalKeys;
  size_t keyLength;
  size_t bmp_len;
  //size_t bit_length = MAX_BYTE_LENGTH;
  double lat;
  double lon;
  double value;
  double missingValue;
  double keyValue_d;
  double *p_rgrib_lat = NULL;
  double *p_rgrib_lon = NULL;
  double *p_rgrib_values = NULL;
  const char *keyName = NULL;
  const char *nameSpace = NULL;
  char keyValue_c[MAX_VAL_LEN];
  //unsigned char keyValue_b[MAX_BYTE_LENGTH];
  //unsigned char * p_rgrib_bytes;
  FILE *file = NULL;
  grib_handle *h = NULL;
  grib_iterator *iter = NULL;
  grib_keys_iterator *keyIter = NULL;
  SEXP rgrib_grib_message;
  SEXP rgrib_lat;
  SEXP rgrib_lon;
  SEXP rgrib_bitmap;
  SEXP rgrib_bytes;
  SEXP rgrib_values;
  SEXP rgrib_slots;
  SEXP rgrib_keys;
  SEXP rgrib_keyNames;
  PROTECT_INDEX pro_bitmap;
  PROTECT_INDEX pro_bytes;

  filter = asInteger(rgrib_filter);
  nameSpace = CHAR(STRING_ELT(rgrib_nameSpace,0));

  file = R_ExternalPtrAddr(rgrib_fileHandle);
  h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
  if (err) {
    gerror("unable to open grib handle", err);
  }

  iter = grib_iterator_new(h, NO_ITER_FLAGS, &err);
  if (err) {
    gerror("unable to create new iterator", err);
  }

  /* Make sure to get the dimensions which
   * are sometimes stored in different keys.
   * Covering all the bases here with
   * Ni/Nx and Nj/Ny */
  err = grib_get_long(h, "Ni", &nx);
  if (err) {
    err = grib_get_long(h, "Nx", &nx);
    if (err) {
    gerror("unable to get x dim", err);
    }
  }

  err = grib_get_long(h, "Nj", &ny);
  if (err) {
    err = grib_get_long(h, "Ny", &ny);
    if (err) {
      gerror("unable to get y dim", err);
    }
  }

  err = grib_get_double(h,"missingValue",&missingValue);
  if (err) {
    gerror("unable to get missing value", err);
  }

  valuesLength = nx*ny;
  rgrib_lat = PROTECT(allocMatrix(REALSXP, nx, ny));
  rgrib_lon = PROTECT(allocMatrix(REALSXP, nx, ny));
  rgrib_values = PROTECT(allocMatrix(REALSXP, nx, ny));
  /* These next two SEXPs get an index since their uses
   * are optional. Having PROTECT() in and if () leads to
   * hard to deal with UNPROTECT() situations. Setting the
   * SEXPs to the most likely value and then redefining them
   * inside REPROTECT(). This ensures that the UNPROTECT()
   * at the end will always have the same number in the stack. */
  PROTECT_WITH_INDEX(rgrib_bitmap = allocMatrix(INTSXP, nx, ny), &pro_bitmap);
  PROTECT_WITH_INDEX(rgrib_bytes = allocVector(RAWSXP, 1), &pro_bytes);

  p_rgrib_lat = REAL(rgrib_lat);
  p_rgrib_lon = REAL(rgrib_lon);
  p_rgrib_values = REAL(rgrib_values);
  p_rgrib_bitmap = INTEGER(rgrib_bitmap);

  err = grib_get_long(h, "bitmapPresent", &bitmapPresent);
  if (err) {
    gerror("unable to get bitmapPresent variable", err);
  } else {
    if (bitmapPresent) {
      grib_get_size(h, "bitmap", &bmp_len);
      if (bmp_len != valuesLength) {
        error("mismatch between bitmap and values array sizes");
      }
      bitmap = malloc(bmp_len*sizeof(long));
      err = grib_get_long_array(h, "bitmap", bitmap, &bmp_len);
      if (err) {
        gerror("unable to get bitmap array", err);
      } else {
        for (i = 0; i < bmp_len; i++) {
          R_CheckUserInterrupt();
          p_rgrib_bitmap[i] = bitmap[i];
        }
        free(bitmap);
      }
    } else {
      REPROTECT(rgrib_bitmap = allocVector(LGLSXP, 1), pro_bitmap);
      LOGICAL(rgrib_bitmap)[0] =  FALSE;
    }
  }

  n = 0;
  while(grib_iterator_next(iter,&lat,&lon,&value) && n < valuesLength) {
    R_CheckUserInterrupt();
    p_rgrib_lat[n] = lat;
    p_rgrib_lon[n] = lon;
    if (value == missingValue || (bitmapPresent && p_rgrib_bitmap[n] == BITMAP_MASK)) {
      p_rgrib_values[n] = NA_REAL;
    } else {
      p_rgrib_values[n] = value;
    }
    n++;
  }

  grib_iterator_delete(iter);

  rgrib_grib_message = PROTECT(allocVector(VECSXP, 5));
  SET_VECTOR_ELT(rgrib_grib_message, 0, rgrib_values);
  SET_VECTOR_ELT(rgrib_grib_message, 1, rgrib_lat);
  SET_VECTOR_ELT(rgrib_grib_message, 2, rgrib_lon);
  SET_VECTOR_ELT(rgrib_grib_message, 3, rgrib_bitmap);

  rgrib_slots = PROTECT(allocVector(STRSXP,5));
  SET_STRING_ELT(rgrib_slots, 0, mkChar("values"));
  SET_STRING_ELT(rgrib_slots, 1, mkChar("lat"));
  SET_STRING_ELT(rgrib_slots, 2, mkChar("lon"));
  SET_STRING_ELT(rgrib_slots, 3, mkChar("bitmap"));
  SET_STRING_ELT(rgrib_slots, 4, mkChar("keys"));
  namesgets(rgrib_grib_message, rgrib_slots);

  keyIter = grib_keys_iterator_new(h, filter, nameSpace);
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

    /* This will go unused for now...always returns zero bit_length.
     * Keeping code for now for reference. GRIB_TYPE_BYTES will be
     * returned as character instead.

    case GRIB_TYPE_BYTES:
      err = grib_get_bytes(h, keyName, keyValue_b, &bit_length);
      if (err) {
        //warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        SET_VECTOR_ELT(rgrib_keys, n++, ScalarInteger(NA_INTEGER));
      } else {
        REPROTECT(rgrib_bytes = allocVector(RAWSXP, bit_length), pro_bytes);
        p_rgrib_bytes = RAW(rgrib_bytes);
        for (i = 0; i < bit_length; i++) {
          p_rgrib_bytes[i] = keyValue_b[i];
        }
        SET_VECTOR_ELT(rgrib_keys, n++, rgrib_bytes);
      }
      bit_length = MAX_BYTE_LENGTH;
      break;
      */

    default:
      //SET_VECTOR_ELT(rgrib_keys, n++, ScalarInteger(NA_INTEGER));
      bzero(keyValue_c, keyLength);
      err = grib_get_string(h, keyName, keyValue_c, &keyLength);
      if (err) {
        //warning("unable to get keyValue for key %s; setting to NA\n", keyName);
        SET_VECTOR_ELT(rgrib_keys, n++, ScalarString(NA_STRING));
      } else {
        SET_VECTOR_ELT(rgrib_keys, n++, mkString(keyValue_c));
      }
    }
  }

  namesgets(rgrib_keys, rgrib_keyNames);
  SET_VECTOR_ELT(rgrib_grib_message, 4, rgrib_keys);

  grib_keys_iterator_delete(keyIter);
  grib_handle_delete(h);

  UNPROTECT(9);
  return rgrib_grib_message;
}
