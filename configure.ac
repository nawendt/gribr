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
# first try pkg-config
found_api="no"
if test "X$GRIB_API_LDFLAGS" = "X" && test "X$GRIB_API_CFLAGS" = "X"; then
    if test "X$PKGCONF" != "X"; then
        AC_MSG_CHECKING([if pkg-config finds grib_api])
        if `"${PKGCONF}" --exists grib_api`; then
            AC_MSG_RESULT([yes])
            if test `"${PKGCONF}" --exists 'grib_api >= 1.10.0'`; then
                GRIB_API_VERSION=`"${PKGCONF}" --modversion grib_api`
                AC_MSG_ERROR([Minimum grib_api version is ${GRIB_API_MIN_VERSION}, but found version ${GRIB_API_VERSION}])
            fi
            GRIB_API_CFLAGS=`"${PKGCONF}" --cflags grib_api`
            GRIB_API_LDFLAGS=`"${PKGCONF}" --libs grib_api`
            CFLAGS="${CFLAGS} ${GRIB_API_CFLAGS}"
            LIBS="${GRIB_API_LDFLAGS}"
            AC_SUBST(LIBS)
            AC_SUBST(CFLAGS)
            found_api="yes"
        else
            AC_MSG_RESULT([no])
        fi
    fi

    # then try system library paths
    if test "$found_api" = "no"; then
        AC_MSG_NOTICE([Checking if grib_api is in a system location])
        AC_CHECK_LIB(grib_api, grib_handle_new, [HAVE_GRIB_API=TRUE], [HAVE_GRIB_API=FALSE])

        if test "${HAVE_GRIB_API}" = "TRUE"; then
            CFLAGS="${CFLAGS}"
            LIBS="-lgrib_api"
            AC_SUBST(LIBS)
            AC_SUBST(CFLAGS)
        else
            AC_MSG_ERROR([grib_api not found with pkg-config or in system libraries. Please set GRIB_API_LIB and GRIB_API_INC environmental variables and try again])
        fi
    fi
elif test "X$GRIB_API_LDFLAGS" != "X" && test "X$GRIB_API_CFLAGS" != "X"; then
    AC_MSG_NOTICE([Using GRIB_API_LDFLAGS and GRIB_API_CFLAGS to use grib_api])
    CFLAGS="${CFLAGS} ${GRIB_API_CFLAGS}"
    LIBS="${GRIB_API_LDFLAGS}"
    AC_SUBST(LIBS)
    AC_SUBST(CFLAGS)
else
    AC_MSG_ERROR([Error finding grip_api])
fi


# Make sure grib_api.h can be found
AC_CHECK_HEADER([grib_api.h], [HAVE_GRIB_HEADERS=TRUE], [HAVE_GRIB_HEADERS=FALSE], [AC_INCLUDES_DEFAULT])


# Summary
echo "==================== GRIBR CONFIGURE SUMMARY ===================="
echo ""
echo "COMPILER/LINKER OPTIONS:"
echo "PKG_LIBS   = ${LIBS}"
echo "PKG_CFLAGS = ${CFLAGS}"
echo ""
echo "================================================================="

AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT