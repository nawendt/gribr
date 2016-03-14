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
