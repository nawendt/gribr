#include <R.h>
#include <Rinternals.h>

#include <string.h>

#include "gribr.h"
// #include "grib_api_extra.h"

SEXP gribr_select(SEXP gribr_filePath, SEXP gribr_fileHandle, SEXP gribr_keyList, SEXP gribr_isMulti) {
  int err;
  int keyType;
  int is_multi;
  long ki;
  double kd;
  size_t lenKeysList;
  size_t lenKeys;
  size_t m;
  size_t i;
  size_t j;
  size_t index_count;
  size_t concat;
  size_t ks_size;
  char *keyString = NULL;
  const char *filePath = NULL;
  const char *keyName = NULL;
  const char *ks = NULL;
  FILE *file = NULL;
  codes_index *index = NULL;
  codes_handle *h = NULL;
  codes_handle *hi = NULL;
  SEXP gribr_selected;
  SEXP gribr_temp;
  PROTECT_INDEX pro_temp;

  is_multi = asLogical(gribr_isMulti);
  PROTECT_WITH_INDEX(gribr_temp = R_NilValue, &pro_temp);

  filePath = CHAR(STRING_ELT(gribr_filePath, 0));
  lenKeysList = xlength(gribr_keyList);
  gribr_selected = PROTECT(allocVector(VECSXP, lenKeysList));
  file = R_ExternalPtrAddr(gribr_fileHandle);
  if (!file) {
    error("gribr: problem with file handle");
  }

  grewind(file);

  ks_size = 0;
  lenKeys = xlength(VECTOR_ELT(gribr_keyList, 0));

  /* Loop to figure out keyString size */
  for (i = 0; i < lenKeys; i++) {
    if (i % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    keyName = CHAR(STRING_ELT(getAttrib(VECTOR_ELT(gribr_keyList, 0), R_NamesSymbol), i));
    ks_size += strlen(keyName);
  }

  /* Add space for commas and nul (lenKeys - 1 + 1)*/
  ks_size += lenKeys;

  /* Allocate keyString */
  keyString = calloc(ks_size, sizeof(char));

  /* Loop to create keyString */
  for (i = 0; i < lenKeys; i++) {
    if (i % INTERRUPT_FREQ == 0) {
      R_CheckUserInterrupt();
    }
    keyName = CHAR(STRING_ELT(getAttrib(VECTOR_ELT(gribr_keyList, 0), R_NamesSymbol), i));
    
    concat = strlcat(keyString, keyName, ks_size);
    if (concat >= ks_size) {
      error("gribr: could not add key to index selection");
    }

    if (i != lenKeys - 1 && strlen(keyName)) {
      concat = strlcat(keyString, ",", ks_size);
      if (concat >= ks_size) {
        error("gribr: could not add separator to index selection");
      }
    }
  }

  if (is_multi) {
    codes_grib_multi_support_on(DEFAULT_CONTEXT);
  }

  index = codes_index_new(DEFAULT_CONTEXT, keyString, &err);
  if (err) {
    gerror("unable to create index", err);
  }
  nfree(keyString);

  err = codes_index_add_file(index, filePath);
  if (err) {
    gerror("unable to add file to index", err);
  }

  /* Have to grab handle from file first in order to get
   * native key types. Will not work with handle from
   * index. Index handles will only work after using
   * the select method
   */
  h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
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
        codes_index_select_string(index, keyName, ks);
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

    // grib_index_rewind(index);

    REPROTECT(gribr_temp = allocVector(VECSXP, index_count), pro_temp);

    m = 0;
    while((hi = codes_handle_new_from_index(index, &err)) && m < index_count) {
      if (err) {
        gerror("unable to create grib handle", err);
      }
      SET_VECTOR_ELT(gribr_temp, m++,
                     gribr_message_from_handle(hi));
      codes_handle_delete(hi);
    }
    SET_VECTOR_ELT(gribr_selected, i, gribr_temp);
  }

  codes_handle_delete(h);
  codes_index_delete(index);

  grewind(file);

  UNPROTECT(2);
  return gribr_selected;
}
