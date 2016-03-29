#' Select multiple GRIB messages
#'
#' \code{grib_select} selects all GRIB messages that match key/value pairs input
#' by the user.
#'
#' \code{grib_select} is a function more robust than \code{grib_get_message} in
#' that it can search through a GRIB file and get all matching messages given
#' key(s)/value(s) pairs. This is achieved by making an index of the input GRIB
#' file and then selecting keys that match user input in \code{keyPairs}.
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
#' @param keyPairs a named list of key/value pairs that will be used to search
#'   through and match messages in the GRIB file using an index
#' @param mask optional logical that controls whether or not missing values are
#'   changed to \code{NA} based on the bitmap and GRIB files missing value.
#' @param filter optional character string that controls what keys will be
#'   filtered out of the resulting GRIB message. The default is "none". See
#'   'Details' for other options
#' @param nameSpace optional character string that can control what special,
#'   pre-defined group of keys gets return. Defaults to returning all. See
#'   'Details' for full description.
#'
#' @return Returns a list of gribMessage objects
#'
#' @seealso \code{\link{grib_get_message}} \code{\link{grib_list}}
#'
#' @export

grib_select <- function(gribObj, keyPairs, mask = FALSE, filter = "none", nameSpace = "") {

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
    stop("gribObj is not of class 'grib'")
  }

  if (!is.list(keyPairs)) {
    stop("keyPairs must be a list")
  }

  if (length(keyPairs) > 1) {
    keyCombo <- expand.grid(keyPairs, stringsAsFactors = FALSE, KEEP.OUT.ATTRS = FALSE)
    keyList <- lapply(1:dim(keyCombo)[1],function(i) as.list(keyCombo[i,]))
  } else if (length(keyPairs) == 1){
    # needs to be list of list to be handled correctly
    # in the c routine that grabs the messages
    keyList <- list(keyPairs)
  } else {
    stop("error with keyPairs input")
  }

  selected <- .Call("rgrib_select", gribObj$file, keyList,
                    mask, gribObj$isMultiMessage,
                    as.integer(gribFilterList[filter]),
                    nameSpace)
  # cleans up output so that each list element is
  # a grib message
  selected <- unlist(selected, recursive = FALSE)

  if (length(selected) < 1) {
    stop("No messages matched keyPairs")
  }

  for (i in length(selected)) {
    selected[[i]] <- expand_grids(selected[[i]])
    class(selected[[i]]) <- "gribMessage"
  }

  selected
}
