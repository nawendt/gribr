.onUnload <- function (libpath = find.package("rGRIB")) {
  library.dynam.unload("rGRIB", libpath)
}

#' @export
#' @useDynLib rGRIB R_grib_open
#' @useDynLib rGRIB R_grib_length
grib_open <- function(file, mode) {

#  if (!missing(gribDefinitionPath)) {
#    Sys.setenv(GRIB_DEFINITION_PATH = path.expand(gribDefinitionPath))
#  }

  handle <- .Call("R_grib_open", path.expand(file), mode)
  if (class(handle) != "externalptr") {
    stop("Problem retrieving grib handle")
  }

  # class definition for rGRIB package
  gribObj <-  list(file = path.expand(file),
                   handle = handle
                   )
  class(gribObj) <- "grib"

  gribObj
}

#' @export
#' @useDynLib rGRIB R_grib_close
grib_close <- function(gribObj) {
  if (is_grib(gribObj)) {
    if (!is_null_ptr(gribObj$handle)) {
      invisible(.Call("R_grib_close", gribObj$handle))
      cat(gribObj$file, "closed\n")
    } else {
      stop("GRIB object is closed or unavailable")
    }
  } else {
    stop("Object is not of class 'grib'")
  }
}

#' @export
#' @useDynLib rGRIB R_grib_list
grib_list <- function(R_gribObj) {
  .Call("R_grib_list", R_gribObj$handle,0, "parameter")
}

# grib_ls <- function(gribObj,filter="none",nameSpace="") {
#   gribFilterList = list(
#     none      = 0,
#     readonly  = bitwShiftL(1,0),
#     optional  = bitwShiftL(1,1),
#     edition   = bitwShiftL(1,2),
#     coded     = bitwShiftL(1,3),
#     computed  = bitwShiftL(1,4),
#     duplicate = bitwShiftL(1,5),
#     func      = bitwShiftL(1,6)
#     )
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



