/*
 * Copyright (C) 2024 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <cell/const.h>

#include "utils.h"

char *safe_strmcat(char **dst, const char *src) {
    size_t r1, r2;
    char *ret;

    r1 = *dst ? strlen(*dst) : 0;
    r2 = strlen(src);

    if (!*dst) {
        if (!(ret = (char *)malloc(r1 + r2 + 1))) {
            return memory_error();
        }
    } else {
        if (!(ret = (char *)realloc(*dst, r1 + r2 + 1))) {
            return memory_error();
        }
    }
    ret[r1] = '\0';
    strncpy(&ret[r1], src, r2 + 1);
    *dst = ret;

    return NULL;
}

char *safe_vstrmcatf(char **dst, const char *fmt, va_list args) {
    int r1, r2;
    char buf[MAXBUFLEN];
    char *ret = *dst;

    r1 = ret ? strlen(ret) : 0;
    r2 = vsnprintf(buf, MAXBUFLEN, fmt, args);

    if (!(ret = realloc(ret, r1 + r2 + 1))) {
        return memory_error();
    }
    ret[r1] = '\0';
    strncpy(&ret[r1], buf, r2);
    ret[r1 + r2] = '\0';

    *dst = ret;

    return NULL;
}

char *safe_strcatf(char *dst, const char *fmt, ...) {
    char *err = NULL;
    char buf[MAXBUFLEN] = {0};
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    strcat(dst, buf);
    va_end(args);

    return err;
}

char *safe_strmcatf(char **dst, const char *fmt, ...) {
    char *err = NULL;
    va_list args;

    va_start(args, fmt);
    err = safe_vstrmcatf(dst, fmt, args);
    va_end(args);

    return err;
}
