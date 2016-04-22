#' Get number of GRIB messages
#'
#' \code{grib_length} loops through a GRIB file to determine the number of
#' messages.
#'
#' \code{grib_length} determines the length of both GRIB files with and without
#' multiple fields per message. The GRIB file is looped though twice, with and
#' without multi-message support turned on, to get the true length.
#'
#' @param gribObj \code{GRIB} class object.
#'
#' @return Returns \code{integer} number of GRIB messages.
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' grib_length(g)
#' grib_close(g)

grib_length <- function(gribObj) {

  if (!is.grib(gribObj)) {
    stop("gribObj must be of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  .Call("gribr_grib_length", gribObj$handle)
}
