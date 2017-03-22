AC_DEFUN([LIBECCODES_VERSION_TEST],
[
AC_MSG_CHECKING([for minimum eccodes version])

cat > eccodes_version.c << EOF
#include <eccodes.h>
#define MIN_VERSION ${ECCODES_MIN_VERSION_INT}

int main(void) {
  long version;

  version = codes_get_api_version();

  if (version < MIN_VERSION) {
  	return 1;
  }
  
  return 0;
}
EOF

if test "X$1" = "X"; then
  $CC eccodes_version.c -leccodes -o eccodes_version
else
  $CC eccodes_version.c $1 $2 -leccodes -o eccodes_version
fi

./eccodes_version

result=$?

if test  $result = 0; then
    good_version="yes"
	AC_MSG_RESULT([yes])
else
    good_version="no"
	AC_MSG_RESULT([no])
fi

])
