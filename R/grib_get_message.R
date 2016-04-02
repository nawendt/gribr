#' Get GRIB messages
#'
#' \code{grib_get_message} retrieves a GRIB message from a GRIB file using the
#' message number
#'
#' \code{grib_get_message} is a quick and easy way to retrieve a GRIB message
#' from a GRIB file if you know what message number it is. An easy way to
#' determine the message number is to use \code{\link{grib_list}} first.
#'
#' @param gribObj GRIB class object.
#' @param messages an integer or vector of integers corresponding to the
#'   messages to extract from the GRIB file.
#' @param mask optional logical that controls whether or not missing values are
#'   changed to \code{NA} based on the bitmap and GRIB files missing value.
#'
#' @return Returns a \code{gribMessage} object.
#'
#' @seealso \code{\link{grib_select}} \code{\link{grib_list}}
#'
#' @export

grib_get_message <- function(gribObj, messages, mask = FALSE) {

  if (!is.integer(messages) && !is.numeric(messages)) {
    stop("requested message vector must be numeric")
  } else {
    # sort it so C loop can be more efficient
    messages <- sort(as.integer(messages))
  }

  if (length(messages) < 1) {
    stop("no messages requested")
  }

  if (!is.grib(gribObj)) {
    stop("Object is not of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  gm <- .Call("rgrib_grib_get_message",
              gribObj$handle, messages, mask,
              gribObj$isMultiMessage)

  if (length(gm) < 1) {
    stop("Error retrieving grib message")
  }

  gm <- expand_grids(gm)

  class(gm) <- "gribMessage"
  gm
}
