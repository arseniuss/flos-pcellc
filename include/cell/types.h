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

#ifndef CELL_TYPES_H
#define CELL_TYPES_H

#include <limits.h>
#include <stdint.h>

#include <slist.h>

#include <cell/node.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct cell_type {
        char name[MAXNAMELEN];
        const char **aliases;
        char llvm_name[MAXNAMELEN];
        int (*validate)(struct cell_node *);
    };

    struct cell_typenode {
        struct cell_node node;
        char name[MAXNAMELEN];

        char metadata[MAXNAMELEN];

        // for resolve
        const struct cell_node *typedecl;
        const struct cell_type* type;
        struct slist_node res;
    };

    struct cell_slicenode {
        struct cell_node node;

        struct slist_head type;
    };

    struct cell_arrnode {
        struct cell_node node;

        uintmax_t cap;
        struct slist_head type;
    };

    enum {
        STRUCT_PACKED
    };

    struct cell_structnode {
        struct cell_node node;
        struct slist_head fields;

        unsigned flags;
    };

    const struct cell_type* cell_type_find(const char *name);

    struct cell_typenode* cell_typenode_init();
    struct cell_slicenode* cell_slicenode_init();
    struct cell_arrnode* cell_arrnode_init();
    struct cell_structnode *cell_structnode_init();

#ifdef __cplusplus
}
#endif

#endif /* CELL_TYPES_H */
