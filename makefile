#!env make
.POSIX:

TARGETS = \
	bin/pcellscan \
	lib/libcell_scan.a \
	lib/libcell_utils.a

include config.mk

.SUFFIXES:
.PHONY: all tests clean coverage $(TARGETS)

all: $(TARGETS)

bin/pcellscan: cmds.mk libscan/pcellscan.mk libscan/source.mk
	$(MAKE) -f cmds.mk MKFILE=libscan/pcellscan.mk SUBDIR=libscan

lib/libcell_scan.a: libs.mk libscan/source.mk
	$(MAKE) -f libs.mk SUBDIR=libscan

lib/libcell_utils.a: libs.mk libutils/source.mk
	$(MAKE) -f libs.mk SUBDIR=libutils

# TODO: check:

#TODO install:

clean:
	rm -rf bin lib $(builddir) pcellc-$(VERSION).tar.gz

distclean:
	rm -rf config.mk deps
