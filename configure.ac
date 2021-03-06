AC_PREREQ(2.52)
m4_include([version.m4])
AC_INIT(moxi, VERSION_NUMBER, moxi@googlegroups.com)
AC_CANONICAL_SYSTEM
AC_CONFIG_SRCDIR(memcached.c)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_CONFIG_HEADER(libmemcached-0.30/libmemcached/libmemcached_config.h)

AC_PROG_CC
AC_PROG_CXX
AC_DISABLE_SHARED
AM_PROG_LIBTOOL

#shared library versioning
MEMCACHED_LIBRARY_VERSION=2:0:0
#                         | | |
#                  +------+ | +---+
#                  |        |     |
#                 current:revision:age
#                  |        |     |
#                  |        |     +- increment if interfaces have been added
#                  |        |        set to zero if interfaces have been removed or changed
#                  |        +- increment if source code has changed
#                  |           set to zero if current is incremented
#                  +- increment if interfaces have been added, removed or changed
AC_SUBST(MEMCACHED_LIBRARY_VERSION)

dnl **********************************************************************
dnl DETECT_ICC ([ACTION-IF-YES], [ACTION-IF-NO])
dnl
dnl check if this is the Intel ICC compiler, and if so run the ACTION-IF-YES
dnl sets the $ICC variable to "yes" or "no"
dnl **********************************************************************
AC_DEFUN([DETECT_ICC],
[
    ICC="no"
    AC_MSG_CHECKING([for icc in use])
    if test "$GCC" = "yes"; then
       dnl check if this is icc acting as gcc in disguise
       AC_EGREP_CPP([^__INTEL_COMPILER], [__INTEL_COMPILER],
         AC_MSG_RESULT([no])
         [$2],
         AC_MSG_RESULT([yes])
         [$1]
         ICC="yes")
    else
       AC_MSG_RESULT([no])
       [$2]
    fi
])

DETECT_ICC([], [])

dnl **********************************************************************
dnl DETECT_SUNCC ([ACTION-IF-YES], [ACTION-IF-NO])
dnl
dnl check if this is the Sun Studio compiler, and if so run the ACTION-IF-YES
dnl sets the $SUNCC variable to "yes" or "no"
dnl **********************************************************************
AC_DEFUN([DETECT_SUNCC],
[
    SUNCC="no"
    AC_MSG_CHECKING([for Sun cc in use])
    AC_RUN_IFELSE(
      [AC_LANG_PROGRAM([], [dnl
#ifdef __SUNPRO_C
   return 0;
#else
   return 1;
#endif
      ])
    ],[
       AC_MSG_RESULT([yes])
       [$1]
       SUNCC="yes"
    ], [
       AC_MSG_RESULT([no])
       [$2]
    ])
])

DETECT_SUNCC([CFLAGS="-mt $CFLAGS"], [])

if test "$ICC" = "no"; then
   AC_PROG_CC_C99
fi

AM_PROG_CC_C_O
AC_PROG_INSTALL

AC_ARG_ENABLE(dtrace,
  [AS_HELP_STRING([--enable-dtrace],[Enable dtrace probes])])
if test "x$enable_dtrace" = "xyes"; then
  AC_PATH_PROG([DTRACE], [dtrace], "no", [/usr/sbin:$PATH])
  if test "x$DTRACE" != "xno"; then
    AC_DEFINE([ENABLE_DTRACE],1,[Set to nonzero if you want to include DTRACE])
    build_dtrace=yes
    # DTrace on MacOSX does not use -G option
    $DTRACE -G -o conftest.$$ -s memcached_dtrace.d 2>/dev/zero
    if test $? -eq 0
    then
        dtrace_instrument_obj=yes
        rm conftest.$$
    fi

    if test "`which tr`" = "/usr/ucb/tr"; then
        AC_MSG_ERROR([Please remove /usr/ucb from your path. See man standards for more info])
    fi
  else
    AC_MSG_ERROR([Need dtrace binary and OS support.])
  fi
fi

AM_CONDITIONAL([BUILD_DTRACE],[test "$build_dtrace" = "yes"])
AM_CONDITIONAL([DTRACE_INSTRUMENT_OBJ],[test "$dtrace_instrument_obj" = "yes"])

AC_SUBST(DTRACE)
AC_SUBST(DTRACEFLAGS)
AC_SUBST(PROFILER_LDFLAGS)

AC_HEADER_TIME

AC_SUBST(NO_STRICT_ALIASING)

sinclude(libmemcached-0.30/m4/pod2man.m4)
sinclude(libmemcached-0.30/m4/debug.m4)
sinclude(libmemcached-0.30/m4/dtrace.m4)
sinclude(libmemcached-0.30/m4/byteorder.m4)
sinclude(libmemcached-0.30/m4/64bit.m4)
sinclude(libmemcached-0.30/m4/protocol_binary.m4)
sinclude(libmemcached-0.30/m4/memcached.m4)
sinclude(libmemcached-0.30/m4/setsockopt.m4)
sinclude(libmemcached-0.30/m4/hsieh.m4)
sinclude(libmemcached-0.30/m4/util.m4)

