is.null.externalptr <- function(x) {
  if (methods::is(x, "externalptr")) {
   .Call("gribr_is_null_ptr", x)
  } else {
    stop("Input is not of class 'externalptr'")
  }
}
