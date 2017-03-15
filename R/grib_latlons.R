#' Extract projected latitudes and longitudes
#'
#' \code{grib_latlons} extracts the latitudes and longitudes from a GRIB message
#' and optionally expands them into \code{matrix} form.
#'
#' \code{grib_latlons} is a convenience function that returns the latitude and
#' longitudes after reversing projected coordinates (using the \code{proj4}
#' package) from the GRIB message. This makes it easy to use latitude and
#' longitude for the axes in plots intstead of eastings and northings. Use of
#' this function is optional given that the user can extract the PROJ.4 string
#' from the GRIB message using \code{\link{grib_proj4str}}. From the user can
#' use whatever package/work flow to project the data. Not all grid types may be
#' supported at this time. New grid types may be added in the future.
#'
#' This function is influenced by the latitude and longitude extraction method
#' used in the pygrib Python module (see
#' \href{https://github.com/jswhit/pygrib/blob/master/pygrib.pyx}{pygrib
#' source}).
#'
#' @param gribMessage \code{gribMessage} class object.
#' @param expand optional \code{logical} inidcating whether or not to return
#'   coordinates as a \code{matrix} or not. Default \code{FALSE}.
#'
#' @return a \code{list} or a \code{matrix} of the latitudes and longitudes
#'   after projection.
#'
#' @seealso \code{\link{grib_select}} \code{\link{grib_get_message}}
#'   \code{\link{grib_expand_grids}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' gm <- grib_get_message(g, 1)
#' latlon <- grib_latlons(gm)
#' grib_close(g)

