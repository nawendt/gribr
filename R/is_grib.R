#' GRIB Messages
#'
#' \code{is.grib} tests whether and object is a GRIB object
#'
#' @param obj object to be tested
#'
#' @return Returns a \code{logical}
#'
#' @seealso \code{\link{is.grib.message}}
#'
#' @export
is.grib <- function(obj) {
  if (class(obj) == "grib") {
    TRUE
  } else {
    FALSE
  }
}
