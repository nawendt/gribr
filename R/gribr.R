#' gribr: GRIB interface for R using the ECMWF ecCodes
#'
#' The gribr package provides a simple interface between R and GRIB 1/2 files.
#' The package is designed so that data is kept in R's intuitive data structures
#' and can be easily projected using your preferred plotting package.
#'
#' @section Notice: This package depends on installing the ECMWF library called
#'   \href{https://confluence.ecmwf.int/display/ECC/ecCodes+Home}{ecCodes}.
#'   It is important to install the requisite libraries
#'   (e.g., jasper/openjpeg for GRIB2) prior to building ecCodes to ensure
#'   full functionality.
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
#'   of the use of the ecCodes, this is possible. Using this package as a
#'   template of sort, one could create new functions that work to expand on the
#'   current capabilities. The
#'   \href{https://confluence.ecmwf.int/display/ECC/GRIB+examples}{ecCodes
#'   examples} are good place to see the basic implementations in C. Beyond
#'   that, this package is hosted on
#'   \href{https://github.com/nawendt/gribr}{GitHub} where you will not only
#'   find the source for this package, but will be able to submit any of your
#'   own developments for inclusion in future releases.
#'
#' @docType package
#' @name gribr
#'
NULL

#' @useDynLib gribr, .registration = TRUE

.onUnload <- function(libpath = find.package("gribr")) {
  library.dynam.unload("gribr", libpath)
}
