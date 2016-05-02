AC_INIT([gribr], 1.0, [nawendt@ou.edu])

# Set globals
GRIB_API_MIN_VERSION="1.10.0"

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
AC_PROG_CC
AC_PROG_CC_C99

# Check for system headers
AC_HEADER_STDC
AC_CHECK_HEADERS([ctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Check for pkg-config
AC_PATH_PROG([PKGCONF],[pkg-config],[],[$PATH:/usr/local/bin:ext/bin:ext:/sw/bin:/opt/bin])

# Check for grib_api
if test "X$GRIB_API_LIB" = "X" && test "X$GRIB_API_INC" = "X"; then
    if test "X$PKGCONF" != "X"; then
        AC_MSG_CHECKING([if pkg-config finds grib_api])
        if `"${PKGCONF}" --exists grib_api`; then
            AC_MSG_RESULT([yes])
            if test `"${PKGCONF}" --exists 'grib_api >= 1.10.0'`; then
                GRIB_API_VERSION=`"${PKGCONF}" --modversion grib_api`
                AC_MSG_ERROR([Minimum grib_api version is ${GRIB_API_MIN_VERSION}, but found version ${GRIB_API_VERSION}])
            fi
            GRIB_API_CFLAGS=`"${PKGCONF}" --cflags grib_api`
            GRIB_API_LIBS=`"${PKGCONF}" --libs grib_api`
            GRIBR_CFLAGS="${CFLAGS} ${GRIB_API_CFLAGS}"
            GRIBR_LIBS="${GRIB_API_LIBS}"
            AC_SUBST(GRIBR_LIBS)
            AC_SUBST(GRIBR_CFLAGS)
        else
            AC_MSG_RESULT([no])
        fi
    else
        AC_MSG_NOTICE([Checking if grib_api is in a system location])
        AC_CHECK_LIB(grib_api, grib_new_handle, [HAVE_GRIB_API=TRUE], [HAVE_GRIB_API=FALSE])

        if test "${HAVE_GRIB_API}" = "TRUE"; then
            GRIBR_CFLAGS="${CFLAGS}"
            GRIBR_LIBS="-lgrib_api"
            AC_SUBST(GRIBR_LIBS)
            AC_SUBST(GRIBR_CFLAGS)
        else
            AC_MSG_ERROR([grib_api not found with pkg-config or in system libraries. Please set GRIB_API_LIB and GRIB_API_INC environmental variables and try again.])
        fi
    fi
fi

# Make sure grib_api.h can be found


# Summary
echo "==================== GRIBR CONFIGURE SUMMARY ===================="
echo ""
echo "COMPILER/LINKER OPTIONS:"
echo "PKG_LIBS   = ${GRIBR_LIBS}"
echo "PKG_CFLAGS = ${GRIBR_CFLAGS}"
echo ""
echo "================================================================="

AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT