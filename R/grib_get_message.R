#' @export

grib_get_message <- function(gribObj, messages, mask = FALSE, filter = "none", nameSpace = "") {

  # this matches what is defined in the GRIB API
  gribFilterList = list(
    none      = 0,
    readonly  = bitwShiftL(1, 0),
    optional  = bitwShiftL(1, 1),
    edition   = bitwShiftL(1, 2),
    coded     = bitwShiftL(1, 3),
    computed  = bitwShiftL(1, 4),
    duplicate = bitwShiftL(1, 5),
    func      = bitwShiftL(1, 6)
  )

  if (!is.integer(messages) && !is.numeric(messages)) {
    stop("requested message vector must be numeric")
  } else {
    # sort it so C loop can be more efficient
    messages <- sort(as.integer(messages))
  }

  if (length(messages) < 1) {
    stop("no messages requested")
  }

  if (!is.grib(gribObj)) {
    stop("Object is not of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  if (is.null(nameSpace)) {
    nameSpace = ""
  }

  gm <- .Call("rgrib_grib_get_message",
              gribObj$handle, messages, mask,
              gribObj$isMultiMessage,
              as.integer(gribFilterList[filter]),
              nameSpace)

  if (length(gm) < 1) {
    stop("Error retrieving grib message")
  }

  gm <- expand_grids(gm)

  class(gm) <- "gribMessage"
  gm
}
