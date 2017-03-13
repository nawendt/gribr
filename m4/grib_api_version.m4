AC_DEFUN([LIBGRIB_VERSION_TEST],
[
AC_MSG_CHECKING([for minimum GRIB API version])

cat > grib_api_version.c << EOF
#include <grib_api.h>
#define MIN_VERSION ${GRIB_API_MIN_VERSION_INT}

int main(void) {
  long version;

  version = grib_get_api_version();

  if (version < MIN_VERSION) {
  	return 1;
  }
  
  return 0;
}
EOF

if test "X$1" = "X"; then
  $CC grib_api_version.c -lgrib_api -o grib_api_version
else
  $CC grib_api_version.c $1 $2 -o grib_api_version
fi

./grib_api_version

result=$?

if test  $result = 0; then
    good_version="yes"
	AC_MSG_RESULT([yes])
else
    good_version="no"
	AC_MSG_RESULT([no])
fi

])