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
  }

  selected
}
