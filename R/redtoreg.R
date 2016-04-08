redtoreg <- function(nlons, lonsperlat, reducedGrid) {

  if (is.matrix(reducedGrid)) {
    reducedGrid <- as.numeric(reducedGrid)
  }

  .Call("rgrib_redtoreg", nlons, lonsperlat, reducedGrid)
}
