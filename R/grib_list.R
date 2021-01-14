#' List contents of GRIB file
#'
#' \code{grib_list} lists the contents of a GRIB file using a manageable subset
#' of the keys in each message. This can be used to efficiently grab the message
#' you want when used in conjunction with \code{\link{grep}} and
#' \code{\link{grib_get_message}}.
#'
#' The \code{filter} parameter controls which keys will be filtered out of the
#' results. More information can be found in the
#' \href{https://confluence.ecmwf.int/display/ECC/ecCodes+API+Reference}{
#' ECCODES API Reference}. Below are the options used in this package:
#'
#' \itemize{ \item "none": no keys filtered \item "readonly": skip readonly keys
#' \item "optional": skip optional keys \item "edition": skip edition-specific
#' keys \item "coded": skip coded keys \item "computed": skip keys computed from
#' other keys \item "duplicate: skip duplicate keys \item "func": skip keys that
#' are functions }
#'
#' The \code{namespace} parameter is a quick and easy way to grab a subset of
#' the keys available in a GRIB file using pre-defined groups of related keys.
#' To get a full set of keys, extract a \code{gribMessage} and view the
#' \code{names}. More information about GRIB key namespaces can be found
#' \href{https://confluence.ecmwf.int/display/UDOC/What+are+namespaces+-+ecCodes+GRIB+FAQ}{
#' here}. The \code{namespace} options used in this package are described below:
#'
#' \itemize{ \item "ls": most commonly used keys \item "parameter": keys related
#' to the parameter \item "statistics": keys related to statiscal values of the
#' data \item "time": keys related to the forecast time/initialization \item
#' "geography": keys describing the grid geometry \item "vertical": keys
#' describing levels and layers \item "mars": ECMWF's Meteorological Archive and
#' Retrieval System keys }
#'
#' \strong{NOTE:} The output of \code{grib_list} is predicated on having the
#' appropriate GRIB definition files available to the GRIB API. By default that
#' location is in \code{share/grib_api/definitions} directory in the GRIB API
#' root. Any locally defined paramters will not likely be displayed correctly
#' without first overriding the parameter definitions manually. You can read
#' more information about creating your own local definitions
#' \href{https://confluence.ecmwf.int/display/UDOC/Creating+your+own+local+definitions+-+ecCodes+GRIB+FAQ}{
#' here}. Once your own definitions are created, be sure
#' to set the \code{ECCODES_DEFINITION_PATH} environment variable to the location
#' of your local defintions before using this function to display the contents.
#'
#' @param gribObj GRIB class object.
#' @param filter optional \code{character} string that controls what keys will
#'   be filtered out of the resulting GRIB message. The default is "none". See
#'   'Details' for other options
#' @param namespace optional \code{character} string that can control what
#'   special, pre-defined group of keys gets return. Defaults to returning all.
#'   See 'Details' for full description.
#' @param output optional \code{character} string that controls the output
#'   format. The default is "table" and will display the results in a
#'   \code{data.frame} for easy subsetting. The other option is "string" which
#'   will output each messages keys as a \code{character} vector of strings.
#'
#' @return Returns a \code{character} vector containing the keys subset for each
#'   message.
#'
#' @seealso \code{\link{grep}} \code{\link{grib_get_message}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' grib_list(g)
#'
#' # Use grib_list output to help select messages
#' msg_loc_df <- which(grib_list(g)$shortName == "u")
#' gm <- grib_get_message(g, msg_loc_df)
#'
#' msg_loc_str <- grep("shortName=2t", grib_list(g, output = "string")) # find 2m temp message
#' gm <- grib_get_message(g, msg_loc_str)
#'
#' grib_close(g)

grib_list <- function(gribObj, filter = "none", namespace = "ls",
                      output = "table") {
  # this matches what is defined in the GRIB API
  gribFilterList <- list(
    none      = 0,
    readonly  = bitwShiftL(1, 0),
    optional  = bitwShiftL(1, 1),
    edition   = bitwShiftL(1, 2),
    coded     = bitwShiftL(1, 3),
    computed  = bitwShiftL(1, 4),
    duplicate = bitwShiftL(1, 5),
    func      = bitwShiftL(1, 6)
    )

  if (!is.grib(gribObj)) {
    stop("Object is not of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  if (!(filter %in% names(gribFilterList))) {
    stop("Invalid GRIB keys filter")
  }

  if (!(namespace %in% c("ls", "parameter", "statistics",
                         "time", "geography", "vertical",
                         "mars"))) {
    stop("Invalid GRIB keys namespace")
  }

  if (output == "table") {
    .Call("gribr_grib_df", gribObj$handle, as.integer(gribFilterList[filter]),
                          namespace, gribObj$isMultiMessage)
  } else if (output == "string") {
    .Call("gribr_grib_list", gribObj$handle, as.integer(gribFilterList[filter]),
          namespace, gribObj$isMultiMessage)
  } else {
    stop("Unknown output type")
  }

}
