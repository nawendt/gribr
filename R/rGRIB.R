#######################
#     GLOBAL VARS
#######################
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

#' @export
#' @useDynLib rGRIB R_grib_open
grib_open <- function(file,mode) {

#  if (!missing(gribDefinitionPath)) {
#    Sys.setenv(GRIB_DEFINITION_PATH = path.expand(gribDefinitionPath))
#  }

  handle <- .Call("R_grib_open",path.expand(file),mode)
  if (class(handle) != "externalptr") {
    stop("Problem retrieving grib handle")
  }

  # class definition for rGRIB package
  gribObj <- list(file = path.expand(file), handle = handle)
  class(gribObj) <- "grib"

  gribObj
}

#' @export
#' @useDynLib rGRIB R_grib_close
grib_close <- function(gribObj) {
  if (is_grib(gribObj)) {
    if (!is_null_ptr(gribObj$handle)) {
      .Call("R_grib_close",gribObj$handle)
    } else {
      stop("GRIB object is closed or unavailable")
    }
  } else {
    stop("Object is not of class 'grib'")
  }
}

#' @export
#' @useDynLib rGRIB R_grib_ls
grib_ls <- function() {
  .Call("R_grib_ls")
}

# grib_ls <- function(gribObj,filter="none",nameSpace="") {
#   if (is.grib(gribObj)) {
#     if (!is.null.externalptr(gribObj$gribHandle)) {
#       if (is.null(nameSpace)) {
#         # the c function that uses this parameter
#         # will accept the null string in order to
#         # print all keys, allow NULL to be input
#         # as well
#         nameSpace <- ""
#       }
#       .Call("rgrib_ls",gribObj$gribHandle,as.integer(gribFilterList[filter]),nameSpace)
#     } else {
#       stop("GRIB object is closed or unavailable")
#     }
#   } else {
#     stop("Object is not of class 'grib'")
#   }
# }



