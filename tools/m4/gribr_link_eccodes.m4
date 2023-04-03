AC_DEFUN([GRIBR_LINK_ECCODES],[
# Check for eccodes
# first try pkg-config
found_eccodes="no"
if test "X$ECCODES_LIBS" = "X" && test "X$ECCODES_CPPFLAGS" = "X"; then
    if test "X$PKGCONF" != "X"; then
        AC_MSG_CHECKING([if pkg-config finds eccodes])
        if `"${PKGCONF}" --exists eccodes`; then
            AC_MSG_RESULT([yes])
            if test `"${PKGCONF}" --exists 'eccodes >= ${ECCODES_MIN_VERSION}'`; then
                ECCODES_VERSION=`"${PKGCONF}" --modversion eccodes`
                AC_MSG_ERROR([Minimum eccodes version is ${ECCODES_MIN_VERSION}, but found version ${ECCODES_VERSION}])
            fi
            ECCODES_CPPFLAGS=`"${PKGCONF}" --cflags eccodes`
            ECCODES_LIBS=`"${PKGCONF}" --libs eccodes`
            CPPFLAGS="${CPPFLAGS} ${ECCODES_CPPFLAGS}"
            LIBS="${ECCODES_LIBS}"
            found_eccodes="yes"
        else
            AC_MSG_RESULT([no])
        fi
    fi

    # then try system library paths
    if test "$found_eccodes" = "no"; then
        AC_MSG_NOTICE([Checking if eccodes is in a system location])
        AC_CHECK_LIB(eccodes, codes_handle_new, [HAVE_ECCODES=TRUE], [HAVE_ECCODES=FALSE])

        if test "${HAVE_ECCODES}" = "TRUE"; then
            LIBECCODES_VERSION_TEST
            if test "$good_version" = "yes"; then
                CPPFLAGS="${CPPFLAGS}"
                LIBS="-leccodes"
            else
                AC_MSG_ERROR([eccodes version does not meet minimum requirement of ${ECCODES_MIN_VERSION}])
            fi
        else
            AC_MSG_ERROR([eccodes not found with pkg-config or in system libraries. Please set ECCODES_LIBS and ECCODES_CPPFLAGS environmental variables (or in $HOME/.Renviron) and try again])
        fi
    fi
elif test "X$ECCODES_LIBS" != "X" && test "X$ECCODES_CPPFLAGS" != "X"; then
    AC_MSG_NOTICE([Using ECCODES_LIBS and ECCODES_CPPFLAGS to use eccodes])
    LIBECCODES_VERSION_TEST($ECCODES_CPPFLAGS, $ECCODES_LIBS)
    if test "$good_version" = "yes"; then
        CPPFLAGS="${CPPFLAGS} ${ECCODES_CPPFLAGS}"
        LIBS="${ECCODES_LIBS} -leccodes"
    else
        AC_MSG_ERROR([eccodes version does not meet minimum requirement of ${ECCODES_MIN_VERSION}])
    fi
else
    AC_MSG_ERROR([Error finding eccodes])
fi

# Make sure eccodes.h can be found
AC_CHECK_HEADER([eccodes.h], [HAVE_ECCODES_HEADERS=TRUE], [HAVE_ECCODES_HEADERS=FALSE], [AC_INCLUDES_DEFAULT])
])
