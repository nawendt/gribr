.onLoad <- function(library, package) {
	# Windows will be binary distributions most of the time
	# Need to set GRIB_DEFINITION_PATH to avoid crash if it
	# is not already set
	if(.Platform$OS.type == "windows") {
		if (Sys.getenv("GRIB_DEFINITION_PATH") == "") {
			normPath <- normalizePath(system.file("extdata", "definitions", package = "gribr"), mustWork = FALSE)
			if (normPath == "") {
				warning("R was unable to set GRIB_DEFINITION_PATH. Please set manually.")
			} else {
				Sys.setenv(GRIB_DEFINITION_PATH = normPath)
				Sys.setenv(GRIBR_MOD = 1)
			}
		}
	}
}

.onUnload <- function(libpath) {
	# Clean up if .onLoad made mods to env
	if(.Platform$OS.type == "windows") {
		if (Sys.getenv(GRIBR_MOD) == 1) {
			Sys.unsetenv("GRIB_DEFINITION_PATH")
			Sys.unsetenv("GRIBR_MOD")
		}
	}

	library.dynam.unload("gribr", libpath)
}