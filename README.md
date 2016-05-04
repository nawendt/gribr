# gribr
GRIB interface for R using the [ECMWF GRIB API](http://software.ecmwf.int/wiki/display/GRIB/Home)

####Purpose
This project was started to address a need to easily read GRIB data into R as has been done in other languages (e.g., Python). Working on this also presents a great opportunity to learn more about R, C, and the interface between the two.

####Status
gribr is functional at this point in time. Several functions exist for reading GRIB messages and extracting them. Functions are also available to help with projecting the data. All of the documentation has been written and some basic examples exist to get you started.

####Installation
**NOTE:** This is not available for Windows at this time.

1. Prerequisites
  * gribr depends on the ECMWF GRIB API (>= 1.10.0) and the proj4 R package
  * The GRIB API can be easily installed via a package manager on Linux (apt-get, yum, etc.) or MacOS (port, fink, brew). This would be the recommened option. Since some repositories carry versions that are too old, you may have to install from source.
    * To install from source, download the GRIB API [here](https://software.ecmwf.int/wiki/display/GRIB/Releases)
2. Set up environment (if necessary)
  * _GRIB API installed in a system location_: The the libraries/headers should be found by the linker/compiler without any additional environmental settings.
  * _GRIB API installed in a non-system/user location_: When the GRIB API is installed in a non-system location, ...
3. Install gribr

####Contact
[Nathan Wendt](mailto:nawendt@ou.edu)

