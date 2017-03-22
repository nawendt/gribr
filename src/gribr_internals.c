#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

void nfree(void *ptr) {
  if (ptr) {
    free(ptr);
    ptr = NULL;
  }
}

void file_finalizer(SEXP ptr) {
  if(!R_ExternalPtrAddr(ptr)) return;
  R_ClearExternalPtr(ptr);
}

SEXP gribr_is_null_ptr (SEXP gribr_ptr) {
  return ScalarLogical(!R_ExternalPtrAddr(gribr_ptr));
}

void gerror(const char *str, int err) {
  error("gribr: %s\nGRIB ERROR %s", str, codes_get_error_message(err));
}

void grewind(FILE* file) {
  if (ftell(file) != GRIB_FILE_START) {
    if (fseek(file, GRIB_FILE_START, SEEK_SET)) {
      error("gribr: unable to rewind file");
    }
  }
}

int skip_keys(const char* keyName, int keyType, int err) {
    if (!strcmp(keyName, "zero") ||
        !strcmp(keyName, "one") ||
        !strcmp(keyName, "eight") ||
        !strcmp(keyName, "eleven") ||
        !strcmp(keyName, "false") ||
        !strcmp(keyName, "thousand") ||
        !strcmp(keyName, "file") ||
        !strcmp(keyName, "localDir") ||
        !strcmp(keyName, "7777") ||
        !strcmp(keyName, "oneThousand") ||
        !strcmp(keyName, "hundred") ||
        keyType == CODES_TYPE_BYTES ||
        keyType == CODES_TYPE_LABEL ||
        keyType == CODES_TYPE_MISSING ||
        keyType == CODES_TYPE_SECTION ||
        keyType == CODES_TYPE_UNDEFINED ||
        err) {
      return 1;
    }
    return 0;
}

SEXP getListElement(SEXP list, const char *str) {
  int i;
  SEXP elmt = R_NilValue, names = getAttrib(list, R_NamesSymbol);
  for (i = 0; i < length(list); i++)
    if(strcmp(CHAR(STRING_ELT(names, i)), str) == 0) {
      elmt = VECTOR_ELT(list, i);
      break;
    }
    return elmt;
}

SEXP gribr_is_multi_message(SEXP fileHandle) {
  int err;
  int n_on;
  int n_off;
  codes_handle *h = NULL;
  FILE *file = NULL;

  file = R_ExternalPtrAddr(fileHandle);

  err = codes_count_in_file(DEFAULT_CONTEXT, file, &n_off);
  if (err) {
    gerror("unable to count grib messages", err);
  }

  codes_grib_multi_support_on(DEFAULT_CONTEXT);

  n_on = 0;
  while((h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    n_on++;
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  /* This rewind is important as neglecting to do so will
   * leave the file handle in a unusable state and cause
   * R to crash */
  if (fseek(file, 0, SEEK_SET)) {
    error("gribr: unable to rewind file");
  }

  codes_handle_delete(h);

  if (n_on != n_off) {
    return ScalarLogical(TRUE);
  }

  return ScalarLogical(FALSE);
}

