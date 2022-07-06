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

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cell/declarations.h>
#include <cell/expression.h>
#include <cell/node.h>
#include <cell/operator.h>
#include <cell/parser.h>
#include <cell/scope.h>
#include <cell/token.h>
#include <cell/types.h>
#include <slist.h>

#include "parser.h"
#include "scanner.h"
#include "utils.h"

#define CONSUME() \
                do { \
                    r = cell_scanner_consume(prs->scanner); \
                    if(r < 0) goto error; \
                    t = cell_scanner_token(prs->scanner); \
                } while(0)

#define EATLINE() \
                do { \
                    CONSUME(); \
                } while(t != TOKEN_NEWLINE && t != TOKEN_EOF)

struct op {
    struct slist_node list;
    struct pos pos;
    char op[5];
};

static int parse_type(struct parser *prs, const char *part, struct slist_head* root,
        struct __cell_scope *scope);
static void dump(struct slist_head* root, int lvl);

static struct op* op_init(const char *str) {
    struct op *o;

    if (!(o = (struct op *) malloc(sizeof (struct op)))) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    INIT_SLIST_NODE(&o->list);
    o->op[0] = 0;
    strcat(o->op, str);

    return o;
}

static void dump_node(struct cell_node *n, int lvl) {
    if (!n) return;

    for (int i = 0; i < lvl; i++) dprintf("  ");
    dprintf("%s ", node_names[n->type]);
    fflush(stdout);

    switch (n->type) {
        case NODE:
            assert(0);
            break;
        case NODE_NUMBER:
        {
            struct cell_numnode *m = (struct cell_numnode *) n;

            dprintf("%ld\n", m->value);
            break;
        }
        case NODE_STRING:
        {
            struct cell_strnode *m = (struct cell_strnode *) n;

            dprintf("\"%s\"\n", m->str);
            break;
        }
        case NODE_CHAR:
        {
            break;
        }
        case NODE_MODULE:
            assert(0);
            break;
        case NODE_UNOPER:
        {
            struct cell_unopnode *m = (struct cell_unopnode *) n;

            dprintf("%s\n", m->op);
            dump_node(m->target, lvl + 1);

            break;
        }
        case NODE_BINOPER:
        {
            struct cell_binopnode *m = (struct cell_binopnode *) n;
            dprintf("%s\n", m->op);
            dump_node(m->left, lvl + 1);
            dump_node(m->right, lvl + 1);

            break;
        }
        case NODE_IDENT:
        {
            struct cell_identnode *m = (struct cell_identnode *) n;

            dprintf("%s\n", m->name);

            break;
        }
        case NODE_TYPENODE:
        {
            struct cell_typenode *m = (struct cell_typenode *) n;

            dprintf("name:%s", m->name);
            if (m->type)
                dprintf(" %s", m->type->name);
            dprintf("\n");

            break;
        }
        case NODE_SLICENODE:
        {
            struct cell_slicenode *m = (struct cell_slicenode *) n;

            dprintf("\n");

            break;
        }
        case NODE_ARRNODE:
        {
            struct cell_arrnode *m = (struct cell_arrnode *) n;

            dprintf("cap %ld\n", m->cap);

            break;
        }
        case NODE_STRUCTNODE:
        {
            struct cell_structnode *m = (struct cell_structnode *) n;
            struct cell_node *p;

            dprintf("\n");

            slist_for_each_entry(p, &m->fields, node) {
                dump_node(p, lvl + 1);
            }

            break;
        }
        case NODE_SCOPE:
        {
            struct cell_scope *m = (struct cell_scope *) n;

            dprintf("\n");

            dump(&m->scope.imports, lvl + 1);
            dump(&m->scope.types, lvl + 1);
            dump(&m->scope.variables, lvl + 1);
            dump(&m->scope.funcs, lvl + 1);
            break;
        }
        case NODE_VARDECL:
        {
            struct cell_vardecl *m = (struct cell_vardecl *) n;

            dprintf("%s\n", m->name);
            dump(&m->type, lvl + 1);
            dump(&m->value, lvl + 1);

            break;
        }
        case NODE_TYPEDECL:
        {
            struct cell_typedecl *m = (struct cell_typedecl *) n;

            dprintf("%s\n", m->name);
            dump(&m->type, lvl + 1);

            break;
        }
        case NODE_FUNCDECL:
        {
            struct cell_funcdecl *m = (struct cell_funcdecl *) n;

            if (m->objname[0])
                dprintf("object:%s ", m->objname);
            dprintf("name:%s proto:%d\n", m->name, (m->flags & FUNC_PROTO != 0));
            dump(&m->objtype, lvl + 1);
            dump(&m->args, lvl + 1);
            dump(&m->rettype, lvl + 1);

            break;
        }
        default:
            assert(0);
            break;
    }
}

