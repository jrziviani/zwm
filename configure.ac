AC_INIT([ZWM Window Manager], [0.1])

AM_INIT_AUTOMAKE([1.9])

AC_PROG_CXX
AC_LANG_PUSH([C++])

AC_HEADER_STDC
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])

ZWM_INCLUDES=""
ZWM_LIBS=""

AC_CHECK_LIB([X11], [XOpenDisplay], [], [
         echo "X11 lib is required"
         exit -1])

ZWM_LIBS="$ZWM_LIBS -lX11"

AC_CHECK_LIB([Xft], [XftDrawCreate], [], [
         echo "Xft lib is required"
         exit -1])

ZWM_LIBS="$ZWM_LIBS -lXft"

AC_CHECK_LIB([freetype], [FT_Init_FreeType], [], [
         echo "Freetype2 lib is required"
         exit -1])

ZWM_INCLUDES="$ZWM_INCLUDE $(freetype-config --cflags)"
ZWM_LIBS="$ZWM_LIBS $(freetype-config --libs)"

AC_SUBST(ZWM_INCLUDES)
AC_SUBST(ZWM_LIBS)

AC_TYPE_SIZE_T
AC_TYPE_PID_T

AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
