/*
 * Copyright (C) 2024 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
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
#include <ctype.h>

#include <cell/scan.h>
#include <cell/token.h>

#include "../libutils/utils.h"
#include "scanner.h"

const char *cell_tokens[CELL_TOKEN_LAST] = {
#define __(u, l, str, chr) str,
#include <cell/token.inc.h>
};

const char cell_operators[CELL_OPERATOR_LAST + 1] = {
#define __TOKEN_TYPE       CELL_TOKEN_OPERATORS
#define __(u, l, str, chr) chr,
#include <cell/token.inc.h>
    0};

const char *cell_keywords[CELL_KEYWORD_LAST] = {
#define __TOKEN_TYPE       CELL_TOKEN_KEYWORDS
#define __(u, l, str, chr) str,
#include <cell/token.inc.h>
};

char *cell_scanner_init(cell_source_t *src, cell_scanner_t **scn) {
    char *err = NULL;
    cell_scanner_t *scanner;

    alloc_mem(scanner, err, ret);

    memset(scanner, 0, sizeof(cell_scanner_t));

    src->rewind(src);

    scanner->source = src;
    scanner->pos.start.row = scanner->pos.start.col = 1;
    scanner->pos.end.row = scanner->pos.end.col = 1;

    scanner->token = scanner->prev_token = CELL_TOKEN_INVALID;

    *scn = scanner;

ret:
    return err;
}

void cell_scanner_free(cell_scanner_t *scn) { free(scn); }

cell_token_t cell_scanner_token(cell_scanner_t *scn) { return scn->token; }
cell_pos_t cell_scanner_pos(cell_scanner_t *scn) { return scn->pos; }
uintmax_t cell_scanner_num(cell_scanner_t *scn) { return scn->value.num; }
const char *cell_scanner_text(cell_scanner_t *scn) { return scn->text; }
const char *cell_scanner_str(cell_scanner_t *scn) { return scn->value.str; }

static int require_newline(cell_scanner_t *scn) {
    return scn->token == CELL_TOKEN_META || scn->token == CELL_TOKEN_IDENT || scn->token == CELL_TOKEN_RETURN ||
           scn->token == CELL_TOKEN_STR || scn->token == CELL_TOKEN_COMMENT || scn->token == CELL_TOKEN_NUMBER ||
           scn->token == CELL_TOKEN_RQBRACK || scn->token == CELL_TOKEN_RCBRACK || scn->token == CELL_TOKEN_RPAREN ||
           scn->token == CELL_TOKEN_CHAR;
}

static void nextchar(cell_scanner_t *scn) {
    int len = 0;

    scn->chr = 0;
    scn->letter[len] = '\0';

    do {
        int ch;

        if (scn->peekchr) {
            ch = scn->peekchr;
            scn->peekchr = 0;
        } else {
            ch = scn->source->getc(scn->source);
        }

        if (ch == '\n') {
            scn->pos.end.line_pos = scn->source->ftell(scn->source);
            scn->pos.end.row++;
            scn->pos.end.col = 1;
        } else {
            scn->pos.end.col++;
        }

        if (ch == -1) {
            scn->chr = utf8_error;
            scn->letter[len] = -1;
            return;
        }

        scn->letter[len++] = ch;
        scn->letter[len] = '\0';
    } while (utf8_strnlen(scn->letter, len) == 0);

    utf8_nstr2chr(&scn->chr, scn->letter, len);
}

static int peekchar(cell_scanner_t *scn) { return (scn->peekchr = scn->source->getc(scn->source)); }

#define CHECK(expr, label)                                                                                             \
    do {                                                                                                               \
        if ((err = expr))                                                                                              \
            goto label;                                                                                                \
    } while (0)

#define SET_TOKEN(t)                                                                                                   \
    do {                                                                                                               \
        scn->prev_token = scn->token;                                                                                  \
        scn->token = t;                                                                                                \
    } while (0)

static char *parse_escape(cell_scanner_t *scn) {
    char *err = NULL;

    assert(scn->chr == '\\');

    CHECK(safe_strmcat(&scn->text, scn->letter), ret);

    nextchar(scn);
    CHECK(safe_strmcat(&scn->text, scn->letter), ret);

    switch (scn->chr) {
    /* simple escapes */
    case '"':
    case '\\':
    case '\'':
    case '?':
        CHECK(safe_strcatf(scn->value.str, "%c", scn->chr), ret);
        break;
    /* special escapes */
    case '0':
        CHECK(safe_strcatf(scn->value.str, "%c", '\0'), ret);
        break;
    case 'a':
        CHECK(safe_strcatf(scn->value.str, "%c", 0x07), ret);
        break;
    case 'b':
        CHECK(safe_strcatf(scn->value.str, "%c", 0x08), ret);
        break;
    case 'f':
        CHECK(safe_strcatf(scn->value.str, "%c", 0x0C), ret);
        break;
    case 'n':
        CHECK(safe_strcatf(scn->value.str, "%c", 0x0A), ret);
        break;
    case 'r':
        CHECK(safe_strcatf(scn->value.str, "%c", 0x0D), ret);
        break;
    case 't':
        CHECK(safe_strcatf(scn->value.str, "%c", 0x09), ret);
        break;
    case 'v':
        CHECK(safe_strcatf(scn->value.str, "%c", 0x0B), ret);
        break;
    /* \x <hex> <hex> */
    case 'x':
        {
            char hex = 0;

            nextchar(scn);
            CHECK(safe_strmcat(&scn->text, scn->letter), ret);

            if (utf8_isxdigit(scn->chr)) {
                hex = utf8_xdigit2num(scn->chr) << 4;

                nextchar(scn);
                CHECK(safe_strmcat(&scn->text, scn->letter), ret);

                if (utf8_isxdigit(scn->chr)) {
                    hex |= utf8_xdigit2num(scn->chr);
                    CHECK(safe_strcatf(scn->value.str, "%c", hex), ret);
                    break;
                }
            }

            err = reportf(scn->source, &scn->pos, SEV_ERROR, "expecting two hexdecimal digits in escape sequence");
            goto ret;
        }
    /* \u  <hex>{1,6} */
    case 'u':
        {
            utf8_char value = 0;

            for (int i = 0; i < utf8_max; i++) {
                value = (value << 4) | utf8_xdigit2num(scn->chr);

                /* we don't know length of escape sequence */
                if (utf8_isxdigit(peekchar(scn))) {
                    nextchar(scn);
                    CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                } else {
                    break;
                }
            }

            char ch[utf8_max + 1] = {0};
            int len = utf8_chr2str(ch, &value);

            ch[len] = '0';
            CHECK(safe_strcatf(scn->value.str, "%s", ch), ret);
            break;
        }
    default:
        /* \ ( <octal-digit> ){1,3} */
        if (scn->chr >= '1' && scn->chr <= '7') {
            utf8_char value = 0;
            char ch[utf8_max + 1] = {0};

            for (int i = 0; i < 3; i++) {
                value = value * 8 + scn->chr - '0';

                /* we don't know length of escape sequence */
                if (utf8_isxdigit(peekchar(scn))) {
                    nextchar(scn);
                    CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                } else {
                    break;
                }
            }

            int len = utf8_chr2str(ch, &value);
            ch[len] = '0';
            CHECK(safe_strcatf(scn->value.str, "%s", ch), ret);
            break;
        }

        err = reportf(scn->source, &scn->pos, SEV_ERROR, "unrecognised escape character '%s'", scn->letter);
        goto ret;
    }

    nextchar(scn);

