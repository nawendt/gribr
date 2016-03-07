is.null.externalptr <- function(x) {
  if (is(x, "externalptr")) {
   .Call("rgrib_is_null_ptr", x)
  } else {
    stop("Input is not of class 'externalptr'")
  }
}
