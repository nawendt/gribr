#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

void file_finalizer(SEXP ptr)
{
  if(!R_ExternalPtrAddr(ptr)) return;
  R_ClearExternalPtr(ptr);
}

SEXP rgrib_is_null_ptr (SEXP rgrib_ptr) {
  return ScalarLogical(!R_ExternalPtrAddr(rgrib_ptr));
}

void gerror(const char *str, int err) {
  error("%s(%d): %s\nGRIB ERROR %s", __FILE__, __LINE__, str, grib_get_error_message(err));
}

SEXP getListElement(SEXP list, const char *str)
{
  int i;
  SEXP elmt = R_NilValue, names = getAttrib(list, R_NamesSymbol);
  for (i = 0; i < length(list); i++)
    if(strcmp(CHAR(STRING_ELT(names, i)), str) == 0) {
      elmt = VECTOR_ELT(list, i);
      break;
    }
    return elmt;
}

SEXP rgrib_is_multi_message(SEXP fileHandle) {
  int err;
  int n_on;
  int n_off;
  grib_handle *h = NULL;
  FILE *file = NULL;

  file = R_ExternalPtrAddr(fileHandle);

  err = grib_count_in_file(DEFAULT_CONTEXT, file, &n_off);
  if (err) {
    gerror("unable to count grib messages", err);
  }

  grib_multi_support_on(DEFAULT_CONTEXT);
  err = grib_count_in_file(DEFAULT_CONTEXT, file, &n_on);
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    n_on++;
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  /* This rewind is important as neglecting to do so will
   * leave the file handle in a unusable state and cause
   * R to crash */
  if (fseek(file, 0, SEEK_SET)) {
    error("%s(%d): unable to rewind file", __FILE__ ,__LINE__);
  }

  grib_handle_delete(h);

  if (n_on != n_off) {
    return ScalarLogical(TRUE);
  }

  return ScalarLogical(FALSE);
}

