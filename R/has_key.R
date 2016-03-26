#' @export

has.key <- function(gribMessage, key) {

	if (!is.grib.message(gribMessage)) {
	  stop("input must be of class 'gribMessage'")
	}

  if (is.null(gribMessage[[key]]) ||
      is.na(gribMessage[[key]]) ||
      gribMessage[[key]] == "") {
    FALSE
  } else {
    TRUE
  }
}
