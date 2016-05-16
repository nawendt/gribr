#' Print GRIB Details
#'
#' \code{print.grib} displays basic status information about a GRIB file.
#'
#' \code{print.grib} is the S3 method for printing GRIB files. Its behavior
#' differs from most other print methods in that it displays some basic
#' information about the GRIB file. When invoked on a GRIB class object, it will
#' display the location of the GRIB file, whether the GRIB file has an active
#' file handle in memory, and whether or not the file containins multiple fields
#' in some messages.
#'
#' @param x \code{GRIB} class object.
#'
#' @param ... futher arguments; ignored here.
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' print(g)
#' grib_close(g)

print.grib <- function(x, ...) {

  cat("GRIB FILE: ", x$file, "\n")

  if (is.null.externalptr(x$handle)) {
    cat("Status: closed\n")
  } else {
    cat("Status: open\n")
    cat("isMultiMessage:", x$isMultiMessage, "\n")
  }

}
