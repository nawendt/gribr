#include <grib_api.h>

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
SEXP rgrib_grib_open(SEXP rgrib_fileName, SEXP rgrib_mode);
SEXP rgrib_grib_get_values(SEXP rgrib_fileHandle);
SEXP rgrib_grib_list(SEXP rgrib_fileHandle, SEXP rgrib_filter, SEXP rgrib_nameSpace);
SEXP rgrib_grib_length(SEXP rgrib_fileHandle);
SEXP rgrib_grib_get_message(SEXP rgrib_fileHandle, SEXP rgrib_filter, SEXP rgrib_nameSpace);
SEXP rgrib_is_null_ptr (SEXP rgrib_ptr);
SEXP rgrib_is_multi_message(SEXP rgrib_fileHandle);
SEXP rgrib_select(SEXP filePath, SEXP keyPairs, SEXP isMulti);

/* Extra GRIB API function prototypes */
void grib_index_rewind(grib_index* index);

/* Internal functions */
void file_finalizer(SEXP ptr);
void gerror(const char *str, int err);
SEXP getListElement(SEXP list, const char *str);
