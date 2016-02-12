#' @export
grib_open <- function(file, mode) {

#  if (!missing(gribDefinitionPath)) {
#    Sys.setenv(GRIB_DEFINITION_PATH = path.expand(gribDefinitionPath))
#  }

  handle <- .Call("rgrib_grib_open", path.expand(file), mode)
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
