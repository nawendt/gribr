#' @export

grib_expand_grids <- function(gribMessage) {

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

  values
}