static void dump(struct slist_head* root, int lvl) {


    if (slist_empty(root)) {
        for (int i = 0; i < lvl; i++) dprintf("  ");
        dprintf("(empty)\n");
        return;
    }

    struct cell_node *n;

    slist_for_each_entry(n, root, node) {
        dump_node(n, lvl);
    }

    fflush(stdout);
}

static int make_tree(struct slist_head* operators, int curr_pres, struct slist_head *root) {
    struct op *first = slist_first_entry(operators, struct op, list);

    if (!first) {
        dprintf("no oper\n");
        return 0;
    }

    int first_pres = cell_precedence(first->op);

    if (curr_pres > first_pres) {
        dprintf("curr prec > first prec\n");
        return 0;
    }

    struct cell_node *n1 = slist_first_entry(root, struct cell_node, node);

    assert(n1);

    slist_del(&n1->node, root);

    if (slist_empty(root)) {
        struct cell_unopnode *n;

        n = (struct cell_unopnode *) malloc(sizeof (struct cell_unopnode));
        if (!n) {
            fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
            return -1;
        }

        INIT_SLIST_NODE(&n->node.node);
        n->node.type = NODE_UNOPER;
        memcpy(&n->node.pos, &first->pos, sizeof (struct pos));

        strcat(n->op, first->op);
        n->target = n1;
        slist_add(&n->node.node, root);
    } else {
        struct cell_node *n2 = slist_first_entry(root, struct cell_node, node);
        struct cell_binopnode *n;

        assert(n2);

        slist_del(&n2->node, root);

        n = (struct cell_binopnode *) malloc(sizeof (struct cell_binopnode));
        if (!n) {
            fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
            return -1;
        }

        memset(n, 0, sizeof (*n));

        INIT_SLIST_NODE(&n->node.node);
        n->node.type = NODE_BINOPER;
        memcpy(&n->node.pos, &first->pos, sizeof (struct pos));

        n->op[0] = 0;
        strcat(n->op, first->op);
        n->left = n2;
        n->right = n1;
        dprintf("making %ld %s %ld\n", ((struct cell_numnode *) n2)->value,
                first->op, ((struct cell_numnode *) n1)->value);
        slist_add(&n->node.node, root);
    }

    slist_del(&first->list, operators);

    free(first);

    return 1;
}

static int parse_arg_list(struct parser *prs, struct slist_head* fields, struct __cell_scope *scope) {
    int t, r;

    do {
        struct slist_head args;

        INIT_SLIST_HEAD(&args);

        t = cell_scanner_token(prs->scanner);

        if (t == TOKEN_IDENT) {
            do {
                struct cell_vardecl *v;

                v = cell_vardecl_init();
                if (!v) {
                    fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
                    return -1;
                }

                strcat(v->name, cell_scanner_text(prs->scanner));
                memcpy(&v->hdr.pos, &prs->scanner->pos, sizeof (struct pos));
                slist_add_tail(&v->hdr.node, &args);

                CONSUME();
                if (t == TOKEN_COMMA) {
                    CONSUME();
                    continue;
                }
                break;

            } while (1);
        }


        struct slist_head type;

        INIT_SLIST_HEAD(&type);

        if (parse_type(prs, "", &type, scope) != 0)
            goto error;

        struct cell_vardecl *p;

        slist_for_each_entry(p, &args, hdr.node) {
            memcpy(&p->type, &type, sizeof (struct slist_head));
        }

        slist_append(&args, fields);

        if (t == TOKEN_COMMA) {
            CONSUME();
            continue;
        }

        break;
    } while (1);



    return 0;
error:
    return -1;
}

