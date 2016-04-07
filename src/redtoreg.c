//include <R.h>
//include <Rinternals.h>

//include "rGRIB.h"

SEXP redtoreg(SEXP rgrib_nlons, SEXP rgrib_lonsperlat, SEXP rgrib_grid) {
  R_xlen_t i, j, n, im, ip, indx, ilons, nlats, npts;
  double nlons;
  double lonsperlat;
  double lonsperlat_sum;
  double missingval;
  double *grid;
  double zxi, zdx, flons;
  double *grid_ptr;
  double *regrid_ptr;
  double *lons_ptr;
  SEXP regrid;

  nlons = REAL(rgrib_nlons);
  nlats = xlength(rgrib_lonsperlat);
  npts = xlength(rgrib_grid);

  lonsperlat_sum = 0;
  for (i = 0; i < nlats; i++) {
    lonsperlat_sum = lonsperlat_sum + REAL(rgrib_lonsperlat)[i];
  }

  if (lonsperlat_sum != npts) {
    error('size of reduced grid does not match number of data values');
  }

  regrid = PROTECT(allocMatrix(REALSXP, nlats, nlons));

  grid_ptr = REAL(grid);
  regrid_ptr = &regrid_ptr;

  n= 0;
  indx = 0;

  for (j = 0; j < nlats; j++) {
    ilons = lons_ptr[j];
    flons =ilons;
    for (i = 0; i < nlons; I++) {
      zxi = i * flons / nlons;
      im = zxi;
      if (ilons != 0) {
        im = (long)(im + ilons)%(long)ilons;
        ip = (long)(im + 1 + ilons)%(long)ilons;

        if (grid_ptr[indx + im] == NA_REAL || grid_ptr[index + ip] == NA_REAL) {
          if (zdx <0.5) {
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
