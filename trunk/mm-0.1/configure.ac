#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([mm], [0.1], [more.andres@gmail.com])
AM_INIT_AUTOMAKE([mm],[0.1])
AC_CONFIG_SRCDIR([src/mm.c])
AC_CONFIG_HEADER([src/config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([stdlib.h sys/time.h syslog.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strncasecmp gettimeofday])

AC_CONFIG_FILES([Makefile src/Makefile t/Makefile doc/Makefile inc/Makefile], [chmod +x t/*.sh])
AC_OUTPUT
