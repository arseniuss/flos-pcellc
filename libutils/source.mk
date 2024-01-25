LIB = \
    lib/libcell_utils.a

HDRS = \
    include/cell/source.h

SRCS = \
	libutils/report.c \
	libutils/safe.c \
    libutils/source.c

CFLAGS += -Iinclude -I$(BASE)/flos/libutf8/include

PKGS =
