#######################
#     GLOBAL VARS
#######################
gribFilterList = list(
  none      = 0,
  readOnly  = bitwShiftL(1,0),
  optional  = bitwShiftL(1,1),
  edition   = bitwShiftL(1,2),
  coded     = bitwShiftL(1,3),
  computed  = bitwShiftL(1,4),
  duplicate = bitwShiftL(1,5),
  func      = bitwShiftL(1,6)
)

#######################
#    MAIN FUNCTIONS
#######################
#' @export
#' @useDynLib rGRIB rgrib_open

grib_open <- function(file,mode){

#  if (!missing(gribDefinitionPath)) {
#    Sys.setenv(GRIB_DEFINITION_PATH = path.expand(gribDefinitionPath))
#  }

  handle <- .Call("rgrib_open",path.expand(file),mode)
  if (class(handle) != "externalptr") {
    stop("Problem retrieving grib handle")
  }

  gribObj <- list(file = path.expand(file), handle = handle)
  class(gribObj) <- "grib"

  return(gribObj)
}

#' @export
#' @useDynLib rGRIB rgrib_close

grib_close <- function(gribObj) {
  if (is.grib(gribObj)) {
    if (!is.null.externalptr(gribObj$handle)) {
      return(invisible(.Call("rgrib_close",gribObj$handle)))
    } else {
      stop("GRIB object is closed or unavailable")
    }
  } else {
    stop("Object is not of class 'grib'")
  }
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

#######################
#    PRIVATE FUNCS
#######################

is.grib <- function(obj) {
  if (class(obj) == "grib") {
    return(TRUE)
  } else {
    return(FALSE)
  }
}

#' @useDynLib rGRIB isNullPointer

is.null.externalptr <- function(pointer) {
  if (is(pointer, "externalptr")) {
    return(.Call("isNullPointer", pointer))
  } else {
    stop("Input is not of class 'externalptr'")
  }
}
