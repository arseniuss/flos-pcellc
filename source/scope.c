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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cell/expression.h>
#include <cell/scope.h>
#include <cell/types.h>

#include "scanner.h"
#include "utils.h"

int
cell_module_visit(struct cell_module *mod, const struct cell_visitors *vst) {

    {
        struct cell_nodeimp *p;

        slist_for_each_entry(p, &mod->scope.imports, hdr.node) {
            if (vst->visit_import)
                if (vst->visit_import(vst, p) != 0)
                    return -1;
        }
    }

    vst->out("\n\n\n");

    {

        struct cell_typedecl *p;

        slist_for_each_entry(p, &mod->scope.types, hdr.node) {
            if (vst->visit_typedecl)
                if (vst->visit_typedecl(vst, p) != 0)
                    return -2;
        }
    }

    vst->out("\n\n\n");

    {
        struct cell_vardecl *p;

        slist_for_each_entry(p, &mod->scope.variables, hdr.node) {
            if (vst->visit_vardecl)
                if (vst->visit_vardecl(vst, p, &mod->scope) != 0)
                    return -3;
        }
    }

    vst->out("\n\n\n");

    {
        struct cell_funcdecl *p;

        slist_for_each_entry(p, &mod->scope.funcs, hdr.node) {
            if (vst->visit_funcdecl)
                if (vst->visit_funcdecl(vst, p) != 0)
                    return -4;
        }
    }

    return 0;
}

struct cell_module*
cell_module_init() {
    struct cell_module *m;

    m = (struct cell_module *) malloc(sizeof (struct cell_module));
    if (!m) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    m->hdr.type = NODE_SCOPE;
    INIT_SLIST_NODE(&m->hdr.node);

    m->scope.parent = NULL;
    INIT_SLIST_HEAD(&m->scope.imports);
    INIT_SLIST_HEAD(&m->scope.types);
    INIT_SLIST_HEAD(&m->scope.unres_types);
    INIT_SLIST_HEAD(&m->scope.unres_ident);
    INIT_SLIST_HEAD(&m->scope.variables);
    INIT_SLIST_HEAD(&m->scope.funcs);
    m->scope.tmpcount = 0;

    m->name[0] = 0;
    m->flags = 0;
    m->arch[0] = 0;
    m->vendor[0] = 0;
    m->os[0] = 0;
    m->env[0] = 0;

    return m;
}

struct cell_nodeimp*
cell_nodeimp_init(const char *module, const char *loc, const char *alias) {
    struct cell_nodeimp *s;

    s = (struct cell_nodeimp *) malloc(sizeof (struct cell_nodeimp));
    if (!s) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    s->hdr.type = NODE_SCOPE;
    INIT_SLIST_NODE(&s->hdr.node);
    s->name[0] = 0;
    s->alias[0] = 0;
    s->loc[0] = 0;
    strcat(s->name, module);
    strcat(s->alias, alias);
    strcat(s->loc, loc);

    return s;
}

static int validate_type() {

}

static struct cell_node *eval_binary(struct cell_binopnode *op, struct cell_node *left, struct cell_node *right) {
    struct cell_node *ret = NULL;

    switch (left->type) {
        case NODE_NUMBER:
        {
            switch (right->type) {
                case NODE_NUMBER:
                {
                    struct cell_numnode *l = (struct cell_numnode *) left;
                    struct cell_numnode *r = (struct cell_numnode *) right;
                    struct cell_numnode *res = (struct cell_numnode *) malloc(sizeof (struct cell_numnode));

                    if (!res) {
                        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
                        return NULL;
                    }

                    INIT_SLIST_NODE(&res->node.node);
                    res->node.type = NODE_NUMBER;
                    memcpy(&res->node.pos, &op->node.pos, sizeof (struct pos));
                    ret = &res->node;

                    if (strlen(op->op) == 1) {
                        switch (op->op[0]) {
                            case '/':
                                res->value = l->value / r->value;
                                break;
                            case '*':
                                res->value = l->value * r->value;
                                break;
                            case '+':
                                res->value = l->value + r->value;
                                break;
                            default:
                                report_error(&op->node.pos, "cannot resolve simple binary operation '%s'", op->op);
                                goto error;
                        }

                        report_debug(&op->node.pos, "%ld %s %ld = %ld", l->value, op->op, r->value, res->value);
                    } else {
                        report_error(&op->node.pos, "multi-character binary operations are not impelmented");
                        goto error;
                    }

                    break;
                }
                default:
                    report_error(&right->pos, "cannot resolve binary operator's right part");
                    return NULL;
            }

            break;
        }
        default:
            report_error(&left->pos, "cannot resolve binary operator's left part");
            return NULL;
    }

