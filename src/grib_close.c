#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_close(SEXP gribr_fileHandle) {

  int err;
  FILE *file = NULL;
  file = R_ExternalPtrAddr(gribr_fileHandle);

  if (file == NULL) {
    error("grib file already closed");
  } else {
    err = fclose(file);
    file = NULL;
    if (err) {
      error("gribr: unable to close file");
    }
    R_ClearExternalPtr(gribr_fileHandle);
  }
  return R_NilValue;
}
