#' Get GRIB messages
#'
#' \code{grib_get_message} retrieves a GRIB message from a GRIB file using the
#' message number
#'
#' \code{grib_get_message} is a quick and easy way to retrieve a GRIB message
#' from a GRIB file if you know what message number it is. An easy way to
#' determine the message number is to use \code{\link{grib_list}} first.
#'
#' The \code{filter} parameter controls which keys will be filtered out of the
#' results. More information can be found in the
#' \href{https://software.ecmwf.int/wiki/display/GRIB/grib_api.h+File+Reference}{
#' GRIB API Reference}. Below are the options used in this package:
#'
#'  \itemize{
#'   \item "none": no keys filtered
#'   \item "readonly": skip readonly keys
#'   \item "optional": skip optional keys
#'   \item "edition": skip edition-specific keys
#'   \item "coded": skip coded keys
#'   \item "computed": skip keys computed from other keys
#'   \item "duplicate: skip duplicate keys
#'   \item "func": skip keys that are functions
#' }
#'
#' The \code{nameSpace} parameter is a quick and easy way to grab a subset of the
#' keys available in a GRIB file using pre-defined groups of related keys. More
#' information about GRIB key namespaces can be found
#' \href{https://software.ecmwf.int/wiki/display/GRIB/GRIB+API+Frequently+Asked
#' +Questions+FAQ#GRIBAPIFrequentlyAskedQuestionsFAQ-Whatarenamespaces?}{
#' here}. The \code{nameSpace} options used in this package are described
#' below:
#'
#'  \itemize{
#'   \item "" or \code{NULL}: all keys
#'   \item "ls": most commonly used keys
#'   \item "parameter": keys related to the parameter
#'   \item "statistics": keys related to statiscal values of the data
#'   \item "time": keys related to the forecast time/initialization
#'   \item "geography": keys describing the grid geometry
#'   \item "vertical": keys describing levels and layers
#'   \item "mars: ECMWF's Meteorological Archive and Retrieval System keys
#' }
#'
#' @param gribObj GRIB class object.
#' @param messages an integer or vector of integers corresponding to the
#'   messages to extract from the GRIB file.
#' @param mask optional logical that controls whether or not missing values are
#'   changed to \code{NA} based on the bitmap and GRIB files missing value.
#' @param filter optional character string that controls what keys will be
#'   filtered out of the resulting GRIB message. The default is "none". See
#'   'Details' for other options
#' @param nameSpace optional character string that can control what special,
#'   pre-defined group of keys gets return. Defaults to returning all. See
#'   'Details' for full description.
#'
#' @return Returns a \code{gribMessage} object.
#'
#' @seealso \code{\link{grib_select}} \code{\link{grib_list}}
#'
#' @export

grib_get_message <- function(gribObj, messages, mask = FALSE, filter = "none", nameSpace = "") {

  # this matches what is defined in the GRIB API
  gribFilterList = list(
    none      = 0,
    readonly  = bitwShiftL(1, 0),
    optional  = bitwShiftL(1, 1),
    edition   = bitwShiftL(1, 2),
    coded     = bitwShiftL(1, 3),
    computed  = bitwShiftL(1, 4),
    duplicate = bitwShiftL(1, 5),
    func      = bitwShiftL(1, 6)
  )

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

  if (is.null(nameSpace)) {
    nameSpace = ""
  }

  gm <- .Call("rgrib_grib_get_message",
              gribObj$handle, messages, mask,
              gribObj$isMultiMessage,
              as.integer(gribFilterList[filter]),
              nameSpace)

  if (length(gm) < 1) {
    stop("Error retrieving grib message")
  }

  gm <- expand_grids(gm)

  class(gm) <- "gribMessage"
  gm
}
