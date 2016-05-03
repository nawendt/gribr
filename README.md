# gribr
GRIB interface for R using the [ECMWF GRIB API](http://software.ecmwf.int/wiki/display/GRIB/Home)

####Purpose
This project was started to address a need to easily read GRIB data into R as has been done in other languages (e.g., Python). Working on this also presents a great opportunity to learn more about R, C, and the interface between the two.

####Status
gribr is functional at this point in time. Several functions exist for reading GRIB messages and extracting them. Functions are also available to help with projecting the data. Most of the documentation has been written, but examples have not been incorporated just yet.

####Installation
**NOTE:** This is not available for Windows at this time.
* Prerequisites
  * gribr depends on the ECMWF GRIB API (>= 1.10.0) and the proj4 R package
  * The GRIB API can be easily installed via a package manager on Linux (apt-get, yum, etc.) or MacOS (port, fink, brew). This would be the recommened option. Since some repositories carry versions that are too old, you may have to install from source.
    * To install from source, download the GRIB API [here](https://software.ecmwf.int/wiki/display/GRIB/Releases)
* Set up environment (if necessary)
  * If you are able to install from your system's package manager, this step is likely unnecessary.
* Install gribr

####Contact
[Nathan Wendt](mailto:nawendt@ou.edu)

