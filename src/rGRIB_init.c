#include <R_ext/Rdynload.h>
#include <Rconfig.h>
#include <Rinternals.h>

#include "rGRIB.h"

static const
  R_CallMethodDef callMethods[] = {
    {"grib_open",             (DL_FUNC) &R_grib_open,               2},
    {"grib_close",            (DL_FUNC) &R_grib_close,              1},
    {"grib_list" ,            (DL_FUNC) &R_grib_close,              3},
    {"grib_length",           (DL_FUNC) &R_grib_close,              1},
    {"grib_get_values",       (DL_FUNC) &R_grib_close,              1},
    {NULL,                    NULL,                                 0}
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
   * R_RegisterCCallable("rGRIB","grib_open",           (DL_FUNC) &R_grib_open);
   *
   */

}

