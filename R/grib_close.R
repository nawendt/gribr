#' @export
grib_close <- function(gribObj) {
  if (is.grib(gribObj)) {
    if (!is.null.externalptr(gribObj$handle)) {
      invisible(.Call("rgrib_grib_close", gribObj$handle))
      cat(gribObj$file, "closed\n")
    } else {
      stop("GRIB object is closed or unavailable")
    }
  } else {
    stop("Object is not of class 'grib'")
  }
}