grib_latlons <- function(gribMessage, expand = FALSE) {

  if (!is.grib.message(gribMessage)) {
    stop("input must be of class 'gribMessage'")
  }

  if (grib_proj4str(gribMessage) == "") {
    stop("no proj4 string for this message")
  }

  gridType <- gribMessage$gridType

  if (gribMessage$gridType %in% c("regular_gg", "regular_ll")) {
    nx <- gribMessage$Ni
    ny <- gribMessage$Nj
    lon1 <- gribMessage$longitudeOfFirstGridPointInDegrees
    lon2 <- gribMessage$longitudeOfLastGridPointInDegrees

    if (lon1 >= 0 && lon2 < 0 && gribMessage$iDirectionIncrement > 0) {
      lon2 <- lon2 + 360
    }
    if (lon1 >= 0 && lon2 < lon1 && gribMessage$iDirectionIncrement > 0) {
      lon1 <- lon1 - 360
    }
    lat1 <- gribMessage$latitudeOfFirstGridPointInDegrees
    lat2 <- gribMessage$latitudeOfLastGridPointInDegrees

    if (grepl("grid_complex", gribMessage$packingType)) {
      lats <- seq(lat1, lat2, length.out = ny)
      lons <- seq(lon1, lon2, length.out = nx)
    } else {
      lats <- gribMessage$distinctLatitudes
      if (lat2 < lat1 && lats[length(lats)] > lats[1]) {
        lats <- rev(lats)
      }
      lons <- gribMessage$distinctLongitudes
    }
    if (lon2 < 0) {
      lons <- seq(lon1, lon2, length.out = nx)
    }
    mg <- meshgrid(lons, lats)
    lons <- mg$X
    lats <- mg$Y

  } else if (gribMessage$gridType == "reduced_gg") {

    if (expand) {
      lat1 <- gribMessage$latitudeOfFirstGridPointInDegrees
      lat2 <- gribMessage$latitudeOfLastGridPointInDegrees
      lats <- gribMessage$distinctLatitudes
      if (lat2 < lat1 && lats[length(lats)] > lats[1]) {
        lats <- rev(lats)
      }
      ny <- gribMessage$Nj
      nx <- ny * 2
      lon1 <- gribMessage$longitudeOfFirstGridPointInDegrees
      lon2 <- gribMessage$longitudeOfLastGridPointInDegrees
      lons <- seq(lon1, lon2, length.out = nx)
      mg <- meshgrid(lons, lats)
      lons <- mg$X
      lats <- mg$Y
    } else {
      lats <- gribMessage$latitudes
      lons <- gribMessage$longitudes
    }

  } else if (gribMessage$gridType == "reduced_ll") {

    if (expand) {
      lat1 <- gribMessage$latitudeOfFirstGridPointInDegrees
      lat2 <- gribMessage$latitudeOfLastGridPointInDegrees
      lon1 <- gribMessage$longitudeOfFirstGridPointInDegrees
      lon2 <- gribMessage$longitudeOfLastGridPointInDegrees
      ny <- gribMessage$Nj
      nx <- ny * 2
      lons <- seq(lon1, lon2, length.out = nx)
      lats <- seq(lat1, lat2, length.out = ny)
      mg <- meshgrid(lons, lats)
      lons <- mg$X
      lats <- mg$Y
    } else {
      lats <- gribMessage$latitudes
      lons <- gribMessage$longitudes
    }

  } else if (gribMessage$gridType == "polar_stereographic") {

    lat1 <- gribMessage$latitudeOfFirstGridPointInDegrees
    lon1 <- gribMessage$longitudeOfFirstGridPointInDegrees

    if (has.key(gribMessage, "Nx") && has.key(gribMessage, "Ny")) {
      nx <- gribMessage$Nx
      ny <- gribMessage$Ny
    } else {
      nx <- gribMessage$Ni
      ny <- gribMessage$Nj
    }

    if (has.key(gribMessage, "xDirectionGridLengthInMetres") &&
       has.key(gribMessage, "yDirectionGridLengthInMetres")) {
      dx <- gribMessage$xDirectionGridLengthInMetres
      dy <- gribMessage$yDirectionGridLengthInMetres
    } else {
      dx <- gribMessage$DxInMetres
      dy <- gribMessage$DyInMetres
    }

    prj <- proj4::project(list(lon1, lat1), grib_proj4str(gribMessage))
    llcx <- prj$x
    llcy <- prj$y
    x <- llcx + dx * seq_len(nx)
    y <- llcy + dy * seq_len(ny)
    mg <- meshgrid(x, y)
    unprj <- proj4::project(list(mg$X, mg$Y),
                            grib_proj4str(gribMessage),
                            inverse = TRUE)
    lons <- unprj$x
    lats <- unprj$y

  } else if (gridType == "lambert") {

    lat1 <- gribMessage$latitudeOfFirstGridPointInDegrees
    lon1 <- gribMessage$longitudeOfFirstGridPointInDegrees

    if (has.key(gribMessage, "Nx") && has.key(gribMessage, "Ny")) {
      nx <- gribMessage$Nx
      ny <- gribMessage$Ny
    } else {
      nx <- gribMessage$Ni
      ny <- gribMessage$Nj
    }

    dx <- gribMessage$DxInMetres
    dy <- gribMessage$DyInMetres
    prj <- proj4::project(list(lon1, lat1), grib_proj4str(gribMessage))
    llcx <- prj$x
    llcy <- prj$y
    x <- llcx + dx * seq_len(nx)
    y <- llcy + dy * seq_len(ny)
    mg <- meshgrid(x, y)
    unprj <- proj4::project(list(mg$X, mg$Y),
                            grib_proj4str(gribMessage),
                            inverse = TRUE)
    lons <- unprj$x
    lats <- unprj$y

  } else if (gribMessage$gridType == "albers") {

    lat1 <- gribMessage$latitudeOfFirstGridPointInDegrees
    lon1 <- gribMessage$longitudeOfFirstGridPointInDegrees

    if (has.key(gribMessage, "Nx") && has.key(gribMessage, "Ny")) {
      nx <- gribMessage$Nx
      ny <- gribMessage$Ny
    } else {
      nx <- gribMessage$Ni
      ny <- gribMessage$Nj
    }

    dx <- gribMessage$Dx * 0.001
    dy <- gribMessage$Dy * 0.001
    scale <- as.numeric(gribMessage$grib2divider)
    prj <- proj4::project(list(lon1, lat1), grib_proj4str(gribMessage))
    llcx <- prj$x
    llcy <- prj$y
    x <- llcx + dx * seq_len(nx)
    y <- llcy + dy * seq_len(ny)
    mg <- meshgrid(x, y)
    unprj <- proj4::project(list(mg$X, mg$Y),
                            grib_proj4str(gribMessage),
                            inverse = TRUE)
    lons <- unprj$x
    lats <- unprj$y

  } else if (gribMessage$gridType == "space_view") {

    pjs <- grib_proj4str(gribMessage)

    if (pjs$proj == "nsper" && pjs$a != pjs$b) {
      stop("unsupported nsper: earth not a perfect sphere")
    }

    if (has.key(gribMessage, "Nx") && has.key(gribMessage, "Ny")) {
      nx <- gribMessage$Nx
      ny <- gribMessage$Ny
    } else {
      nx <- gribMessage$Ni
      ny <- gribMessage$Nj
    }

    scale <- as.numeric(gribMessage$grib2divider)
    lon_0 <- pjs$lon_0
    lat_0 <- pjs$lat_0
    lonmax <- lon_0 + 90 - (180 / pi) * asin(scale / gribMessage$Nr)
    latmax <- lat_0 + 90 - (180 / pi) * asin(scale / gribMessage$Nr)
    lonmax <- integer(lonmax * 1000) / 1000
    latmax <- integer(latmax * 1000) / 1000
    prj <- proj4::project(list(lon_0, lonmax), grib_proj4str(gribMessage))
    y1 <- prj$y
    prj <- proj4::project(list(lat_0, latmax), grib_proj4str(gribMessage))
    x2 <- prj$x
    width <- x2 * 2
    height <- y1 * 2
    dx <- width / gribMessage$dx
    dy <- height / gribMessage$dy
    xmax <- dx * (nx - 1)
    ymax <- dy * (ny - 1)
    x <- seq(-0.5 * xmax, 0.5 * xmax, length.out = nx)
    y <- seq(-0.5 * ymax, 0.5 * ymax, length.out = ny)
    mg <- meshgrid(x, y)
    unprj <- proj4::project(list(mg$X, mg$Y),
                            grib_proj4str(gribMessage),
                            inverse = TRUE)
    lons <- unprj$x
    lats <- unprj$y
    abslons <- abs(lons)
    abslats <- abs(lats)
    lons[which(abslons < 1e+20)] <- NA
    lats[which(abslats < 1e+20)] <- NA

  } else if (gribMessage$gridType == "equatorial_azimuthal_equidistant") {

    lat1 <- gribMessage$latitudeOfFirstGridPointInDegrees
    lon1 <- gribMessage$longitudeOfFirstGridPointInDegrees
    dx <- gribMessage$Dx
    dy <- gribMessage$Dy
    prj <- proj4::project(list(lon1, lat1), grib_proj4str(gribMessage))
    llcx <- prj$x
    llcy <- prj$y

    if (has.key(gribMessage, "Nx") && has.key(gribMessage, "Ny")) {
      nx <- gribMessage$Nx
      ny <- gribMessage$Ny
    } else {
      nx <- gribMessage$Ni
      ny <- gribMessage$Nj
    }

    x <- llcx + dx * seq_len(nx)
    y <- llcy + dy * seq_len(ny)
    mg <- meshgrid(x, y)
    x <- mg$X
    y <- mg$Y
    unprj <- proj4::project(list(mg$X, mg$Y),
                            grib_proj4str(gribMessage),
                            inverse = TRUE)
    lons <- unprj$x
    lats <- unprj$y

  } else if (gribMessage$gridType == "lambert_azimuthal_equal_area") {

    dx <- gribMessage$Dx / 1e+3
    dy <- gribMessage$Dy / 1e+3
    lon1 <- gribMessage$longitudeOfLastGridPointInDegrees
    lat1 <- gribMessage$latitudeOfFirstGridPointInDegrees
    prj <- proj4::project(list(lon1, lat1), grib_proj4str(gribMessage))
    llcx <- prj$x
    llcy <- prj$y

    if (has.key(gribMessage, "Nx") && has.key(gribMessage, "Ny")) {
      nx <- gribMessage$Nx
      ny <- gribMessage$Ny
    } else {
      nx <- gribMessage$Ni
      ny <- gribMessage$Nj
    }

    x <- llcx + dx * seq_len(nx)
    y <- llcy + dy * seq_len(ny)
    mg <- meshgrid(x, y)
    x <- mg$X
    y <- mg$Y
    unprj <- proj4::project(list(mg$X, mg$Y),
                            grib_proj4str(gribMessage),
                            inverse = TRUE)
    lons <- unprj$x
    lats <- unprj$y

  } else if (gribMessage$gridType == "mercator") {

    scale <- gribMessage$grib2divider
    if (is.null(scale)) {
      scale <- 1000
    } else {
      scale <- as.numeric(scale)
    }

    lon1 <- gribMessage$longitudeOfFirstGridPoint / scale
    lon2 <- gribMessage$longitudeOfLastGridPoint / scale

    if (has.key(gribMessage, "truncateDegrees") &&
        gribMessage$truncateDegrees) {
      lon1 <- integer(lon1)
      lon2 <- integer(lon2)
    }

    lat1 <- gribMessage$latitudeOfFirstGridPoint
    lat2 <- gribMessage$latitudeOfLastGridPoint

    if (has.key(gribMessage, "truncateDegrees") &&
        gribMessage$truncateDegrees) {
      lat1 <- integer(lat1)
      lat2 <- integer(lat2)
    }

    prj <- proj4::project(list(lon1, lat1), grib_proj4str(gribMessage))
    llcx <- prj$x
    llcy <- prj$y
    prj <- proj4::project(list(lon2, lat2), grib_proj4str(gribMessage))
    urcx <- prj$x
    urcy <- prj$y

    if (has.key(gribMessage, "Nx") && has.key(gribMessage, "Ny")) {
      nx <- gribMessage$Nx
      ny <- gribMessage$Ny
    } else {
      nx <- gribMessage$Ni
      ny <- gribMessage$Nj
    }

    dx <- (urcx - llcx) / (nx - 1)
    dy <- (urcy - llcy) / (ny - 1)
    x <- llcx + dx * seq_len(nx)
    y <- llcy + dy * seq_len(ny)
    mg <- meshgrid(x, y)
    x <- mg$X
    y <- mg$Y
    unprj <- proj4::project(list(mg$X, mg$Y),
                            grib_proj4str(gribMessage),
                            inverse = TRUE)
    lons <- unprj$x
    lats <- unprj$y

  } else {
    stop("unsupported grid type: ", gribMessage$gridType)
  }

  if (expand) {
    lats <- matrix(lats, ny, nx, byrow = gribMessage$jPointsAreConsecutive)
    lons <- matrix(lons, ny, nx)
  }
  list(lats = lats, lons = lons)
}
