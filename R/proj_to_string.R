proj_to_string <- function(proj4list, userProj4 = NULL) {

  if (!is.null(userProj4)) {
    # add in user params and overwrite if
    # the params already exist in the proj4
    # list given in the GRIB file
    for (name in names(userProj4)) {
      proj4list[[name]] <- userProj4[[name]]
    }
  }

  paste(sprintf("+%s=%s", names(proj4list), proj4list), collapse = " ")
}