    return ret;
error:
    free(ret);
    return NULL;

}

static struct cell_node *eval_unary(struct cell_unopnode *op, struct cell_node *value) {
    struct cell_node *ret = NULL;

    switch (value->type) {
        case NODE_NUMBER:
        {
            struct cell_numnode *n = (struct cell_numnode *) value;
            struct cell_numnode *res = (struct cell_numnode *) malloc(sizeof (struct cell_numnode));

            if (!res) {
                fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
                return NULL;
            }

            INIT_SLIST_NODE(&res->node.node);
            res->node.type = NODE_NUMBER;
            memcpy(&res->node.pos, &op->node.pos, sizeof (struct pos));
            ret = &res->node;

            if (strlen(op->op) == 1) {
                switch (op->op[0]) {
                    case '-':

                        res->value = -n->value;

                        break;
                    default:
                        report_error(&op->node.pos, "cannot resolve simple unary operation '%s'", op->op);
                        goto error;
                }

                report_debug(&op->node.pos, "%s %ld = %ld", op->op, n->value, res->value);
            } else {
                report_error(&op->node.pos, "multi-character unary operations are not impelmented");
                goto error;
            }

            break;
        }
        default:
            report_error(&op->node.pos, "cannot resolve unary operation '%s'", op->op);
            return NULL;
    }

    return ret;
error:
    free(ret);
    return NULL;
}

static struct cell_node* resolve_constexpr_node(struct cell_node *value, struct slist_head *type) {
    struct cell_node *ret = NULL;
    struct cell_node *t = NULL;

    if (type) {
        t = slist_first_entry(type, struct cell_node, node);
    }

