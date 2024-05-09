#!env make
.POSIX:

include config.mk
include $(SUBDIR)/source.mk
include $(MKFILE)

OBJS != echo $(SRCS:.c=.o) | sed -e 's/$(SUBDIR)\//$(builddir)\/$(SUBDIR)\//g'
DEPS != echo $(SRCS:.c=.d) | sed -e 's/$(SUBDIR)\//$(builddir)\/$(SUBDIR)\//g'
SUBS != echo $(PKGS:.a=) | sed -e 's/lib\/libcell_/clean-lib/g'

.SUFFIXES:
.PHONY: all clean $(SUBS) $(TARGET)

all: $(TARGET)

$(TARGET): $(OBJS) $(PKGS)
	mkdir -p bin
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJS): $(builddir)/$(SUBDIR)/%.o: $(SUBDIR)/%.c
	mkdir -p $(@D)
	$(PP) $(CFLAGS) $< > $(builddir)/$<.pp
	$(CC) $(CFLAGS) -MMD -MF $(builddir)/$*.d -c -o $@ $<

lib/libcell_%.a:
	make -f libs.mk SUBDIR=lib$*

$(SUBS): clean-%:
	make -f libs.mk SUBDIR=$* clean

clean: $(SUBS)
	rm -rf $(builddir)/$(SUBDIR) $(CMD)

-include $(DEPS)
