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
  int is_multi;
  size_t messageCount = 0;
  FILE *file = NULL;
  grib_handle *h = NULL;
  const char *nameSpace = NULL;
  char *lastComma = NULL;
  char keyString[MAX_VAL_LEN];
  int filter;
  char value[MAX_VAL_LEN];
  size_t valueLength=MAX_VAL_LEN;
  SEXP gribr_grib_df;
  SEXP keyNames;
  SEXP keyTypes;
  grib_keys_iterator* keyIter=NULL;

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
  n = 0;
  m = 0;
  toggle = 0;
  
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    n++;
    if (toggle == 0) {
        keyIter=grib_keys_iterator_new(h, filter, (char*)nameSpace);
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
        while(grib_keys_iterator_next(keyIter)) {
            const char *keyName = grib_keys_iterator_get_name(keyIter);
            err = grib_get_native_type(h, keyName, &keyType);
            SET_STRING_ELT(keyNames, i, mkChar(keyName));
            err = grib_get_native_type(h, keyName, &keyType);
            SET_STRING_ELT(keyTypes, i, keyType);
        }
    }
    toggle++;
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  /* Key names will be column names */
  namesgets(gribr_grib_df, keyNames);

  for (i = 0; i < m; i ++) {
    switch(INTEGER(keyTypes)[i]) {
        case GRIB_TYPE_DOUBLE:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(REALSXP, n));
        case GRIB_TYPE_LONG:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(INTSXP, n));
        case GRIB_TYPE_STRING:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(STRSXP, n));
        default:
          SET_VECTOR_ELT(gribr_grib_df, i, allocVector(STRSXP, n));
    }
  }

  // err = grib_keys_iterator_rewind(keyIter);
  // if (err) {
  //   gerror("unable to rewind keys iterator", err);
  // }

  /* Make sure it is rewound */
  if (ftell(file) != GRIB_FILE_START) {
    if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
      error("gribr: unable to rewind file");
    }
  }

  i = 0;
  j = 0;
  // /* The grib handle is our GRIB message iterator. Each time we call new_from_file,
  //    we are advancing to the next message in the file. */
  // while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err)) != NULL) {
  //   grib_keys_iterator* keyIter=NULL;

  //   if (h == NULL) {
  //     gerror("gribr: unable to create grib handle", err);
  //   }

  //   keyIter=grib_keys_iterator_new(h, filter, (char*)nameSpace);
  //   if (keyIter == NULL) {
  //     error("gribr: unable to create key iterator");
  //   }

  //   memset(keyString, '\0', MAX_VAL_LEN);
  //   while(grib_keys_iterator_next(keyIter)) {
  //     valueLength = MAX_VAL_LEN;
  //     const char *keyName = grib_keys_iterator_get_name(keyIter);
  //     memset(value, '\0', valueLength);
  //     err = grib_get_string(h, keyName, value, &valueLength);
  //     if ((strlen(keyString) + strlen(keyName) + strlen(value) + 2) < MAX_VAL_LEN ||
  //         (strlen(keyString) + NA_KEY_LEN < MAX_VAL_LEN)) {
  //       if (err) {
  //         strncat(keyString," NA=NA,",NA_KEY_LEN);
  //       } else {
  //         strncat(keyString ,keyName, strlen(keyName));
  //         strncat(keyString, "=", 1);
  //         strncat(keyString, value, valueLength);
  //         strncat(keyString, ",", 1);
  //       }
  //     } else {
  //       warning("gribr: string overflow, truncating");
  //     }
  //   }
  //   /* Clean up the trailing comma */
  //   lastComma = strrchr(keyString,',');
  //   *lastComma = '\0';
  //   SET_STRING_ELT(gribr_grib_vec, messageCount++, mkChar(keyString));

  //   grib_keys_iterator_delete(keyIter);
  //   grib_handle_delete(h);

  // }
  /* Be kind, please rewind. Without this the next call of grib_list will fail */
  if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
    error("gribr: unable to rewind file");
  }

  classgets(gribr_grib_df, mkString("data.frame"));
  grib_handle_delete(h);
  UNPROTECT(3);
  return gribr_grib_df;
}
