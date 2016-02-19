#include <grib_api.h>

#include <R.h>
#include <Rinternals.h>

#include "rGRIB.h"

SEXP rgrib_select(SEXP filePath, SEXP keyPairs) {
  int err;
  grib_index *index = NULL;
  grib_handle *h = NULL;
  const char *file = NULL;
  const char *keyString = NULL;

  file = CHAR(STRING_ELT(filePath, 0));

  grib_index_new(DEFAULT_CONTEXT, file, &err);
  if (err) {
    gerror("unable to create index", err);
  }

}
