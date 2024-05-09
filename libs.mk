#!env make
.POSIX:

# Global configuration
include config.mk
#
# Source makefile should include lists of these variables:
#    TARGET - name of the library
#    HDRS - list of headers to install
#    SRCS - list of source files
#    PKGS - list of dependent libraries
#
# Also it can modify some variables:
#    CFLAGS - build flags for C files
#
include $(SUBDIR)/source.mk

OBJS != echo $(SRCS:.c=.o) | sed -e 's/$(SUBDIR)\//$(builddir)\/$(SUBDIR)\//g'
DEPS != echo $(SRCS:.c=.d) | sed -e 's/$(SUBDIR)\//$(builddir)\/$(SUBDIR)\//g'
PPS != echo $(SRCS:.c=.c.pp) | sed -e 's/$(SUBDIR)\//$(builddir)\/$(SUBDIR)\//g'

.SUFFIXES:
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p lib
	rm -f $@
	$(AR) -rcs $@ $^

$(builddir)/%.o: %.c
	@mkdir -p $(@D)
	$(PP) $(CFLAGS) $< > $(builddir)/$*.c.pp
	$(CC) $(CFLAGS) -MMD -MF $(builddir)/$*.d -c -o $@ $<

clean:
	rm -rf $(builddir)/$(SUBDIR) $(TARGET)

-include $(DEPS)
