#' @export
grib_get_message <- function(gribObj) {
  .Call("R_grib_get_message", gribObj$handle)
}
