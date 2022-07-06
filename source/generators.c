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

#include <string.h>

#include <cell/expression.h>
#include <cell/generators.h>
#include <cell/types.h>

#include "scanner.h"

static int gen_typelist(const struct cell_visitors *vst, struct cell_node *p);

static int gen_import(const struct cell_visitors *vst, struct cell_nodeimp *p) {

    // TODO specify library
    vst->out("; import %s\n", p->name);

    return 0;
}

static int gen_type(const struct cell_visitors *vst, struct slist_head *p) {
    struct cell_node *n;

    n = slist_first_entry(p, struct cell_node, node);

    assert(n);

    switch (n->type) {
        case NODE_STRUCTNODE:
        {
            struct cell_structnode *s = (struct cell_structnode *) n;

            if (s->flags & STRUCT_PACKED != 0) {
                vst->out("<");
            }
            vst->out("{");

            // type list
            struct cell_node *p;

            slist_for_each_entry(p, &s->fields, node) {
                if (&p->node != s->fields.head) {
                    vst->out(", ");
                }

                if (gen_typelist(vst, p) != 0)
                    return -1;
            }

            vst->out("}");
            if (s->flags & STRUCT_PACKED != 0) {
                vst->out(">");
            }

            break;
        }
        case NODE_TYPENODE:
        {
            struct cell_typenode *t = (struct cell_typenode *) n;

            if (!t->type && !t->typedecl) {
                report_error(&t->node.pos, "unresolved type '%s'", t->name);
                return -1;
            }



            if (t->type)
                vst->out("%s", t->type->llvm_name);
            else {
                struct cell_typedecl *d = (struct cell_typedecl *) t->typedecl;

                if (gen_type(vst, &d->type) != 0)
                    return -1;
            }

            break;
        }
        case NODE_ARRNODE:
        {
            struct cell_arrnode *a = (struct cell_arrnode *) n;

            vst->out("[%ld x ", a->cap);

            if (gen_type(vst, &a->type) != 0)
                return -1;

            vst->out("]");

            break;
        }
        default:
            report_error(&n->pos, "cannot generate type node '%s'", node_names[n->type]);
            return -1;
    }

    assert(!n->node.next);

    return 0;
}

static int gen_typename(const struct cell_visitors *vst, struct slist_head *p) {
    struct cell_node *n;

    n = slist_first_entry(p, struct cell_node, node);

    assert(n);

    switch (n->type) {
        case NODE_SLICENODE:
        {
            struct cell_slicenode *t = (struct cell_slicenode *) n;

            vst->out("%%builtin.slice");
            break;
        }
        case NODE_ARRNODE:
        {
            struct cell_arrnode *t = (struct cell_arrnode *) n;

            vst->out("[%d x ", t->cap);

            gen_typename(vst, &t->type);

            vst->out("]");

            break;
        }
        case NODE_TYPENODE:
        {
            struct cell_typenode *t = (struct cell_typenode *) n;

            if (!t->type && !t->typedecl) {
                report_error(&t->node.pos, "unresolved type '%s'", t->name);
                return -1;
            }



            if (t->type)
                vst->out("%s", t->type->llvm_name);
            else {
                struct cell_typedecl *d = (struct cell_typedecl *) t->typedecl;

                vst->out("%%%s", d->name);
            }

            break;
        }
        default:
            report_error(&n->pos, "cannot generate type name for node '%s'", node_names[n->type]);
            return -1;
    }

    assert(!n->node.next);

    return 0;

}

static int gen_typelist(const struct cell_visitors *vst, struct cell_node *p) {
    switch (p->type) {
        case NODE_TYPENODE:
        {
            struct cell_typenode *t = (struct cell_typenode *) p;

            if (!t->type) {
                report_error(&p->pos, "no assigned convertion type to '%s'", t->name);
                return -1;
            }

            if (!t->type->llvm_name[0]) {
                report_error(&p->pos, "cannot convert type '%s' to LLVM IR type", t->name);
                return -1;
            }

            vst->out("%s", t->type->llvm_name);

            break;
        }
        case NODE_VARDECL:
        {
            struct cell_vardecl *v = (struct cell_vardecl *) p;

            struct cell_node *t;

            slist_for_each_entry(t, &v->type, node) {
                if (gen_typelist(vst, t) != 0)
                    return -1;
            }

            break;
        }
        default:
            report_error(&p->pos, "unrecognised type in structure field '%s'\n", node_names[p->type]);
            return -1;
    }

    return 0;
}

static int gen_typedecl(const struct cell_visitors *vst, struct cell_typedecl *d) {
    struct cell_node *n = slist_first_entry(&d->type, struct cell_node, node);

    if (!n) return -1;

    switch (n->type) {
        case NODE_STRUCTNODE:
        {
            struct cell_structnode *s = (struct cell_structnode *) n;

            vst->out("%%%s = type ", d->name);
            if (s->flags & STRUCT_PACKED != 0) {
                vst->out("<");
            }
            vst->out("{");

            // type list
            struct cell_node *p;

            slist_for_each_entry(p, &s->fields, node) {
                if (&p->node != s->fields.head) {
                    vst->out(", ");
                }

                if (gen_typelist(vst, p) != 0)
                    return -1;
            }

            if (s->flags & STRUCT_PACKED != 0)
                vst->out(">");
            vst->out("}\n");

            break;
        }
        case NODE_TYPENODE:
        {
            struct cell_typenode *t = (struct cell_typenode *) n;

            vst->out("; type %s %s\n", t->name, t->type->llvm_name);

            break;
        }
        default:
            report_error(&n->pos, "unrecognised type %s", node_names[n->type]);
            return -1;
    }

    return 0;
}

