#' @useDynLib rGRIB, .registration = TRUE, .fixes = "C_"

.onUnload <- function (libpath = find.package("rGRIB")) {
  library.dynam.unload("rGRIB", libpath)
}

