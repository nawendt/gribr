#' @export
grib_get <- function(gribObj) {
  .Call("rgrib_message_list", gribObj$handle)
}
