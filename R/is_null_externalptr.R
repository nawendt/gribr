
is.null.externalptr <- function(pointer) {
  if (is(pointer, "externalptr")) {
   .Call("R_is_null_ptr", pointer)
  } else {
    stop("Input is not of class 'externalptr'")
  }
}
