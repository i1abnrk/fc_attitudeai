#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([attitude], [0.0.0])
AC_CONFIG_SRCDIR([attitude/aivariant.c])
AC_CONFIG_HEADERS([aiatt_config.h])

# Checks for programs.
AC_PROG_CC
LT_INIT([shared])
AM_INIT_AUTOMAKE

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_ARG_WITH([freecivbld],
  AS_HELP_STRING([--with-freecivbdl], [path to freeciv build tree]),
            [FCBLDPATH=$withval],
            [FCBLDPATH=".."])
AC_ARG_WITH([freecivsrc],
  AS_HELP_STRING([--with-freecivsrc], [patch to freeciv source tree]),
            [FCSRCPATH=$withval],
            [FCSRCPATH="$FCBLDPATH"])

AC_SUBST([FCBLDPATH])
AC_SUBST([FCSRCPATH])

AC_ARG_WITH([aimoddir],
  AS_HELP_STRING([--with-aimoddir], [freeciv AI module directory]),
            [aimoduledir="$withval"],
            [aimoduledir="${libdir}/fcai"])

AC_SUBST([aimoduledir])

AC_CONFIG_FILES([
        Makefile
        attitude/Makefile])
AC_OUTPUT
