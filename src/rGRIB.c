#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#define MAX_KEY_LEN  255
#define MAX_VAL_LEN  1024

static void fileFinalizer(SEXP ptr);

/*
 * OPENING
 */

SEXP rgrib_open(SEXP R_fileName, SEXP R_mode) {

  const char *p_fileName, *p_mode;
  SEXP R_fileHandle;
  FILE *input = NULL;

  p_fileName = CHAR(STRING_ELT(R_fileName,0));
  p_mode = CHAR(STRING_ELT(R_mode,0));
  input = fopen(p_fileName,p_mode);

  if(input == NULL) {
    error("%s(%d): unable to open file %s",__FILE__,__LINE__,p_fileName);
    //error("Unable to open file %s",p_fileName);
  }

  R_fileHandle = PROTECT(R_MakeExternalPtr(input,install("gribFile"),R_NilValue));
  R_RegisterCFinalizerEx(R_fileHandle, fileFinalizer, TRUE);

  UNPROTECT(1);
  return R_fileHandle;
}

/*
 * OPERATIONS
 */




/*
 * CLOSING
 */

SEXP rgrib_close(SEXP R_fileHandle) {

  int err;
  FILE *file;
  file = R_ExternalPtrAddr(R_fileHandle);

  if (file == NULL) {
    warning("grib file already closed");
  } else {
    err = fclose(file);
    if (err) {
      error("%s(%d): unable to close file %s",__FILE__,__LINE__);
    }
    R_ClearExternalPtr(R_fileHandle);
  }
  return R_NilValue;
}

/*
 * HELPER FUNCTIONS
 */

static void fileFinalizer(SEXP ptr)
{
  if(!R_ExternalPtrAddr(ptr)) return;
  R_ClearExternalPtr(ptr);
}

SEXP isNullPointer(SEXP R_ptr) {
  return ScalarLogical(!R_ExternalPtrAddr(R_ptr));
}

