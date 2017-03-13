AC_DEFUN([GRIBR_LINK_GRIB_API],[
m4_include([m4/grib_api_version.m4])
# Check for grib_api
# first try pkg-config
found_api="no"
if test "X$GRIB_API_LDFLAGS" = "X" && test "X$GRIB_API_CFLAGS" = "X"; then
    if test "X$PKGCONF" != "X"; then
        AC_MSG_CHECKING([if pkg-config finds grib_api])
        if `"${PKGCONF}" --exists grib_api`; then
            AC_MSG_RESULT([yes])
            if test `"${PKGCONF}" --exists 'grib_api >= ${GRIB_API_MIN_VERSION'`; then
                GRIB_API_VERSION=`"${PKGCONF}" --modversion grib_api`
                AC_MSG_ERROR([Minimum grib_api version is ${GRIB_API_MIN_VERSION}, but found version ${GRIB_API_VERSION}])
            fi
            GRIB_API_CFLAGS=`"${PKGCONF}" --cflags grib_api`
            GRIB_API_LDFLAGS=`"${PKGCONF}" --libs grib_api`
            CFLAGS="${CFLAGS} ${GRIB_API_CFLAGS}"
            LIBS="${GRIB_API_LDFLAGS}"
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
            LIBGRIB_VERSION_TEST
            if test "$good_version" = "yes"; then
                CFLAGS="${CFLAGS}"
                LIBS="-lgrib_api"
            else
                AC_MSG_ERROR([GRIB API version does not meet minimum requirement of ${GRIB_API_MIN_VERSION}])
            fi
        else
            AC_MSG_ERROR([grib_api not found with pkg-config or in system libraries. Please set GRIB_API_LDFLAGS and GRIB_API_CFLAGS environmental variables (or in $HOME/.Renviron) and try again])
        fi
    fi
elif test "X$GRIB_API_LDFLAGS" != "X" && test "X$GRIB_API_CFLAGS" != "X"; then
    AC_MSG_NOTICE([Using GRIB_API_LDFLAGS and GRIB_API_CFLAGS to use grib_api])
    LIBGRIB_VERSION_TEST($GRIB_API_CFLAGS, $GRIB_API_LDFLAGS)
    if test "$good_version" = "yes"; then
        CFLAGS="${CFLAGS} ${GRIB_API_CFLAGS}"
        LIBS="${GRIB_API_LDFLAGS}"
    else
        AC_MSG_ERROR([GRIB API version does not meet minimum requirement of ${GRIB_API_MIN_VERSION}])
    fi
else
    AC_MSG_ERROR([Error finding grip_api])
fi

# Make sure grib_api.h can be found
AC_CHECK_HEADER([grib_api.h], [HAVE_GRIB_HEADERS=TRUE], [HAVE_GRIB_HEADERS=FALSE], [AC_INCLUDES_DEFAULT])

linked_lib="GRIB API (stand-alone)"
])