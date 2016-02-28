#include <R.h>
#include <Rinternals.h>

#include <string.h>

#include "rGRIB.h"

SEXP rgrib_select(SEXP rgrib_filePath, SEXP rgrib_keyPairs, SEXP isMulti) {
  int err;
  int keyType;
  int ki;
  size_t indexSize;
  R_len_t i;
  R_len_t j;
  size_t n;
  size_t n_tot;
  size_t kvl;
  size_t reallocLength = MAX_VAL_LEN;
  R_len_t pairsLength;
  double kd;
  grib_index *index = NULL;
  grib_handle *h = NULL;
  grib_handle *hi = NULL;
  const char *file = NULL;
  const char *name = NULL;
  const char *ks = NULL;
  char *ksc = NULL;
  char *keyString = NULL;
  FILE *fileHandle = NULL;
  SEXP rgrib_keyNames;
  SEXP rgrib_selectList;

  file = CHAR(STRING_ELT(rgrib_filePath, 0));
  pairsLength = length(rgrib_keyPairs);
  rgrib_keyNames = PROTECT(getAttrib(rgrib_keyPairs, R_NamesSymbol));
  fileHandle = fopen(file,"r");
  if (!fileHandle) {
    error("rGRIB: unable to open grib file");
  }

  keyString = calloc(MAX_VAL_LEN, sizeof(char));
  if (!keyString) {
    error("rGRIB: unable to allocate keyString");
  }

  /* Keep track of number of characters and reallocate the final string
   * as necessary. The whole idea is to minimize chances of overflow
   * keeping the code as dynamic as possible. MAX_VAL_LEN is most
   * likely going to be more than enough in preponderance of cases
   **********************************************************************/
  n = 0; n_tot = 0;
  for (i = 0; i < pairsLength; i++) {
    name  = CHAR(STRING_ELT(rgrib_keyNames, i));
    n = strlen(name);
    if (!n) {
      error("rGRIB: unable to allocate keyString");
    }
    n_tot += n;
    if (n_tot > reallocLength - 1) {
      keyString = realloc(keyString, reallocLength += MAX_KEY_LEN);
    }
    strncat(keyString, CHAR(STRING_ELT(rgrib_keyNames, i)), n);
    if (i != pairsLength - 1) {
      strncat(keyString, ",", 1);
    }
  }

  if (asLogical(isMulti)) {
    grib_multi_support_on(DEFAULT_CONTEXT);
  }

  index = grib_index_new(DEFAULT_CONTEXT, keyString, &err);
  if (err) {
    gerror("unable to create index", err);
  }

  err = grib_index_add_file(index, file);
  if (err) {
    gerror("unable to add file to index", err);
  }

  /* Have to grab handle from file first in order to get
   * native key types. Will not work with handle from
   * index. Index handles will only work after using
   * the select method
   */
  h = grib_handle_new_from_file(DEFAULT_CONTEXT, fileHandle, &err);
  if (err) {
    gerror("unable to create grib handle", err);
  }

  for (i = 0; i < pairsLength; i++) {
    name  = CHAR(STRING_ELT(rgrib_keyNames, i));
    err = grib_get_native_type(h, name, &keyType);
    switch (keyType) {
    case GRIB_TYPE_STRING:
      kvl = length(VECTOR_ELT(rgrib_keyPairs, i));
      for (j = 0; j < kvl; j++) {
        ks = CHAR(STRING_ELT((VECTOR_ELT(rgrib_keyPairs, i)), j));
        ksc = calloc(strlen(ks), sizeof(char));
        strcpy(ksc, ks);
        grib_index_select_string(index, name, ksc);
        free(ksc);
      }
      break;
    case GRIB_TYPE_DOUBLE:
      kvl = length(VECTOR_ELT(rgrib_keyPairs, i));
      for (j = 0; j < kvl; j++) {
        kd = REAL(VECTOR_ELT(rgrib_keyPairs, i))[j];
        grib_index_select_double(index, name, kd);
      }
      break;
    case GRIB_TYPE_LONG:
      kvl = length(VECTOR_ELT(rgrib_keyPairs, i));
      for (j = 0; j < kvl; j++) {
        /* Need to coerce vector to integer as R list components entered
         * as integers really end up being numeric (double in C). Doing
         * corecion here make the most sense as the type gets deciced in
         * the get_naitve_type call. The C routines know more about the
         * typing than the R routines.
         */
        ki = INTEGER(coerceVector(VECTOR_ELT(rgrib_keyPairs, i), INTSXP))[j];
        grib_index_select_long(index, name, ki);
      }
      break;
    default:
      /* Skip other key types with no grib_select_* methods to handle them */
      break;
    }
  }

  /* rewind file to be sure nothing is askew */
  if (fseek(fileHandle, 0, SEEK_SET)) {
    error("rGRIB: unable rewind grib file");
  }
  grib_handle_delete(h);

  /* Need to get length of selected index */
  indexSize = 0;
  while ((hi = grib_handle_new_from_index(index, &err))){
    if (err) {
      error("rGRIB: unable get grib handle from index");
    }
    indexSize++;
  }
  grib_handle_delete(hi);

  rgrib_selectList = PROTECT(allocVector(VECSXP, indexSize));

  /*grib_index_rewind(index);
  while (!(hi = grib_handle_new_from_index(index, &err))){
    if (err) {
      error("rGRIB: unable get grib handle from index");
    }
  }
  grib_handle_delete(hi);*/

  Rprintf("%d\n", indexSize);

  grib_index_delete(index);
  free(keyString);

  UNPROTECT(2);
  return R_NilValue;
}
