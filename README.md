# gribr
GRIB interface for R using the [ECMWF GRIB API](https://software.ecmwf.int/wiki/display/GRIB) or the [ECMWF ecCodes package](https://software.ecmwf.int/wiki/display/ECC).

[![Travis-CI Build Status](https://travis-ci.org/nawendt/gribr.svg?branch=master)](https://travis-ci.org/nawendt/gribr)

### Purpose
This project was started to address a need to easily read GRIB data into R as has been done in other languages (e.g., Python). Working on this also presents a great opportunity to learn more about R, C, and the interface between the two.

### Status
gribr is functional at this point in time. Several functions exist for reading GRIB messages and extracting them. Functions are also available to help with projecting the data. All of the documentation has been written and some basic examples exist to get you started. I have yet to develop good plotting examples.

As ECMWF continues to develop their ecCodes package, their plan is to phase out the GRIB API in favor of ecCodes. They have "strongly advised" users to make the transition to ecCodes as it is now production-ready. The plan for gribr is to support both the GRIB API and ecCodes until the GRIB API is deprecated by ECMWF. For now, both libraries are virtually identical in terms of GRIB functionality and should not require me to select one over the other. I will update you on this issue as need be.

### Installation
**NOTE:** This is not _natively_ available for Windows at this time due to difficulties compiling the GRIB API on that platform. See below for details.

1. Prerequisites
  * gribr depends on the ECMWF GRIB API (>= 1.10.0), or the ECMWF ecCodes package (>=2.0.0), and the proj4 R package
  * The GRIB API can be easily installed via a package manager on Linux (`apt-get`, `yum`, etc.) or MacOS (`port`, `fink`, `brew`). This would be the recommened option. Since some repositories carry versions that are too old, you may have to install from source.
    * To install from source, download the GRIB API [here](https://software.ecmwf.int/wiki/display/GRIB/Releases)
  * To use ecCodes you will have to install from source. It is not available from any package management system as of this writing. Get it [here](https://software.ecmwf.int/wiki/display/ECC/Releases)
2. Set up environment (if necessary)
  * __NOTE__: Because the ecCodes GRIB libraries have the same name as those in the GRIB API, the same environmental variables below can be used.
  * _GRIB API installed in a system location_: The the libraries/headers should be found by the linker/compiler without any additional environmental settings.
  * _GRIB API installed in a non-system/user location_: When the GRIB API is installed in a non-system location (i.e., a path that is not in the `ld` search paths), there are some extra steps that need to be taken depending on your use case:
    * _R_: Prior to running `R CMD INSTALL`, set the `GRIB_API_LDFLAGS` and `GRIB_API_CFLAGS` environment variables to the GRIB API library and header directories, respectively. The configure script should pick up on these variables and build the `PKG_LIBS` and `PKG_CFLAGS` for gribr appropriately.

    * _RStudio_: RStudio does not grab the envionment variables that you set on the terminal or in your shell login scripts. Instead, use .Renviron. If you do not have this file, create it in your home directory (this is where R searches for it by default). Setting `GRIB_API_LDFLAGS` and `GRIB_API_CFLAGS` will cause RStudio to load them into the session and allow the configure script to use them.
  * __NOTE__: When linking to the GRIB API from a non-system location, be sure to include the `GRIB_API_LDFLAGS` variable in your `LD_LIBRARY_PATH` environment variable. R will not otherwise be able to load the gribr shared object as it will need to know where the linked GRIB API functions are. One option to avoid setting/altering `LD_LIBRARY_PATH` is to use `-Wl,-rpath=$GRIB_API_LDFLAGS` in the compiler flags. I have had some luck with this using `gcc`.
3. Install gribr
  * _From the command line_:
   ```shell
   R CMD INSTALL /path/to/gribr --configure-vars="GRIB_API_CFLAGS=-I/path/to/grib_api/include GRIB_API_LDFLAGS='-L/path/to/grib_api/lib -lgrib_api'"
   ```
  * _From within R or RStudio_:
   ```R
   install.packages("path/to/cloned/gribr/repo", repos = NULL, configure.vars = c("GRIB_API_LDFLAGS='-L/path/to/grib_api/lib -lgrib_api'", "GRIB_API_CFLAGS=-I/path/to/grib_api/include"))
   ```

### Windows Install Options
While a native Windows GRIB API library that will work with R is not available, there are ways to run gribr on Windows. The first option is to use the [Windows 10 Linux Subsystem](https://msdn.microsoft.com/en-us/commandline/wsl/install_guide). Simply follow the [instructions](https://cran.r-project.org/bin/linux/ubuntu/) to install the latest R as you would on any other Linux system. Any other necessary libraries should be available on the package manager. Build gribr as usual and enjoy using it on Windows.

The second option is to use gribr via [Cygwin](https://cygwin.org/). A Cygwin build of R is available through their package manager. Simply make sure to also install the dev versions of the libraries that you will need. This will likely include libraries that the R package proj4 depends on. You will have to install GRIB API from source for this. From there, install gribr from the R CMD taking care to include all the correct configure-vars. If you build the GRIB API with PNG or JPEG support, you'll need to add the appropriate linker flags.

### Contributing
You are welcome to contribute to this project. At some point in the future I will put together some more formal guidelines so as to keep this project as uniform as possible. For now, simply take advantage of the features of git and github and sumbit pull requests for anything you would like to contribute.

__Current Needs__: While all improvements are welcome, here are a few specific needs:
  * Build GRIB API on Windows to get gribr ported to that platform
  * Plotting wrapper functions to give some quick/easy plotting functionality to the package

####Contact
[Nathan Wendt](mailto:nawendt@ou.edu)

