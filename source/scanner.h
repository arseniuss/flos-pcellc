/*
 * Copyright (C) 2022 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
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

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <utf8.h>

#include <cell/scanner.h>
#include <cell/position.h>

#define SEV_ERROR 1
#define SEV_DEBUG 2

#ifdef __cplusplus
extern "C" {
#endif

#define unimplemented() \
    do {\
        fprintf(stderr, "%s:%d: unimplemented\n", __FILE__, __LINE__); \
    } while (0)

    struct scanner {
        FILE* f;

        struct pos pos;

        /* next character */
        utf8_char chr;
        char letter[utf8_max];
        int peekchr;

        /* current token */
        char text[MAXNAMELEN];
        int token;

        int prev_token;

        union {
            utf8_char ch;
            char str[MAXNAMELEN];
            uintmax_t num;
        } value;
    };

    // TODO: move this

    static __inline__ void report(const struct pos *p, int sev, const char *fmt, va_list args) {
        fprintf(stderr, "%s:%d:%d: ", p->filename, p->line, p->pos);
        switch (sev) {
            case SEV_ERROR:
                fprintf(stderr, "error: ");
                break;
            default:
                fprintf(stderr, "note: ");
                break;
        }

        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");

        assert(p->line && p->pos);
    }

    static __inline__ void report_debug(const struct pos *p, const char *fmt, ...) {
        va_list args;

        va_start(args, fmt);

        report(p, SEV_DEBUG, fmt, args);

        va_end(args);
    }

    static __inline__ void report_error(const struct pos *p, const char *fmt, ...) {
        va_list args;

        va_start(args, fmt);

        report(p, SEV_ERROR, fmt, args);

        va_end(args);
    }



#ifdef __cplusplus
}
#endif

#endif /* SCANNER_H */
