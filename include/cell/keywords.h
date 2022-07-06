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

#ifndef CELL_KEYWORDS_H
#define CELL_KEYWORDS_H

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        KEYWORD_INVALID = -1,
#define __TOKEN_TYPE TOKEN_KEYWORDS
#define __(u, l, str, chr)   KEYWORD_##u,
#include <cell/token.inc.h>

        KEYWORD_LAST
    };

    extern const char *cell_keywords[KEYWORD_LAST];

#ifdef __cplusplus
}
#endif

#endif /* CELL_KEYWORDS_H */