static int parse_ident_expr(struct parser *prs, struct slist_head *root, struct __cell_scope *scope) {
    int r, t;

    t = cell_scanner_token(prs->scanner);

    switch (t) {
        case TOKEN_IDENT:
        {
            struct cell_identnode* n;

            if (!(n = cell_identnode_init())) return -1;

            memcpy(&n->node.pos, &prs->scanner->pos, sizeof (struct pos));

            strcat(n->name, cell_scanner_text(prs->scanner));

            slist_add_tail(&n->node.node, root);
            slist_add_tail(&n->resnode, &scope->unres_ident);

            CONSUME();

            break;
        }
        default:
            goto error;
    }

    return 0;
error:
    return 1;
}

static int parse_expr(struct parser *prs, const char *part, struct slist_head *root, struct __cell_scope *scope) {
    int t, r;
    struct slist_head operators;

    INIT_SLIST_HEAD(&operators);

    t = cell_scanner_token(prs->scanner);

    do {
        t = cell_scanner_token(prs->scanner);

        switch (t) {
            case TOKEN_IDENT:
            {
                if (parse_ident_expr(prs, root, scope) != 0) {
                    report_error(&prs->scanner->pos, "malformed identity expression");
                    goto error;
                }
                break;
            }
            case TOKEN_RPAREN:
                CONSUME();

                while (!slist_empty(&operators)) {
                    if (!make_tree(&operators, 0, root)) {
                        report_error(&prs->scanner->pos, "malformed expression");
                        goto error;
                    }
                }


                return 0;
            case TOKEN_LPAREN:
            {
                struct slist_head new;

                INIT_SLIST_HEAD(&new);

                CONSUME();

                if (parse_expr(prs, part, &new, scope) != 0)
                    goto error;

                if (!slist_empty(&new)) {
                    slist_append(&new, root);
                    make_tree(&operators, 0, root);
                } else {
                    report_error(&prs->scanner->pos, "empty expression");
                    goto error;
                }

                break;
            }
            case TOKEN_NUMBER:
            {
                struct cell_numnode *n = (struct cell_numnode *) malloc(sizeof (struct cell_numnode));
                if (!n) {
                    fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
                    return -1;
                }

                INIT_SLIST_NODE(&n->node.node);
                n->node.type = NODE_NUMBER;
                memcpy(&n->node.pos, &prs->scanner->pos, sizeof (struct pos));

                n->value = prs->scanner->value.num;

                slist_add(&n->node.node, root);
                dprintf("number %ld\n", n->value);

                CONSUME();

                break;
            }
            case TOKEN_STR:
            {
                struct cell_strnode *n = (struct cell_strnode *) malloc(sizeof (struct cell_strnode));
                if (!n) {
                    fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
                    return -1;
                }

                INIT_SLIST_NODE(&n->node.node);
                n->node.type = NODE_STRING;
                memcpy(&n->node.pos, &prs->scanner->pos, sizeof (struct pos));

                n->str[0] = 0;
                n->tmpname[0] = 0;
                strcat(n->str, cell_scanner_str(prs->scanner));

                slist_add_tail(&n->node.node, root);

                CONSUME();

                break;
            }
            case TOKEN_CHAR:
            {
                struct cell_chrnode *n = (struct cell_chrnode *) malloc(sizeof (struct cell_chrnode));
                if (!n) {
                    fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
                    return -1;
                }

                INIT_SLIST_NODE(&n->node.node);
                n->node.type = NODE_CHAR;
                memcpy(&n->node.pos, &prs->scanner->pos, sizeof (struct pos));

                n->chr = cell_scanner_chr(prs->scanner);
                n->text[0] = 0;
                strcat(n->text, cell_scanner_text(prs->scanner));

                slist_add_tail(&n->node.node, root);

                CONSUME();

                break;
            }
            case TOKEN_NEWLINE:
            {
                int expr_done = 1;
                int p = cell_scanner_prevtoken(prs->scanner);
                dprintf("newline\n");

                if (p >= TOKEN_PLUS && p <= TOKEN_AT)
                    expr_done = 0;

                if (expr_done) {
                    while (!slist_empty(&operators)) {
                        if (!make_tree(&operators, 0, root)) {
                            report_error(&prs->scanner->pos, "malformed expression");
                            goto error;
                        }
                    }

                    return 0;
                }
                CONSUME();
                break;
            }
            default:
                if (t == TOKEN_OPER || (t >= TOKEN_PLUS && t <= TOKEN_AT)) {
                    const char *op = cell_scanner_text(prs->scanner);
                    int curr_pres = cell_precedence(op);

                    if (curr_pres < 0) {
                        report_error(&prs->scanner->pos, "unrecognised operator '%s'", op);
                        goto error;
                    }

                    do {
                        if (slist_empty(&operators)) {
                            struct op *o = op_init(op);

                            if (!o) return -1;

                            memcpy(&o->pos, &prs->scanner->pos, sizeof (struct pos));
                            slist_add(&o->list, &operators);
                            dprintf("op empty: add %s to op\n", o->op);
                            break;
                        } else {
                            if (make_tree(&operators, curr_pres, root)) {
                                dprintf("made tree\n");
                                continue;
                            } else {
                                struct op *o = op_init(op);

                                if (!o) return -1;

                                memcpy(&o->pos, &prs->scanner->pos, sizeof (struct pos));
                                slist_add(&o->list, &operators);
                                dprintf("no tree: add %s to op\n", o->op);
                                break;
                            }
                        }
                    } while (1);

                    CONSUME();
                } else {
                    report_error(&prs->scanner->pos, "malformed expression, got %s", cell_tokens[t]);
                    goto error;
                }
                break;
        }
    } while (t != TOKEN_EOF);

    dprintf("DONE\n");

    return 0;
error:
    return -1;
}

