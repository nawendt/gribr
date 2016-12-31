#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_open(SEXP gribr_fileName) {

  const char *p_fileName = NULL;
  SEXP gribr_fileHandle;
  FILE *input = NULL;

  p_fileName = CHAR(STRING_ELT(gribr_fileName, 0));
  input = fopen(p_fileName, "rb");

  if(input == NULL) {
    error("gribr: unable to open file %s", p_fileName);
  }

  gribr_fileHandle = PROTECT(R_MakeExternalPtr(input, R_NilValue, R_NilValue));
  R_RegisterCFinalizerEx(gribr_fileHandle, file_finalizer, TRUE);

  UNPROTECT(1);
  return gribr_fileHandle;
}
