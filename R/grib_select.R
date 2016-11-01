#' Select multiple GRIB messages
#'
#' \code{grib_select} selects all GRIB messages that match key/value pairs input
#' by the user.
#'
#' \code{grib_select} is a function more robust than \code{grib_get_message} in
#' that it can search through a GRIB file and get all matching messages given
#' key(s)/value(s) pairs. This is achieved by making an index of the input GRIB
#' file and then selecting keys that match user input in \code{keyPairs}. The
#' values returned in the GRIB message are masked if they are coded as a missing
#' value or the bitmap, if present, masks them.
#'
#' All grid (latitude, longitude, values, etc.) are output as vectors for
#' simplicity and flexibility. Many plotting functions want input in
#' \code{matrix} form (e.g., \code{image()}, \code{fields::image.plot()} while
#' others want input in long form \code{data.frame}s or vectors (e.g.,
#' \code{ggplot}). Keeping vector form allows the most flexibility for the user
#' to do what is needed to format the data appropriately. Some helper functions
#' in this package will do some data formatting for you.
#'
#' @param gribObj \code{GRIB} class object.
#' @param keyPairs a named \code{list} of key/value pairs that will be used to
#'   search through and match messages in the GRIB file using an index.
#'
#' @return Returns a \code{gribMessage} or a list of \code{gribMessage} objects
#'   if more than one message is selected.
#'
#' @seealso \code{\link{grib_get_message}} \code{\link{grib_list}}
#'   \code{\link{grib_expand_grids}} \code{\link{grib_latlons}}
#'
#' @export
#'
#' @examples
#' g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
#' # select all mixing ratio messages
#' gm1 <- grib_select(g, list(shortName = "q"))
#' # select mixing ratio, but only at 1000 and 850 hPa
#' gm2 <- grib_select(g, list(shortName = "q", level = c(1000, 850)))
#' grib_close(g)

grib_select <- function(gribObj, keyPairs) {

  if (!is.grib(gribObj)) {
    stop("gribObj is not of class 'grib'")
  }

  if (is.null.externalptr(gribObj$handle)) {
    stop("GRIB object is closed or unavailable")
  }

  if (!is.list(keyPairs)) {
    stop("keyPairs must be a list")
  }

  if (length(keyPairs) > 1) {
    keyCombo <- expand.grid(keyPairs, KEEP.OUT.ATTRS = FALSE, stringsAsFactors = FALSE)
    keyList <- lapply(1:dim(keyCombo)[1],function(i) as.list(keyCombo[i,]))
  } else if (length(keyPairs) == 1){
    # needs to be list of lists to be handled correctly
    # in the c routine that grabs the messages
    # Solution below is a bit hacky, but it works
    keyList <- lapply(keyPairs[[1]],function(x) {names(x) <- names(keyPairs); as.list(x)})
  } else {
    stop("error with keyPairs input")
  }

  selected <- .Call("gribr_select", gribObj$file, keyList,
                    gribObj$isMultiMessage)

  # cleans up output so that each list element is
  # a grib message
  selected <- unlist(selected, recursive = FALSE)

  if (length(selected) < 1) {
    stop("No messages matched keyPairs")
  }

  # Just return the gribMessage if only one
  if (length(selected) == 1) {
    selected <- selected[[1]]
  }

  selected
}
