AC_DEFUN([GRIBR_LINK_ECCODES],[
m4_include([m4/eccodes_version.m4])
# Check for grib_api
# first try pkg-config
found_api="no"
if test "X$GRIB_API_LDFLAGS" = "X" && test "X$GRIB_API_CFLAGS" = "X"; then
    if test "X$PKGCONF" != "X"; then
        AC_MSG_CHECKING([if pkg-config finds eccodes])
        if `"${PKGCONF}" --exists eccodes`; then
            AC_MSG_RESULT([yes])
            if test `"${PKGCONF}" --exists 'eccodes >= ${ECCODES_MIN_VERSION}'`; then
                ECCODES_VERSION=`"${PKGCONF}" --modversion eccodes`
                AC_MSG_ERROR([Minimum eccodes version is ${ECCODES_MIN_VERSION}, but found version ${ECCODES_VERSION}])
            fi
            GRIB_API_CFLAGS=`"${PKGCONF}" --cflags eccodes`
            GRIB_API_LDFLAGS=`"${PKGCONF}" --libs eccodes`
            CFLAGS="${CFLAGS} ${GRIB_API_CFLAGS}"
            LIBS="${GRIB_API_LDFLAGS}"
            found_api="yes"
        else
            AC_MSG_RESULT([no])
        fi
    fi

    # then try system library paths
    if test "$found_api" = "no"; then
        AC_MSG_NOTICE([Checking if eccodes is in a system location])
        AC_CHECK_LIB(eccodes, grib_handle_new, [HAVE_GRIB_API=TRUE], [HAVE_GRIB_API=FALSE])

        if test "${HAVE_GRIB_API}" = "TRUE"; then
            LIBGRIB_VERSION_TEST
            if test "$good_version" = "yes"; then
                CFLAGS="${CFLAGS}"
                LIBS="-lgrib_api"
            else
                AC_MSG_ERROR([eccodes version does not meet minimum requirement of ${ECCODES_MIN_VERSION}])
            fi
        else
            AC_MSG_ERROR([eccodes not found with pkg-config or in system libraries. Please set GRIB_API_LDFLAGS and GRIB_API_CFLAGS environmental variables (or in $HOME/.Renviron) and try again])
        fi
    fi
elif test "X$GRIB_API_LDFLAGS" != "X" && test "X$GRIB_API_CFLAGS" != "X"; then
    AC_MSG_NOTICE([Using GRIB_API_LDFLAGS and GRIB_API_CFLAGS to use eccodes])
    LIBECCODES_VERSION_TEST($GRIB_API_CFLAGS, $GRIB_API_LDFLAGS)
    if test "$good_version" = "yes"; then
        CFLAGS="${CFLAGS} ${GRIB_API_CFLAGS}"
        LIBS="${GRIB_API_LDFLAGS}"
    else
        AC_MSG_ERROR([eccodes version does not meet minimum requirement of ${ECCODES_MIN_VERSION}])
    fi
else
    AC_MSG_ERROR([Error finding eccodes])
fi

# Make sure grib_api.h can be found
AC_CHECK_HEADER([grib_api.h], [HAVE_GRIB_HEADERS=TRUE], [HAVE_GRIB_HEADERS=FALSE], [AC_INCLUDES_DEFAULT])

linked_lib="eccodes"
])