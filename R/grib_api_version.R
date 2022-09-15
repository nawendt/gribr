#' Print ecCodes version
#'
#' \code{grib_api_version} prints the ecCodes library version.
#'
#' @export
#' 
#' @return Returns a string of the format MAJOR.MINOR.REVISION.
#'
#' @examples
#' grib_api_version()

grib_api_version <- function() {
    .Call("gribr_api_version")
}
