AC_INIT([gribr], 1.1.4, [nawendt@ou.edu])

m4_include([m4/gribr_link_grib_api.m4])
m4_include([m4/gribr_link_eccodes.m4])

# Set globals
GRIB_API_MIN_VERSION="1.10.0"
GRIB_API_MIN_VERSION_INT=11000
ECCODES_MIN_VERSION="2.0.0"
ECCODES_MIN_VERSION_INT=20000

# Find R_HOME; set CC/CFLAGS
: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi
RBIN="${R_HOME}/bin/R"
CC=`"${RBIN}" CMD config CC`;
CFLAGS=`"${RBIN}" CMD config CFLAGS`

# Compiler checks
AC_PROG_CC_C99
AS_IF([test "x${ac_cv_prog_cc_c99}" = xno],
      [AC_MSG_ERROR([C99 compiler is required])])
AC_LANG([C])

# Check ENV VARS
AC_ARG_VAR(GRIB_API_CFLAGS, [Compiler flags for the GRIB API installation])
AC_ARG_VAR(GRIB_API_LDFLAGS, [Linker flags for the GRIB API installation])

# Check for system headers
AC_HEADER_STDC
AC_CHECK_HEADERS([ctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Check for pkg-config
AC_PATH_PROG([PKGCONF],[pkg-config],[],[$PATH:/usr/local/bin:ext/bin:ext:/sw/bin:/opt/bin])

AC_ARG_WITH([grib-api], [AS_HELP_STRING([--with-grib-api], 
    [use stand-alone GRIB API library])], [], [with_grib_api=no])

if test "x$with_grib_api" != "xno"; then
    GRIBR_LINK_GRIB_API
else
    GRIBR_LINK_ECCODES
fi

# Summary
echo "==================== GRIBR CONFIGURE SUMMARY ===================="
echo ""
echo "LINKED LIBRARY: $linked_lib"
echo ""
echo "COMPILER/LINKER OPTIONS:"
echo "PKG_LIBS   = ${LIBS}"
echo "PKG_CFLAGS = ${CFLAGS}"
echo ""
echo "================================================================="

AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
