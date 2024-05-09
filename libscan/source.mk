TARGET = \
    lib/libcell_scan.a

HDRS = \
    include/cell/scan.h

SRCS = \
    libscan/scan.c

CFLAGS += \
	-Iinclude \
	-I$(root)/src/flos/libds/include \
	-I$(root)/src/flos/libutf8/include

PKGS = \
    $(root)/src/flos/libutf8/lib/libutf8.a \
    $(root)/src/flos/libds/lib/libds.a \
