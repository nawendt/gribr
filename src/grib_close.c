#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

SEXP R_grib_close(SEXP R_fileHandle) {

  int err;
  FILE *file = NULL;
  file = R_ExternalPtrAddr(R_fileHandle);

  if (file == NULL) {
    error("grib file already closed");
  } else {
    err = fclose(file);
    file = NULL;
    if (err) {
      error("%s(%d): unable to close file",__FILE__,__LINE__);
    }
    R_ClearExternalPtr(R_fileHandle);
  }
  return R_NilValue;
}
