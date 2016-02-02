#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#define MAX_KEY_LEN     255
#define MAX_VAL_LEN     1024
#define NA_KEY_LEN      8
#define DEFAULT_CONTEXT 0

static void file_finalizer(SEXP ptr);

/*
 * OPENING
 */

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

/*
 * OPERATIONS
 */

/* OLD TEST
SEXP R_grib_ls() {
  SEXP result;
  SEXP names;

  result = PROTECT(allocVector(VECSXP, 4));
  SET_VECTOR_ELT(result, 0, ScalarInteger(25));
  SET_VECTOR_ELT(result, 1, ScalarLogical(1));
  SET_VECTOR_ELT(result, 2, ScalarReal(5.5));
  SET_VECTOR_ELT(result, 3, mkString("test"));

  names = PROTECT(allocVector(STRSXP, 4));
  SET_STRING_ELT(names, 0, mkChar("junk"));
  SET_STRING_ELT(names, 1, mkChar("stuff"));
  SET_STRING_ELT(names, 2, mkChar("things"));
  SET_STRING_ELT(names, 3, mkChar("crap"));

  namesgets(result, names);

  UNPROTECT(2);
  return result;
}
*/

SEXP R_grib_list(SEXP R_fileHandle, SEXP R_filter, SEXP R_nameSpace ) {
  int err,n;
  R_len_t floc;
  size_t messageCount = 0;
  FILE *file = NULL;
  grib_handle *h = NULL;
  const char *nameSpace = NULL;
  char keyString[MAX_VAL_LEN];
  int filter;
  char value[MAX_VAL_LEN];
  size_t valueLength=MAX_VAL_LEN;

  filter = asInteger(R_filter);
  nameSpace = CHAR(STRING_ELT(R_nameSpace,0));

  file = R_ExternalPtrAddr(R_fileHandle);
  if (file == NULL) {
    error("%s(%d): grib file not opened", __FILE__ ,__LINE__);
  }

  err = grib_count_in_file(DEFAULT_CONTEXT, file, &n);
  if (err) {
    error("%s(%d): unable to count messages; GRIB ERROR %3d", __FILE__, __LINE__, err);
  }
  SEXP R_grib_vec = PROTECT(allocVector(STRSXP, n));

  /* The grib handle is our GRIB message iterator. Each time we call new_from_file,
     we are advancing to the next message in the file. */
  while((h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err)) != NULL) {
    grib_keys_iterator* keyIter=NULL;

    if(h == NULL) {
      error("%s(%d): unable to create grib handle: GRIB ERROR %3d", __FILE__, __LINE__, err);
    }

    keyIter=grib_keys_iterator_new(h, filter, nameSpace);
    if (keyIter == NULL) {
      error("%s(%d): unable to create key iterator", __FILE__, __LINE__);
    }

    bzero(keyString,MAX_VAL_LEN);
    while(grib_keys_iterator_next(keyIter)) {
      valueLength = MAX_VAL_LEN;
      const char *keyName = grib_keys_iterator_get_name(keyIter);
      bzero(value, valueLength);
      err = grib_get_string(h, keyName, value, &valueLength);
      if (err) {
        strncat(keyString," NA:NA ",NA_KEY_LEN);
      } else {
        strncat(keyString,value,valueLength);
      }
    }

    SET_STRING_ELT(R_grib_vec, messageCount++, mkChar(keyString));

    grib_keys_iterator_delete(keyIter);
    grib_handle_delete(h);

  }
  /* Be kind, please rewind. Without this the next call of grib_list will fail */
  //rewind(file);

  UNPROTECT(1);
  return R_grib_vec;
}

/*
 * CLOSING
 */

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

SEXP R_grib_length(SEXP R_fileHandle) {

  int err, n;
  FILE *file = NULL;
  file = R_ExternalPtrAddr(R_fileHandle);

  err = grib_count_in_file(DEFAULT_CONTEXT,file,&n);
  file = NULL;
  if (err) {
    error("%s(%d): unable to count messages; GRIB ERROR %3d",__FILE__,__LINE__,err);
  } else {
    return ScalarInteger(n);
  }
}

/*
 * HELPER FUNCTIONS
 */

static void file_finalizer(SEXP ptr)
{
  if(!R_ExternalPtrAddr(ptr)) return;
  R_ClearExternalPtr(ptr);
}

SEXP R_is_null_ptr (SEXP R_ptr) {
  return ScalarLogical(!R_ExternalPtrAddr(R_ptr));
}

