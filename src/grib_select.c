#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_select(SEXP rgrib_filePath, SEXP rgrib_keyPairs) {
  int err;
  int keyType;
  R_len_t i;
  size_t n;
  size_t n_tot;
  size_t k;
  size_t reallocLength = MAX_VAL_LEN;
  R_len_t pairsLength;
  grib_index *index = NULL;
  grib_handle *h = NULL;
  const char *file = NULL;
  char *keyString = NULL;
  SEXP rgrib_keyNames;

  file = CHAR(STRING_ELT(rgrib_filePath, 0));
  pairsLength = length(rgrib_keyPairs);
  rgrib_keyNames = PROTECT(getAttrib(rgrib_keyPairs, R_NamesSymbol));

  keyString = calloc(MAX_VAL_LEN, sizeof(char));
  if (!keyString) {
    error("%s(%d): unable to allocate keyString", __FILE__, __LINE__);
  }

  /* Keep track of number of characters and reallocate the final string
   * as necessary. The whole idea is to minimize chances of overflow
   * keeping the code as dynamic as possible. MAX_VAL_LEN is most
   * likely going to be more than enough in preponderance of cases */
  n = 0; k = 0;
  for (i = 0; i < pairsLength; i++) {
    n = strnlen(CHAR(STRING_ELT(rgrib_keyNames, i)), MAX_KEY_LEN - 1);
    if (!n) {
      error("%s(%d): unable to allocate keyString", __FILE__, __LINE__);
    }
    n_tot += n;
    if (n_tot > reallocLength - 1) {
      k++;
      keyString = realloc(keyString, reallocLength += MAX_KEY_LEN * k);
    }
    strncat(keyString, CHAR(STRING_ELT(rgrib_keyNames, i)), n);
    if (i != pairsLength - 1) {
      strncat(keyString, ",", 1);
    }
  }

  index = grib_index_new(DEFAULT_CONTEXT, keyString, &err);
  if (err) {
    gerror("unable to create index", err);
  }

  err = grib_index_add_file(index, file);
  if (err) {
    gerror("unable to add file to index", err);
  }

  h = grib_handle_new_from_index(index, &err);

  for (i = 0; i < pairsLength; i++) {
    err = grib_get_native_type(h, CHAR(STRING_ELT(rgrib_keyNames, i)), &keyType);
    switch (keyType) {
      case
    }
  }

  grib_index_delete(index);
  grib_handle_delete(h);

  UNPROTECT(1);
  return mkString(keyString);
}
