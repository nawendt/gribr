#' Open GRIB files
#'
#' \code{grib_open} opens a GRIB file for reading or writing and returns a GRIB
#' class object.
#'
#' \code{grib_open} is the constructor function for the S3 GRIB class object.
#' The GRIB object is list with three elements: \code{file}, \code{handle}, and
#' \code{isMultiMessage}. \code{file} is the path to the opened file,
#' \code{handle} is an \code{externalptr} to the \code{file} in memory, and
#' \code{isMultiMessage} is a \code{logical} indicating if there are multiple
#' entries in any of the \code{file}'s messages.
#'
#' @param file \code{character} string. A path to a GRIB file.
#'
#' @return Returns a \code{GRIB} class object: see 'Details'.
#'
#' @seealso \code{\link{grib_close}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' g
#' grib_close(g)

grib_open <- function(file) {

#  if (!missing(gribDefinitionPath)) {
#    Sys.setenv(GRIB_DEFINITION_PATH = path.expand(gribDefinitionPath))
#  }

  handle <- .Call("gribr_grib_open", path.expand(file))
  if (class(handle) != "externalptr") {
    stop("Problem retrieving grib handle")
  }

  # Need to know if multiple fields in one GRIB message
  isMultiMessage <- .Call("gribr_is_multi_message", handle)

  # class definition for gribr package
  gribObj <-  list(file = path.expand(file),
                   handle = handle,
                   isMultiMessage = isMultiMessage
                   )
  class(gribObj) <- "grib"

  gribObj
}
