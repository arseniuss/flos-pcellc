#!/usr/bin/env sh

# exit on error
set -e

PROGNAME=${0##*/}

VERSION='0.1'

force=0

feature_debug=0
feature_coverage=0

usage() {
    cat << EOF
Usage: ${PROGNAME} [OPTIONS]
EOF
}

for i in "$@"; do
    case $i in
    --debug)
        feature_debug=1
        ;;
    --force)
        echo "warning: forcing configuration replacement"
        force=1
        ;;
    --coverage)
        echo "warning: forcing coverage configuration"
        feature_coverage=1
        ;;
    *)
        echo "error: unknown argument '$i'"
        usage
        exit 1
        ;;
    esac
done

if [ -f config.mk ] && [ "$force" -eq 0 ]; then
    echo "Configuration file 'config.mk' exists";
    echo "Use --force to force rewrite"
    exit 1
fi

cat <<EOF > config.mk
VERSION = $VERSION

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
EOF

if [ "$feature_debug" -eq 1 ]; then
    echo "CFLAGS += -g" >> config.mk
fi

if [ "$feature_coverage" -eq 1 ]; then
    echo "CFLAGS += --coverage" >> config.mk
    echo "LDFLAGS += -lgcov" >> config.mk
fi

cat << EOF >> config.mk
CFLAGS += -DVERSION=\"\$(VERSION)\"
EOF
