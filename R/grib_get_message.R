#' @export
grib_get_message2 <- function(gribObj, messages, mask = FALSE, filter = "none", nameSpace = "") {

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
    messages <- sort(as.integer(messages))
  }

  if (length(messages) < 1) {
    stop("no messages requested")
  }

  if (is.grib(gribObj)) {
    if (is.null.externalptr(gribObj$handle)) {
      stop("GRIB object is closed or unavailable")
    } else {
      if (is.null(nameSpace)) {
        nameSpace = ""
      }
      gm <- .Call("rgrib_grib_get_message2",
                  gribObj$handle, messages, mask,
                  gribObj$isMultiMessage,
                  as.integer(gribFilterList[filter]),
                  nameSpace)
      class(gm) <- "gribMessage"
    }
  }

  # Make sure both Ni/Nx, Nj/Ny are in list
  # so scripting is easier across files
  if (is.null(gm$Nx)) {
    gm <- c(gm, Nx = gm$Ni)
  } else if(is.null(gm$Ni)) {
    gm <- c(gm, Ni = gm$Nx)
  }

  if (is.null(gm$Ny)) {
    gm <- c(gm, Ny = gm$Nj)
  } else if(is.null(gm$Nj)) {
    gm <- c(gm, Nj = gm$Ny)
  }

  gm
}
