AC_INIT([libclara], [0.0], [bkoenig@alpha-tierchen.de])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/include/Makefile
])
AC_CHECK_HEADER(netinet/sctp.h, [], [AC_MSG_ERROR(SCTP support required)])
AC_CHECK_DECL(__linux__, [
	AC_CHECK_LIB(sctp, sctp_sendmsg, [LIBS="$LIBS -lsctp"], [AC_MSG_ERROR(unresolved symbol)])
	AC_CHECK_LIB(rt, clock_gettime, [LIBS="$LIBS -lrt"], [AC_MSG_ERROR(unresolved symbol)])
])

AC_CHECK_LIB(lzo2, [lzo1x_decompress], [LIBS="$LIBS -llzo2"], [AC_MSG_ERROR(function not found)])
AC_CHECK_HEADER(lzo/lzoconf.h, [], [AC_MSG_ERROR(header not found)])
AC_CHECK_HEADER(lzo/lzo1x.h, [], [AC_MSG_ERROR(header not found)])
AC_OUTPUT
