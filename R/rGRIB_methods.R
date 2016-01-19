#' @export
#' @useDynLib rGRIB R_grib_length
length.grib <- function(x) {
  .Call("R_grib_length", x$handle)
}

#' @export
#' @useDynLib rGRIB R_grib_length
print.grib <- function(x, ...) {
  cat("GRIB FILE: ", x$file, "\n")
  cat("Total Messages: ", .Call("R_grib_length", x$handle), "\n")
}
