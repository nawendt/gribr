#include <R.h>
#include <Rinternals.h>

#include <grib_api.h>

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
