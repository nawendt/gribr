#' @export
is.grib <- function(obj) {
  if (class(obj) == "grib") {
    TRUE
  } else {
    FALSE
  }
}
