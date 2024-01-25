VERSION = 0.1

AR = ar
CC = cc
COV = gcov
LD = cc
PP = cpp
PROVE = prove

BASE = ../..
BUILDDIR = build

CFLAGS = -std=c99 -Wall -Wshadow -Wextra
LDFLAGS = -lc
CFLAGS += -g
CFLAGS += -DVERSION=\"$(VERSION)\"
