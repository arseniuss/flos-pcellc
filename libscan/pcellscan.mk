TARGET = bin/pcellscan

SRCS += \
    libscan/pcellscan.c

CFLAGS += \
	-I$(root)/src/flos/libutils/include

PKGS = \
	lib/libcell_utils.a \

LIBS += \
    $(root)/src/flos/libutf8/lib/libutf8.a \
	$(root)/src/flos/libutils/lib/libutils.a
