#!env make
.POSIX:

CMDS = \
	bin/pcellscan

COVERAGE = \
	libscan \
	libutils

LIBS = \
	lib/libcell_scan.a \
	lib/libcell_utils.a

include config.mk

COVDIRS != echo $(COVERAGE) | sed -e 's/\([a-z]*\)/$(BUILDDIR)\/\1/g'

.SUFFIXES:
.PHONY: all tests clean coverage $(CMDS) $(LIBS)

all: $(LIBS) $(CMDS)

bin/pcellscan: cmds.mk libscan/pcellscan.mk libscan/source.mk
	$(MAKE) -f cmds.mk MKFILE=libscan/pcellscan.mk SUBDIR=libscan

lib/libcell_scan.a: libs.mk libscan/source.mk
	$(MAKE) -f libs.mk SUBDIR=libscan

lib/libcell_utils.a: libs.mk libutils/source.mk
	$(MAKE) -f libs.mk SUBDIR=libutils

coverage: all
	@ for i in $(COVERAGE); do \
		if [ -x $$i/coverage.sh ]; then $$i/coverage.sh; fi \
	done
	mkdir -p $(BUILDDIR)/coverage
	gcovr --html-details $(BUILDDIR)/coverage/ $(COVDIRS)

clean:
	rm -rf bin lib $(BUILDDIR) pcellc-$(VERSION).tar.gz
