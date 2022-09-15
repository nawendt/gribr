#include <R.h>
#include <Rinternals.h>

#include "gribr.h"

SEXP gribr_redtoreg(SEXP gribr_nlons, SEXP gribr_lonsperlat, SEXP gribr_grid) {
  R_xlen_t i, j, n, im, ip, indx, ilons, nlats, npts;
  double nlons;
  double lonsperlat_sum;
  double zxi, zdx, flons;
  double *grid_ptr = NULL;
  double *regrid_ptr = NULL;
  double *lons_ptr = NULL;
  SEXP regrid;

  nlons = asReal(gribr_nlons);
  nlats = xlength(gribr_lonsperlat);
  npts = xlength(gribr_grid);

  lonsperlat_sum = 0;
  for (i = 0; i < nlats; i++) {
    lonsperlat_sum = lonsperlat_sum + REAL(gribr_lonsperlat)[i];
  }

  if (lonsperlat_sum != npts) {
    error("size of reduced grid does not match number of data values");
  }

  regrid = PROTECT(allocVector(REALSXP, nlats*nlons));

  lons_ptr = REAL(gribr_lonsperlat);
  grid_ptr = REAL(gribr_grid);
  regrid_ptr = REAL(regrid);

  n= 0;
  indx = 0;

  for (j = 0; j < nlats; j++) {
    ilons = lons_ptr[j];
    flons =ilons;
    for (i = 0; i < nlons; i++) {
      zxi = i * flons / nlons;
      im = zxi;
      zdx = zxi - im;
      if (ilons != 0) {
        im = (long)(im + ilons) % (long)ilons;
        ip = (long)(im + 1 + ilons) % (long)ilons;

        if (grid_ptr[indx + im] == NA_REAL || grid_ptr[indx + ip] == NA_REAL) {
          if (zdx < 0.5) {
            regrid_ptr[n] = grid_ptr[indx + im];
          } else {
            regrid_ptr[n] = grid_ptr[indx + ip];
          }
        } else {
          regrid_ptr[n] = grid_ptr[indx + im] * (1. - zdx) + grid_ptr[indx + ip] * zdx;
        }
      }
      n++;
    }
    indx = indx + ilons;
  }
  UNPROTECT(1);
  return regrid;
}
