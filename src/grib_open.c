#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP R_grib_open(SEXP R_fileName, SEXP R_mode) {

  const char *p_fileName, *p_mode;
  SEXP R_fileHandle;
  FILE *input = NULL;

  p_fileName = CHAR(STRING_ELT(R_fileName,0));
  p_mode = CHAR(STRING_ELT(R_mode,0));
  input = fopen(p_fileName,p_mode);

  if(input == NULL) {
    error("%s(%d): unable to open file %s",__FILE__,__LINE__,p_fileName);
  }

  R_fileHandle = PROTECT(R_MakeExternalPtr(input,R_NilValue,R_NilValue));
  R_RegisterCFinalizerEx(R_fileHandle, file_finalizer, TRUE);

  UNPROTECT(1);
  return R_fileHandle;
}
