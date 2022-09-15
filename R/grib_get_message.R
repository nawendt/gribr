#' Get GRIB messages
#'
#' \code{grib_get_message} retrieves a GRIB message from a GRIB file using the
#' message number.
#'
#' \code{grib_get_message} is a quick and easy way to retrieve a GRIB message
#' from a GRIB file if you know what message number it is. An easy way to
#' determine the message number is to use \code{\link{grib_list}} first (see
#' examples). The values returned in the GRIB message are masked if they are
#' coded as a missing value or the bitmap, if present, masks them.
#'
#' All grid (latitude, longitude, values, etc.) are output as vectors for
#' simplicity and flexibility. Many plotting functions want input in matrix form
#' (e.g., \code{image()}, \code{fields::image.plot()} while others want input in
#' long form \code{data.frame}s or vectors (e.g., \code{ggplot}). Keeping vector
#' form allows the most flexibility for the user to do what is needed to format
#' the data appropriately. Some helper functions in this package will do some
#' data formatting for you.
#'
#' @param gribObj \code{GRIB} class object.
#' @param messages an \code{integer} or vector of \code{integer}s corresponding
#'   to the messages to extract from the GRIB file.
#'
#' @return Returns a \code{gribMessage} object.
#'
#' @seealso \code{\link{grib_select}} \code{\link{grib_list}}
#'   \code{\link{grib_expand_grids}} \code{\link{grib_latlons}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' # select one message
#' gm <- grib_get_message(g, 1)
#' # select multiple messages
#' gm_multi <- grib_get_message(g, c(2, 4, 6))
#'
#' # Use grib_list output to help select messages
#' msg_loc <- which(grib_list(g)$shortName == "2t") # find 2m temp message
#' gm_ls <- grib_get_message(g, msg_loc)
#' grib_close(g)

grib_get_message <- function(gribObj, messages) {

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
    stop("object is not of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  gm <- .Call("gribr_grib_get_message",
              gribObj$handle, messages,
              gribObj$isMultiMessage)

  if (length(gm) < 1) {
    stop("problem retrieving grib message(s)")
  }

  gm
}
