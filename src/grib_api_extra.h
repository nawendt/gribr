/* This header contains functions/structs in the GRIB API that have not been
   made public via the main header but are used by gribr */

void grib_index_rewind(grib_index* index);
char *grib_context_full_defs_path(grib_context* c, const char* basename);
