#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_df(SEXP gribr_fileHandle, SEXP gribr_filter, SEXP gribr_namespace, SEXP gribr_isMulti) {
  int err;
  int keyType;
  size_t toggle;
  R_xlen_t n;
  R_xlen_t m;
  R_xlen_t i;
  R_xlen_t j;
  size_t keyLength;
  long *keyVal_l = NULL;
  double *keyVal_d = NULL;
  char *keyVal_c = NULL;
  const char *keyName = NULL;
  int is_multi;
  FILE *file = NULL;
  codes_handle *h = NULL;
  const char *namespace = NULL;
  int filter;
  SEXP gribr_grib_df;
  SEXP keyNames;
  SEXP keyTypes;
  SEXP rowNames;
  codes_keys_iterator* keyIter = NULL;

  filter = asInteger(gribr_filter);
  namespace = CHAR(STRING_ELT(gribr_namespace, 0));
  is_multi = asLogical(gribr_isMulti);

  file = R_ExternalPtrAddr(gribr_fileHandle);
  if (file == NULL) {
    error("gribr: grib file not opened");
  }

  /* Make sure it is rewound */
  grewind(file);

  if (is_multi) {
    codes_grib_multi_support_on(DEFAULT_CONTEXT);
  }

  /* Get information about number of messages and keys */
  n = 0; /* message count */
  m = 0; /* key count */
  toggle = 0;

  while((h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    if (n % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    n++;
    keyIter = codes_keys_iterator_new(h, filter, (char*)namespace);
    if (keyIter == NULL) {
      error("gribr: unable to create key iterator");
    }
    if (toggle == 0) {
      while(codes_keys_iterator_next(keyIter)) {
        m++;
      }
      codes_keys_iterator_delete(keyIter);
    }
    toggle++;
    codes_handle_delete(h);
  }
  if (err) {
    gerror("unable to count grib messages/keys", err);
  }

  /* Make sure it is rewound */
  grewind(file);

  /* The final data frame will be list of m (key names) length
   and it will contain lists of length n (key values) for each
  key name  */
  gribr_grib_df = PROTECT(allocVector(VECSXP, m));
  keyNames = PROTECT(allocVector(STRSXP, m));
  keyTypes = PROTECT(allocVector(INTSXP, m));

  while((h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    i = 0;
    keyIter = codes_keys_iterator_new(h, filter, (char*)namespace);
    while(codes_keys_iterator_next(keyIter)) {
      const char *keyName = codes_keys_iterator_get_name(keyIter);
      SET_STRING_ELT(keyNames, i, mkChar(keyName));
      err = codes_get_native_type(h, keyName, &keyType);
      if (err) {
        gerror("unable to get native type", err);
      }
      INTEGER(keyTypes)[i] = keyType;
      i++;
    }
    codes_keys_iterator_delete(keyIter);
    codes_handle_delete(h);
  }
  if (err) {
    gerror("unable to process table header", err);
  }

  /* We need row.names so make them */
  rowNames = PROTECT(allocVector(INTSXP, n));
  for (i = 0; i < n; i++) {
    INTEGER(rowNames)[i] = i + 1; /* R indices start at 1 */
  }

  /* Key names will be column names */
  namesgets(gribr_grib_df, keyNames);

  for (i = 0; i < m; i ++) {
    if (i % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    switch(INTEGER(keyTypes)[i]) {
        case CODES_TYPE_DOUBLE:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(REALSXP, n));
          break;
        case CODES_TYPE_LONG:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(REALSXP, n));
          break;
        case CODES_TYPE_STRING:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(STRSXP, n));
          break;
        default:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(STRSXP, n));
          break;
    }
  }

  /* Make sure it is rewound */
  grewind(file);

  j = 0;
  /* The grib handle is our GRIB message iterator. Each time we call new_from_file,
     we are advancing to the next message in the file. */
  while((h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err)) != NULL) {
    if (j % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    keyIter = codes_keys_iterator_new(h, filter, (char*)namespace);
    i = 0;
    while(codes_keys_iterator_next(keyIter)) {
      keyName = codes_keys_iterator_get_name(keyIter);
      keyType = INTEGER(keyTypes)[i];
      switch(keyType) {
      case CODES_TYPE_STRING:
        codes_get_length(h, keyName, &keyLength);
        keyVal_c = calloc(keyLength, sizeof(char));
        err = codes_get_string(h, CHAR(STRING_ELT(keyNames, i)), keyVal_c, &keyLength);
        if (err) {
          gerror("unable to get string", err);
        } else {
          SET_STRING_ELT(VECTOR_ELT(gribr_grib_df, i), j, mkChar(keyVal_c));
        }
        nfree(keyVal_c);
        i++;
        break;

      case CODES_TYPE_LONG:
        keyVal_l = malloc(sizeof(long));
        err = codes_get_long(h, CHAR(STRING_ELT(keyNames, i)), keyVal_l);
        if (err) {
          gerror("unable to get long scalar", err);
        }
        REAL(VECTOR_ELT(gribr_grib_df, i))[j] = (double)*keyVal_l;
        nfree(keyVal_l);
        i++;
        break;
      case CODES_TYPE_DOUBLE:
        keyVal_d = malloc(sizeof(double));
        err = codes_get_double(h, CHAR(STRING_ELT(keyNames, i)), keyVal_d);
        if (err) {
          gerror("unable to get double scalar", err);
        }
        REAL(VECTOR_ELT(gribr_grib_df, i))[j] = *keyVal_d;
        nfree(keyVal_d);
        i++;
        break;
      default:
        codes_get_length(h, keyName, &keyLength);
        keyVal_c = calloc(keyLength, sizeof(char));
        err = codes_get_string(h, CHAR(STRING_ELT(keyNames, i)), keyVal_c, &keyLength);
        if (err) {
          gerror("unable to get string", err);
        } else {
          SET_STRING_ELT(VECTOR_ELT(gribr_grib_df, i), j, mkChar(keyVal_c));
        }
        nfree(keyVal_c);
        i++;
        break;
      }
    }
    j++;
    codes_keys_iterator_delete(keyIter);
    codes_handle_delete(h);
  }

  /* Be kind, please rewind. Without this the next call of grib_df will fail */
  grewind(file);

  /* Finalize data.frame attributes and class */
  classgets(gribr_grib_df, mkString("data.frame"));
  setAttrib(gribr_grib_df, R_RowNamesSymbol, rowNames);

  UNPROTECT(4);
  return gribr_grib_df;
}
