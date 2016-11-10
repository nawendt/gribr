#' GRIB Messages
#'
#' \code{is.grib} tests whether and object is a GRIB object.
#'
#' @param obj object to be tested.
#'
#' @return Returns a \code{logical}.
#'
#' @seealso \code{\link{is.grib.message}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' gm <- grib_get_message(g, 1)
#' is.grib.message(g)
#' # should be FALSE
#' is.grib(gm)
#' grib_close(g)

is.grib <- function(obj) {
  if (class(obj) == "grib") {
    TRUE
  } else {
    FALSE
  }
}
