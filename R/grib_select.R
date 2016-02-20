#' @export

grib_select <- function(gribObj, keyPairs) {
  if (is.grib(gribObj)) {
    if (is.list(keyPairs)) {
      .Call("rgrib_select",gribObj$file,keyPairs)
    } else {
      stop("keyPairs must be a list")
    }
  } else {
    stop("gribObj is not of class 'grib'")
  }
}
