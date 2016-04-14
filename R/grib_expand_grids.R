#' Expand GRIB message value grids
#'
#' \code{grib_expand_grids} extracts the values from a GRIB message and expands
#' them into \code{matrix} form.
#'
#' \code{grib_expand_grids} is another convenience function that will format the
#' values of the GRIB message into a \code{matrix}. In the case of a reduced
#' grid, an interpolation will be done to expand the values accordingly.
#'
#' This function is influenced by the reduced to regular C interpolation routine
#' used in the pygrib Python module (see
#' \href{https://github.com/jswhit/pygrib/blob/master/pygrib.pyx}{pygrib
#' source}).
#'
#' @param gribMessage \code{gribMessage} class object
#' @param vector optional \code{logical} inidicating whether or not to return as
#'   a vector. Default \code{FALSE}.
#'
#' @return a \code{matrix} or vector of values from the GRIB message.
#'
#' @seealso \code{\link{grib_latlons}}
#'
#' @export

grib_expand_grids <- function(gribMessage, vector = FALSE) {

  if(has.key(gribMessage, "Nx") && has.key(gribMessage, "Ny")) {
    nx <- gribMessage$Nx
    ny <- gribMessage$Ny
  } else {
    nx <- gribMessage$Ni
    ny <- gribMessage$Nj
  }

  if (grepl("reduced", gribMessage$gridType)) {
    nx <- ny * 2
    values <- .Call("rgrib_redtoreg", nx, gribMessage$pl,
                    gribMessage$values)
    values <- matrix(values, nx, ny,
                     byrow = gribMessage$jPointsAreConsecutive)
  } else {
    values <- matrix(gribMessage$values, nx, ny,
                     byrow = gribMessage$jPointsAreConsecutive)
  }

  if (vector) {
    values <- as.numeric(values)
  }

  values
}
