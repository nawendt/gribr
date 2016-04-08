#include <grib_api.h>
#include <ctype.h>

#define MAX_KEY_LEN     255
#define MAX_VAL_LEN     1024
#define NA_KEY_LEN      8
#define DEFAULT_CONTEXT 0
#define GRIB_FILE_START 0
#define NO_ITER_FLAGS   0
#define NULL_NAMESPACE  NULL
#define NO_FILTER       0
#define MAX_BYTE_LENGTH 512
#define BITMAP_MASK     0

/* Functions to be registered */
SEXP rgrib_grib_close(SEXP rgrib_fileHandle);
SEXP rgrib_grib_open(SEXP rgrib_fileName);
SEXP rgrib_grib_list(SEXP rgrib_fileHandle, SEXP rgrib_filter,
                     SEXP rgrib_nameSpace, SEXP rgrib_isMulti);

SEXP rgrib_grib_length(SEXP rgrib_fileHandle);
SEXP rgrib_is_null_ptr (SEXP rgrib_ptr);
SEXP rgrib_api_version(void);
SEXP rgrib_is_multi_message(SEXP rgrib_fileHandle);
SEXP rgrib_grib_get_message(SEXP rgrib_fileHandle, SEXP rgrib_messages,
                            SEXP rgrib_isMulti);

SEXP rgrib_select(SEXP rgrib_filePath, SEXP rgrib_keyList, SEXP rgrib_isMulti);
SEXP rgrib_redtoreg(SEXP rgrib_nlons, SEXP rgrib_lonsperlat, SEXP rgrib_grid);

/* Extra GRIB API function prototypes */
void grib_index_rewind(grib_index* index);

/* Internal functions */
void file_finalizer(SEXP ptr);
void nfree(void *ptr);
void gerror(const char *str, int err);
SEXP getListElement(SEXP list, const char *str);
SEXP rgrib_message_from_handle(grib_handle *h, int isMulti);
