#' Close GRIB files
#'
#' \code{grib_close} closes a GRIB file and frees it from memory.
#'
#' \code{grib_close} checks to see if the GRIB object is open before closing it.
#' No formal return value is given with this function. Only feedback is a short
#' message inidicating success or failure.
#'
#' @param gribObj \code{GRIB} class object.
#'
#' @seealso \code{\link{grib_open}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' # ...do some analysis...
#' grib_close(g)
#' g # check status

grib_close <- function(gribObj) {

  if (!is.grib(gribObj)) {
    stop("object is not of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  invisible(.Call(gribr_grib_close, gribObj$handle))
  cat(gribObj$file, "closed\n")
}
