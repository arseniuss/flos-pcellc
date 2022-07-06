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

#ifndef CELL_NODE_H
#define CELL_NODE_H

#include <slist.h>
#include <cell/position.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        NODE,
        NODE_NUMBER,
        NODE_STRING,
        NODE_CHAR,
        NODE_MODULE,
        NODE_UNOPER,
        NODE_BINOPER,
        NODE_IDENT,
        NODE_TYPENODE,
        NODE_SLICENODE,
        NODE_ARRNODE,
        NODE_STRUCTNODE,
        NODE_SCOPE,
        NODE_VARDECL,
        NODE_TYPEDECL,
        NODE_FUNCDECL,
        NODE_LAST
    };

    struct cell_node {
        struct slist_node node;
        int type;
        struct pos pos;
    };

    extern const char *node_names[NODE_LAST];


#ifdef __cplusplus
}
#endif

#endif /* CELL_NODE_H */
