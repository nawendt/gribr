#' @export
grib_get_message <- function(gribObj) {
  .Call("rgrib_grib_get_message", gribObj$handle)
}
