LIB = \
    lib/libcell_scan.a

HDRS = \
    include/cell/scan.h

SRCS = \
    libscan/scan.c

CFLAGS += -Iinclude -I$(BASE)/flos/libds/include -I$(BASE)/flos/libutf8/include

PKGS = \
    $(BASE)/flos/libutf8/lib/libutf8.a \
    $(BASE)/flos/libds/lib/libds.a \
