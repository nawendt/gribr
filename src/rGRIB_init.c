#include <R_ext/Rdynload.h>
#include <Rconfig.h>
#include <Rinternals.h>

#include "rGRIB.h"

#define CALLDEF(name, n) {#name, (DL_FUNC) &name, n}

static const R_CallMethodDef callMethods[] = {
  CALLDEF(rgrib_grib_open, 2),
  CALLDEF(rgrib_grib_close, 1),
  CALLDEF(rgrib_grib_list, 3),
  CALLDEF(rgrib_grib_length, 1),
  CALLDEF(rgrib_is_null_ptr, 1),
  CALLDEF(rgrib_is_multi_message, 1),
  CALLDEF(rgrib_grib_get_message, 6),
  CALLDEF(rgrib_select, 6),
  CALLDEF(rgrib_api_version, 0),
  {NULL, NULL, 0}
};

void R_init_rGRIB(DllInfo *info)
{
  R_registerRoutines(info,
                     NULL,
                     callMethods,
                     NULL,
                     NULL);

  R_useDynamicSymbols(info, TRUE);

  /* Here is where we could export the C routines
   * so that other packages could call them from
   * there C routines.
   *
   * E.g,
   *
   * R_RegisterCCallable("rGRIB","grib_open",           (DL_FUNC) &rgrib_grib_open);
   *
   */

}

