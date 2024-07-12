#include <eccodes.h>
#include <ctype.h>

#define DEFAULT_CONTEXT 0
#define GRIB_FILE_START 0
#define NULL_NAMESPACE  NULL
#define NO_FILTER       0
#define BITMAP_MASK     0
#define INTERRUPT_FREQ  256

/* Functions to be registered */
SEXP gribr_grib_close(SEXP gribr_fileHandle);
SEXP gribr_grib_open(SEXP gribr_fileName);
SEXP gribr_grib_list(SEXP gribr_fileHandle, SEXP gribr_filter,
                     SEXP gribr_namespace, SEXP gribr_isMulti);
SEXP gribr_grib_df(SEXP gribr_fileHandle, SEXP gribr_filter,
                   SEXP gribr_namespace, SEXP gribr_isMulti);

SEXP gribr_grib_length(SEXP gribr_fileHandle);
SEXP gribr_is_null_ptr (SEXP gribr_ptr);
SEXP gribr_api_version(void);
SEXP gribr_is_multi_message(SEXP gribr_fileHandle);
SEXP gribr_grib_get_message(SEXP gribr_fileHandle, SEXP gribr_messages,
                            SEXP gribr_isMulti);

SEXP gribr_select(SEXP gribr_filePath, SEXP gribr_fileHandle, 
                  SEXP gribr_keyList, SEXP gribr_isMulti);
SEXP gribr_redtoreg(SEXP gribr_nlons, SEXP gribr_lonsperlat, SEXP gribr_grid);
SEXP gribr_grib_test(SEXP gribr_fileName);

/* Internal functions */
int skip_keys(const char* keyName, int keyType, int err);
void file_finalizer(SEXP ptr);
void nfree(void *ptr);
void grewind(FILE* file);
void gerror(const char *str, int err);
void gwarn(const char *str, int err);
SEXP getListElement(SEXP list, const char *str);
SEXP gribr_message_from_handle(grib_handle *h);

/* Other functions */
#ifndef NATIVE_STRLCAT
size_t strlcat(char *dst, const char *src, size_t dsize);
#endif
