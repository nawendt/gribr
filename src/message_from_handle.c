#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_message_from_handle(codes_handle *h, int isMulti) {

  int err;
  int keyType;
  int ctoi;
  long bitmapBool;
  long *bitmap = NULL;
  long *keyVal_l = NULL;
  double *keyVal_d = NULL;
  double missingValue;
  R_len_t totalKeys;
  R_len_t i;
  R_len_t n;
  size_t keySize;
  size_t keyLength;
  size_t bmp_len;
  const char *keyName = NULL;
  char intChar;
  char *keyVal_c = NULL;
  codes_keys_iterator *keyIter = NULL;
  SEXP gribr_grib_message;
  SEXP gribr_list_names;
  SEXP gribr_char;
  /* gribr_char = PROTECT(allocVector(STRSXP, 1)); */
  PROTECT_INDEX pro_char;
  PROTECT_WITH_INDEX(gribr_char = R_NilValue, &pro_char);
  SEXP gribr_double;
  double *p_rgib_double = NULL;
  PROTECT_INDEX pro_double;
  PROTECT_WITH_INDEX(gribr_double = R_NilValue, &pro_double);
  SEXP gribr_long;
  /* This is double since we will convert to R numeric */
  double *p_rgib_long = NULL;
  PROTECT_INDEX pro_long;
  PROTECT_WITH_INDEX(gribr_long = R_NilValue, &pro_long);

  /*
   * Take care of these now in order to give the option
   * of masking out data with NAs
   */
  err = codes_get_double(h,"missingValue",&missingValue);
  if (err) {
    gerror("unable to get missing value", err);
  }

  err = codes_get_long(h, "bitmapPresent", &bitmapBool);
  if (err) {
    gerror("unable to get bitmapPresent variable", err);
  } else {
    if (bitmapBool) {
      codes_get_size(h, "bitmap", &bmp_len);
      bitmap = malloc(bmp_len*sizeof(long));
      err = codes_get_long_array(h, "bitmap", bitmap, &bmp_len);
      if (err) {
        gerror("unable to get bitmap array", err);
      }
    }
  }

  keyIter = codes_keys_iterator_new(h, NO_FILTER, NULL_NAMESPACE);
  if (keyIter == NULL) {
    error("gribr: unable to create key iterator");
  }

  /* This seems like the only way to get the keys
   * count to then allocate a list to contain them.
   * Will monitor for a better solution. */
  totalKeys = 0;
  while(codes_keys_iterator_next(keyIter)) {
    if (totalKeys % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    keyName = codes_keys_iterator_get_name(keyIter);
    err = codes_get_native_type(h, keyName, &keyType);
    if (skip_keys(keyName, keyType, err)) {
      continue;
    } else {
      totalKeys++;
    }
  }

  err = codes_keys_iterator_rewind(keyIter);
  if (err) {
    gerror("unable to rewind keys iterator", err);
  }

  gribr_grib_message = PROTECT(allocVector(VECSXP, totalKeys));
  gribr_list_names = PROTECT(allocVector(STRSXP, totalKeys));

  n = 0;
  while(codes_keys_iterator_next(keyIter)) {
    if (n % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    keyName = codes_keys_iterator_get_name(keyIter);
    err = codes_get_native_type(h, keyName, &keyType);
    if (skip_keys(keyName, keyType, err)) {
      continue;
    } else {
      SET_STRING_ELT(gribr_list_names, n, mkChar(keyName));
      err = codes_get_size(h, keyName, &keySize);
      if (err) {
        gerror("unable to get key value size", err);
      }
      switch(keyType) {

      case CODES_TYPE_DOUBLE:
        if (keySize > 1) {
          keyVal_d = malloc(keySize*sizeof(double));
          err = codes_get_double_array(h, keyName, keyVal_d, &keySize);
          if (err) {
            gerror("unable to get double array", err);
          }
          REPROTECT(gribr_double = allocVector(REALSXP, keySize), pro_double);
          p_rgib_double = REAL(gribr_double);
          for (i = 0; i < keySize; i++) {
            p_rgib_double[i] = keyVal_d[i];
          }
          nfree(keyVal_d);
          if (!strcmp(keyName, "values")) {
            for (i = 0; i < keySize; i++) {
              if (p_rgib_double[i] == missingValue || (bitmapBool && bitmap[i] == BITMAP_MASK)) {
              p_rgib_double[i] = NA_REAL;
              }
            }
          }
        } else {
          keyVal_d = malloc(sizeof(double));
          err = codes_get_double(h, keyName, keyVal_d);
          if (err) {
            gerror("unable to get double scalar", err);
          }
          if (*keyVal_d == CODES_MISSING_DOUBLE) {
            REPROTECT(gribr_double = ScalarReal(NA_REAL), pro_double);
            nfree(keyVal_d);
          } else {
            REPROTECT(gribr_double = ScalarReal(*keyVal_d), pro_double);
            nfree(keyVal_d);
          }
        }
        SET_VECTOR_ELT(gribr_grib_message, n, gribr_double);
        break;
/*
 * Since R does not have a native long type, we can keep precision by doing
 * some casting and using doubles instead. Basically, C long --> R numeric
 */
      case CODES_TYPE_LONG:
        if (keySize > 1) {
          keyVal_l = malloc(keySize*sizeof(long));
          err = codes_get_long_array(h, keyName, keyVal_l, &keySize);
          if (err) {
            gerror("unable to get long array", err);
          }
          REPROTECT(gribr_long = allocVector(REALSXP, keySize), pro_long);
          p_rgib_long = REAL(gribr_long);
          for (i = 0; i < keySize; i++) {
            p_rgib_long[i] = (double)keyVal_l[i];
          }
          nfree(keyVal_l);
          if (!strcmp(keyName, "values")) {
            for (i = 0; i < keySize; i++) {
              if (p_rgib_long[i] == missingValue || (bitmapBool && bitmap[i] == BITMAP_MASK)) {
                p_rgib_long[i] = NA_REAL;
              }
            }
          }
        } else {
          keyVal_l = malloc(sizeof(long));
          err = codes_get_long(h, keyName, keyVal_l);
          if (err) {
            gerror("unable to get long scalar", err);
          }
          if (*keyVal_l == CODES_MISSING_LONG) {
            REPROTECT(gribr_long = ScalarInteger(NA_INTEGER), pro_long);
            nfree(keyVal_l);
          } else {
            REPROTECT(gribr_long = ScalarReal((double)*keyVal_l), pro_long);
            nfree(keyVal_l);
          }

        }
        SET_VECTOR_ELT(gribr_grib_message, n, gribr_long);
        break;

      case CODES_TYPE_STRING:
        keyLength = MAX_VAL_LEN;
        keyVal_c = malloc(keyLength);
        memset(keyVal_c, '\0', keyLength);
        err = codes_get_string(h, keyName, keyVal_c, &keyLength);
        if (err) {
          gerror("unable to get string", err);
        } else {
          intChar = keyVal_c[0];
          if (strlen(keyVal_c) == 1 && isdigit(intChar)) {
            /* Convert character integers to integers */
            ctoi = intChar - '0';
            REPROTECT(gribr_char = ScalarInteger(ctoi), pro_char);
          } else {
            REPROTECT(gribr_char = allocVector(STRSXP, 1), pro_char);
            SET_STRING_ELT(gribr_char, 0, mkChar(keyVal_c));
          }
        }
        SET_VECTOR_ELT(gribr_grib_message, n, gribr_char);
        nfree(keyVal_c);
        break;

      default:
        /* Skip others/errors */
        break;
      }
    }
    n++;
  }

  codes_keys_iterator_delete(keyIter);

  if (bitmap) {
    nfree(bitmap);
  }
  namesgets(gribr_grib_message, gribr_list_names);
  classgets(gribr_grib_message, mkString("gribMessage"));

  UNPROTECT(5);
  return gribr_grib_message;
}
