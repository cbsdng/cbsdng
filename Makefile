CXXFLAGS += -I. -I/usr/local/include -std=c++17
SRCS != ls -1 src/*.cpp
LDADD = -pthread -L/usr/local/lib -lreplxx
PROG_CXX = cbsdsh
MK_MAN = no

.include <bsd.prog.mk>
