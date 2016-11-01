#' Create 3D volume of a GRIB variable
#'
#' \code{grib_cube} creates a three-dimensional array from one variable along a
#' chosen vertical coordinate.
#'
#' \code{grib_cube} is a wrapper function for \code{grib_select} to conveniently
#' create a three-dimensional cube. The user inputs a variable to search for and
#' the vertical coordinate to use when finding each level.
#'
#' @param gribObj \code{GRIB} class object.
#'
#' @param shortName The short name given in the GRIB file of the variable to
#'   select.
#' @param typeOfLevel The vertical coordinate to use as given by the typeOfLevel
#'   key in the GRIB file.
#'
#' @param decreasing Parameter to tell the array's vertical coordinate to be
#'   increasing or decreasing.
#'
#' @return Returns a three-dimenional array.
#'
#' @seealso \code{\link{grib_get_message}} \code{\link{grib_list}}
#'   \code{\link{grib_expand_grids}} \code{\link{grib_latlons}} \code{\link{grib_select}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' cube <- grib_cube(g, 'u', 'isobaricInhPa', TRUE)
#' grib_close(g)

grib_cube <- function(gribObj, shortName, typeOfLevel, decreasing = FALSE) {

  cubeSelect <- grib_select(gribObj, list(shortName = shortName, typeOfLevel = typeOfLevel))

  # Not all vertical coordinates increase with height
  levels <- sapply(cubeSelect, function(x) x$level)
  oLevels <- order(levels, decreasing = decreasing)

  # Check dimensions
  nx <- sapply(cubeSelect, function(x) x$Nx)
  ny <- sapply(cubeSelect, function(x) x$Ny)
  nz <- length(oLevels)

  if (length(unique(nx)) > 1 || length(unique(ny)) > 1) {
    stop('All level dimensions must be equal to create cube')
  } else {
    nx <- unique(nx)
    ny <- unique(ny)
  }

  cube <- array(dim = c(nx, ny, nz))

  for (ll in oLevels) {
    cube[,,ll] <- cubeSelect[[ll]]$values
  }

  cube
}
