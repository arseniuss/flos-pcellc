#!env make
.POSIX:

CMD = bin/pcellc

SRCS = \
    source/declarations.c \
    source/expression.c \
    source/generators.c \
    source/keyword.c \
    source/main.c \
    source/node.c \
    source/operator.c \
    source/parser.c \
    source/scanner.c \
    source/scope.c \
    source/token.c \
    source/types.c

PKG = \
    $(BASE)/flos/libutf8/lib/libutf8.a

include config.mk

OBJS != echo $(SRCS:.c=.o) | sed -e 's/source/$(BUILDDIR)/g'
DEPS != echo $(SRCS:.c=.d) | sed -e 's/source/$(BUILDDIR)/g'
PPS != echo $(SRCS:.c=.c.pp) | sed -e 's/source/$(BUILDDIR)/g'

CFLAGS += -I$(BASE)/flos/libutf8/include
CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -D_XOPEN_SOURCE=700

all: $(CMD)

$(CMD): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(PKG)

$(BUILDDIR)/%.o: source/%.c
	@mkdir -p $(BUILDDIR)
	$(PP) $(CFLAGS) $< > $(BUILDDIR)/$*.c.pp
	$(CC) $(CFLAGS) -MMD -MF $(BUILDDIR)/$*.d -c -o $@ $<

clean:
	rm -rf bin $(BUILDDIR) $(DEPS) $(PPS)
	
-include $(DEPS)
