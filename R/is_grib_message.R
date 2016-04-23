#' GRIB Messages
#'
#' \code{is.grib.message} tests whether and object is a GRIB message
#'
#' @param obj object to be tested
#'
#' @return Returns a \code{logical}
#'
#' @seealso \code{\link{is.grib}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' gm <- grib_get_message(g, 1)
#' is.grib.message(gm)
#' # should be FALSE
#' is.grib.message(g)
#' grib_close(g)

is.grib.message <- function(obj) {
  if (class(obj) == "gribMessage") {
    TRUE
  } else {
    FALSE
  }
}
