#' Get PROJ4 string for a GRIB file
#'
#' \code{grib_proj4str} generates a PROJ4 string for a given gribMessage class
#' object based on the message's keys and, optionally, user input.
#'
#' \code{grib_proj4str} uses a GRIB message's keys to generate a PROJ4 string on
#' the fly. A user can even input their own PROJ4 elements using the
#' \code{userProj4} arugment. Be aware that any user PROJ4 elements that
#' conflict with already present elements will override the elements extracted
#' from the GRIB file. Several other packages have functions that can use this
#' PROJ4 string to appropriately project the values in the GRIB message. Several
#' common grid types are supported at this time, but likely not all.
#'
#' This function is influenced by the PROJ4 string method used in the pygrib
#' Python module (see
#' \href{https://github.com/jswhit/pygrib/blob/master/pygrib.pyx}{pygrib
#' source}).
#'
#' @param gribMessage \code{gribMessage} class object.
#' @param userProj4 An optional argument that can be a named \code{list} or
#'   named \code{character} vector of additional PROJ4 elements the user would
#'   like to add. User PROJ4 elements take precedence over that given in GRIB
#'   file.
#'
#' @return Returns a \code{character} string of the PROJ4 elements associate
#'   with the \code{gribMessage}.
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' gm <- grib_get_message(g, 1)
#' grib_proj4str(gm)
#'
#' # add proj4 parameters, if needed
#' # overrides proj in this case
#' grib_proj4str(gm, list(lat_0 = 35, lon_0 = -95, proj = "lcc"))
#' grib_close(g)

