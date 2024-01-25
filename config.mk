VERSION = 0.1

AR = ar
CC = cc
COV = gcov
LD = ld
PP = cpp
PROVE = prove

BASE = ../..
BUILDDIR = build

CFLAGS = -std=c99 -Wall -Wshadow
CFLAGS += --coverage
LDFLAGS += -lgcov
CFLAGS += -DVERSION=\"$(VERSION)\"
