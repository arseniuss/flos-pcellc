#!env make
.POSIX:

include config.mk
include $(SUBDIR)/source.mk
include $(MKFILE)

OBJS != echo $(SRCS:.c=.o) | sed -e 's/$(SUBDIR)\//$(BUILDDIR)\/$(SUBDIR)\//g'
DEPS != echo $(SRCS:.c=.d) | sed -e 's/$(SUBDIR)\//$(BUILDDIR)\/$(SUBDIR)\//g'
SUBS != echo $(LIBS:.a=) | sed -e 's/lib\/libcell_/clean-lib/g'

.SUFFIXES:
.PHONY: all clean $(SUBS) $(LIBS)

all: $(CMD)

$(CMD): $(OBJS) $(LIBS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(PKGS)

$(OBJS): $(BUILDDIR)/$(SUBDIR)/%.o: $(SUBDIR)/%.c
	@mkdir -p $(@D)
	$(PP) $(CFLAGS) $< > $(BUILDDIR)/$<.pp
	$(CC) $(CFLAGS) -MMD -MF $(BUILDDIR)/$*.d -c -o $@ $<

$(LIBS): lib/libcell_%.a:
	make -f libs.mk SUBDIR=lib$*

$(SUBS): clean-%:
	make -f libs.mk SUBDIR=$* clean

coverage:
	$(COV) -s $(SUBDIR) -o $(BUILDDIR)/$(SUBDIR) $(CMD) $(COVFLAGS)

clean: $(SUBS)
	rm -rf $(BUILDDIR)/$(SUBDIR) $(CMD)

-include $(DEPS)
