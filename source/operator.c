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

#include <cell/token.h>
#include <cell/operator.h>

const char cell_operators[OPERATOR_LAST] = {
#define __TOKEN_TYPE TOKEN_OPERATORS
#define __(u, l, str, chr) chr,
#include <cell/token.inc.h>
};

int
cell_precedence(const char *op) {
    if (op[1] == 0) {
        switch (op[0]) {
            case '+':
            case '-':
                return 10;
            case '/':
            case '*':
                return 100;
            case '^':
                return 200;
        }
    }

    return -1;
}
