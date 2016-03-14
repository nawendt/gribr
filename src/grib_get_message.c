#include <R.h>
#include <Rinternals.h>

#include <string.h>

#include "rGRIB.h"

SEXP rgrib_grib_get_message(SEXP rgrib_fileHandle, SEXP rgrib_messages, SEXP mask, SEXP rgrib_isMulti, SEXP rgrib_filter, SEXP rgrib_nameSpace) {

  int err;
  int filter;
  int is_multi;
  int is_masked;
  int *p_rgrib_messages = NULL;
  R_len_t n;
  R_len_t i;
  R_len_t messagesLength;
  R_len_t count;
  const char *nameSpace;
  FILE *file = NULL;
  grib_handle *h = NULL;
  PROTECT_INDEX pro_message;
  SEXP rgrib_message;

  file = R_ExternalPtrAddr(rgrib_fileHandle);
  filter = asInteger(rgrib_filter);
  nameSpace = CHAR(STRING_ELT(rgrib_nameSpace,0));
  messagesLength = xlength(rgrib_messages);
  if (messagesLength == 0) {
    error("rGRIB: zero messages requested");
  }
  is_masked = asLogical(mask);
  is_multi = asLogical(rgrib_isMulti);
  PROTECT_WITH_INDEX(rgrib_message = R_NilValue, &pro_message);

  if (is_multi) {
    grib_multi_support_on(DEFAULT_CONTEXT);
  }

  grib_count_in_file(DEFAULT_CONTEXT, file, &count);

  file = R_ExternalPtrAddr(rgrib_fileHandle);
  if (ftell(file) != 0) {
    fseek(file, 0, SEEK_SET);
  }

  p_rgrib_messages = INTEGER(rgrib_messages);

  if (messagesLength > 1) {
    REPROTECT(rgrib_message = allocVector(VECSXP, messagesLength), pro_message);
    /*
     * rgrib_messages will come in sorted so that only one
     * loop through the grib file handles will be necessary
     */
    for (i = 0, n = 0; i < count && n < messagesLength; i++) {
      R_CheckUserInterrupt();
      if ((p_rgrib_messages[n] - 1) > count || p_rgrib_messages[n] < 1) {
        error("rGRIB: message number out of bounds");
      }

      h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
      if (err) {
        gerror("unable to get grib handle", err);
      }

      if ((p_rgrib_messages[n] - 1) == i) {
        SET_VECTOR_ELT(rgrib_message, n++,rgrib_message_from_handle(h, is_masked, is_multi, filter, nameSpace));
      }
    }
  } else {
    if (p_rgrib_messages[0] < 1 || (p_rgrib_messages[0] - 1) > count) {
      error("rGRIB: message number out of bounds");
    }
    for (i = 0; i < p_rgrib_messages[0]; i++) {
      R_CheckUserInterrupt();
      h = grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
      if (err) {
        gerror("unable to open grib handle", err);
      }
      if ((p_rgrib_messages[0] - 1) == i) {
        REPROTECT(rgrib_message = rgrib_message_from_handle(h, is_masked, is_multi, filter, nameSpace), pro_message);
      }
    }
  }

  grib_handle_delete(h);
  fseek(file, 0, SEEK_SET);

  UNPROTECT(1);
  return rgrib_message;
}
