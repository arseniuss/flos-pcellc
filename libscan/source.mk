TARGET = \
    lib/libcell_scan.a

HDRS = \
    include/cell/scan.h

SRCS = \
    libscan/scan.c

CFLAGS += \
	-Iinclude \
	-I$(root)/src/flos/libds/include \
	-I$(libutf8_INCLUDE)

PKGS = \
    $(libutf8_LIB)/libutf8.a \
    $(root)/src/flos/libds/lib/libds.a \
