# #' @export
# #' @useDynLib rGRIB R_grib_length
#length.grib <- function(x) {
#  .Call("R_grib_length", x$handle)
#}

# #' @export
#print.grib <- function(x, ...) {
#  cat("GRIB FILE: ", x$file, "\n")
#  if (is_null_ptr(x$handle)) {
#    cat("Status: closed\n")
#  } else {
#    cat("Status: open\n")
#    cat("Total Messages: ", .Call("R_grib_length", x$handle), "\n")
#  }
#}
