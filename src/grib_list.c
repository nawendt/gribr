#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_list(SEXP gribr_fileHandle, SEXP gribr_filter, SEXP gribr_namespace, SEXP gribr_isMulti) {
  int err;
  R_xlen_t n;
  int is_multi;
  size_t messageCount = 0;
  size_t ks_size;
  size_t concat = 0;
  FILE *file = NULL;
  codes_handle *h = NULL;
  const char *namespace = NULL;
  size_t keyLength;
  const char *keyName = NULL;
  char *keyString = NULL;
  int filter;
  char *keyVal_c = NULL;
  codes_keys_iterator* keyIter = NULL;
  SEXP gribr_grib_vec;

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
  
  n = 0;
  while((h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    n++;
    codes_handle_delete(h);
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  /* Make sure it is rewound */
  grewind(file);

  gribr_grib_vec = PROTECT(allocVector(STRSXP, n));

  /* The grib handle is our GRIB message iterator. Each time we call new_from_file,
     we are advancing to the next message in the file. */
  while((h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err)) != NULL) {

    if (h == NULL) {
      gerror("gribr: unable to create grib handle", err);
    }

    keyIter = codes_keys_iterator_new(h, filter, (char*)namespace);
    if (keyIter == NULL) {
      error("gribr: unable to create key iterator");
    }

    /* Loop through the keys to get size of key:value pairs */
    ks_size = 0;
    while(codes_keys_iterator_next(keyIter)) {
      keyName = codes_keys_iterator_get_name(keyIter);
      codes_get_length(h, keyName, &keyLength);
      keyVal_c = malloc(keyLength);
      if (!keyVal_c) {
        error("gribr: problem allocating value string");
      }
      err = codes_get_string(h, keyName, keyVal_c, &keyLength);
      if (err) {
        gerror("unable to get value string", err);
      }
      /* Add space for '=' and ',' */
      ks_size += strlen(keyName) + strlen(keyVal_c) + 2;
      nfree(keyVal_c);
    }
    /* space for null terminator */
    ks_size += 1;

    keyString = calloc(ks_size, sizeof(char));
    if (!keyString) {
      error("gribr: problem allocating key:value string");
    }

    err = codes_keys_iterator_rewind(keyIter);
    if (err) {
      gerror("could not rewind keys iterator", err);
    }

    while(codes_keys_iterator_next(keyIter)) {
      keyName = codes_keys_iterator_get_name(keyIter);
      codes_get_length(h, keyName, &keyLength);
      keyVal_c = malloc(keyLength);
      if (!keyVal_c) {
        error("gribr: problem allocating value string");
      }
      err = codes_get_string(h, keyName, keyVal_c, &keyLength);
      if (err) {
        gerror("unable to get value string", err);
      } else {
        concat = strlcat(keyString, keyName, ks_size);
        concat = strlcat(keyString, "=", ks_size);
        concat = strlcat(keyString, keyVal_c, ks_size);
        concat = strlcat(keyString, ",", ks_size);
      }
      nfree(keyVal_c);
    }

    if (concat >= ks_size) {
      warning("gribr: key:value string was truncated");
    }

    /* Clean up the trailing comma */
    keyString[ks_size - 2] = 0;

    SET_STRING_ELT(gribr_grib_vec, messageCount++, mkChar(keyString));

    codes_keys_iterator_delete(keyIter);
    codes_handle_delete(h);
    nfree(keyString);
  }

  /* Be kind, please rewind. Without this the next call of grib_list will fail */
  grewind(file);

  UNPROTECT(1);
  return gribr_grib_vec;
}