ret:
    return err;
}

static char *parse_base(cell_scanner_t *scn, int base) {
    char *err = NULL;

    while (isxdigit(scn->chr) || scn->chr == '_') {
        if (scn->chr != '_') {
            switch (base) {
            case 2:
                if (scn->chr != '1' && scn->chr != '0') {
                    err = reportf(scn->source, &scn->pos, SEV_ERROR, "expecting only binary digits");
                    goto ret;
                }
                break;
            case 8:
                if (scn->chr > '7') {
                    err = reportf(scn->source, &scn->pos, SEV_ERROR, "expecting only octal digits");
                    goto ret;
                }
                break;
            case 10:
                if (!isdigit(scn->chr)) {
                    err = reportf(scn->source, &scn->pos, SEV_ERROR, "expecting only decimal digits");
                    goto ret;
                }
            }
            CHECK(safe_strcat(scn->value.str, scn->letter), ret);
        }

        CHECK(safe_strmcat(&scn->text, scn->letter), ret);
        nextchar(scn);
    }

ret:
    return err;
}

static char *parse_number(cell_scanner_t *scn) {
    char *err = NULL;
    int base = 10;

    /* number */
    if (isdigit(scn->chr)) {
        if (scn->chr == '0') {
            CHECK(safe_strmcat(&scn->text, scn->letter), ret);
            nextchar(scn);

            switch (tolower(scn->chr)) {
            case 'o':
                base = 8;
                break;
            case 'x':
                base = 16;
                break;
            case 'b':
                base = 2;
                break;
            }

            if (base != 10) {
                CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                nextchar(scn);
            }
        }

        CHECK(parse_base(scn, base), ret);
    }

    if (scn->chr == '.') {
        CHECK(safe_strmcat(&scn->text, scn->letter), ret);
        nextchar(scn);
        CHECK(parse_base(scn, base), ret);
    } else {
        char *end;

        scn->value.num = strtoul(scn->text, &end, base);
        SET_TOKEN(CELL_TOKEN_NUMBER);
        goto ret;
    }

ret:
    return err;
}

