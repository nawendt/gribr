#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#define MAX_KEY_LEN  255
#define MAX_VAL_LEN  1024

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
    //error("%s(%d): unable to open file %s",__FILE__,__LINE__,p_fileName);
    error("Unable to open file %s",p_fileName);
  }

  h = grib_handle_new_from_file(0,input,&err);
  if (h == NULL) {
    //error("%s(%d): unable to create handle from file %s",__FILE__,__LINE__,p_fileName);
    error("Unable to create handle from file %s",p_fileName);
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

SEXP rgrib_ls(SEXP R_gribHandle, SEXP R_filter, SEXP R_nameSpace){
  grib_handle *h;
  grib_keys_iterator *keyIter;

  char value[MAX_VAL_LEN];
  size_t vlen=MAX_VAL_LEN;
  int filter = INTEGER(R_filter)[0];
  const char *p_nameSpace = CHAR(STRING_ELT(R_nameSpace,0));

  h = R_ExternalPtrAddr(R_gribHandle);
  if (h == NULL) {
    //error("%s(%d): error with gribHandle input",__FILE__,__LINE__);
    error("Error with gribHandle input");
  } else {
    keyIter=grib_keys_iterator_new(h,filter,p_nameSpace);
    if (keyIter == NULL) {
      //error("%s(%d): Unable to create keys iterator",__FILE__,__LINE__);
      error("Unable to create keys iterator");
    }

    while(grib_keys_iterator_next(keyIter))
    {
      const char* name = grib_keys_iterator_get_name(keyIter);
      vlen=MAX_VAL_LEN;
      bzero(value,vlen);
      GRIB_CHECK(grib_get_string(h,name,value,&vlen),name);
      Rprintf("%s = %s\n",name,value);
    }
  }
  return R_NilValue;
}


/*
 * CLOSING
 */

SEXP rgrib_close(SEXP R_gribHandle) {

  grib_handle *h;
  h = R_ExternalPtrAddr(R_gribHandle);

  if (h == NULL) {
    warning("grib handle already closed");
  } else {
    grib_handle_delete(h);
    R_ClearExternalPtr(R_gribHandle);
  }
  return R_NilValue;
}

/*
 * HELPER FUNCTIONS
 */

static void handleFinalizer(SEXP ptr)
{
  if(!R_ExternalPtrAddr(ptr)) return;
  R_ClearExternalPtr(ptr);
}

SEXP isNullPointer(SEXP R_ptr) {
  return ScalarLogical(!R_ExternalPtrAddr(R_ptr));
}

