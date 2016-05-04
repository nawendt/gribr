# gribr
GRIB interface for R using the [ECMWF GRIB API](http://software.ecmwf.int/wiki/display/GRIB/Home)

#### Purpose
This project was started to address a need to easily read GRIB data into R as has been done in other languages (e.g., Python). Working on this also presents a great opportunity to learn more about R, C, and the interface between the two.

#### Status
gribr is functional at this point in time. Several functions exist for reading GRIB messages and extracting them. Functions are also available to help with projecting the data. All of the documentation has been written and some basic examples exist to get you started.

#### Installation
**NOTE:** This is not available for Windows at this time.

1. Prerequisites
  * gribr depends on the ECMWF GRIB API (>= 1.10.0) and the proj4 R package
  * The GRIB API can be easily installed via a package manager on Linux (`apt-get`, `yum`, etc.) or MacOS (`port`, `fink`, `brew`). This would be the recommened option. Since some repositories carry versions that are too old, you may have to install from source.
    * To install from source, download the GRIB API [here](https://software.ecmwf.int/wiki/display/GRIB/Releases)
2. Set up environment (if necessary)
  * _GRIB API installed in a system location_: The the libraries/headers should be found by the linker/compiler without any additional environmental settings.
  * _GRIB API installed in a non-system/user location_: When the GRIB API is installed in a non-system location (i.e., a path that is not in the `ld` search paths), there are some extra steps that need to be taken depending on your use case:
    * _R_: Prior to running `R CMD INSTALL`, set the `GRIB_API_LIB` and `GRIB_API_INC` environment variables to the GRIB API library and header directories, respectively. The configure script should pick up on these variables and build the `PKG_LIBS` and `PKG_CFLAGS` for gribr appropriately.
    * _RStudio_: RStudio does not grab the envionment variables that you set on the terminal or in your shell login scripts. Instead, use .Renviron. If you do not have this file, create it in your home directory (this is where R searches for it by default). Setting `GRIB_API_LIB` and `GRIB_API_INC` will cause RStudio to load them into the session and allow the configure script to use them.
  * When linking to the GRIB API from a non-system location, be sure to include the `GRIB_API_LIB` variable in your `LD_LIBRARY_PATH` environment variable. R will otherwise not be able to load the gribr shared object as it will need to know where the linked GRIB API functions are. One option to avoid setting/altering `LD_LIBRARY_PATH` is to use `-Wl,-rpath=$GRIB_API_LIB` in the compiler flags. I have had some luck with this using `gcc`.
3. Install gribr
  *Run `R CMD INSTALL gribr` on the command line after downloading from this repository

#### Contributing
Coming soon...

####Contact
[Nathan Wendt](mailto:nawendt@ou.edu)