static int parse_type(struct parser *prs, const char *part, struct slist_head* root,
        struct __cell_scope *scope) {
    int t, r;

    t = cell_scanner_token(prs->scanner);

    switch (t) {
        case TOKEN_STRUCT:
        {
            struct slist_head fields;
            unsigned flags = 0;

            INIT_SLIST_HEAD(&fields);


            CONSUME();

            while (t == TOKEN_IDENT) {

                if (strcmp(cell_scanner_text(prs->scanner), "packed") == 0) {
                    flags &= STRUCT_PACKED;
                } else {
                    report_error(&prs->scanner->pos, "unrecognised structure attribute '%s'",
                            prs->scanner->text);
                    goto error;
                }

                CONSUME();
            }

            if (t != TOKEN_LCBRACK) {
                report_error(&prs->scanner->pos, "structure expects curly brackets");
                goto error;
            }
            CONSUME();

            while (t != TOKEN_RCBRACK) {

                if (parse_arg_list(prs, &fields, scope) != 0)
                    goto error;

                t = cell_scanner_token(prs->scanner);

                if (t == TOKEN_NEWLINE) {
                    CONSUME();
                    continue;
                } else if (t == TOKEN_RCBRACK)
                    break;
            }

            struct cell_structnode *n = cell_structnode_init();
            if (!n) goto error;

            memcpy(&n->node.pos, &prs->scanner->pos, sizeof (struct pos));
            memcpy(&n->fields, &fields, sizeof (struct slist_head));
            n->flags = flags;
            slist_add(&n->node.node, root);

            CONSUME();

            break;
        }
        case TOKEN_IDENT:
        {
            const char *name = cell_scanner_text(prs->scanner);
            const struct cell_type *type;

            type = cell_type_find(name);

            struct cell_typenode *n = cell_typenode_init();
            if (!n) goto error;

            memcpy(&n->node.pos, &prs->scanner->pos, sizeof (struct pos));
            strcat(n->name, name);
            n->type = type;

            slist_add(&n->node.node, root);

            if (!type) {
                report_debug(&prs->scanner->pos, "unrecognised type '%s'", name);

                slist_add(&n->res, &scope->unres_types);
            }

            CONSUME();

            break;
        }
        case TOKEN_LQBRACK:
            CONSUME();
            if (t == TOKEN_RQBRACK) {
                /* slice */
                CONSUME();

                struct cell_slicenode *n = cell_slicenode_init();
                if (!n) goto error;

                if (parse_type(prs, part, &n->type, scope) != 0)
                    goto error;

                slist_add(&n->node.node, root);
            } else if (t == TOKEN_NUMBER) {
                /* array */
                uintmax_t cap = cell_scanner_num(prs->scanner);

                CONSUME();

                if (t != TOKEN_RQBRACK) {
                    report_error(&prs->scanner->pos, "mallformed array type");
                    goto error;
                }

                CONSUME();

                struct cell_arrnode *n = cell_arrnode_init();
                if (!n) goto error;

                n->cap = cap;

                if (parse_type(prs, part, &n->type, scope) != 0)
                    goto error;

                slist_add(&n->node.node, root);
            } else {
                report_error(&prs->scanner->pos, "unrecognised array index '%s'", cell_scanner_text(prs->scanner));
                goto error;
            }
            break;
        default:
            report_error(&prs->scanner->pos, "%s expects type, got '%s'", part, cell_tokens[t]);
            goto error;
            break;
    }

    if (t == TOKEN_META) {
        if (slist_empty(root)) {
            report_error(&prs->scanner->pos, "no type to add metadata");
            goto error;
        }
        struct cell_node *n = slist_first_entry(root, struct cell_node, node);

        switch (n->type) {
            case NODE_TYPENODE:
            {
                struct cell_typenode *t = (struct cell_typenode *) n;

                strcat(t->metadata, cell_scanner_text(prs->scanner));
                break;
            }
            default:
                report_error(&n->pos, "cannot add metadata to %s node", node_names[n->type]);
                goto error;
        }

        CONSUME();
    }

    return 0;
error:
    return -1;
}

