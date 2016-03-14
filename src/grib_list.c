#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_list(SEXP rgrib_fileHandle, SEXP rgrib_filter, SEXP rgrib_nameSpace) {
  int err,n;
  size_t messageCount = 0;
  FILE *file = NULL;
  grib_handle *h = NULL;
  const char *nameSpace = NULL;
  char *lastComma = NULL;
  char keyString[MAX_VAL_LEN];
  int filter;
  char value[MAX_VAL_LEN];
  size_t valueLength=MAX_VAL_LEN;
  SEXP rgrib_grib_vec;

  filter = asInteger(rgrib_filter);
  nameSpace = CHAR(STRING_ELT(rgrib_nameSpace,0));

  file = R_ExternalPtrAddr(rgrib_fileHandle);
  if (file == NULL) {
    error("rGRIB: grib file not opened");
  }

  /* Make sure it is rewound */
  if (ftell(file) != GRIB_FILE_START) {
    if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
      error("rGRIB: unable to rewind file");
    }
  }

  grib_multi_support_on(DEFAULT_CONTEXT);
  err = grib_count_in_file(DEFAULT_CONTEXT, file, &n);
  if (err) {
    gerror("rGRIB: unable to count messages", err);
  }

  rgrib_grib_vec = PROTECT(allocVector(STRSXP, n));

  /* The grib handle is our GRIB message iterator. Each time we call new_from_file,
     we are advancing to the next message in the file. */
  grib_multi_support_on(DEFAULT_CONTEXT);
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err)) != NULL) {
    grib_keys_iterator* keyIter=NULL;

    if (h == NULL) {
      gerror("rGRIB: unable to create grib handle", err);
    }

    keyIter=grib_keys_iterator_new(h, filter, nameSpace);
    if (keyIter == NULL) {
      error("rGRIB: unable to create key iterator");
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
        warning("rGRIB: string overflow, truncating");
      }
    }
    /* Clean up the trailing comma */
    lastComma = strrchr(keyString,',');
    *lastComma = '\0';
    SET_STRING_ELT(rgrib_grib_vec, messageCount++, mkChar(keyString));

    grib_keys_iterator_delete(keyIter);
    grib_handle_delete(h);

  }
  /* Be kind, please rewind. Without this the next call of grib_list will fail */
  if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
    error("rGRIB: unable to rewind file");
  }

  grib_handle_delete(h);
  UNPROTECT(1);
  return rgrib_grib_vec;
}
