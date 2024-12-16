TARGET = bin/pcellscan

SRCS += \
    libscan/pcellscan.c

CFLAGS += \
	-I$(libutils_INCLUDE)

PKGS = \
	lib/libcell_utils.a \

LIBS += \
    $(libutf8_LIB)/libutf8.a \
	$(libutils_LIB)/libutils.a