static int parse_ident_path(struct parser *prs, char *path, const char *part) {
    int t, r;

    t = cell_scanner_token(prs->scanner);

    if (t == TOKEN_IDENT) {
        int got_dot;

        do {
            got_dot = 0;

            strcat(path, cell_scanner_text(prs->scanner));

            CONSUME();
            if (t == TOKEN_DOT) {
                strcat(path, cell_scanner_text(prs->scanner));
                CONSUME();
                got_dot = 1;
            }
        } while (got_dot);
    } else {
        report_error(&prs->scanner->pos, "%s is expecting identifier path", part);
        goto error;
    }

    return 1;
error:
    return 0;
}

static int parse_declmodule(struct parser *prs, struct cell_module* mod) {
    int t, r;
    char module[NAME_MAX] = {0};

    CONSUME();

    if (parse_ident_path(prs, module, "module declaration")) {
        t = cell_scanner_token(prs->scanner);

        if (t == TOKEN_COMMENT) CONSUME();
        if (t != TOKEN_NEWLINE && t != TOKEN_SEMICOLON) {
            report_error(&prs->scanner->pos, "module declaration expects newline at the end");
            goto error;
        }

        if (mod->name[0]) {
            report_error(&prs->scanner->pos, "module name is already set");
            goto error;
        }

        strcat(mod->name, module);

        return 0;
    }

error:
    return -1;
}

