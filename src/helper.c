#include <R.h>
#include <Rinternals.h>

#include <grib_api.h>

#include "rGRIB.h"

extern void file_finalizer(SEXP ptr)
{
  if(!R_ExternalPtrAddr(ptr)) return;
  R_ClearExternalPtr(ptr);
}

extern SEXP rgrib_is_null_ptr (SEXP rgrib_ptr) {
  return ScalarLogical(!R_ExternalPtrAddr(rgrib_ptr));
}

extern void gerror(const char *str, int err) {
  error("%s(%d): %s\nGRIB ERROR %s",__FILE__,__LINE__,str,grib_get_error_message(err));
}

SEXP getListElement(SEXP list, const char *str)
{
  SEXP elmt = R_NilValue, names = getAttrib(list, R_NamesSymbol);
  for (int i = 0; i < length(list); i++)
    if(strcmp(CHAR(STRING_ELT(names, i)), str) == 0) {
      elmt = VECTOR_ELT(list, i);
      break;
    }
    return elmt;
}

extern int is_multi_message(FILE *file) {
  int err;
  int n_on;
  int n_off;
  grib_context *c;

  c = grib_context_get_default();

  grib_multi_support_off(c);
  err = grib_count_in_file(c, file, &n_off);
  if (err) {
    gerror("unable to count grib messages", err);
  }

  grib_multi_support_on(c);
  err = grib_count_in_file(c, file, &n_on);
  if (err) {
    gerror("unable to count grib messages", err);
  }

  //grib_context_delete(c);

  if (n_on != n_off) {
    return 1;
  }

  return 0;
}
