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

#ifndef CELL_DECLARATIONS_H
#define CELL_DECLARATIONS_H

#include <limits.h>

#include <slist.h>
#include <cell/node.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        FUNC_PROTO = (1 << 0)
    };

    struct cell_funcdecl {
        struct cell_node hdr;

        char objname[MAXNAMELEN];
        struct slist_head objtype;

        char name[MAXNAMELEN];
        // Only cell_vardecl
        struct slist_head args;

        struct slist_head rettype;

        unsigned flags;
    };

    struct cell_typedecl {
        struct cell_node hdr;

        char name[MAXNAMELEN];
        struct slist_head type;
    };

    enum {
        VAR_EXTERNAL = (1 << 0)
    };

    struct cell_vardecl {
        struct cell_node hdr;

        char name[MAXNAMELEN];
        struct slist_head type;
        struct slist_head value;

        unsigned flags;
    };

    struct cell_typedecl* cell_typedecl_init();
    struct cell_vardecl* cell_vardecl_init();
    struct cell_funcdecl* cell_funcdecl_init();


#ifdef __cplusplus
}
#endif

#endif /* CELL_DECLARATIONS_H */
