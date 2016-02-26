#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

/*SEXP rgrib_message_list(grib_handle *h, int filter, char *nameSpace) {*/
SEXP rgrib_message_list(SEXP handle) {

  int err;
  int keyType;
  long nx;
  long ny;
  long *keyVal_l;
  double *keyVal_d;
  double missingValue;
  R_len_t totalKeys;
  R_len_t i;
  R_len_t n;
  size_t keySize;
  const char *keyName;
  grib_keys_iterator *keyIter;
  SEXP rgrib_grib_message;
  SEXP rgrib_list_names;
  SEXP rgrib_double;
  double *p_rgib_double;
  PROTECT_INDEX pro_double;
  PROTECT_WITH_INDEX(rgrib_double = R_NilValue, &pro_double);
  SEXP rgrib_long;
  int *p_rgib_long;
  PROTECT_INDEX pro_long;
  PROTECT_WITH_INDEX(rgrib_long = R_NilValue, &pro_long);

  grib_handle *h;
  FILE *file;
  file = R_ExternalPtrAddr(handle);
  h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);

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

  //keyIter = grib_keys_iterator_new(h, filter, nameSpace);
  keyIter = grib_keys_iterator_new(h, GRIB_KEYS_ITERATOR_ALL_KEYS, "");
  if (keyIter == NULL) {
    error("%s(%d): unable to create key iterator", __FILE__, __LINE__);
  }

  /* This seems like the only way to get the keys
   * count to then allocate a list to contain them.
   * Will monitor for a better solution. */
  totalKeys = 0;
  while(grib_keys_iterator_next(keyIter)) {
    keyName = grib_keys_iterator_get_name(keyIter);
    err = grib_get_native_type(h, keyName, &keyType);
    if (!strcmp(keyName, "zero") ||
        !strcmp(keyName, "one") ||
        !strcmp(keyName, "eight") ||
        !strcmp(keyName, "eleven") ||
        !strcmp(keyName, "false") ||
        !strcmp(keyName, "thousand") ||
        !strcmp(keyName, "file") ||
        !strcmp(keyName, "localDir") ||
        !strcmp(keyName, "7777") ||
        !strcmp(keyName, "oneThousand") ||
        !strcmp(keyName, "hundred") ||
        keyType == GRIB_TYPE_BYTES ||
        keyType == GRIB_TYPE_LABEL ||
        keyType == GRIB_TYPE_MISSING ||
        keyType == GRIB_TYPE_SECTION ||
        keyType == GRIB_TYPE_UNDEFINED ||
        err) {
      continue;
    } else {
      totalKeys++;
    }
  }

  err = grib_keys_iterator_rewind(keyIter);
  if (err) {
    gerror("unable to rewind keys iterator", err);
  }

  rgrib_grib_message = PROTECT(allocVector(VECSXP, totalKeys));
  rgrib_list_names = PROTECT(allocVector(STRSXP, totalKeys));

  n = 0;
  while(grib_keys_iterator_next(keyIter)) {
    keyName = grib_keys_iterator_get_name(keyIter);
    err = grib_get_native_type(h, keyName, &keyType);
    if (!strcmp(keyName, "zero") ||
        !strcmp(keyName, "one") ||
        !strcmp(keyName, "eight") ||
        !strcmp(keyName, "eleven") ||
        !strcmp(keyName, "false") ||
        !strcmp(keyName, "thousand") ||
        !strcmp(keyName, "file") ||
        !strcmp(keyName, "localDir") ||
        !strcmp(keyName, "7777") ||
        !strcmp(keyName, "oneThousand") ||
        !strcmp(keyName, "hundred") ||
        keyType == GRIB_TYPE_BYTES ||
        keyType == GRIB_TYPE_LABEL ||
        keyType == GRIB_TYPE_MISSING ||
        keyType == GRIB_TYPE_SECTION ||
        keyType == GRIB_TYPE_UNDEFINED ||
        err) {
      continue;
    } else {
      SET_STRING_ELT(rgrib_list_names, n, mkChar(keyName));
      err = grib_get_size(h, keyName, &keySize);
      if (err) {
        gerror("unable to get key value size", err);
      }
      switch(keyType) {

      case GRIB_TYPE_DOUBLE:
        if (keySize > 1) {
          keyVal_d = malloc(keySize*sizeof(double));
          err = grib_get_double_array(h, keyName, keyVal_d, &keySize);
          if (err) {
            gerror("unable to get double array", err);
          }
          REPROTECT(rgrib_double = allocVector(REALSXP, keySize), pro_double);
          p_rgib_double = REAL(rgrib_double);
          for (i = 0; i < keySize; i++) {
            p_rgib_double[i] = keyVal_d[i];
          }
          free(keyVal_d);
        } else {
          keyVal_d = malloc(sizeof(double));
          err = grib_get_double(h, keyName, keyVal_d);
          if (err) {
            gerror("unable to get double scalar", err);
          }
          REPROTECT(rgrib_double = ScalarReal(*keyVal_d), pro_double);
          free(keyVal_d);
        }
        SET_VECTOR_ELT(rgrib_grib_message, n, rgrib_double);

      case GRIB_TYPE_LONG:
        if (keySize > 1) {
          keyVal_l = malloc(keySize*sizeof(long));
          err = grib_get_long_array(h, keyName, keyVal_l, &keySize);
          if (err) {
            gerror("unable to get long array", err);
          }
          REPROTECT(rgrib_long = allocVector(INTSXP, keySize), pro_long);
          p_rgib_long = INTEGER(rgrib_long);
          for (i = 0; i < keySize; i++) {
            p_rgib_long[i] = (int)keyVal_l[i];
          }
          free(keyVal_l);
        } else {
          keyVal_l = malloc(sizeof(long));
          err = grib_get_long(h, keyName, keyVal_l);
          if (err) {
            gerror("unable to get long scalar", err);
          }
          REPROTECT(rgrib_long = ScalarInteger(*keyVal_l), pro_long);
          free(keyVal_l);
        }
        SET_VECTOR_ELT(rgrib_grib_message, n, rgrib_long);

      default:
        break;
      }
    }
    n++;
  }

  namesgets(rgrib_grib_message, rgrib_list_names);

  UNPROTECT(4);
  return rgrib_grib_message;
}
