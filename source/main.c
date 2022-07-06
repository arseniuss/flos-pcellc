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
#include <stdlib.h>

#include <cell/generators.h>
#include <cell/parser.h>
#include <cell/scanner.h>
#include <cell/token.h>

int scan(const char *f) {
    int r;
    struct scanner* scn;

    if (!(scn = cell_scanner_init(f)))
        return -1;

    printf("Scan result: \n");

    while ((r = cell_scanner_consume(scn)) > 0) {
        int token = cell_scanner_token(scn);

        switch (token) {
            case TOKEN_NEWLINE:
                printf("\\n\n");
                break;
            case TOKEN_NUMBER:
                printf("%ld|0x%lX ", cell_scanner_num(scn), cell_scanner_num(scn));
                break;
            case TOKEN_OPER:
            case TOKEN_IDENT:
                printf("%s(i) ", cell_scanner_text(scn));
                break;
            case TOKEN_COMMENT:
                printf("%s(c) ", cell_scanner_text(scn));
                break;
            case TOKEN_STR:
            case TOKEN_CHAR:

                printf("\"%s\" ", cell_scanner_str(scn));
                break;
            default:
                if (token >= TOKEN_MODULE && token <= TOKEN_FOR) {
                    printf("%s(k) ", cell_tokens[token]);
                } else if (token >= TOKEN_LPAREN && token <= TOKEN_AT) {
                    printf("%s ", cell_scanner_text(scn));
                } else {
                    printf("?");
                }

        }
        fflush(stdout);
    }
    printf("\n");

    if (r != 0) {
        const char * text = cell_scanner_text(scn);

        printf("nok %s\n", text);
        return 1;
    }
    printf("ok\n");

    return 0;
}

static void visit_node() {

}

struct cell_module* parse(const char *f) {
    struct scanner* scn;
    struct parser* prs;
    struct cell_module *mod;

    if (!(scn = cell_scanner_init(f)))
        return NULL;

    if (!(prs = cell_parser_init(scn)))
        return NULL;

    printf("Parsing...\n");

    if (!(mod = cell_parser_parse(prs))) {
        printf("Parse fail");
        return NULL;
    }

    printf("-------------\n");

    printf("ok\n");

    return mod;
}

int analyze(struct cell_module *m) {
    int r;

    printf("Analyzing...\n");

    r = cell_module_analyze(m);

    printf("-------------\n");
    if (r != 0)
        printf("FAIL");

    return r;
}

int generate(struct cell_module *m) {
    int r;

    printf("Generating...\n");

    r = cell_llvm_generate(m, printf);

    printf("-------------\n");

    return r;
}

int main(int argc, char** argv) {


    if (argc != 2) {
        fprintf(stderr, "error: source file is not provided");
        return -1;
    }

    if (scan(argv[1]) != 0)
        return -1;

    struct cell_module * mod = parse(argv[1]);
    if (!mod)
        return -1;

    if (analyze(mod) != 0)
        return -1;

    if (generate(mod) != 0)
        return -1;



    return 0;
}
