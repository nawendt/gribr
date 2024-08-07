AC_INIT([gribr],[1.2.7],[nathan.wendt@noaa.gov])

AC_CONFIG_MACRO_DIR([tools/m4])

# Set globals
ECCODES_MIN_VERSION="2.19.0"
ECCODES_MIN_VERSION_INT=21900

# Find R_HOME; set CC/CPPFLAGS
: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi

RBIN="${R_HOME}/bin/R"
CC=`"${RBIN}" CMD config CC`;
CPPFLAGS=`"${RBIN}" CMD config CPPFLAGS`

# Compiler checks
AC_PROG_CC
AS_IF([test "x${ac_cv_prog_cc_c99}" = xno],
      [AC_MSG_ERROR([C99 compiler is required])])
AC_LANG([C])

# Check ENV VARS
AC_ARG_VAR(ECCODES_CPPFLAGS, [Preprocessor flags for the ecCodes installation])
AC_ARG_VAR(ECCODES_LIBS, [Linker flags for the ecCodes installation])

# Check for system headers
AC_CHECK_HEADERS([ctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Check for pkg-config
AC_PATH_PROG([PKGCONF],[pkg-config],[],[$PATH:/usr/local/bin:ext/bin:ext:/sw/bin:/opt/bin])

# Check for non-standard functions
AC_CHECK_FUNC([strlcat], [HAVE_STRLCAT=TRUE], [HAVE_STRLCAT=FALSE])

if test "${HAVE_STRLCAT}" = "TRUE"; then
    CPPFLAGS="${CPPFLAGS} -DNATIVE_STRLCAT"
fi

GRIBR_LINK_ECCODES

# Summary
echo "==================== GRIBR CONFIGURE SUMMARY ===================="
echo ""
echo "COMPILER/LINKER OPTIONS:"
echo "PKG_LIBS   = ${LIBS}"
echo "PKG_CPPFLAGS = ${CPPFLAGS}"
echo ""
echo "================================================================="

AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
