/*
 * Copyright (C) 2023 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
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

#ifndef __UTILS_H
#define __UTILS_H

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <flos/utf8.h>

#include <cell/pos.h>
#include <cell/source.h>

#ifndef typeof
    #define typeof __typeof__
#endif

#define internal_error()  reportf(NULL, NULL, SEV_ERROR, "internal@%s:%d", __FILE__, __LINE__)
#define unimplemented()   reportf(NULL, NULL, SEV_ERROR, "unimplemented@%s:%d", __FILE__, __LINE__)
#define memory_error()    reportf(NULL, NULL, SEV_ERROR, "alloc@%s:%d", __FILE__, __LINE__)
#define buffer_overflow() reportf(NULL, NULL, SEV_ERROR, "overflow@%s:%d", __FILE__, __LINE__)

#define alloc_mem(var, err, label)                                                                                     \
    if (!(var = (typeof(var))malloc(sizeof(typeof(*var))))) {                                                          \
        err = memory_error();                                                                                          \
        goto label;                                                                                                    \
    }

#define safe_strcat(dst, src)                                                                                          \
    (strlen(dst) + strlen(src) >= sizeof(dst) ? buffer_overflow() : (strncat(dst, src, sizeof(dst) - 1), NULL))

#define safe_strncat(dst, src, sz)  (strlen(dst) + strlen(src) >= sz ? buffer_overflow() : (strncat(dst, src, sz), NULL))

#define safe_strcpy(dst, src)       (strlen(src) >= sizeof(dst) ? buffer_overflow() : (strncpy(dst, src, sizeof(dst)), NULL))

#define safe_sprintf(buf, fmt, ...) (__snprintf(buf, sizeof(buf), fmt, ##__VA_ARGS__) < 0 ? buffer_overflow() : NULL)

#define safe_malloc(ptr, type)      (*(ptr) || __malloc((void **)(ptr), sizeof(type)) == NULL ? memory_error() : NULL)

typedef enum {
    SEV_NOTE = 0,
    SEV_DEBUG = 1,
    SEV_WARNING = 2,
    SEV_ERROR = 3
} cell_sev_t;

char *reportf(cell_source_t *src, const cell_pos_t *pos, cell_sev_t sev, const char *fmt, ...);

char *safe_strmcat(char **dst, const char *src);
char *safe_vstrmcatf(char **dst, const char *fmt, va_list args);
char *safe_strcatf(char *dst, const char *fmt, ...);
char *safe_strmcatf(char **dst, const char *fmt, ...);

extern int print_location;

#endif /* __UTILS_H */
