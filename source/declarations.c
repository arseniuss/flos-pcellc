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

#include <cell/declarations.h>
#include <cell/scanner.h>

struct cell_typedecl*
cell_typedecl_init() {
    struct cell_typedecl *decl;

    decl = (struct cell_typedecl *) malloc(sizeof (struct cell_typedecl));
    if (!decl) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    decl->hdr.type = NODE_TYPEDECL;
    INIT_SLIST_NODE(&decl->hdr.node);

    decl->name[0] = 0;
    INIT_SLIST_HEAD(&decl->type);

    return decl;
}

struct cell_vardecl*
cell_vardecl_init() {
    struct cell_vardecl *decl;

    decl = (struct cell_vardecl *) malloc(sizeof (struct cell_vardecl));
    if (!decl) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    decl->hdr.type = NODE_VARDECL;
    INIT_SLIST_NODE(&decl->hdr.node);

    INIT_SLIST_HEAD(&decl->type);
    INIT_SLIST_HEAD(&decl->value);

    decl->flags = 0;

    return decl;
}

struct cell_funcdecl*
cell_funcdecl_init() {
    struct cell_funcdecl *decl;

    decl = (struct cell_funcdecl *) malloc(sizeof (struct cell_funcdecl));
    if (!decl) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    decl->hdr.type = NODE_FUNCDECL;
    INIT_SLIST_NODE(&decl->hdr.node);

    decl->objname[0] = 0;
    INIT_SLIST_HEAD(&decl->objtype);

    decl->name[0] = 0;
    INIT_SLIST_HEAD(&decl->args);

    INIT_SLIST_HEAD(&decl->rettype);
    decl->flags = 0;


    return decl;
}
