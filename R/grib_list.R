#' @export

grib_list <- function(gribObj,filter="none",nameSpace="ls") {
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
    if (!is.null.externalptr(gribObj$handle)) {
      if (is.null(nameSpace)) {
        # the c function that uses this parameter
        # will accept the null string in order to
        # print all keys, allow NULL to be input
        # as well
        nameSpace <- ""
      }
      .Call("R_grib_list",gribObj$handle,as.integer(gribFilterList[filter]),nameSpace)
    } else {
      stop("GRIB object is closed or unavailable")
    }
  } else {
    stop("Object is not of class 'grib'")
  }
}
