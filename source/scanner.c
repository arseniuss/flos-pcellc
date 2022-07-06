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
#include <stdlib.h>
#include <string.h>

#include <utf8.h>

#include <cell/token.h>
#include <cell/keywords.h>
#include <cell/operator.h>

#include "scanner.h"

static void nextchar(struct scanner* scn);
static int peekchar(struct scanner* scn);

struct scanner*
cell_scanner_init(const char *filename) {
    FILE *f;
    struct scanner *scn;

    if (!(f = fopen(filename, "r"))) {
        fprintf(stderr, "error: cannot open file %s\n", filename);
        return NULL;
    }

    scn = (struct scanner*) malloc(sizeof (struct scanner));
    if (!scn) {
        fprintf(stderr, "error: alloc@%s:%d", __FILE__, __LINE__);
        return NULL;
    }

    scn->f = f;
    scn->pos.line = 1;
    scn->pos.pos = 1;
    scn->chr = 0;
    scn->letter[0] = '\0';
    scn->text[0] = '\0';
    scn->token = TOKEN_INVALID;
    scn->prev_token = TOKEN_INVALID;
    strcat(scn->pos.filename, filename);

    return scn;
}

int
cell_scanner_token(struct scanner *scn) {
    return scn->token;
}

int
cell_scanner_prevtoken(struct scanner *scn) {
    return scn->prev_token;
}

const char *
cell_scanner_text(struct scanner *scn) {
    return scn->text;
}

const char *
cell_scanner_str(struct scanner *scn) {
    assert(scn->token == TOKEN_STR ||
            scn->token == TOKEN_CHAR);

    return scn->value.str;
}

utf8_char
cell_scanner_chr(struct scanner *scn) {
    assert(scn->token == TOKEN_CHAR);

    return scn->value.ch;
}

uintmax_t
cell_scanner_num(struct scanner *scn) {
    assert(scn->token == TOKEN_NUMBER);

    return scn->value.num;
}

int REQUIRE_NEWLINE(struct scanner *scn) {
    return
    scn->token == TOKEN_IDENT ||
            scn->token == TOKEN_STR ||
            scn->token == TOKEN_COMMENT ||
            scn->token == TOKEN_NUMBER ||
            scn->token == TOKEN_RCBRACK ||
            scn->token == TOKEN_RPAREN ||
            scn->token == TOKEN_CHAR;
}

static void set_token(struct scanner *scn, int token) {
    scn->prev_token = scn->token;
    scn->token = token;
}

