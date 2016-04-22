#' Test for keys
#'
#' \code{has.key} tests whether a gribMessage object has a given key
#'
#' @param gribMessage gribMessage object to be tested
#' @param key character string of the key to be tested for
#'
#' @return Returns a \code{logical}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' gm <- grib_get_message(g, 1)
#' # this key exists
#' has.key(gm, "shortName")
#' # this key does not exist
#' has.key(gm, "grib")
#' grib_close(g)

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
