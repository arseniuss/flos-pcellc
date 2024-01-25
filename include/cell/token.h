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

#ifndef CELL__TOKEN_H
#define CELL__TOKEN_H

#define CELL_TOKEN_SYSTEM    1
#define CELL_TOKEN_LITERAL   2
#define CELL_TOKEN_BRACKETS  3
#define CELL_TOKEN_OPERATORS 4
#define CELL_TOKEN_KEYWORDS  5

typedef enum {
    CELL_TOKEN_INVALID = -1,

#define __(u, l, str, chr) CELL_TOKEN_##u,
#include <cell/token.inc.h>

    CELL_TOKEN_LAST
} cell_token_t;

enum CELL_OPERATORS {
    OPERATOR_INVALID = -1,

#define __TOKEN_TYPE       CELL_TOKEN_OPERATORS
#define __(u, l, str, chr) CELL_OPERATOR_##u,
#include <cell/token.inc.h>

    CELL_OPERATOR_LAST
};

enum CELL_KEYWORDS {
    CELL_KEYWORD_INVALID = -1,

#define __TOKEN_TYPE       CELL_TOKEN_KEYWORDS
#define __(u, l, str, chr) CELL_KEYWORD_##u,
#include <cell/token.inc.h>

    CELL_KEYWORD_LAST
};

extern const char *cell_tokens[CELL_TOKEN_LAST];

#endif /* CELL__TOKEN_H */
