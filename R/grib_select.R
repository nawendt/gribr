#' Select multiple GRIB messages
#'
#' \code{grib_select} selects all GRIB messages that match key/value pairs input
#' by the user.
#'
#' \code{grib_select} is a function more robust than \code{grib_get_message} in
#' that it can search through a GRIB file and get all matching messages given
#' key(s)/value(s) pairs. This is achieved by making an index of the input GRIB
#' file and then selecting keys that match user input in \code{keyPairs}. The
#' values returned in the GRIB message are masked if they are coded as a missing
#' value or the bitmap, if present, masks them.
#'
#' @param gribObj GRIB class object.
#' @param keyPairs a named list of key/value pairs that will be used to search
#'   through and match messages in the GRIB file using an index.
#'
#' @return Returns a list of gribMessage objects
#'
#' @seealso \code{\link{grib_get_message}} \code{\link{grib_list}}
#'
#' @export

grib_select <- function(gribObj, keyPairs) {

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
                    gribObj$isMultiMessage)

  # cleans up output so that each list element is
  # a grib message
  selected <- unlist(selected, recursive = FALSE)

  if (length(selected) < 1) {
    stop("No messages matched keyPairs")
  }

  # Just return the gribMessage if only one
  if (length(selected) == 1) {
    selected <- selected[[1]]
  }

  selected
}
