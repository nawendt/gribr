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
#' @param gribObj GRIB class object.
#' @param keyPairs a named list of key/value pairs that will be used to search
#'   through and match messages in the GRIB file using an index
#' @param mask optional logical that controls whether or not missing values are
#'   changed to \code{NA} based on the bitmap and GRIB files missing value.
#'
#' @return Returns a list of gribMessage objects
#'
#' @seealso \code{\link{grib_get_message}} \code{\link{grib_list}}
#'
#' @export

grib_select <- function(gribObj, keyPairs, mask = FALSE) {

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
                    mask, gribObj$isMultiMessage)

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
