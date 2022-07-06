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

#ifndef CELL_OPERATOR_H
#define CELL_OPERATOR_H

#include <cell/token.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        OPERATOR_INVALID = -1,
#define __TOKEN_TYPE TOKEN_OPERATORS
#define __(u, l, str, chr)   OPERATOR_##u,
#include <cell/token.inc.h>

        OPERATOR_LAST
    };

    extern const char cell_operators[OPERATOR_LAST];

    int cell_precedence(const char *op);

#ifdef __cplusplus
}
#endif

#endif /* CELL_OPERATOR_H */
