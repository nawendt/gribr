#' GRIB Messages
#'
#' \code{is.grib.message} tests whether and object is a GRIB message
#'
#' @param obj object to be tested
#'
#' @return Returns a logical
#'
#' @seealso \code{\link{is.grib}}
#'
#' @export
is.grib.message <- function(obj) {
  if (class(obj) == "gribMessage") {
    TRUE
  } else {
    FALSE
  }
}
