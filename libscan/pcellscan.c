/*
 * Copyright (C) 2023-2014 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <flos/utils.h>

#include <cell/scan.h>
#include <cell/source.h>
#include <cell/token.h>

#include "../libutils/utils.h"

#define NOFMT  "\033[0m"
#define GREEN  "\033[38;5;35m"
#define GRAY   "\033[38;5;254m"
#define YELLOW "\033[38;5;226m"
#define PINK   "\033[38;5;206m"
#define AQUA   "\033[38;5;14m"

static void usage(const char *name) {
    fprintf(stderr, "usage: %s\n", name);
    exit(1);
}

static int scan(const char *filename) {
    char *err = NULL;
    cell_source_t *src = NULL;

    if ((err = cell_source_init_file(filename, &src))) {
        goto error0;
    }

    cell_scanner_t *scn = NULL;

    if ((err = cell_scanner_init(src, &scn))) {
        goto error1;
    }

    printf("Scan result (%s): \n", filename);
    printf(" %-5d | ", cell_scanner_pos(scn).start.row);

    cell_token_t token;

    do {
        err = cell_scanner_consume(scn);
        token = cell_scanner_token(scn);

        switch (token) {
        case CELL_TOKEN_COMMENT:
            {
                const char *comment = cell_scanner_text(scn);
                int col = cell_scanner_pos(scn).start.col;

                printf(GREEN "//");
                for (int i = 0; comment[i]; i++) {
                    if (comment[i] == '\n') {
                        printf(NOFMT);
                    }
                    printf("%c", comment[i]);
                    if (comment[i] == '\n') {
                        printf(" %-5d | " GREEN "//", col++);
                    }
                }
                printf(NOFMT);
                break;
            }
        case CELL_TOKEN_IDENT:
            printf(YELLOW "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_NUMBER:
            printf(AQUA "%ld|0x%lX" NOFMT, cell_scanner_num(scn), cell_scanner_num(scn));
            break;
        case CELL_TOKEN_FLOAT:
            printf(AQUA "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_CHAR:
            printf(YELLOW "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_STR:
            printf(AQUA "%s|%s" NOFMT, cell_scanner_text(scn), cell_scanner_str(scn));
            break;
        case CELL_TOKEN_OPER:
            printf(GRAY "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_NEWLINE:
            printf("\\n");
            printf("\n %-5d | ", cell_scanner_pos(scn).end.row);
            continue;
        case CELL_TOKEN_COMMA:
            printf(GREEN "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_META:
            printf(GREEN "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_DOT:
            printf(GREEN "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_INTERVAL:
            printf(GREEN "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_ELLIPSIS:
            printf(GREEN "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_INVALID:
            printf(PINK "%s" NOFMT, cell_scanner_text(scn));
            break;
        case CELL_TOKEN_EOF:
            break;
        default:
            if (token >= CELL_TOKEN_MODULE && token <= CELL_TOKEN_AS) {
                printf(PINK "%s" NOFMT, cell_tokens[token]);
            } else if (token >= CELL_TOKEN_LQBRACK && token < CELL_TOKEN_LAST_OPER) {
                printf(GRAY "%s " NOFMT, cell_scanner_text(scn));
            } else {
                printf("?%s", cell_tokens[token]);
            }
            break;
        }
        printf(" ");

        if (err) {
            fprintf(stderr, "%s\n", err);
            free(err);
        }

        if (token == CELL_TOKEN_EOF) {
            break;
        }
    } while (token != CELL_TOKEN_EOF);

    printf("\n");

    cell_scanner_free(scn);

error1:
    cell_source_free(src);

error0:
    return 0;
}

int main(int argc, char *argv[]) {
    int ret = 0;
    utf8_char c;
    char *optarg;

    while ((c = utils_getopt(&argc, &argv, &optarg, "l"))) {
        switch (c) {
        case 'l':
            print_location = 0;
            break;
        default:
            fprintf(stderr, "error: unrecognised program argument '%s'\n", optarg);
            usage(argv[0]);
        }
    }

    if (argc < 1) {
        ret = scan("/dev/stdin");
    } else {
        for (; argc; argv++, argc--) {
            if ((ret = scan(argv[0]))) {
                break;
            }
        }
    }

    return ret;
}
