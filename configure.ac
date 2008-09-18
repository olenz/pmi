AC_INIT([pmi],[0.9])

AC_CONFIG_MACRO_DIR(build-aux/macros)
AC_CONFIG_AUX_DIR(build-aux)
AC_CONFIG_SRCDIR([pmi/pmi.hpp])
AM_INIT_AUTOMAKE(1.10)

AXES_IMPORT_EXTRA_PREFIXES

AC_PROG_CXX
AC_LANG([C++])

AC_PROG_LIBTOOL

AXES_MPI
if test "x$axes_cv_mpi" = "xno"; then
   AC_MSG_ERROR([could not compile an MPI program])
fi

AXES_BOOST_BASE([1.35])
if test "x$axes_cv_boost" = "xno"; then
    AC_MSG_ERROR([could not find the boost headers])
fi

AXES_BOOST_SERIALIZATION
if test "x$axes_cv_boost_lib_serialization" = "xno"; then
    AC_MSG_ERROR([could not find the boost serialization library])
fi

AXES_BOOST_MPI
if test "x$axes_cv_boost_mpi" = "xno"; then
  if test "x$axes_cv_boost_mpi_links" = "xno"; then
      AC_MSG_ERROR([could not link against the boost MPI library.
Are you sure you are using the same MPI compiler/library that was used to
compile the boost mpi library?])
  else
      AC_MSG_ERROR([could not find the boost MPI library])
  fi
fi

AXES_LOG4ESPP

CPPFLAGS='-I $(top_srcdir) '"$BOOST_CPPFLAGS $CPPFLAGS"
LIBS="$LOG4ESPP_LIBS $BOOST_MPI_LIBS $LIBS"

AC_CONFIG_HEADERS([acconfig.hpp])
AC_CONFIG_FILES([Makefile \
		pmi/Makefile \
		pmi/mpitransmit/Makefile \
		examples/Makefile \
		examples/0_basic/Makefile \
		])

AC_OUTPUT
	
