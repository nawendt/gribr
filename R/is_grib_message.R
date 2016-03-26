#' @export
is.grib.message <- function(obj) {
  if (class(obj) == "gribMessage") {
    TRUE
  } else {
    FALSE
  }
}
