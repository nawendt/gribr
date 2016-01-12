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
    invisible(.Call("rgrib_close",gribObj$gribHandle))
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