static int parse_declimport(struct parser *prs, struct cell_module* mod) {
    int t, r;

    char module[NAME_MAX] = {0};

    CONSUME();

    if (parse_ident_path(prs, module, "import declaration")) {
        char alias[NAME_MAX] = {0};
        char loc[NAME_MAX] = {0};

        t = cell_scanner_token(prs->scanner);

        if (t == TOKEN_AS) {
            CONSUME();
            if (cell_scanner_token(prs->scanner) == TOKEN_IDENT) {
                strcat(alias, cell_scanner_text(prs->scanner));
                CONSUME();
            } else {
                report_error(&prs->scanner->pos, "import declaration's as part requires identifier");
                goto error;
            }
        }

        if (t == TOKEN_FROM) {
            CONSUME();
            if (t == TOKEN_STR) {
                strcat(loc, cell_scanner_str(prs->scanner));
                CONSUME();
            }
        }

        if (t == TOKEN_COMMENT) CONSUME();
        if (t != TOKEN_NEWLINE && t != TOKEN_SEMICOLON) {
            report_error(&prs->scanner->pos, "expecting newline at end of import declaration");
            goto error;
        }

        struct cell_nodeimp *s;

        s = cell_nodeimp_init(module, loc, alias);
        if (!s) goto error;

        slist_add_tail(&s->hdr.node, &mod->scope.imports);

        return 0;
    }


error:
    return -1;
}

static int parse_decltype(struct parser *prs, struct __cell_scope *scope) {
    int r, t;
    struct cell_typedecl *decl;

    if (!(decl = cell_typedecl_init())) {
        report_error(&prs->scanner->pos, "alloc");
        goto error;
    }

    memcpy(&decl->hdr.pos, &prs->scanner->pos, sizeof (struct pos));

    CONSUME();

    if (t != TOKEN_IDENT) {
        report_error(&prs->scanner->pos, "expecting identifier in type declaration");
        goto error;
    }

    strcat(decl->name, cell_scanner_text(prs->scanner));

    CONSUME();

    if (parse_type(prs, "type declaration", &decl->type, scope) != 0)
        goto error;

    t = cell_scanner_token(prs->scanner);

    if (t != TOKEN_NEWLINE) {
        report_error(&prs->scanner->pos, "expecting newline at the end of type declaration");
        goto error;
    }

    slist_add_tail(&decl->hdr.node, &scope->types);

    return 0;
error:
    free(decl);
    EATLINE();
    return -1;
}

static int parse_declfunc(struct parser *prs, struct cell_module *mod) {
    int r, t;

    struct cell_funcdecl *decl;

    if (!(decl = cell_funcdecl_init())) {
        goto error;
    }

    memcpy(&decl->hdr.pos, &prs->scanner->pos, sizeof (struct pos));

    CONSUME();

    if (t == TOKEN_LPAREN) {
        CONSUME();

        if (t != TOKEN_IDENT) {
            report_error(&prs->scanner->pos, "expecting identifier for method");
            goto error;
        }

        strcat(decl->objname, cell_scanner_text(prs->scanner));

        if (parse_type(prs, "method", &decl->objtype, &mod->scope) != 0)
            goto error;

        t = cell_scanner_token(prs->scanner);

        if (t != TOKEN_RPAREN) {
            report_error(&prs->scanner->pos, "expecting '(' at end of object");
            goto error;
        }

        CONSUME();
    }

    if (t != TOKEN_IDENT) {
        report_error(&prs->scanner->pos, "expecting function name");
        goto error;
    }

    strcat(decl->name, cell_scanner_text(prs->scanner));

    CONSUME();

    if (t != TOKEN_LPAREN) {
        report_error(&prs->scanner->pos, "expecting '('");
        goto error;
    }

    CONSUME();

    if (parse_arg_list(prs, &decl->args, &mod->scope) != 0)
        goto error;

    t = cell_scanner_token(prs->scanner);
    if (t != TOKEN_RPAREN) {
        report_error(&prs->scanner->pos, "expecting ')'");
        goto error;
    }

    CONSUME();

    if (parse_type(prs, "function", &decl->rettype, &mod->scope))
        goto error;

    t = cell_scanner_token(prs->scanner);

    if (t == TOKEN_LCBRACK) {
        do {
            CONSUME();
            //TODO
        } while (t != TOKEN_RCBRACK && t != TOKEN_EOF);
    } else if (t == TOKEN_NEWLINE) {
        decl->flags &= FUNC_PROTO;
        CONSUME();
    } else {
        report_error(&prs->scanner->pos, "unrecognised token %s", cell_tokens[t]);
        goto error;
    }

    slist_add_tail(&decl->hdr.node, &mod->scope.funcs);

    return 0;
error:
    free(decl);
    EATLINE();
    return -1;
}

