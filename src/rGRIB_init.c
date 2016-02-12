#include <R_ext/Rdynload.h>
#include <Rconfig.h>
#include <Rinternals.h>

#include "rGRIB.h"

static const
  R_CallMethodDef callMethods[] = {
    {"grib_open",             (DL_FUNC) &rgrib_grib_open,                    2},
    {"grib_close",            (DL_FUNC) &rgrib_grib_close,                   1},
    {"grib_list" ,            (DL_FUNC) &rgrib_grib_list,                    3},
    {"grib_length",           (DL_FUNC) &rgrib_grib_length,                  1},
    {"grib_get_values",       (DL_FUNC) &rgrib_grib_get_values,              1},
    {"grib_get_message",      (DL_FUNC) &rgrib_grib_get_message,             1},
    {NULL,                    NULL,                                      0}
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

