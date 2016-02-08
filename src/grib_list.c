#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP R_grib_list(SEXP R_fileHandle, SEXP R_filter, SEXP R_nameSpace) {
  int err,n;
  R_len_t floc;
  size_t messageCount = 0;
  FILE *file = NULL;
  grib_handle *h = NULL;
  const char *nameSpace = NULL;
  char *lastComma = NULL;
  char keyString[MAX_VAL_LEN];
  int filter;
  char value[MAX_VAL_LEN];
  size_t valueLength=MAX_VAL_LEN;

  filter = asInteger(R_filter);
  nameSpace = CHAR(STRING_ELT(R_nameSpace,0));

  file = R_ExternalPtrAddr(R_fileHandle);
  if (file == NULL) {
    error("%s(%d): grib file not opened", __FILE__ ,__LINE__);
  }

  /* Make sure it is rewound */
  if (ftell(file) != GRIB_FILE_START) {
    if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
      error("%s(%d): unable to rewind file",__FILE__,__LINE__);
    }
  }

  err = grib_count_in_file(DEFAULT_CONTEXT, file, &n);
  if (err) {
    error("%s(%d): unable to count messages; GRIB ERROR %3d", __FILE__, __LINE__, err);
  }
  SEXP R_grib_vec = PROTECT(allocVector(STRSXP, n));

  /* The grib handle is our GRIB message iterator. Each time we call new_from_file,
     we are advancing to the next message in the file. */
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err)) != NULL) {
    grib_keys_iterator* keyIter=NULL;

    if(h == NULL) {
      error("%s(%d): unable to create grib handle: GRIB ERROR %3d", __FILE__, __LINE__, err);
    }

    keyIter=grib_keys_iterator_new(h, filter, nameSpace);
    if (keyIter == NULL) {
      error("%s(%d): unable to create key iterator", __FILE__, __LINE__);
    }

    bzero(keyString,MAX_VAL_LEN);
    while(grib_keys_iterator_next(keyIter)) {
      valueLength = MAX_VAL_LEN;
      const char *keyName = grib_keys_iterator_get_name(keyIter);
      bzero(value, valueLength);
      err = grib_get_string(h, keyName, value, &valueLength);
      if (err) {
        strncat(keyString," NA:NA,",NA_KEY_LEN);
      } else {
        strncat(keyString,keyName,strnlen(keyName,MAX_KEY_LEN));
        strncat(keyString,"=",1);
        strncat(keyString,value,valueLength);
        strncat(keyString,",",1);
      }
    }

    /* Clean up the trailing comma */
    lastComma = strrchr(keyString,',');
    *lastComma = '\0';
    SET_STRING_ELT(R_grib_vec, messageCount++, mkChar(keyString));

    grib_keys_iterator_delete(keyIter);
    grib_handle_delete(h);

  }
  /* Be kind, please rewind. Without this the next call of grib_list will fail */
  if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
    error("%s(%d): unable to rewind file",__FILE__,__LINE__);
  }

  UNPROTECT(1);
  return R_grib_vec;
}
