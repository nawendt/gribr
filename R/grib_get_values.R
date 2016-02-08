#' @export
grib_get_values <- function(gribObj) {
  if (!is.grib(gribObj)) {
    stop("Object is not of class 'grib'")
  } else {
    if (is.null.externalptr(gribObj$handle)) {
      stop("Grib object is closed or unavailable")
    }
    tmp <- .Call("R_grib_get_values", gribObj$handle)
    matrix(tmp$values, tmp$nx, tmp$ny)
  }
}
