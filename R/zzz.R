.onLoad <- function(library, package) {
	if(.Platform$OS.type == "windows") {
		if (Sys.getenv("GRIB_DEFINITION_PATH") == "") {
			Sys.setenv(GRIB_DEFINITION_PATH = "")
		}
	}
}