AC_ARG_ENABLE(coverage,
  [AS_HELP_STRING([--disable-coverage],[Disable code coverage])])

if test "x$enable_coverage" != "xno"; then
   if test "$ICC" = "yes"
   then
      :
      dnl ICC trying to be gcc, but not well
   elif test "$GCC" = "yes"
   then
      AC_PATH_PROG([PROFILER], [gcov], "no", [$PATH])
      if test "x$PROFILER" != "xno"; then
         PROFILER_FLAGS="-fprofile-arcs -ftest-coverage"
         PROFILER_LDFLAGS="-lgcov"
      fi
   elif test "$SUNCC" = "yes"
   then
      AC_PATH_PROG([PROFILER], [tcov], "no", [$PATH])
      if test "x$PROFILER" != "xno"; then
         PROFILER_FLAGS=-xprofile=tcov
      fi
   fi
fi
AC_SUBST(PROFILER_FLAGS)


AC_ARG_ENABLE(64bit,
  [AS_HELP_STRING([--enable-64bit],[build 64bit version])])
if test "x$enable_64bit" = "xyes"
then
    org_cflags=$CFLAGS
    CFLAGS=-m64
    AC_RUN_IFELSE(
      [AC_LANG_PROGRAM([], [dnl
return sizeof(void*) == 8 ? 0 : 1;
      ])
    ],[
      CFLAGS="-m64 $org_cflags"
    ],[
    AC_MSG_ERROR([Don't know how to build a 64-bit object.])
    ])
fi

useconflate="no"
AC_ARG_WITH(libconflate,
            AS_HELP_STRING([--with-libconflate=PATH],
            [build with libconflate xmpp management]),
            [
              if test "x$withval" != "xno"
                then
                AC_DEFINE([HAVE_CONFLATE_H], 1,
                  [Defined if building with libconflate XMPP management])
                CPPFLAGS="-I$withval $CPPFLAGS"
                LDFLAGS="$LDFLAGS -L$withval "
                LIBS="$LIBS -lconflate"
                useconflate="yes"
              fi
            ])
AM_CONDITIONAL([BUILD_WITH_CONFLATE],[test "$useconflate" = "yes"])


usestrophe="no"
AC_ARG_WITH(libstrophe,
            AS_HELP_STRING([--with-libstrophe=PATH],
            [build with libstrophe xmpp libs]),
            [
              if test "x$withval" != "xno"
                then
                AC_DEFINE([HAVE_STROPHE_H], 1,
                  [Defined if building with libstrophe XMPP client])
                CPPFLAGS="-I$withval $CPPFLAGS"
                LDFLAGS="$LDFLAGS -L$withval -L$withval/expat "
                LIBS="$LIBS -lstrophe -lexpat -lssl"
                usestrophe="yes"
              fi
            ])
AM_CONDITIONAL([BUILD_WITH_STROPHE],[test "$usestrophe" = "yes"])

# Enable Lua extension
dnl Check for lua
AC_MSG_CHECKING(if with lua)
uselua="no"
AC_ARG_WITH(lua,
  AC_HELP_STRING([--with-lua],
	[build with Lua extension]),
	[BUILD_WITH_LUA=$withval],[BUILD_WITH_LUA=yes])

	if test "$BUILD_WITH_LUA" != "no"; then
 		AC_MSG_RESULT($BUILD_WITH_LUA)
 		# try pkgconfig
 		if test "$BUILD_WITH_LUA" = "yes"; then
    		LUAPC=lua
  	else
    		LUAPC=$BUILD_WITH_LUA
  	fi

		PKG_CHECK_MODULES(LUA, $LUAPC >= 5.1,
			[ AC_DEFINE([HAVE_LUA], [1], [liblua])
				AC_DEFINE([HAVE_LUA_H], [1], [lua.h])
			],
			[ PKG_CHECK_MODULES(LUA, lua5.1 >= 5.1,
				[ AC_DEFINE([HAVE_LUA], [1], [liblua])
					AC_DEFINE([HAVE_LUA_H], [1], [lua.h]) ]
				)
			]
			)

		AC_SUBST(LUA_CFLAGS)
		AC_SUBST(LUA_LIBS)

		CFLAGS="$LUA_CFLAGS $CFLAGS"
		LDFLAGS="$LDFLAGS $LUA_LIBS"
		LIBS="$LIBS $LUA_LIBS"

	fi

# lua
AM_CONDITIONAL([BUILD_WITH_LUA],[test "$uselua" = "yes"])


trylibeventdir=""
AC_ARG_WITH(libevent,
       [  --with-libevent=PATH     Specify path to libevent installation ],
       [
                if test "x$withval" != "xno" ; then
                        trylibeventdir=$withval
                fi
       ]
)

dnl ------------------------------------------------------
dnl libevent detection.  swiped from Tor.  modified a bit.

LIBEVENT_URL=http://www.monkey.org/~provos/libevent/

AC_CACHE_CHECK([for libevent directory], ac_cv_libevent_dir, [
  saved_LIBS="$LIBS"
  saved_LDFLAGS="$LDFLAGS"
  saved_CPPFLAGS="$CPPFLAGS"
  le_found=no
  for ledir in $trylibeventdir "" $prefix /usr/local ; do
    LDFLAGS="$saved_LDFLAGS"
    LIBS="$saved_LIBS -levent"

    # Skip the directory if it isn't there.
    if test ! -z "$ledir" -a ! -d "$ledir" ; then
       continue;
    fi
    if test ! -z "$ledir" ; then
      if test -d "$ledir/lib" ; then
        LDFLAGS="-L$ledir/lib $LDFLAGS"
      else
        LDFLAGS="-L$ledir $LDFLAGS"
      fi
      if test -d "$ledir/include" ; then
        CPPFLAGS="-I$ledir/include $CPPFLAGS"
      else
        CPPFLAGS="-I$ledir $CPPFLAGS"
      fi
    fi
    # Can I compile and link it?
    AC_TRY_LINK([#include <sys/time.h>
#include <sys/types.h>
#include <event.h>], [ event_init(); ],
       [ libevent_linked=yes ], [ libevent_linked=no ])
    if test $libevent_linked = yes; then
       if test ! -z "$ledir" ; then
         ac_cv_libevent_dir=$ledir
       else
         ac_cv_libevent_dir="(system)"
       fi
       le_found=yes
       break
    fi
  done
  LIBS="$saved_LIBS"
  LDFLAGS="$saved_LDFLAGS"
  CPPFLAGS="$saved_CPPFLAGS"
  if test $le_found = no ; then
    AC_MSG_ERROR([libevent is required.  You can get it from $LIBEVENT_URL

      If it's already installed, specify its path using --with-libevent=/dir/
])
  fi
])

LIBS="$LIBS -levent"

if test $ac_cv_libevent_dir != "(system)"; then
  if test -d "$ac_cv_libevent_dir/lib" ; then
    LDFLAGS="-L$ac_cv_libevent_dir/lib $LDFLAGS"
    le_libdir="$ac_cv_libevent_dir/lib"
  else
    LDFLAGS="-L$ac_cv_libevent_dir $LDFLAGS"
    le_libdir="$ac_cv_libevent_dir"
  fi
  if test -d "$ac_cv_libevent_dir/include" ; then
    CPPFLAGS="-I$ac_cv_libevent_dir/include $CPPFLAGS"
  else
    CPPFLAGS="-I$ac_cv_libevent_dir $CPPFLAGS"
  fi
fi

dnl ----------------------------------------------------------------------------

AC_SEARCH_LIBS(socket, socket)
AC_SEARCH_LIBS(gethostbyname, nsl)
AC_SEARCH_LIBS(umem_cache_create, umem)

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

AC_HEADER_STDBOOL
AC_C_CONST

dnl From licq: Copyright (c) 2000 Dirk Mueller
dnl Check if the type socklen_t is defined anywhere
AC_DEFUN([AC_C_SOCKLEN_T],
[AC_CACHE_CHECK(for socklen_t, ac_cv_c_socklen_t,
[
  AC_TRY_COMPILE([
    #include <sys/types.h>
    #include <sys/socket.h>
  ],[
    socklen_t foo;
  ],[
    ac_cv_c_socklen_t=yes
  ],[
    ac_cv_c_socklen_t=no
  ])
])
if test $ac_cv_c_socklen_t = no; then
  AC_DEFINE(socklen_t, int, [define to int if socklen_t not available])
fi
])

AC_C_SOCKLEN_T

dnl Check if we're a little-endian or a big-endian system, needed by hash code
AC_DEFUN([AC_C_ENDIAN],
[AC_CACHE_CHECK(for endianness, ac_cv_c_endian,
[
  AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([], [dnl
        long val = 1;
        char *c = (char *) &val;
        exit(*c == 1);
    ])
  ],[
    ac_cv_c_endian=big
  ],[
    ac_cv_c_endian=little
  ])
])
if test $ac_cv_c_endian = big; then
  AC_DEFINE(ENDIAN_BIG, 1, [machine is bigendian])
fi
if test $ac_cv_c_endian = little; then
  AC_DEFINE(ENDIAN_LITTLE, 1, [machine is littleendian])
fi
])

AC_C_ENDIAN

dnl Check whether the user's system supports pthread
AC_SEARCH_LIBS(pthread_create, pthread)
if test "x$ac_cv_search_pthread_create" = "xno"; then
  AC_MSG_ERROR([Can't enable threads without the POSIX thread library.])
fi

AC_CHECK_FUNCS(mlockall)
AC_CHECK_FUNCS(getpagesizes)
AC_CHECK_FUNCS(memcntl)
AC_CHECK_FUNCS(sigignore)

AC_DEFUN([AC_C_ALIGNMENT],
[AC_CACHE_CHECK(for alignment, ac_cv_c_alignment,
[
  AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([
#include <stdlib.h>
#include <inttypes.h>
    ], [
       char *buf = malloc(32);
       uint64_t *ptr = (uint64_t*)(buf+2);
       *ptr = 0x1;
       return 0;
    ])
  ],[
    ac_cv_c_alignment=none
  ],[
    ac_cv_c_endian=need
  ])
])
if test $ac_cv_c_endian = need; then
  AC_DEFINE(NEED_ALIGN, 1, [Machine need alignment])
fi
])

AC_C_ALIGNMENT

dnl Check for the requirements for running moxi with less privileges
dnl than the default privilege set. On Solaris we need setppriv and priv.h
dnl If you want to add support for other platforms you should check for
dnl your requirements, define HAVE_DROP_PRIVILEGES, and make sure you add
dnl the source file containing the implementation into moxi_SOURCE
dnl in Makefile.am
AC_CHECK_FUNCS(setppriv, [
   AC_CHECK_HEADER(priv.h, [
      AC_DEFINE([HAVE_DROP_PRIVILEGES], 1,
         [Define this if you have an implementation of drop_privileges()])
      build_solaris_privs=yes
   ], [])
],[])

AM_CONDITIONAL([BUILD_SOLARIS_PRIVS],[test "$build_solaris_privs" = "yes"])

AC_CHECK_HEADER(umem.h, [
   AC_DEFINE([HAVE_UMEM_H], 1,
         [Define this if you have umem.h])
   build_cache=no
], [build_cache=yes])

AM_CONDITIONAL([BUILD_CACHE], [test "x$build_cache" = "xyes"])

AC_ARG_ENABLE(docs,
  [AS_HELP_STRING([--disable-docs],[Disable documentation generation])])

AC_PATH_PROG([XML2RFC], [xml2rfc], "no")
AC_PATH_PROG([XSLTPROC], [xsltproc], "no")

AM_CONDITIONAL([BUILD_SPECIFICATIONS],
               [test "x$enable_docs" != "xno" -a "x$XML2RFC" != "xno" -a "x$XSLTPROC" != "xno"])


dnl Let the compiler be a bit more picky. Please note that you cannot
dnl specify these flags to the compiler before AC_CHECK_FUNCS, because
dnl the test program will generate a compilation warning and hence fail
dnl to detect the function ;-)
if test "$ICC" = "yes"
then
   dnl ICC trying to be gcc.
   CFLAGS="$CFLAGS -diag-disable 187 -Wall -Werror"
   AC_DEFINE([_GNU_SOURCE],[1],[find sigignore on Linux])
