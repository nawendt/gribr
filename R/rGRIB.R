#' @export
#' @useDynLib rGRIB rgrib_open

grib_open <- function(file,mode,gribDefinitionPath){

  if (!missing(gribDefinitionPath)) {
    Sys.setenv(GRIB_DEFINITION_PATH = gribDefinitionPath)
  }

  handle <- .Call("rgrib_open",file,mode)
  if (class(handle) != "externalptr") {
    stop("Problem retrieving grib handle")
  }

  fileName <- basename(file)

  gribObj <- list(fileName = fileName, gribHandle = handle)
  class(gribObj) <- "grib"

  return(gribObj)
}

#' @export
#' @useDynLib rGRIB rgrib_close

grib_close <- function(gribObj) {
  if (is.grib(gribObj)) {
    if (!is.null.externalptr(gribObj$gribHandle)) {
      return(invisible(.Call("rgrib_close",gribObj$gribHandle)))
    } else {
      stop("GRIB object is already closed")
    }
  } else {
    stop("Object is not of class 'grib'")
  }
}

#
# PRIVATE FUNCTIONS
#

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
