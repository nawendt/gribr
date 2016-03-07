#include <R.h>
#include <Rinternals.h>

#include <string.h>

#include "rGRIB.h"

SEXP rgrib_select(SEXP rgrib_filePath, SEXP rgrib_keyList, SEXP rgrib_mask, SEXP rgrib_isMulti,
                   SEXP rgrib_filter, SEXP rgrib_nameSpace) {
  int err;
  int keyType;
  int filter;
  int is_masked;
  int is_multi;
  long ki;
  double kd;
  R_len_t lenKeysList;
  R_len_t lenKeys;
  R_len_t m;
  R_len_t n;
  R_len_t n_tot;
  R_len_t i;
  R_len_t j;
  R_len_t index_count;
  size_t reallocLength = MAX_VAL_LEN;
  char *keyString = NULL;
  const char *filePath = NULL;
  const char *keyName = NULL;
  const char *nameSpace = NULL;
  const char *ks = NULL;
  char *ksc = NULL;
  FILE *fileHandle = NULL;
  grib_index *index = NULL;
  grib_handle *h = NULL;
  grib_handle *hi = NULL;
  SEXP rgrib_selected;
  SEXP rgrib_temp;
  PROTECT_INDEX pro_temp;

  PROTECT_WITH_INDEX(rgrib_temp = R_NilValue, &pro_temp);

  nameSpace = CHAR(STRING_ELT(rgrib_nameSpace, 0));
  filter = asInteger(rgrib_filter);
  is_multi = asLogical(rgrib_isMulti);
  is_masked = asLogical(rgrib_mask);

  filePath = CHAR(STRING_ELT(rgrib_filePath, 0));
  lenKeysList = xlength(rgrib_keyList);
  rgrib_selected = PROTECT(allocVector(VECSXP, lenKeysList));
  fileHandle = fopen(filePath, "r");
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
  lenKeys = xlength(VECTOR_ELT(rgrib_keyList, 0));
  /*
   * One loop will work since the input list will have all the same
   * keys
   */
  for (i = 0; i < lenKeys; i++) {
    keyName = CHAR(STRING_ELT(getAttrib(VECTOR_ELT(rgrib_keyList, 0), R_NamesSymbol), i));
    n = strlen(keyName);
    if (!n) {
      error("rGRIB: unable to get keyString length");
    }
    n_tot += n;
    if (n_tot > reallocLength - 1) {
      keyString = realloc(keyString, reallocLength += MAX_KEY_LEN);
    }
    strncat(keyString, keyName, n);
    if (i != lenKeys - 1) {
      strncat(keyString, ",", 1);
    }
  }

  if (asLogical(rgrib_isMulti)) {
    grib_multi_support_on(DEFAULT_CONTEXT);
  }

  index = grib_index_new(DEFAULT_CONTEXT, keyString, &err);
  if (err) {
    gerror("unable to create index", err);
  }

  err = grib_index_add_file(index, filePath);
  if (err) {
    gerror("unable to add file to index", err);
  }

  nfree(keyString);

  /* Have to grab handle from file first in order to get
   * native key types. Will not work with handle from
   * index. Index handles will only work after using
   * the select method
   */
  h = grib_handle_new_from_file(DEFAULT_CONTEXT, fileHandle, &err);
  if (err) {
    gerror("unable to create grib handle", err);
  }

  for (i = 0; i < lenKeysList; i++) {
    lenKeys = xlength(VECTOR_ELT(rgrib_keyList, i));
    for (j = 0; j < lenKeys; j++) {
      keyName = CHAR(STRING_ELT(getAttrib(VECTOR_ELT(rgrib_keyList, i), R_NamesSymbol), j));
      err = grib_get_native_type(h, keyName, &keyType);
      if (err) {
        gerror("unable to get native type", err);
      }
      switch (keyType) {
      case GRIB_TYPE_DOUBLE:
        kd = asReal(VECTOR_ELT(VECTOR_ELT(rgrib_keyList, i), j));
        grib_index_select_double(index, keyName, kd);
        break;
      case GRIB_TYPE_LONG:
        /* Need to coerce vector to integer as R list components entered
         * as integers really end up being numeric (double in C). Doing
         * corecion here make the most sense as the type gets deciced in
         * the get_naitve_type call. The C routines know more about the
         * typing than the R routines.
         */
        ki = (long)asReal(VECTOR_ELT(VECTOR_ELT(rgrib_keyList, i), j));
        grib_index_select_long(index, keyName, ki);
        break;
      case GRIB_TYPE_STRING:
        ks = CHAR(asChar(VECTOR_ELT(VECTOR_ELT(rgrib_keyList, i), j)));
        ksc = calloc(strlen(ks) + 1, sizeof(char));
        strcpy(ksc, ks);
        grib_index_select_string(index, keyName, ksc);
        nfree(ksc);
        break;
      default:
        /* Skip other key types with no grib_select_* methods to handle them */
        break;
      }
    }
    index_count = 0;
    while((hi = grib_handle_new_from_index(index, &err))) {
      if (err) {
        gerror("unable to create grib handle", err);
      }
      index_count++;
    }

    grib_index_rewind(index);

    REPROTECT(rgrib_temp = allocVector(VECSXP, index_count), pro_temp);

    m = 0;
    while((hi = grib_handle_new_from_index(index, &err)) && m < index_count) {
      if (err) {
        gerror("unable to create grib handle", err);
      }
      SET_VECTOR_ELT(rgrib_temp, m++,
                     rgrib_message_from_handle(hi, is_masked, is_multi, filter, nameSpace));
    }
    SET_VECTOR_ELT(rgrib_selected, i, rgrib_temp);
  }

  grib_handle_delete(h);
  grib_handle_delete(hi);
  grib_index_delete(index);

  UNPROTECT(2);
  return rgrib_selected;
}
