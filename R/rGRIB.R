#' @useDynLib rGRIB, .registration = TRUE, .fixes = "rgc_"

.onUnload <- function (libpath = find.package("rGRIB")) {
  library.dynam.unload("rGRIB", libpath)
}

