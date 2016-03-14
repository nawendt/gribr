#' @export

grib_close <- function(gribObj) {

  if (!is.grib(gribObj)) {
    stop("Object is not of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  invisible(.Call("rgrib_grib_close", gribObj$handle))
  cat(gribObj$file, "closed\n")
}
