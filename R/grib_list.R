#' List contents of GRIB file
#'
#' \code{grib_list} lists the contents of a GRIB file using a manageable subset
#' of the keys in each message. This can be used to efficiently grab the message
#' you want when used in conjunction with \code{\link{grep}} and
#' \code{\link{grib_get_message}}.
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
#' \code{grib_list} is a simple tool
#' @param gribObj GRIB class object.
#' @param filter optional character string that controls what keys will be
#'   filtered out of the resulting GRIB message. The default is "none". See
#'   'Details' for other options
#' @param nameSpace optional character string that can control what special,
#'   pre-defined group of keys gets return. Defaults to returning all. See
#'   'Details' for full description.
#'
#' @return Returns a character vector containing the keys subset for each
#'   message
#'
#' @seealso \code{\link{grep}} \code{\link{grib_get_message}}
#'
#' @export

grib_list <- function(gribObj, filter = "none", nameSpace = "ls") {
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

  if (!is.grib(gribObj)) {
    stop("Object is not of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  if (is.null(nameSpace)) {
    # the c function that uses this parameter
    # will accept the null string in order to
    # print all keys, allow NULL to be input
    # as well
    nameSpace <- ""
  }

  .Call("rgrib_grib_list",gribObj$handle, as.integer(gribFilterList[filter]), nameSpace)

}