static char *parse_oper(cell_scanner_t *scn) {
    char *err = NULL;

    /* operators */
    for (;;) {
        CHECK(safe_strmcat(&scn->text, scn->letter), ret);
        nextchar(scn);
        if (!strchr(cell_operators, scn->chr) || scn->chr == '(') {
            break;
        }
    }

    if (strlen(scn->text) == 1) {
        SET_TOKEN(CELL_TOKEN_INVALID_OPER + strchr(cell_operators, scn->text[0]) - cell_operators);
    } else {
        SET_TOKEN(CELL_TOKEN_OPER);
    }

ret:
    return err;
}

char *cell_scanner_consume(cell_scanner_t *scn) {
    char *err = NULL;
    int ch;

    if (scn->text) {
        free(scn->text);
        scn->text = 0;
    }
    memset(&scn->value, 0, sizeof(scn->value));

    /* skip whitespace */
    while (utf8_isspace(scn->chr) || scn->chr == 0) {
        if (scn->chr == '\n' && require_newline(scn)) {
            SET_TOKEN(CELL_TOKEN_NEWLINE);
            CHECK(safe_strmcat(&scn->text, cell_tokens[CELL_TOKEN_NEWLINE]), ret);

            nextchar(scn);
            goto ret;
        }
        nextchar(scn);
    }

    /* exit on end-of-file */
    if (scn->chr == utf8_error && scn->letter[0] == -1) {
        SET_TOKEN(CELL_TOKEN_EOF);
        CHECK(safe_strmcat(&scn->text, cell_tokens[CELL_TOKEN_EOF]), ret);
        goto ret;
    }

    /* let's begin! */
    memcpy(&scn->pos.start, &scn->pos.end, sizeof(cell_cursor_t));

    switch (scn->chr) {
    case '"':
        CHECK(safe_strmcat(&scn->text, scn->letter), ret);

        nextchar(scn);

        for (;;) {
            if (scn->chr == '\\') {
                CHECK(parse_escape(scn), ret);
            } else if (scn->chr == '"') {
                CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                break;
            } else if (scn->chr == utf8_error) {
                err = reportf(scn->source, &scn->pos, SEV_ERROR, "missing closing '\"'");
                goto ret;
            } else if (utf8_isprint(scn->chr)) {
                CHECK(safe_strcatf(scn->value.str, "%s", scn->letter), ret);
                CHECK(safe_strmcat(&scn->text, scn->letter), ret);

                nextchar(scn);
            } else {
                err = reportf(scn->source, &scn->pos, SEV_ERROR, "missing closing '\"', got \\u%d", scn->chr);
                goto ret;
            }
        }

        SET_TOKEN(CELL_TOKEN_STR);
        nextchar(scn);
        break;
    case '\'':
        CHECK(safe_strmcat(&scn->text, scn->letter), ret);

        nextchar(scn);

        if (scn->chr == '\\') {
            CHECK(parse_escape(scn), ret);
        } else if (scn->chr != '\'') {
            CHECK(safe_strmcat(&scn->text, scn->letter), ret);
            CHECK(safe_strcat(scn->value.str, scn->letter), ret);
            nextchar(scn);
        }

        if (scn->chr != '\'') {
            err = reportf(scn->source, &scn->pos, SEV_ERROR, "missing terminating \' character");
            goto ret;
        }

        CHECK(safe_strmcat(&scn->text, scn->letter), ret);
        SET_TOKEN(CELL_TOKEN_CHAR);
        nextchar(scn);
        break;
    case ')':
        switch (peekchar(scn)) {
        case '.':
        case ')':
            SET_TOKEN(CELL_TOKEN_RPAREN);
            CHECK(safe_strmcat(&scn->text, cell_tokens[CELL_TOKEN_RPAREN]), ret);
            nextchar(scn);
            break;
        default:
            CHECK(parse_oper(scn), ret);
            break;
        }
        break;
    case '(':
        ch = peekchar(scn);

        if (ch == ')' || !strchr(cell_operators, ch)) {
            SET_TOKEN(CELL_TOKEN_LPAREN);
            CHECK(safe_strmcat(&scn->text, cell_tokens[CELL_TOKEN_LPAREN]), ret);
            nextchar(scn);
        } else {
            CHECK(parse_oper(scn), ret);
        }
        break;
    case ',':
        SET_TOKEN(CELL_TOKEN_COMMA);
        CHECK(safe_strmcat(&scn->text, cell_tokens[CELL_TOKEN_COMMA]), ret);
        nextchar(scn);
        goto ret;
    case '.':
        switch (peekchar(scn)) {
        case '0' ... '9':
            CHECK(parse_number(scn), ret);
            break;
        case '.':
            CHECK(safe_strmcat(&scn->text, scn->letter), ret);
            nextchar(scn);

            if (peekchar(scn) != '.') {
                SET_TOKEN(CELL_TOKEN_INTERVAL);
                CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                nextchar(scn);
            } else {
                SET_TOKEN(CELL_TOKEN_ELLIPSIS);
                CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                nextchar(scn);
            }
            break;
        default:
            SET_TOKEN(CELL_TOKEN_DOT);
            CHECK(safe_strmcat(&scn->text, scn->letter), ret);
            nextchar(scn);
            break;
        }
        break;
    case '/':
        switch (peekchar(scn)) {
        case '/':
            nextchar(scn);
            nextchar(scn);

            for (ch = scn->chr; ch != '\n' && ch != utf8_error; ch = scn->chr) {
                CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                nextchar(scn);
            }

            SET_TOKEN(CELL_TOKEN_COMMENT);
            break;
        case '*':
            nextchar(scn);
            nextchar(scn);

            do {
                ch = scn->chr;

                if (ch == '*') {
                    if (peekchar(scn) == '/') {
                        nextchar(scn);
                        nextchar(scn);
                        break;
                    }
                }

                CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                nextchar(scn);
            } while (ch != utf8_error);

            SET_TOKEN(CELL_TOKEN_COMMENT);
            break;
        default:
            CHECK(parse_oper(scn), ret);
            break;
        }
        break;
    case '0' ... '9':
        CHECK(parse_number(scn), ret);
        break;
    case ';':
        SET_TOKEN(CELL_TOKEN_NEWLINE);
        CHECK(safe_strmcat(&scn->text, cell_tokens[CELL_TOKEN_NEWLINE]), ret);
        nextchar(scn);
        break;
    case '`':;
        nextchar(scn);
        while (scn->chr != '\n' && scn->chr != utf8_error) {
            CHECK(safe_strmcat(&scn->text, scn->letter), ret);
            nextchar(scn);
            if (scn->chr == '`') {
                nextchar(scn);
                break;
            }
        }
        SET_TOKEN(CELL_TOKEN_META);
        break;

#define __TOKEN_TYPE CELL_TOKEN_BRACKETS
#define __(u, l, str, chr)                                                                                             \
    case chr:                                                                                                          \
        SET_TOKEN(CELL_TOKEN_##u);                                                                                     \
        CHECK(safe_strmcat(&scn->text, scn->letter), ret);                                                             \
        nextchar(scn);                                                                                                 \
        break;
#include <cell/token.inc.h>

    default:
        /* identifiers and keywords */
        if (utf8_isalpha(scn->chr) || scn->chr == '_') {
            do {
                CHECK(safe_strmcat(&scn->text, scn->letter), ret);
                nextchar(scn);
            } while (utf8_isalpha(scn->chr) || scn->chr == '_' || utf8_isdigit(scn->chr));

            SET_TOKEN(CELL_TOKEN_IDENT);

            for (int i = 0; i < CELL_KEYWORD_LAST; i++) {
                const char *keyword = cell_keywords[i];

                if (strcmp(scn->text, keyword) == 0) {
                    SET_TOKEN(i + CELL_TOKEN_MODULE);
                    break;
                }
            }
        } else if (strchr(cell_operators, scn->chr)) {
            CHECK(parse_oper(scn), ret);
        } else {
            CHECK(safe_strmcat(&scn->text, scn->letter), ret);
            err = reportf(scn->source, &scn->pos, SEV_ERROR, "unrecognised character '%s'", scn->letter);
            nextchar(scn);
        }
        break;
    }

ret:
    return err;
}