elif test "$GCC" = "yes"
then
  GCC_VERSION=`gcc -dumpversion`
  CFLAGS="$CFLAGS -Wall -Werror -pedantic -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls"
  case $GCC_VERSION in
    4.4.*)
    CFLAGS="$CFLAGS -fno-strict-aliasing"
    ;;
  esac
  AC_DEFINE([_GNU_SOURCE],[1],[find sigignore on Linux])
  NO_STRICT_ALIASING="-fno-strict-aliasing -Wno-strict-aliasing"
elif test "$SUNCC" = "yes"
then
  CFLAGS="$CFLAGS -errfmt=error -errwarn -errshort=tags"
fi

AC_ARG_VAR([CONFLATE_DB_PATH], [Directory for libconflate config database (default /var/lib/]AC_PACKAGE_NAME[)])
if test "x$useconflate" = "xyes"; then
  if test "x$CONFLATE_DB_PATH" = x; then
    CONFLATE_DB_PATH=/var/lib/$PACKAGE
  fi
  AC_DEFINE_UNQUOTED([CONFLATE_DB_PATH], ["$CONFLATE_DB_PATH"], [directory for libconflate config database])
fi


AC_SEARCH_LIBS(memcached_create, memcached)

AC_SEARCH_LIBS([res_query], resolv)
AC_SEARCH_LIBS(res_9_query, resolv)
dnl Linux needs resolv for the above, but doesn't seem to know it
AC_SEARCH_LIBS(__res_query, resolv)

LIBS="$LIBS -lm -lsqlite3 -lssl"

AC_CONFIG_FILES(Makefile doc/Makefile libmemcached-0.30/libmemcached/Makefile)
AC_OUTPUT

