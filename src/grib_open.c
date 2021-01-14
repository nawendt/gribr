#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_open(SEXP gribr_fileName) {

  const char *p_fileName = NULL;
  SEXP gribr_fileHandle;
  FILE *input = NULL;
  codes_context *c = NULL;
  char *def_path = NULL;

  /* This important step makes sure that the GRIB
  definition files for ecCodes are found. If they
  are not found and some actions are performed
  on a GRIB file, ecCodes will throw an exception
  and R will crash. */
  c = codes_context_get_default();
  def_path = codes_definition_path(c);
  if (!def_path) {
    error("gribr: GRIB definition files not found; Please set the ECCODES_DEFINITION_PATH");
  }

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
