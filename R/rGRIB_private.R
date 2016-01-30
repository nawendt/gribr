is_grib <- function(obj) {
  if (class(obj) == "grib") {
    TRUE
  } else {
    FALSE
  }
}

#' @useDynLib rGRIB R_is_null_ptr
is_null_ptr <- function(pointer) {
  if (is(pointer, "externalptr")) {
    .Call("R_is_null_ptr", pointer)
  } else {
    stop("Input is not of class 'externalptr'")
  }
}
