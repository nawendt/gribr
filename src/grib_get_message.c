#include <R.h>
#include <Rinternals.h>

#include <string.h>

#include "gribr.h"

SEXP gribr_grib_get_message(SEXP gribr_fileHandle, SEXP gribr_messages, SEXP gribr_isMulti) {

  int err;
  int is_multi;
  int *p_gribr_messages = NULL;
  R_len_t n;
  R_len_t i;
  R_len_t messagesLength;
  R_len_t count;
  FILE *file = NULL;
  codes_handle *h = NULL;
  PROTECT_INDEX pro_message;
  SEXP gribr_message;

  file = R_ExternalPtrAddr(gribr_fileHandle);
  messagesLength = xlength(gribr_messages);
  if (messagesLength == 0) {
    error("gribr: zero messages requested");
  }

  is_multi = asLogical(gribr_isMulti);
  PROTECT_WITH_INDEX(gribr_message = R_NilValue, &pro_message);

  if (is_multi) {
    codes_grib_multi_support_on(DEFAULT_CONTEXT);
  }

  count = 0;
  while((h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err))) {
    count++;
    codes_handle_delete(h);
  }
  if (err) {
    gerror("unable to count grib messages", err);
  }

  grewind(file);

  p_gribr_messages = INTEGER(gribr_messages);

  if (messagesLength > 1) {
    REPROTECT(gribr_message = allocVector(VECSXP, messagesLength), pro_message);
    /*
     * gribr_messages will come in sorted so that only one
     * loop through the grib file handles will be necessary
     */
    for (i = 0, n = 0; i < count && n < messagesLength; i++) {
      if (i % INTERRUPT_FREQ == 0) {
        R_CheckUserInterrupt();
      }
      if ((p_gribr_messages[n] - 1) >= count || p_gribr_messages[n] < 1) {
        error("gribr: message number out of bounds");
      }

      h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
      if (err) {
        gerror("unable to get grib handle", err);
      }

      if ((p_gribr_messages[n] - 1) == i) {
        SET_VECTOR_ELT(gribr_message, n++, gribr_message_from_handle(h));
      }
      codes_handle_delete(h);
    }
  } else {
    if (p_gribr_messages[0] < 1 || (p_gribr_messages[0] - 1) >= count) {
      error("gribr: message number out of bounds");
    }
    for (i = 0; i < p_gribr_messages[0]; i++) {
      if (i % INTERRUPT_FREQ == 0) {
        R_CheckUserInterrupt();
      }
      h = codes_grib_handle_new_from_file(DEFAULT_CONTEXT, file, &err);
      if (err) {
        gerror("unable to open grib handle", err);
      }
      if ((p_gribr_messages[0] - 1) == i) {
        REPROTECT(gribr_message = gribr_message_from_handle(h), pro_message);
      }
      codes_handle_delete(h);
    }
  }

  grewind(file);

  UNPROTECT(1);
  return gribr_message;
}
