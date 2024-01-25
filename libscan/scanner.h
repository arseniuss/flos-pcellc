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

#ifndef SCANNER_H
#define SCANNER_H

#include <flos/utf8.h>

#include <cell/const.h>
#include <cell/pos.h>
#include <cell/source.h>

struct cell_scanner_s {
    cell_source_t *source;

    /* location */
    cell_pos_t pos;

    /* next character */
    utf8_char chr;
    char letter[utf8_max];
    int peekchr;

    /* current token */
    char *text; /* contains test as written in file */
    cell_token_t token;
    cell_token_t prev_token;

    union {
        utf8_char ch;
        char str[MAXNAMELEN]; /* contains useable value */
        uintmax_t num;
    } value;
};

#endif /* SCANNER_H */
