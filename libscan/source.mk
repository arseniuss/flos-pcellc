TARGET = \
    lib/libcell_scan.a

HDRS = \
    include/cell/scan.h

SRCS = \
    libscan/scan.c

CFLAGS += \
	-Iinclude \
	-I$(libds_INCLUDE) \
	-I$(libutf8_INCLUDE)

PKGS = \
    $(libutf8_LIB)/libutf8.a \
    $(libds_LIB)/libds.a \
