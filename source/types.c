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
#include <string.h>
#include <stdio.h>

#include <cell/expression.h>
#include <cell/node.h>
#include <cell/types.h>

#include "scanner.h"

const char *int1_aliases[] = {"bool", 0};
const char *sint8_aliases[] = {"int8", "byte", 0};
const char *sint16_aliases[] = {"int16", "short", 0};
const char *sint32_aliases[] = {"int32", "int", 0};
const char *sint64_aliases[] = {"int64", "long", 0};
const char *sint128_aliases[] = {"int128", "cent", 0};

const char *uint8_aliases[] = {"ubyte", 0};
const char *uint16_aliases[] = {"ushort", 0};
const char *uint32_aliases[] = {"uint", "char", 0};
const char *uint64_aliases[] = {"ulong", "uintptr", 0}; // TODO: machine dependent
const char *uint128_aliases[] = {"ucent", 0};

const char *float32_aliases[] = {"float", 0};
const char *float64_aliases[] = {"double", 0};

static int sint32_validate(struct cell_node *n) {
    switch (n->type) {
        case NODE_NUMBER:
        {
            struct cell_numnode *p = (struct cell_numnode *) n;

            intmax_t value = (intmax_t) p->value;

            if (value < INT32_MIN) {
                report_error(&n->pos, "value %d is smaller than sint32.min", value);
                return -1;
            } else if (value > INT32_MAX) {
                report_error(&n->pos, "value %d is larger than sint32.max", value);
                return -1;
            }

            break;
        }
        default:
            report_error(&n->pos, "cannot validate sint32 against %s", node_names[n->type]);
            return -1;
    }

    return 0;
}

static int uint32_validate(struct cell_node *n) {
    switch (n->type) {
        case NODE_CHAR:
        {
            // TODO
            break;
        }
        case NODE_NUMBER:
        {
            struct cell_numnode *p = (struct cell_numnode *) n;

            uintmax_t value = p->value;

            if (value > UINT32_MAX) {
                report_error(&n->pos, "value %ld is larger than uint32.max", value);
                return -1;
            }

            break;
        }
        default:
            report_error(&n->pos, "cannot validate uint32 against %s", node_names[n->type]);
            return -1;
    }

    return 0;
}

struct cell_type builtin_types[] = {
    {"int1", (const char **) &int1_aliases, "i1"},
    {"sint8", (const char **) &sint8_aliases, "i8"},
    {"sint16", (const char **) &sint16_aliases, "i16"},
    {"sint32", (const char **) &sint32_aliases, "i32", &sint32_validate},
    {"sint64", (const char **) &sint64_aliases, "i64"},
    {"sint128", (const char **) &sint128_aliases, "i128"},

    {"uint8", (const char **) &uint8_aliases, "i8"},
    {"uint16", (const char **) &uint16_aliases, "i16"},
    {"uint32", (const char **) &uint32_aliases, "i32", &uint32_validate},
    {"uint64", (const char **) &uint64_aliases, "i64"},
    {"uint128", (const char **) &uint128_aliases, "i128"},

    {"float32", (const char **) &float32_aliases, "float"},
    {"float64", (const char **) &float64_aliases, "double"}
};

const struct cell_type*
cell_type_find(const char *name) {
    unsigned i;

    for (i = 0; i < sizeof (builtin_types) / sizeof (builtin_types[0]); i++) {
        const char **p;

        if (strcmp(name, builtin_types[i].name) == 0) {
            return &builtin_types[i];
        }

        for (p = builtin_types[i].aliases; *p; p++) {
            if (strcmp(*p, name) == 0) {
                return &builtin_types[i];
            }
        }

    }


    return NULL;
}

struct cell_typenode*
cell_typenode_init() {
    struct cell_typenode *n;

    n = (struct cell_typenode *) malloc(sizeof (struct cell_typenode));
    if (!n) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    n->node.type = NODE_TYPENODE;
    INIT_SLIST_NODE(&n->node.node);

    n->name[0] = 0;
    n->metadata[0] = 0;
    n->typedecl = 0;
    n->type = 0;
    INIT_SLIST_NODE(&n->res);

    return n;
}

struct cell_slicenode*
cell_slicenode_init() {
    struct cell_slicenode *n;

    n = (struct cell_slicenode *) malloc(sizeof (struct cell_slicenode));
    if (!n) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    n->node.type = NODE_SLICENODE;
    INIT_SLIST_NODE(&n->node.node);

    INIT_SLIST_HEAD(&n->type);

    return n;
}

struct cell_arrnode*
cell_arrnode_init() {
    struct cell_arrnode *n;

    n = (struct cell_arrnode *) malloc(sizeof (struct cell_arrnode));
    if (!n) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    n->node.type = NODE_ARRNODE;
    INIT_SLIST_NODE(&n->node.node);

    n->cap = 0;
    INIT_SLIST_HEAD(&n->type);

    return n;
}

struct cell_structnode *cell_structnode_init() {
    struct cell_structnode *n;

    n = (struct cell_structnode *) malloc(sizeof (struct cell_structnode));
    if (!n) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    n->node.type = NODE_STRUCTNODE;
    INIT_SLIST_NODE(&n->node.node);
    INIT_SLIST_HEAD(&n->fields);

    n->flags = 0;

    return n;
}