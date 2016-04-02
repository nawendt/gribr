#' rGRIB: GRIB interface for R using the ECMWF GRIB API
#'
#' The rGRIB package provides a simple interface between R and GRIB 1/2 files.
#' The package is designed so that data is kept in R's intuitive data structures
#' and can be easily projected using your preferred plotting package.
#'
#' @section Notice: This package depends on installing the
#'   \href{https://software.ecmwf.int/wiki/display/GRIB/Home}{ECMWF GRIB API}.
#'   It is important to install the requisite libraries to ensure full
#'   functionality (e.g., jasper/openjpeg for GRIB2). Be aware that ECMWF has
#'   plans to move the GRIB API into their
#'   \href{https://software.ecmwf.int/wiki/display/ECC}{EC CODES} package in the
#'   future. This package will follow suit once the EC CODES package is out of
#'   beta testing and ready to supplant the GRIB API.
#'
#' @section rGRIB Functions:
#'
#'   The main functions that will be used in this package are those that can
#'   easily display the contents of a GRIB file (grib_list), those that import
#'   messages into R (grib_get_message, grib_select) and those that extract the
#'   projection information from the GRIB message (grib_proj4str). All other
#'   functions are essentially helper functions to aid in scripting with the
#'   main tools.
#'
#' @docType package
#' @name rGRIB
#'
NULL

#' @useDynLib rGRIB, .registration = TRUE, .fixes = "rgc_"

.onUnload <- function (libpath = find.package("rGRIB")) {
  library.dynam.unload("rGRIB", libpath)
}

