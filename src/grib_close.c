#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_close(SEXP rgrib_fileHandle) {

  int err;
  FILE *file = NULL;
  file = R_ExternalPtrAddr(rgrib_fileHandle);

  if (file == NULL) {
    error("grib file already closed");
  } else {
    err = fclose(file);
    file = NULL;
    if (err) {
      error("%s(%d): unable to close file",__FILE__,__LINE__);
    }
    R_ClearExternalPtr(rgrib_fileHandle);
  }
  return R_NilValue;
}
