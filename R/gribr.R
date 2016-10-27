#' gribr: GRIB interface for R using the ECMWF GRIB API
#'
#' The gribr package provides a simple interface between R and GRIB 1/2 files.
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
#' @section gribr Functions:
#'
#'   The main functions that will be used in this package are those that can
#'   easily display the contents of a GRIB file (grib_list), those that import
#'   messages into R (grib_get_message, grib_select) and those that extract the
#'   projection information from the GRIB message (grib_proj4str). All other
#'   functions are essentially helper functions to aid in scripting with the
#'   main tools.
#'
#' @section Extensions: gribr was designed to be simple and extensible. Because
#'   of the use of the GRIB API, this is possible. Using this package as a
#'   template of sort, one could create new functions that work to expand on the
#'   current capabilities. The
#'   \href{https://software.ecmwf.int/wiki/display/GRIB/GRIB+API+examples}{GRIB
#'   API examples} are good place to see the basic implementations in C. Beyond
#'   that, this package is hosted on
#'   \href{https://github.com/nawendt/gribr}{GitHub} where you will not only
#'   find the source for this package, but will be able to submit any of your
#'   own developments for inclusion in future releases.
#'
#' @docType package
#' @name gribr
#'
NULL

#' @useDynLib gribr, .registration = TRUE, .fixes = "rgc_"

.onUnload <- function (libpath = find.package("gribr")) {
  library.dynam.unload("gribr", libpath)
}
