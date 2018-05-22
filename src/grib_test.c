#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_grib_test(SEXP gribr_fileName) {

  int err;
  int count;
  const char *p_fileName = NULL;
  FILE *gribFile = NULL;

  p_fileName = CHAR(STRING_ELT(gribr_fileName, 0));
  gribFile = fopen(p_fileName, "rb");

  if (gribFile == NULL) {
    error("gribr: Could not open file %s", p_fileName);
  }

  err = codes_count_in_file(DEFAULT_CONTEXT, gribFile, &count);

  /*
   * CLOSE THE FILE HERE -- the GRIB file does not have an
   * file handle for R to use yet. The file descriptor must
   * be closed or it will leak and cause issues for
   * large batch processing scripts.
   */
  fclose(gribFile);

  /*
   * TRUE denotes the file is likely not a GRIB file. TRUE is
   * returned when there are 0 messages in the file or when
   * some sort of error has occurred trying to read the file
   */
  if (err || !count) {
    return ScalarLogical(1);
  }

  /* Return FALSE otherwise...indicating a good GRIB file */
  return ScalarLogical(0);
}
