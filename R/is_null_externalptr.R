is.null.externalptr <- function(x) {
  if (is(x, "externalptr")) {
   .Call("gribr_is_null_ptr", x)
  } else {
    stop("Input is not of class 'externalptr'")
  }
}
