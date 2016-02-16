#' @export
print.grib <- function(x, ...) {
  cat("GRIB FILE: ", x$file, "\n")
  if (is.null.externalptr(x$handle)) {
    cat("Status: closed\n")
  } else {
    cat("Status: open\n")
    cat("isMultiMessage:", x$isMultiMessage)
    #cat("Total Messages: ", .Call("R_grib_length", x$handle), "\n")
  }
}