static int gen_const(const struct cell_visitors *vst, struct slist_head *c) {
    struct cell_node *n = slist_first_entry(c, struct cell_node, node);

    assert(n);

    switch (n->type) {
        case NODE_CHAR:
        {
            struct cell_chrnode *m = (struct cell_chrnode *) n;

            vst->out("%d", m->chr);

            break;
        }
        case NODE_NUMBER:
        {
            struct cell_numnode *m = (struct cell_numnode *) n;

            vst->out("%ld", m->value);

            break;
        }
        case NODE_STRING:
        {
            struct cell_strnode *m = (struct cell_strnode *) n;

            if (m->tmpname[0] == 0) {
                report_error(&m->node.pos, "string constant has no container name");
                return -1;
            }

            unsigned len = strlen(m->str);

            // TODO: resolve depending on structure's metadata

            // TODO: align 8
            vst->out("{ i8* getelementptr inbounds ([%d x i8], [%d x i8]* %s, i32 0, i32 0), i64 %d }",
                    len, len, m->tmpname, len);

            break;
        }
        default:
            report_error(&n->pos, "unresolved constant '%s'", node_names[n->type]);
            return -1;
    }

    return 0;
}

static int gen_unnamedconst(const struct cell_visitors *vst, struct slist_head *value, struct __cell_scope *scope) {
    struct cell_node *n = slist_first_entry(value, struct cell_node, node);

    if (n) {
        switch (n->type) {
            case NODE_STRING:
            {
                struct cell_strnode *m = (struct cell_strnode *) n;
                char tmpname[NAME_MAX] = {0};

                snprintf(tmpname, NAME_MAX, "@.str.%d", scope->tmpcount);

                strcat(m->tmpname, tmpname);

                vst->out("%s = private unnamed_addr constant [%d x i8] c\"%s\", align 1\n",
                        tmpname, strlen(m->str), m->str);
                scope->tmpcount++;
                break;
            }
        }
    }
}

static int gen_vardecl(const struct cell_visitors *vst, struct cell_vardecl *p, struct __cell_scope *scope) {

    if (!slist_empty(&p->value)) {
        gen_unnamedconst(vst, &p->value, scope);
    }

    vst->out("@%s = ", p->name);
    // Linkage
    if (p->flags & VAR_EXTERNAL != 0) {
        vst->out("external ");
    }

    // TODO: preemption
    // TODO visibility
    // TODO: thread local

    // addressing
    vst->out("unnamed_addr ");

    // TODO: address space
    // TODO: externally initialized

    vst->out("global ");

    if (slist_empty(&p->type)) {
        report_error(&p->hdr.pos, "unresolved type for variable '%s'", p->name);
        return -1;
    }

    if (gen_typename(vst, &p->type) != 0)
        return -1;

    if (!slist_empty(&p->value)) {
        vst->out(" ");
        if (gen_const(vst, &p->value) != 0)
            return -1;
    }

    vst->out("\n");

    // TODO: add metadata to module

    return 0;
}

static int gen_funcdecl(const struct cell_visitors *vst, struct cell_funcdecl *p) {
    if (p->flags & FUNC_PROTO != 0) {
        vst->out("declare ");
    } else {
        vst->out("define ");
    }

    // TODO: Linkage

    // Preemption, TOSO
    vst->out("dso_local ");

    // TODO: visibility

    if (gen_typename(vst, &p->rettype) != 0)
        return -1;

    vst->out(" @%s(", p->name);

    struct cell_vardecl *v;

    slist_for_each_entry(v, &p->args, hdr.node) {
        assert(v->hdr.type == NODE_VARDECL);

        if (gen_typename(vst, &v->type) != 0)
            return -2;

        // TODO: parameter attribute
        vst->out(" %%%s", v->name);

        if (!slist_entry_is_last(v, &p->args, hdr.node)) {
            vst->out(", ");
        }
    }

    vst->out(")");

    // TODO: function attributes

    if (p->flags & FUNC_PROTO != 0) {
        vst->out("\n");
    } else {
        vst->out(" {");

        vst->out("\n}\n");

        unimplemented();
        return -1;
    }

    return 0;
}

int cell_llvm_generate(struct cell_module *mod, out_func out) {

    struct cell_visitors gen_visitors = {
        gen_import,
        gen_typedecl,
        gen_vardecl,
        gen_funcdecl,
        out
    };

    out("; ModuleID = '%s'\n", mod->name);
    out("source_filename = \"%s\"\n", mod->hdr.pos.filename);

    out("target datalayout = \"");
    if (mod->flags & MODULE_BIGENDIAN != 0) out("E-");
    else out("e-");
    out("m:e-"); // ELF mangling
    if (strcmp(mod->arch, "x86_64") == 0) {
        out("p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128");
    } else {
        report_error(&mod->hdr.pos, "unrecignised architecture '%s'", mod->arch);
        return -1;
    }
    out("\"\n");

    out("target triple = \"%s-%s-%s", mod->arch,
            (mod->vendor[0] ? mod->vendor : "unknown"),
            (mod->os[0] ? mod->os : "unknown"));
    if (mod->env[0]) {
        out("-%s", mod->env);
    }
    out("\"\n");

    out("\n\n\n");

    if (cell_module_visit(mod, &gen_visitors) != 0)
        return -1;

    return 0;
}