int
cell_scanner_consume(struct scanner *scn) {

    if (scn->chr == utf8_error && scn->letter[0] == -1) {
        set_token(scn, TOKEN_EOF);
        strcpy(scn->text, cell_tokens[TOKEN_EOF]);
        return 0;
    }

    scn->text[0] = '\0';

    /* skip whitespace */
    while (utf8_isspace(scn->chr) || scn->chr == 0) {
        if (scn->chr == '\n' && REQUIRE_NEWLINE(scn)) {
            set_token(scn, TOKEN_NEWLINE);
            strcpy(scn->text, cell_tokens[TOKEN_NEWLINE]);
            nextchar(scn);
            goto do_return;
        }
        nextchar(scn);
    }


    switch (scn->chr) {
        case ';':
            set_token(scn, TOKEN_NEWLINE);
            strcpy(scn->text, cell_tokens[TOKEN_NEWLINE]);
            nextchar(scn);
            goto do_return;
        case '`':
            nextchar(scn);
            while (scn->chr != '\n' && scn->chr != utf8_error) {
                strcat(scn->text, scn->letter);
                nextchar(scn);
            }
            set_token(scn, TOKEN_META);
            goto do_return;
        case '/':
            if (peekchar(scn) == '/') {
                nextchar(scn);
                nextchar(scn);

                int ch = scn->chr;

                while (ch != '\n' && ch != utf8_error) {
                    strcat(scn->text, scn->letter);
                    nextchar(scn);
                    ch = scn->chr;
                }

                set_token(scn, TOKEN_COMMENT);
                goto do_return;
            }
            break;

#define __TOKEN_TYPE TOKEN_BRACKETS
#define __(u, l, str, chr) \
        case chr: \
            set_token(scn, TOKEN_##u); \
            goto do_sym;
#include <cell/token.inc.h>

do_sym:
            strcpy(scn->text, scn->letter);

            nextchar(scn);
            goto do_return;
    }

    /* identifiers and keywords */
    if (utf8_isalpha(scn->chr) || scn->chr == '_') {
        int i;

        /* FIXME: overflow */
        do {
            strcat(scn->text, scn->letter);

            nextchar(scn);
        } while (utf8_isalpha(scn->chr) || scn->chr == '_' || utf8_isdigit(scn->chr));

        set_token(scn, TOKEN_IDENT);

        for (i = 0; i < KEYWORD_LAST; i++) {
            const char *keyword = cell_keywords[i];

            if (strcmp(scn->text, keyword) == 0) {
                set_token(scn, i + TOKEN_MODULE);
                break;
            }
        }

        goto do_return;
    }

    /* string/character literals */
    if (scn->chr == '"' || scn->chr == '\'') {
        const utf8_char lit = scn->chr;
        int escape = 0;
        int num = 0;

        scn->value.str[0] = '\0';

        for (;;) {
            nextchar(scn);
            if (scn->chr == lit && !escape) {
                if (lit == '\'' && num != 1) {
                    return -1;
                }
                nextchar(scn);
                break;
            }
            escape = 0;
            if (scn->chr == utf8_error)
                break;
            if (scn->chr == '\\') {
                escape = 1;
            } else {
                strcat(scn->value.str, scn->letter);
            }
            strcat(scn->text, scn->letter);
            num++;
        }


        if (lit == '\'') {
            utf8_str2chr(&scn->value.ch, scn->text);
            set_token(scn, TOKEN_CHAR);
        } else
            set_token(scn, TOKEN_STR);

        goto do_return;
    }

    if (strchr(cell_operators, scn->chr)) {
        for (;;) {
            strcat(scn->text, scn->letter);
            nextchar(scn);
            if (!strchr(cell_operators, scn->chr))
                break;
        }

        if (strlen(scn->text) == 1) {
            set_token(scn, TOKEN_PLUS + strchr(cell_operators, scn->text[0]) - cell_operators);
        } else
            set_token(scn, TOKEN_OPER);
        goto do_return;
    }

    /* number */
    if (utf8_isdigit(scn->chr)) {
        int base = 10;
        int pnt = 0;
        int exp = 0;
        int flt = 0;

        if (scn->chr == '0') {
            utf8_char prefix;

            strcat(scn->text, scn->letter);
            nextchar(scn);

            prefix = utf8_tolower(scn->chr);

            switch (prefix) {
                case 'o':
                    base = 8;
                    break;
                case 'x':
                    base = 16;
                    break;
                case 'b':
                    base = 2;
                    break;
                default:
                    /* TODO: get full number */
                    return -1;
            }
        }

        for (;;) {
            int ok = 0;

            strcat(scn->text, scn->letter);
            nextchar(scn);

            switch (scn->chr) {
                case '.':
                    if (pnt || (base != 10 && base != 16)) {
                        strcat(scn->text, scn->letter);
                        /* TODO: invalid floating point number */
                        return -1;
                    }
                    pnt = 1;
                    continue;
                case 'e':
                case 'E':
                    if (exp || base != 10) {
                        strcat(scn->text, scn->letter);
                        /* TODO: invalid floating point number */
                        return -1;
                    }
                    exp = 1;
                    continue;
                case 'p':
                case 'P':
                    if (exp || base != 16) {
                        strcat(scn->text, scn->letter);
                        /* TODO: invalid floating point number */
                        return -1;
                    }
                    exp = 1;
                    continue;
                case '+':
                case '-':
                    if (pnt)
                        continue;
                    break;
            }

            /*TODO: finish*/

            switch (base) {
                case 2:
                    ok = scn->chr == '0' || scn->chr == '1' || scn->chr == '_';
                    break;
                case 8:
                    ok = (scn->chr >= '0' && scn->chr <= '7') || scn->chr == '_';
                    break;
                case 10:
                    ok = (scn->chr >= '0' && scn->chr <= '9') || scn->chr == '_';
                    break;
                case 16:
                    ok = (scn->chr >= '0' && scn->chr <= '9') ||
                            (scn->chr >= 'a' && scn->chr <= 'f') ||
                            (scn->chr >= 'A' && scn->chr <= 'F') ||
                            scn->chr == '_';
                    break;
            }
            if (!ok)
                break;
        }

        if (flt)
            set_token(scn, TOKEN_FLOAT);
        else {
            int i = 0;
            uintmax_t n = 0;
            char ch;

            for (i = 0; scn->text[i]; i++) {
                ch = scn->text[i];
                if (ch == '_')
                    continue;
                n *= base;
                if (ch >= '0' && ch <= '9')
                    n += ch - '0';
                else if (ch >= 'a' && ch <= 'f')
                    n += ch - 'a' + 10;
                else if (ch >= 'A' && ch <= 'F')
                    n += ch - 'A' + 10;
            }

            scn->value.num = n;
            set_token(scn, TOKEN_NUMBER);
        }
        goto do_return;
    }

    strcpy(scn->text, scn->letter);

    return -1;
do_return:
    return 1;
}

static void
nextchar(struct scanner* scn) {
    int len = 0;

    scn->chr = 0;
    scn->letter[len] = '\0';

    do {
        int ch;

        if (scn->peekchr) {
            ch = scn->peekchr;
            scn->peekchr = 0;
        } else {
            ch = fgetc(scn->f);
        }

        if (ch == '\n') {
            scn->pos.line++;
            scn->pos.pos = 1;
        } else {
            scn->pos.pos++;
        }

        if (ch == EOF) {
            scn->chr = utf8_error;
            scn->letter[len] = -1;
            return;
        }

        scn->letter[len++] = ch;
        scn->letter[len] = '\0';
    } while (utf8_strnlen(scn->letter, len) == 0);

    utf8_nstr2chr(&scn->chr, scn->letter, len);
}

static int
peekchar(struct scanner* scn) {
    return (scn->peekchr = fgetc(scn->f));
}
