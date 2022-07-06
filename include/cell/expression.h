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

#ifndef CELL_EXPRESSION_H
#define CELL_EXPRESSION_H

#include <stdint.h>
#include <limits.h>

#include <utf8.h>

#include <cell/node.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct cell_chrnode {
        struct cell_node node;
        utf8_char chr;
        char text[5];
    };

    struct cell_strnode {
        struct cell_node node;

        char tmpname[MAXNAMELEN];
        char str[MAXNAMELEN];
    };

    struct cell_numnode {
        struct cell_node node;
        uintmax_t value;
    };

    struct cell_identnode {
        struct cell_node node;
        char name[MAXNAMELEN];

        // resolve
        int res_type;
        void *res;
        struct slist_node resnode;
    };

    struct cell_binopnode {
        struct cell_node node;
        char op[5];
        struct cell_node* left;
        struct cell_node* right;
    };

    struct cell_unopnode {
        struct cell_node node;
        char op[5];
        struct cell_node* target;
    };

    struct cell_identnode *cell_identnode_init();


#ifdef __cplusplus
}
#endif

#endif /* CELL_EXPRESSION_H */
