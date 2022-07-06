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

#include <stdlib.h>
#include <stdio.h>

#include <cell/expression.h>

struct cell_identnode *cell_identnode_init() {
    struct cell_identnode *n;

    if (!(n = (struct cell_identnode *) malloc(sizeof (struct cell_identnode)))) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    INIT_SLIST_NODE(&n->node.node);
    n->node.type = NODE_IDENT;

    n->name[0] = 0;
    n->res_type = 0;
    n->res = 0;
    INIT_SLIST_NODE(&n->resnode);

    return n;
}
