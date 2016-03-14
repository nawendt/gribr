#' @export

grib_length <- function(gribObj) {

  if (!is.grib(gribObj)) {
    stop("gribObj must be of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  .Call("rgrib_grib_length", gribObj$handle)
}
