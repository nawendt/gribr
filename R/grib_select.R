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

  if (is.grib(gribObj)) {
    if (is.list(keyPairs)) {
      keyCombo <- expand.grid(keyPairs, stringsAsFactors = FALSE, KEEP.OUT.ATTRS = FALSE)
      keyList <- lapply(1:dim(keyCombo)[1],function(i) as.list(keyCombo[i,]))
      selected <- .Call("rgrib_select", gribObj$file, keyList,
                       mask, gribObj$isMultiMessage,
                       as.integer(gribFilterList[filter]),
                       nameSpace)
      selected <- unlist(selected, recursive = FALSE)
      selected
    } else {
      stop("keyPairs must be a list")
    }
  } else {
    stop("gribObj is not of class 'grib'")
  }
}
