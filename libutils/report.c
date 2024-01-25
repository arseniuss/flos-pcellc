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

#include <cell/source.h>

#include "utils.h"

int print_location = 1;

char *vreportf(cell_source_t *src, const cell_pos_t *pos, cell_sev_t sev, const char *fmt, va_list args) {
    char *ret = NULL;
    const char *filename = NULL;

    if (src) {
        filename = src->filename_get(src);
    }

    if (filename) {
        safe_strmcatf(&ret, "%s:", filename);
    }

    if (pos) {
        safe_strmcatf(&ret, "%d:%d:", pos->start.row, pos->start.col);
    }

    if (pos || filename) {
        safe_strmcat(&ret, " ");
    }

    switch (sev) {
    case SEV_NOTE:
        safe_strmcat(&ret, "note: ");
        break;
    case SEV_DEBUG:
        safe_strmcat(&ret, "debug: ");
        break;
    case SEV_WARNING:
        safe_strmcat(&ret, "warning: ");
        break;
    case SEV_ERROR:
        safe_strmcat(&ret, "error: ");
        break;
    }

    safe_vstrmcatf(&ret, fmt, args);

    if (src && pos && print_location) {
        long curr = src->ftell(src);
        int r = src->fseek(src, pos->start.line_pos);
        char *line = NULL;

        if (r == 0) {
            int ch;
            int l = 0;

            do {
                if ((ch = src->getc(src)) == -1) {
                    break;
                }
                safe_strmcatf(&line, "%c", ch);
                l++;
            } while (ch != '\n' && ch != -1);

            if (line[l - 1] != '\n') {
                line[l - 1] = '\n';
            }

            safe_strmcatf(&ret, "\n %-5d | ", pos->start.row);
            safe_strmcatf(&ret, "%s", line);

            safe_strmcatf(&ret, "       | ");
            for (size_t i = 0; line[i]; i++) {
                if (i == pos->start.col - 2) {
                    safe_strmcat(&ret, "^");
                } else if (i > pos->start.col - 2 && i < pos->end.col - 2) {
                    safe_strmcat(&ret, "~");
                } else {
                    if (line[i] == '\t') {
                        safe_strmcat(&ret, "\t");
                    } else {
                        safe_strmcat(&ret, " ");
                    }
                }
            }

            src->fseek(src, curr);
        }
    }

    return ret;
}

char *reportf(cell_source_t *src, const cell_pos_t *pos, cell_sev_t sev, const char *fmt, ...) {
    char *ret;
    va_list args;

    va_start(args, fmt);
    ret = vreportf(src, pos, sev, fmt, args);
    va_end(args);

    return ret;
}
