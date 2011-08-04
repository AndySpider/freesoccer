AC_PREREQ([2.67])
AC_INIT([freesoccer], [0.0.2], http://code.google.com/p/freesoccer/)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION, [dist-bzip2])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([glut],
             [glutKeyboardFunc],
             LIBS="$LIBS -lglut",
             [AC_MSG_ERROR([*** freeglut library not found!])]
             )

AC_CHECK_LIB([GLU],
             [gluPerspective],
             LIBS="$LIBS -lGLU",
             [AC_MSG_ERROR([*** OpenGL utility library not found!])]
             )

# Checks for header files.
AC_CHECK_HEADERS([GL/glut.h netinet/in.h sys/socket.h])

# Checks for library functions.
AC_CHECK_FUNCS([memset socket sqrt])

# Checks for typedefs, structures, and compiler characteristics.
AC_FUNC_MALLOC
AC_HEADER_STDBOOL
AC_TYPE_SSIZE_T

AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 ])

AC_OUTPUT

AC_MSG_RESULT([Configuration successful! Now type 'make'.])

