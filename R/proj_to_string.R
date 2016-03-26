proj_to_string <- function(proj4list, userProj4 = NULL) {

  if (!is.null(userProj4)) {
    proj4list <- c(proj4list, userProj4)
  }

  paste(sprintf("+%s=%s",names(proj4list), proj4list), collapse = " ")
}
