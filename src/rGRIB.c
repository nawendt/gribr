#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

static void handleFinalizer(SEXP ptr);

/*
 * OPENING
 */

SEXP rgrib_open(SEXP R_fileName, SEXP R_mode) {

  const char *p_fileName, *p_mode;
  int err;
  SEXP R_gribHandle;
  FILE *input = NULL;
  grib_handle *h;

  p_fileName = CHAR(STRING_ELT(R_fileName,0));
  p_mode = CHAR(STRING_ELT(R_mode,0));
  input = fopen(p_fileName,p_mode);

  if(input == NULL) {
    error("rGRIB: rgrib_open: unable to open file %s\n",p_fileName);
  }

  h = grib_handle_new_from_file(0,input,&err);
  if (h == NULL) {
    error("rGRIB: rgrib_open: unable to create handle from file %s\n",p_fileName);
  }
  fclose(input);

  R_gribHandle = PROTECT(R_MakeExternalPtr(h,install("gribID"),R_NilValue));
  R_RegisterCFinalizerEx(R_gribHandle, handleFinalizer, TRUE);

  UNPROTECT(1);
  return R_gribHandle;
}

/*
 * OPERATIONS
 */



/*
 * CLOSING
 */

SEXP rgrib_close(SEXP R_gribHandle) {

  grib_handle *h;
  h = R_ExternalPtrAddr(R_gribHandle);

  if (h == NULL) {
    warning("rGRIB: rgrib_close: grib handle already closed");
  } else {
    grib_handle_delete(h);
    R_ClearExternalPtr(R_gribHandle);
  }

  return R_NilValue;
}

static void handleFinalizer(SEXP ptr)
{
  if(!R_ExternalPtrAddr(ptr)) return;
  R_ClearExternalPtr(ptr);
}

SEXP isNullPointer(SEXP R_ptr) {
  return ScalarLogical(!R_ExternalPtrAddr(R_ptr));
}
