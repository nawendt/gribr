#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_message_from_handle(grib_handle *h, int mask, int isMulti, int filter, const char *nameSpace) {

  int err;
  int keyType;
  int ctoi;
  long bitmapBool;
  long *bitmap;
  long *keyVal_l;
  double *keyVal_d;
  double missingValue;
  R_len_t totalKeys;
  R_len_t i;
  R_len_t n;
  size_t keySize;
  size_t keyLength;
  size_t bmp_len;
  const char *keyName;
  char intChar;
  char *keyVal_c;
  grib_keys_iterator *keyIter;
  SEXP rgrib_grib_message;
  SEXP rgrib_list_names;
  SEXP rgrib_char;
  /* rgrib_char = PROTECT(allocVector(STRSXP, 1)); */
  PROTECT_INDEX pro_char;
  PROTECT_WITH_INDEX(rgrib_char = R_NilValue, &pro_char);
  SEXP rgrib_double;
  double *p_rgib_double;
  PROTECT_INDEX pro_double;
  PROTECT_WITH_INDEX(rgrib_double = R_NilValue, &pro_double);
  SEXP rgrib_long;
  /* This is double since we will convert to R numeric */
  double *p_rgib_long;
  PROTECT_INDEX pro_long;
  PROTECT_WITH_INDEX(rgrib_long = R_NilValue, &pro_long);

  /*
   * Take care of these now in order to give the option
   * of masking out data with NAs
   */
  err = grib_get_double(h,"missingValue",&missingValue);
  if (err) {
    gerror("unable to get missing value", err);
  }

  err = grib_get_long(h, "bitmapPresent", &bitmapBool);
  if (err) {
    gerror("unable to get bitmapPresent variable", err);
  } else {
    if (bitmapBool) {
      grib_get_size(h, "bitmap", &bmp_len);
      bitmap = malloc(bmp_len*sizeof(long));
      err = grib_get_long_array(h, "bitmap", bitmap, &bmp_len);
      if (err) {
        gerror("unable to get bitmap array", err);
      }
    }
  }

  keyIter = grib_keys_iterator_new(h, filter, nameSpace);
  if (keyIter == NULL) {
    error("rGRIB: unable to create key iterator");
  }

  /* This seems like the only way to get the keys
   * count to then allocate a list to contain them.
   * Will monitor for a better solution. */
  totalKeys = 0;
  while(grib_keys_iterator_next(keyIter)) {
    R_CheckUserInterrupt();
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
    R_CheckUserInterrupt();
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
          if (mask && !strcmp(keyName, "values")) {
            for (i = 0; i < keySize; i++) {
              if (p_rgib_double[i] == missingValue || (bitmapBool && bitmap[i] == BITMAP_MASK)) {
              p_rgib_double[i] = NA_REAL;
              }
            }
          }
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
        break;
/*
 *Since R does not have a native long type, we can keep precision by doing
 * some casting and using doubles instead. Basically, C long --> R numeric
 */
      case GRIB_TYPE_LONG:
        if (keySize > 1) {
          keyVal_l = malloc(keySize*sizeof(long));
          err = grib_get_long_array(h, keyName, keyVal_l, &keySize);
          if (err) {
            gerror("unable to get long array", err);
          }
          REPROTECT(rgrib_long = allocVector(REALSXP, keySize), pro_long);
          p_rgib_long = REAL(rgrib_long);
          for (i = 0; i < keySize; i++) {
            p_rgib_long[i] = (double)keyVal_l[i];
          }
          free(keyVal_l);
          if (mask && !strcmp(keyName, "values")) {
            for (i = 0; i < keySize; i++) {
              if (p_rgib_long[i] == missingValue || (bitmapBool && bitmap[i] == BITMAP_MASK)) {
                p_rgib_long[i] = NA_REAL;
              }
            }
          }
        } else {
          keyVal_l = malloc(sizeof(long));
          err = grib_get_long(h, keyName, keyVal_l);
          if (err) {
            gerror("unable to get long scalar", err);
          }
          REPROTECT(rgrib_long = ScalarReal((double)*keyVal_l), pro_long);
          free(keyVal_l);
        }
        SET_VECTOR_ELT(rgrib_grib_message, n, rgrib_long);
        break;
/*
 * This is a working version of taking long values to R integers. Will keep
 * for now.
 */
/*      case GRIB_TYPE_LONG:
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
        break;
*/
      case GRIB_TYPE_STRING:
        keyLength = MAX_VAL_LEN;
        keyVal_c = malloc(keyLength);
        memset(keyVal_c, '\0', keyLength);
        err = grib_get_string(h, keyName, keyVal_c, &keyLength);
        if (err) {
          gerror("unable to get string", err);
        } else {
          intChar = keyVal_c[0];
          if (strlen(keyVal_c) == 1 && isdigit(intChar)) {
            /* Convert character integers to integers */
            ctoi = intChar - '0';
            REPROTECT(rgrib_char = ScalarInteger(ctoi), pro_char);
          } else {
            REPROTECT(rgrib_char = allocVector(STRSXP, 1), pro_char);
            SET_STRING_ELT(rgrib_char, 0, mkChar(keyVal_c));
          }
        }
        SET_VECTOR_ELT(rgrib_grib_message, n, rgrib_char);
        free(keyVal_c);
        break;
/* KEEP THIS OLD VERSION JUST IN CASE
      case GRIB_TYPE_STRING:
        keyLength = MAX_VAL_LEN;
        memset(keyVal_c, '\0', keyLength);
        err = grib_get_string(h, keyName, keyVal_c, &keyLength);
        if (err) {
          SET_STRING_ELT(rgrib_char, 0, ScalarString(NA_STRING));
        } else {
          SET_STRING_ELT(rgrib_char, 0, mkChar(keyVal_c));
        }
        SET_VECTOR_ELT(rgrib_grib_message, n, rgrib_char);
        break;
*/
      default:
        /* Skip others/errors */
        break;
      }
    }
    n++;
  }

  if (bitmap) {
    free(bitmap);
  }
  namesgets(rgrib_grib_message, rgrib_list_names);

  UNPROTECT(5);
  return rgrib_grib_message;
}
