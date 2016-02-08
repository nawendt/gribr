#include <R.h>
#include <Rinternals.h>

#include <grib_api.h>

extern void file_finalizer(SEXP ptr)
{
  if(!R_ExternalPtrAddr(ptr)) return;
  R_ClearExternalPtr(ptr);
}

extern SEXP R_is_null_ptr (SEXP R_ptr) {
  return ScalarLogical(!R_ExternalPtrAddr(R_ptr));
}

extern void gerror(const char *str, int err) {
  error("%s(%d): %s\nGRIB ERROR %s",__FILE__,__LINE__,str,grib_get_error_message(err));
}
