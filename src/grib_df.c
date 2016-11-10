#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_df(SEXP gribr_fileHandle, SEXP gribr_filter, SEXP gribr_nameSpace, SEXP gribr_isMulti) {
  int err;
  int keyType;
  int toggle;
  R_xlen_t n;
  R_xlen_t m;
  R_xlen_t i;
  R_xlen_t j;
  size_t keyLength;
  long *keyVal_l = NULL;
  double *keyVal_d = NULL;
  char *keyVal_c = NULL;
  int is_multi;
  FILE *file = NULL;
  grib_handle *h = NULL;
  const char *nameSpace = NULL;
  int filter;
  SEXP gribr_grib_df;
  SEXP keyNames;
  SEXP keyTypes;
  SEXP rowNames;
  grib_keys_iterator* keyIter = NULL;

  filter = asInteger(gribr_filter);
  nameSpace = CHAR(STRING_ELT(gribr_nameSpace,0));
  is_multi = asLogical(gribr_isMulti);

  file = R_ExternalPtrAddr(gribr_fileHandle);
  if (file == NULL) {
    error("gribr: grib file not opened");
  }

  /* Make sure it is rewound */
  if (ftell(file) != GRIB_FILE_START) {
    if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
      error("gribr: unable to rewind file");
    }
  }

  if (is_multi) {
    grib_multi_support_on(DEFAULT_CONTEXT);
  }

  /* Get information about number of messages and keys */
  n = 0; /* message count */
  m = 0; /* key count */
  toggle = 0;

  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    n++;
    if (toggle == 0) {
      keyIter = grib_keys_iterator_new(h, filter, (char*)nameSpace);
      if (keyIter == NULL) {
        error("gribr: unable to create key iterator");
      }
      while(grib_keys_iterator_next(keyIter)) {
        m++;
      }
    } else if (toggle == 1) {
      /* The final data frame will be list of m (key names) length
       and it will contain lists of length n (key values) for each
       key name  */
      gribr_grib_df = PROTECT(allocVector(VECSXP, m));
      keyNames = PROTECT(allocVector(STRSXP, m));
      keyTypes = PROTECT(allocVector(INTSXP, m));

      i = 0;
      keyIter = grib_keys_iterator_new(h, filter, (char*)nameSpace);
      while(grib_keys_iterator_next(keyIter)) {
        const char *keyName = grib_keys_iterator_get_name(keyIter);
        SET_STRING_ELT(keyNames, i, mkChar(keyName));
        err = grib_get_native_type(h, keyName, &keyType);
        if (err) {
          gerror("unable to get native type", err);
        }
        INTEGER(keyTypes)[i] = keyType;
        i++;
      }
    }
    toggle++;
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  /* We need row.names so make them */
  rowNames = PROTECT(allocVector(INTSXP, n));
  for (i = 0; i < n; i++) {
    INTEGER(rowNames)[i] = i;
  }

  /* Key names will be column names */
  namesgets(gribr_grib_df, keyNames);

  for (i = 0; i < m; i ++) {
    switch(INTEGER(keyTypes)[i]) {
        case GRIB_TYPE_DOUBLE:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(REALSXP, n));
          break;
        case GRIB_TYPE_LONG:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(REALSXP, n));
          break;
        case GRIB_TYPE_STRING:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(STRSXP, n));
          break;
        default:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(STRSXP, n));
          /* Make sure the keyType is set correctly */
          //INTEGER(keyTypes)[i] = GRIB_TYPE_STRING;
          break;
    }
  }

  /* Make sure it is rewound */
  if (ftell(file) != GRIB_FILE_START) {
    if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
      error("gribr: unable to rewind file");
    }
  }

  j = 0;
  /* The grib handle is our GRIB message iterator. Each time we call new_from_file,
     we are advancing to the next message in the file. */
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err)) != NULL) {
    keyIter = grib_keys_iterator_new(h, filter, (char*)nameSpace);
    i = 0;
    while(grib_keys_iterator_next(keyIter)) {
      keyType = INTEGER(keyTypes)[i];
      switch(keyType) {
      case GRIB_TYPE_STRING:
        keyLength = MAX_VAL_LEN;
        keyVal_c = malloc(keyLength);
        memset(keyVal_c, '\0', keyLength);
        err = grib_get_string(h, CHAR(STRING_ELT(keyNames,i)), keyVal_c, &keyLength);
        if (err) {
          gerror("unable to get string", err);
        } else {
          SET_STRING_ELT(VECTOR_ELT(gribr_grib_df, i), j, mkChar(keyVal_c));
        }
        nfree(keyVal_c);
        i++;
        break;

      case GRIB_TYPE_LONG:
        keyVal_l = malloc(sizeof(long));
        err = grib_get_long(h, CHAR(STRING_ELT(keyNames,i)), keyVal_l);
        if (err) {
          gerror("unable to get long scalar", err);
        }
        REAL(VECTOR_ELT(gribr_grib_df, i))[j] = (double)*keyVal_l;
        nfree(keyVal_l);
        i++;
        break;
      case GRIB_TYPE_DOUBLE:
        keyVal_d = malloc(sizeof(double));
        err = grib_get_double(h, CHAR(STRING_ELT(keyNames,i)), keyVal_d);
        if (err) {
          gerror("unable to get double scalar", err);
        }
        REAL(VECTOR_ELT(gribr_grib_df, i))[j] = *keyVal_d;
        nfree(keyVal_d);
        i++;
        break;
      default:
        keyLength = MAX_VAL_LEN;
        keyVal_c = malloc(keyLength);
        memset(keyVal_c, '\0', keyLength);
        err = grib_get_string(h, CHAR(STRING_ELT(keyNames,i)), keyVal_c, &keyLength);
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
  }

  /* Be kind, please rewind. Without this the next call of grib_list will fail */
  if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
    error("gribr: unable to rewind file");
  }

  /* Finalize data.frame attributes and class */
  classgets(gribr_grib_df, mkString("data.frame"));
  setAttrib(gribr_grib_df, R_RowNamesSymbol, rowNames);

  grib_handle_delete(h);
  UNPROTECT(4);
  return gribr_grib_df;
}
