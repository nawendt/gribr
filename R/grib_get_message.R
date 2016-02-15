#' @export
grib_get_message <- function(gribObj, filter="none", nameSpace="") {
  # this matches what is defined in the GRIB API
  gribFilterList = list(
    none      = 0,
    readonly  = bitwShiftL(1,0),
    optional  = bitwShiftL(1,1),
    edition   = bitwShiftL(1,2),
    coded     = bitwShiftL(1,3),
    computed  = bitwShiftL(1,4),
    duplicate = bitwShiftL(1,5),
    func      = bitwShiftL(1,6)
  )

  if (is.grib(gribObj)) {
    if (is.null.externalptr(gribObj$handle)) {
      stop("GRIB object is closed or unavailable")
    } else {
      if (is.null(nameSpace)) {
        nameSpace = ""
      }
      .Call("rgrib_grib_get_message", gribObj$handle, as.integer(gribFilterList[filter]), nameSpace)
    }
  }

}
