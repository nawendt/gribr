#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_list(SEXP gribr_fileHandle, SEXP gribr_filter, SEXP gribr_namespace, SEXP gribr_isMulti) {
  int err;
  R_xlen_t n;
  int is_multi;
  size_t messageCount = 0;
  FILE *file = NULL;
  grib_handle *h = NULL;
  const char *namespace = NULL;
  char *lastComma = NULL;
  char keyString[MAX_VAL_LEN];
  int filter;
  char value[MAX_VAL_LEN];
  size_t valueLength=MAX_VAL_LEN;
  SEXP gribr_grib_vec;

  filter = asInteger(gribr_filter);
  namespace = CHAR(STRING_ELT(gribr_namespace,0));
  is_multi = asLogical(gribr_isMulti);

  file = R_ExternalPtrAddr(gribr_fileHandle);
  if (file == NULL) {
    error("gribr: grib file not opened");
  }

  /* Make sure it is rewound */
  grewind(file);

  if (is_multi) {
    grib_multi_support_on(DEFAULT_CONTEXT);
  }
  n = 0;
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    n++;
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  /* Make sure it is rewound */
  grewind(file);

  gribr_grib_vec = PROTECT(allocVector(STRSXP, n));

  /* The grib handle is our GRIB message iterator. Each time we call new_from_file,
     we are advancing to the next message in the file. */
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err)) != NULL) {
    grib_keys_iterator* keyIter=NULL;

    if (h == NULL) {
      gerror("gribr: unable to create grib handle", err);
    }

    keyIter=grib_keys_iterator_new(h, filter, (char*)namespace);
    if (keyIter == NULL) {
      error("gribr: unable to create key iterator");
    }

    memset(keyString, '\0', MAX_VAL_LEN);
    while(grib_keys_iterator_next(keyIter)) {
      valueLength = MAX_VAL_LEN;
      const char *keyName = grib_keys_iterator_get_name(keyIter);
      memset(value, '\0', valueLength);
      err = grib_get_string(h, keyName, value, &valueLength);
      if ((strlen(keyString) + strlen(keyName) + strlen(value) + 2) < MAX_VAL_LEN ||
          (strlen(keyString) + NA_KEY_LEN < MAX_VAL_LEN)) {
        if (err) {
          strncat(keyString," NA=NA,",NA_KEY_LEN);
        } else {
          strncat(keyString ,keyName, strlen(keyName));
          strncat(keyString, "=", 1);
          strncat(keyString, value, valueLength);
          strncat(keyString, ",", 1);
        }
      } else {
        warning("gribr: string overflow, truncating");
      }
    }
    /* Clean up the trailing comma */
    lastComma = strrchr(keyString,',');
    *lastComma = '\0';
    SET_STRING_ELT(gribr_grib_vec, messageCount++, mkChar(keyString));

    grib_keys_iterator_delete(keyIter);
    grib_handle_delete(h);

  }
  /* Be kind, please rewind. Without this the next call of grib_list will fail */
  if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
    error("gribr: unable to rewind file");
  }

  grib_handle_delete(h);
  UNPROTECT(1);
  return gribr_grib_vec;
}
