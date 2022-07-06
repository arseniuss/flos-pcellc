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

#ifndef CELL_TOKEN_H
#define CELL_TOKEN_H

#define TOKEN_SYSTEM    1
#define TOKEN_LITERAL   2
#define TOKEN_BRACKETS  3
#define TOKEN_OPERATORS 4
#define TOKEN_KEYWORDS  5

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        TOKEN_INVALID = -1,
#define __(u, l, str, chr)   TOKEN_##u,
#include <cell/token.inc.h>

        TOKEN_LAST
    };

    extern const char *cell_tokens[TOKEN_LAST];


#ifdef __cplusplus
}
#endif

#endif /* CELL_TOKEN_H */