    switch (value->type) {
        case NODE_UNOPER:
        {
            struct cell_unopnode *s = (struct cell_unopnode *) value;

            struct cell_node *target;

            if (!(target = resolve_constexpr_node(s->target, type))) return NULL;

            ret = eval_unary(s, target);
            if (ret) {
                // No need
                free(s->target);
                s->target = NULL;
            }
            break;
        }
        case NODE_BINOPER:
        {
            struct cell_binopnode *s = (struct cell_binopnode *) value;

            struct cell_node *left, *right;

            if (!(left = resolve_constexpr_node(s->left, type))) return NULL;
            if (!(right = resolve_constexpr_node(s->right, type))) return NULL;

            ret = eval_binary(s, left, right);
            if (ret) {
                // No need
                free(s->left);
                free(s->right);
                s->left = s->right = NULL;
            }
            break;
        }
        case NODE_CHAR:
        {
            struct cell_chrnode *s = (struct cell_chrnode *) value;

            if (t) {
                switch (t->type) {
                    case NODE_TYPENODE:
                    {
                        struct cell_typenode *a = (struct cell_typenode *) t;

                        if (a->type) {
                            if (a->type->validate) {
                                if (a->type->validate(value) != 0)
                                    return NULL;

                                ret = value; // ERROR
                            } else {
                                report_error(&s->node.pos, "cannot assign %s to %s(%s)",
                                        node_names[value->type], a->name, a->type->name);
                                return NULL;
                            }
                        } else if (a->typedecl) {
                            assert(a->typedecl->type == NODE_TYPEDECL);

                            struct cell_typedecl *decl = (struct cell_typedecl *) a->typedecl;

                            ret = value; // ERROR
                        } else {
                            report_error(&s->node.pos, "unresolved type for character");
                            return NULL;
                        }

                        break;
                    }
                    default:
                        report_error(&s->node.pos, "cannot validate character \"%s\" against '%s'",
                                s->text, node_names[t->type]);
                        return NULL;
                        break;
                }
            } else {
                report_error(&s->node.pos, "dynamic type for character is not implemented");
                return NULL;
            }

            break;
        }
        case NODE_STRING:
        {
            struct cell_strnode *s = (struct cell_strnode *) value;

            if (t) {
                switch (t->type) {
                    case NODE_TYPENODE:
                    {
                        struct cell_typenode *a = (struct cell_typenode *) t;

                        if (a->type) {
                            unimplemented();
                            return NULL;
                        } else if (a->typedecl) {
                            assert(a->typedecl->type == NODE_TYPEDECL);

                            struct cell_typedecl *decl = (struct cell_typedecl *) a->typedecl;

                            ret = value; // ERROR
                        } else {
                            report_error(&s->node.pos, "unresolved type for string");
                            return NULL;
                        }

                        break;
                    }
                    default:
                        report_error(&s->node.pos, "cannot validate string \"%s\" against '%s'",
                                s->str, node_names[t->type]);
                        return NULL;
                        break;
                }
            } else {
                report_error(&s->node.pos, "dynamic type for string is not implemented");
                return NULL;
            }
            break;
        }
        case NODE_NUMBER:
        {
            struct cell_numnode *s = (struct cell_numnode *) value;

            if (t) {
                switch (t->type) {
                    case NODE_TYPENODE:
                    {
                        struct cell_typenode *a = (struct cell_typenode *) t;

                        if (a->type) {
                            if (a->type->validate) {
                                if (a->type->validate(value) != 0)
                                    return NULL;

                                ret = value; // ERROR
                            } else {
                                report_error(&s->node.pos, "cannot assign %s to %s(%s)",
                                        node_names[value->type], a->name, a->type->name);
                                return NULL;
                            }
                        } else if (a->typedecl) {
                            unimplemented();
                            return NULL;
                        } else {
                            report_error(&s->node.pos, "unresolved type for number");
                            return NULL;
                        }
                        break;
                    }
                    default:
                        report_error(&s->node.pos, "cannot validate number %ld against '%s'",
                                s->value, node_names[t->type]);
                        return NULL;
                }
            } else {
                report_error(&s->node.pos, "dynamic type is not implemented");
                return NULL;
            }
            break;
        }
        case NODE_IDENT:
        {
            struct cell_identnode *n = (struct cell_identnode *) value;

            unimplemented();
            break;
        }
        default:
            report_error(&value->pos, "cannot resolve constant expression '%s'", node_names[value->type]);
            return NULL;
    }


    return ret;
}

static int resolve_constexpr(struct slist_head *value, struct slist_head *type) {
    // Note. value must be a tree    
    struct slist_head new_value;
    struct cell_node *n = slist_first_entry(value, struct cell_node, node);


    assert(n);

    INIT_SLIST_HEAD(&new_value);

    struct cell_node *r;

    if (!(r = resolve_constexpr_node(n, type)))
        return -1;

    if (r != n) {
        slist_del(&n->node, value);
        free(n);

        assert(slist_empty(value));

        slist_add(&r->node, value);
    }



    return 0;
}

int cell_module_analyze(struct cell_module *mod) {
    struct cell_vardecl *v;
    struct cell_typenode *n;

    dprintf("Resolving types...\n");

    slist_for_each_entry(n, &mod->scope.unres_types, res) {
        if (!n->type && !n->typedecl) {
            struct __cell_scope *s;

            for (s = &mod->scope; s; s = s->parent) {
                struct cell_typedecl *decl;

                slist_for_each_entry(decl, &s->types, hdr.node) {
                    if (strcmp(decl->name, n->name) == 0) {
                        report_debug(&n->node.pos, "resolved type '%s'", n->name);
                        n->typedecl = &decl->hdr;
                        break;
                    }
                }

                if (n->typedecl)
                    break;
            }
        }
    }

    dprintf("Resolving constant expressions...\n");

    slist_for_each_entry(v, &mod->scope.variables, hdr.node) {
        if (!slist_empty(&v->value)) {
            if (resolve_constexpr(&v->value, &v->type) != 0)
                return -1;
        }


    }



    return 0;
}
