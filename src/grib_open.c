#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_grib_open(SEXP rgrib_fileName, SEXP rgrib_mode) {

  const char *p_fileName = NULL;
  const char *p_mode = NULL;
  SEXP rgrib_fileHandle;
  FILE *input = NULL;

  p_fileName = CHAR(STRING_ELT(rgrib_fileName, 0));
  p_mode = CHAR(STRING_ELT(rgrib_mode, 0));
  input = fopen(p_fileName, p_mode);

  if(input == NULL) {
    error("rGRIB: unable to open file %s", p_fileName);
  }

  rgrib_fileHandle = PROTECT(R_MakeExternalPtr(input, R_NilValue, R_NilValue));
  R_RegisterCFinalizerEx(rgrib_fileHandle, file_finalizer, TRUE);

  UNPROTECT(1);
  return rgrib_fileHandle;
}
