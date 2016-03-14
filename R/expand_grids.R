expand_grids <- function(message) {

  # Make sure both Ni/Nx, Nj/Ny are in list
  # so scripting is easier across files
  if (is.null(message$Nx)) {
    message <- c(message, Nx = message$Ni)
  } else if(is.null(message$Ni)) {
    message <- c(message, Ni = message$Nx)
  }

  if (is.null(message$Ny)) {
    message <- c(message, Ny = message$Nj)
  } else if(is.null(message$Nj)) {
    message <- c(message, Nj = message$Ny)
  }

  # Handle matrices here, storage order
  # will be determined by jPointsAreConsecutive key
  listLengths <- vapply(message,length,c(0),USE.NAMES = FALSE)
  loc <- which(listLengths > 1 & listLengths == message$Nx*message$Ny)
  for (key in loc) {
    message[[key]] <- matrix(message[[key]],
                                 message$Nx,
                                 message$Ny,
                                 byrow = message$jPointsAreConsecutive)
  }

  message
}
