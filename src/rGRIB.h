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
SEXP rgrib_grib_close(SEXP R_fileHandle);
SEXP rgrib_grib_open(SEXP R_fileName, SEXP R_mode);
SEXP rgrib_grib_get_values(SEXP R_fileHandle);
SEXP rgrib_grib_list(SEXP R_fileHandle, SEXP R_filter, SEXP R_nameSpace);
SEXP rgrib_grib_length(SEXP R_fileHandle);
SEXP rgrib_grib_get_message(SEXP R_fileHandle, SEXP rgrib_filter, SEXP rgrib_nameSpace);

/* Internal functions */
extern void file_finalizer(SEXP ptr);
extern void gerror(const char *str, int err);
extern int is_multi_message(FILE *file);
