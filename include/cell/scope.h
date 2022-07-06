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

#ifndef CELL_SCOPE_H
#define CELL_SCOPE_H

#include <limits.h>

#include <slist.h>
#include <cell/node.h>
#include <cell/declarations.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct __cell_scope {
        struct __cell_scope* parent;

        struct slist_head imports;
        // Note. Only typedecl
        struct slist_head types;
        // Note. Only cell_typenode
        struct slist_head unres_types;
        struct slist_head unres_ident;
        struct slist_head variables;
        struct slist_head funcs;

        unsigned tmpcount;
    };

    enum {
        MODULE_BIGENDIAN = (1 << 0)
    };

    struct cell_module {
        struct cell_node hdr;
        struct __cell_scope scope;

        char name[MAXNAMELEN];

        unsigned flags;
        char arch[MAXNAMELEN];
        char vendor[MAXNAMELEN];
        char os[MAXNAMELEN];
        char env[MAXNAMELEN];
    };

    struct cell_scope {
        struct cell_node hdr;
        struct __cell_scope scope;

        char name[NAME_MAX];
    };

    struct cell_nodeimp {
        struct cell_node hdr;
        char name[MAXNAMELEN];
        char loc[MAXNAMELEN];
        char alias[MAXNAMELEN];
    };

    struct cell_visitors {
        int (*visit_import)(const struct cell_visitors *vst, struct cell_nodeimp *p);
        int (*visit_typedecl)(const struct cell_visitors *vst, struct cell_typedecl *p);
        int (*visit_vardecl)(const struct cell_visitors *vst, struct cell_vardecl *p, struct __cell_scope *);
        int (*visit_funcdecl)(const struct cell_visitors *vst, struct cell_funcdecl *p);

        int (*out)(const char *fmt, ...);
    };

    struct cell_module* cell_module_init();
    struct cell_nodeimp* cell_nodeimp_init(const char *module, const char *loc, const char *alias);

    int cell_module_visit(struct cell_module *mod, const struct cell_visitors *vst);

    int cell_module_analyze(struct cell_module *mod);
#ifdef __cplusplus
}
#endif

#endif /* CELL_SCOPE_H */
