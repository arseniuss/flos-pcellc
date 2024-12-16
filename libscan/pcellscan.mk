TARGET = bin/pcellscan

SRCS += \
    libscan/pcellscan.c

CFLAGS += \
	-I$(libutils_INCLUDE)

PKGS = \
	lib/libcell_utils.a \

LIBS += \
    $(libutf8_LIB)/libutf8.a \
	$(root)/src/flos/libutils/lib/libutils.a
