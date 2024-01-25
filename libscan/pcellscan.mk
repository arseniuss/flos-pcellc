CMD = bin/pcellscan

SRCS += \
    libscan/pcellscan.c

CFLAGS += -I$(BASE)/flos/libutils/include

PKGS = \
	lib/libcell_utils.a \
    $(BASE)/flos/libutf8/lib/libutf8.a \
	$(BASE)/flos/libutils/lib/libutils.a