grib_proj4str <- function(gribMessage, userProj4 = NULL) {

  GRIB_MISSING_LONG <- 4294967295

	if (!is.grib.message(gribMessage)) {
	  stop("input must be of class 'gribMessage'")
	}

  if (!is.null(userProj4)) {
    if (is.null(names(userProj4))) {
      stop("user PROJ.4 input must be named list or named vector")
    }
  }

	proj4list <- list()

	if (has.key(gribMessage, "radius")) {
	  proj4list$a <- gribMessage$radius
	  proj4list$b <- gribMessage$radius
	} else {
	  if (gribMessage$shapeOfTheEarth == 6) {
	    proj4list$a <- 6371229
	    proj4list$b <- 6371229
	  } else if (gribMessage$shapeOfTheEarth %in% c(3, 7)) {
	    if (has.key(gribMessage, "scaleFactorOfMajorAxisOfOblateSpheroidEarth")) {
	      scaleb <- gribMessage$scaleFactorOfMajorAxisOfOblateSpheroidEarth
	      scalea <- gribMessage$scaleFactorOfMajorAxisOfOblateSpheroidEarth
	      if (scalea != GRIB_MISSING_LONG && scaleb != GRIB_MISSING_LONG) {
	        scalea <- 10^(-scalea)
	        if (gribMessage$shapeOfTheEarth == 3) {
	          scalea <- 1000 * scalea
	        }
	      } else {
	        scalea <- 1
	      }

	      if (scalea != GRIB_MISSING_LONG && scaleb != GRIB_MISSING_LONG) {
	        scaleb <- 10^(-scaleb)
	        if (gribMessage$shapeOfTheEarth == 3) {
	          scaleb <- 1000 * scaleb
	        }
	      } else {
	        scaleb <- 1
	      }
	  } else {
	    scalea <- 1
	    scaleb <- 1
	  }
	    proj4list$a <- gribMessage$scaledValueOfEarthMajorAxis * scalea
	    proj4list$b <- gribMessage$scaledValueOfEarthMajorAxis * scaleb
	  } else if (gribMessage$shapeOfTheEarth == 2) {
	    proj4list$a <- 6378160
	    proj4list$b <- 6356775
	  } else if (gribMessage$shapeOfTheEarth == 1) {
	    if (has.key(gribMessage, "scaleFactorOfRadiusOfSphericalEarth")) {
	      scalea <- gribMessage$scaleFactorOfRadiusOfSphericalEarth
	      if (scalea && scalea != GRIB_MISSING_LONG) {
	        scalea <- 10^(-scalea)
	      } else {
	        scalea <- 1
	      }
	      scaleb <- scalea
	    } else {
	      scalea <- 1
	      scaleb <- 1
	    }
	  } else if (gribMessage$shapeOfTheEarth == 0) {
	    proj4list$a <- 6367470
	    proj4list$b <- 6367470
	  } else if (gribMessage$shapeOfTheEarth == 5) {
	    proj4list$a <- 6378137
	    proj4list$b <- 6356752.3142
	  } else if (gribMessage$shapeOfTheEarth == 8) {
	    proj4list$a <- 6371200
	    proj4list$b <- 6371200
	  } else {
	    stop("unknown shape of the earth")
	  }
	}

	if (gribMessage$gridType %in% c("reduced_gg","reduced_ll","regular_gg","regular_ll")) {
	  proj4list$proj <- "cyl"
	} else if (gribMessage$gridType == "polar_stereographic") {
	  proj4list$proj <- "stere"
	  proj4list$lat_ts <- gribMessage$latitudeWhereDxAndDyAreSpecifiedInDegrees
	  if (has.key(gribMessage, "projectionCentreFlag")) {
	    projectionFlag <- gribMessage$projectionCentreFlag
	  } else if (has.key(gribMessage, "projectionCenterFlag")) {
	    projectionFlag <- gribMessage$projectionCenterFlag
	  } else {
	    stop("cannot find projection center flag")
	  }
	  if (projectionFlag == 0) {
	    proj4list$lat_0 <- 90
	  } else {
	    proj4list$lat_0 <- -90
	  }
	  proj4list$lon_0 <- gribMessage$orientationOfTheGridInDegrees
	} else if (gribMessage$gridType == "lambert") {
	  proj4list$proj <- "lcc"
	  proj4list$lon_0 <- gribMessage$LoVInDegrees
	  proj4list$lat_0 <- gribMessage$LaDInDegrees
	  proj4list$lat_1 <- gribMessage$Latin1InDegrees
	  proj4list$lat_2 <- gribMessage$Latin2InDegrees
	} else if (gribMessage$gridType == "albers") {
	  proj4list$proj <- "aea"
	  scale <- as.numeric(gribMessage$grib2divider)
	  if (gribMessage$truncate) {
	    proj4list$lon_0 <- as.numeric(gribMessage$lon_0)
	    proj4list$lat_0 <- as.numeric(gribMessage$lat_0)
	    proj4list$lat_1 <- as.numeric(gribMessage$lat_1)
	    proj4list$lat_2 <- as.numeric(gribMessage$lat_2)
	  } else {
	    proj4list$lon_0 <- gribMessage$LoV/scale
	    proj4list$lon_0 <- gribMessage$LoD/scale
	    proj4list$lon_0 <- gribMessage$Latin1/scale
	    proj4list$lon_0 <- gribMessage$Latin2/scale
	  }
	} else if (gribMessage$gridType == "space_view") {
	  proj4list$lon_0 <- gribMessage$longitudeOfSubSatellitePointInDegrees
	  proj4list$lat_0 <- gribMessage$latitudeOfSubSatellitePointInDegrees
	  if (proj4list$lat_0 == 0) {
	    proj4list$proj <- "geos"
	  } else {
	    proj4list$proj <- "nsper"
	  }
	  scale <- as.numeric(gribMessage$grib2divider)
	  proj4list$h <- proj4list$a * gribMessage$Nr/scale
	  proj4list$h <- proj4list$h - proj4list$a
	} else if (gribMessage$gridType == "equatorial_azimuthal_equidistant") {
	  proj4list$lat_0 <- gribMessage$standardParallel/1e+6
	  proj4list$lat_0 <- gribMessage$centralLongitude/1e+6
	  proj4list$proj <- "aeqd"
	} else if (gribMessage$gridType == "lambert_azimuthal_equal_area") {
	  proj4list$lat_0 <- gribMessage$standardParallel/1e+6
	  proj4list$lat_0 <- gribMessage$centralLongitude/1e+6
	  proj4list$proj <- "laea"
	} else if (gribMessage$gridType == "mercator") {
	  if (has.key(gribMessage, "grib2divider")) {
	    scale <- gribMessage$grib2divider
	  } else {
	    scale <- 1000
	  }
	  lon1 <- gribMessage$longitudeOfFirstGridPoint/scale
	  lon2 <- gribMessage$longitudeOfLastGridPoint/scale
	  if (has.key(gribMessage, "truncateDegrees")) {
	    lon1 <- as.numeric(lon1)
	    lon2 <- as.numeric(lon2)
	  }
	  if (has.key(gribMessage, "LaD")) {
	    proj4list$lat_ts <- gribMessage$LaD/scale
	  } else {
	    proj4list$lat_ts <- gribMessage$Latin/scale
	  }
	  proj4list$lon_0 <- 0.5 * (lon1 + lon2)
	  proj4list$proj <- "merc"
	} else if (gribMessage$gridType %in% c("rotated_ll", "rotated_gg")) {
	  rot_angle <- gribMessage$angleOfRotationInDegrees
	  pole_lat <- gribMessage$latitudeOfSouthernPoleInDegrees
	  pole_lon <- gribMessage$longitudeOfSouthernPoleInDegrees
	  proj4list$o_proj <- "longlat"
	  proj4list$proj <- "ob_tran"
	  proj4list$o_lat_p <- pole_lat
	  proj4list$o_lon_p <- rot_angle
	  proj4list$lon_0 <- pole_lon
	} else {
	  stop("unsupported grid type: ", gribMessage$gridType)
	}
	proj_to_string(proj4list, userProj4)
}