static int parse_declconst(struct parser *prs, struct cell_module *mod) {
    int r, t;


error:
    EATLINE();
    return -1;
}

static int parse_declvar(struct parser *prs, struct __cell_scope *scope) {
    int r, t;
    struct cell_vardecl *var;

    if (!(var = cell_vardecl_init())) {
        goto error;
    }

    memcpy(&var->hdr.pos, &prs->scanner->pos, sizeof (struct pos));

    CONSUME();

    if (t == TOKEN_IDENT) {
        strcat(var->name, cell_scanner_text(prs->scanner));

        CONSUME();
    } else {
        report_error(&prs->scanner->pos, "variable declaration excepts identifier");
        goto error;
    }

    if (t != TOKEN_EQ && t != TOKEN_NEWLINE && t != TOKEN_EOF) {
        if (parse_type(prs, "variable declaration", &var->type, scope) != 0)
            goto error;
        t = cell_scanner_token(prs->scanner);
    }

    if (t == TOKEN_EQ) {
        CONSUME();
        if (parse_expr(prs, "variable declaration", &var->value, scope) != 0)
            goto error;

        t = cell_scanner_token(prs->scanner);
    }

    if (t == TOKEN_COMMENT) CONSUME();
    if (t != TOKEN_NEWLINE && t != TOKEN_SEMICOLON) {
        report_error(&prs->scanner->pos, "expecting newline at end of variable declaration, got %s",
                cell_tokens[t]);
        goto error;
    }

    slist_add_tail(&var->hdr.node, &scope->variables);

    return 0;

error:
    EATLINE();
    return -1;
}

struct parser*
cell_parser_init(struct scanner *scn) {
    struct parser *prs;

    assert(scn != NULL);

    prs = (struct parser*) malloc(sizeof (struct parser));
    if (!prs) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    memset(prs, 0, sizeof (struct parser));

    prs->scanner = scn;
    prs->flags = PARSE_CONTINUE;

    return prs;
}

struct cell_module*
cell_parser_parse(struct parser *prs) {
    int t, r, c;
    struct cell_module* mod;

    mod = cell_module_init();
    if (!mod) {
        return NULL;
    }

    memcpy(&mod->hdr.pos, &prs->scanner->pos, sizeof (struct pos));
    strcat(mod->arch, "x86_64"); // TODO

    do {

        CONSUME();

        if (t == TOKEN_EOF)
            break;

        switch (t) {
            case TOKEN_FUNC:
                if (parse_declfunc(prs, mod) != 0)
                    if ((prs->flags & PARSE_CONTINUE) == 0)
                        goto error;
                break;
            case TOKEN_CONST:
                if (parse_declconst(prs, mod) != 0)
                    if ((prs->flags & PARSE_CONTINUE) == 0)
                        goto error;
                break;
            case TOKEN_MODULE:
                if (parse_declmodule(prs, mod) != 0)
                    if ((prs->flags & PARSE_CONTINUE) == 0)
                        goto error;
                break;
            case TOKEN_IMPORT:
                if (parse_declimport(prs, mod) != 0)
                    if ((prs->flags & PARSE_CONTINUE) == 0)
                        goto error;
                break;
            case TOKEN_TYPE:
                if (parse_decltype(prs, &mod->scope) != 0)
                    if ((prs->flags & PARSE_CONTINUE) == 0)
                        goto error;
                break;
            case TOKEN_VAR:
                if (parse_declvar(prs, &mod->scope) != 0)
                    if ((prs->flags & PARSE_CONTINUE) == 0)
                        goto error;
                break;
            case TOKEN_COMMENT:
            case TOKEN_NEWLINE:
                continue;
            default:
            {
                c = cell_scanner_token(prs->scanner);
                report_error(&prs->scanner->pos, "unrecognised token '%s'", prs->scanner->text);
                goto error;
            }
        }
    } while (r > 0);

    dump_node(&mod->hdr, 0);

    return mod;
error:
    free(mod);

    return NULL;
}
