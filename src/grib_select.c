#include <R.h>
#include <Rinternals.h>

#include <string.h>

#include "gribr.h"
#include "grib_api_extra.h"

SEXP gribr_select(SEXP gribr_filePath, SEXP gribr_keyList, SEXP gribr_isMulti) {
  int err;
  int keyType;
  int is_multi;
  long ki;
  double kd;
  R_xlen_t lenKeysList;
  R_xlen_t lenKeys;
  R_xlen_t m;
  R_xlen_t n;
  R_xlen_t n_tot;
  R_xlen_t i;
  R_xlen_t j;
  R_xlen_t index_count;
  size_t reallocLength = MAX_VAL_LEN;
  char *keyString = NULL;
  const char *filePath = NULL;
  const char *keyName = NULL;
  const char *ks = NULL;
  char *ksc = NULL;
  FILE *fileHandle = NULL;
  codes_index *index = NULL;
  codes_handle *h = NULL;
  codes_handle *hi = NULL;
  SEXP gribr_selected;
  SEXP gribr_temp;
  PROTECT_INDEX pro_temp;

  PROTECT_WITH_INDEX(gribr_temp = R_NilValue, &pro_temp);

  is_multi = asLogical(gribr_isMulti);

  filePath = CHAR(STRING_ELT(gribr_filePath, 0));
  lenKeysList = xlength(gribr_keyList);
  gribr_selected = PROTECT(allocVector(VECSXP, lenKeysList));
  fileHandle = fopen(filePath, "rb");
  if (!fileHandle) {
    error("gribr: unable to open grib file");
  }

  keyString = calloc(MAX_VAL_LEN, sizeof(char));
  if (!keyString) {
    error("gribr: unable to allocate keyString");
  }

  /* Keep track of number of characters and reallocate the final string
   * as necessary. The whole idea is to minimize chances of overflow
   * keeping the code as dynamic as possible. MAX_VAL_LEN is most
   * likely going to be more than enough in preponderance of cases
   **********************************************************************/
  n = 0; n_tot = 0;
  lenKeys = xlength(VECTOR_ELT(gribr_keyList, 0));
  /*
   * One loop will work since the input list will have all the same
   * keys
   */
  for (i = 0; i < lenKeys; i++) {
    if (i % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    keyName = CHAR(STRING_ELT(getAttrib(VECTOR_ELT(gribr_keyList, 0), R_NamesSymbol), i));
    n = strlen(keyName);
    if (!n) {
      error("gribr: unable to get keyString length");
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

  if (asLogical(gribr_isMulti)) {
    codes_grib_multi_support_on(DEFAULT_CONTEXT);
  }

  index = codes_index_new(DEFAULT_CONTEXT, keyString, &err);
  if (err) {
    gerror("unable to create index", err);
  }

  err = codes_index_add_file(index, filePath);
  if (err) {
    gerror("unable to add file to index", err);
  }

  nfree(keyString);

  /* Have to grab handle from file first in order to get
   * native key types. Will not work with handle from
   * index. Index handles will only work after using
   * the select method
   */
  h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, fileHandle, &err);
  if (err) {
    gerror("unable to create grib handle", err);
  }

  for (i = 0; i < lenKeysList; i++) {
    if (i % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    lenKeys = xlength(VECTOR_ELT(gribr_keyList, i));
    for (j = 0; j < lenKeys; j++) {
      keyName = CHAR(STRING_ELT(getAttrib(VECTOR_ELT(gribr_keyList, i), R_NamesSymbol), j));
      err = codes_get_native_type(h, keyName, &keyType);
      if (err) {
        gerror("unable to get native type", err);
      }
      switch (keyType) {
      case CODES_TYPE_DOUBLE:
        kd = asReal(VECTOR_ELT(VECTOR_ELT(gribr_keyList, i), j));
        codes_index_select_double(index, keyName, kd);
        break;
      case CODES_TYPE_LONG:
        /* Need to coerce vector to integer as R list components entered
         * as integers really end up being numeric (double in C). Doing
         * corecion here makes the most sense as the type gets deciced in
         * the get_naitve_type call. The C routines know more about the
         * typing than the R routines.
         */
        ki = (long)asReal(VECTOR_ELT(VECTOR_ELT(gribr_keyList, i), j));
        codes_index_select_long(index, keyName, ki);
        break;
      case CODES_TYPE_STRING:
        ks = CHAR(asChar(VECTOR_ELT(VECTOR_ELT(gribr_keyList, i), j)));
        ksc = calloc(strlen(ks) + 1, sizeof(char));
        strcpy(ksc, ks);
        codes_index_select_string(index, keyName, ksc);
        nfree(ksc);
        break;
      default:
        /* Skip other key types with no codes_select_* methods to handle them */
        break;
      }
    }
    index_count = 0;
    while((hi = codes_handle_new_from_index(index, &err))) {
      if (err) {
        gerror("unable to create grib handle", err);
      }
      index_count++;
      codes_handle_delete(hi);
    }

    if (index_count == 0) {
      error("gribr: no messages matched");
    }

    grib_index_rewind(index);

    REPROTECT(gribr_temp = allocVector(VECSXP, index_count), pro_temp);

    m = 0;
    while((hi = codes_handle_new_from_index(index, &err)) && m < index_count) {
      if (err) {
        gerror("unable to create grib handle", err);
      }
      SET_VECTOR_ELT(gribr_temp, m++,
                     gribr_message_from_handle(hi, is_multi));
      codes_handle_delete(hi);
    }
    SET_VECTOR_ELT(gribr_selected, i, gribr_temp);
  }

  codes_handle_delete(h);
  codes_index_delete(index);

  UNPROTECT(2);
  return gribr_selected;
}
