PREFIX ?= /usr/local
LOCALBASE ?= /usr/local
BINDIR = ${PREFIX}/bin
MANDIR = ${PREFIX}/man/man
LIBDIR = ${PREFIX}/lib
INCLUDEDIR = ${PREFIX}/include/cbsdng/shell

CXXFLAGS += -I. -I./cbsdng/shell -I${LOCALBASE}/include -std=c++17
SRCS != ls -1 src/*.cpp
LDADD = -pthread -L${LOCALBASE}/lib -lreplxx
PROG_CXX = cbsdngsh
MK_MAN = no

MAININCS != ls -1 cbsdng/shell/*.h
MAININCSDIR = ${INCLUDEDIR}
CLIINCS != ls -1 cbsdng/shell/CLI/*.hpp
CLIINCSDIR = ${INCLUDEDIR}/CLI
INCSGROUPS = MAININCS CLIINCS

beforeinstall:
	${INSTALL} -d -m 0755 ${DESTDIR}${INCLUDEDIR}/CLI

.include <bsd.prog.mk>
