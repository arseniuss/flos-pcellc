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

#ifndef __
#define __(u, l, str, chr)
#endif

#ifndef TOKEN_SYSTEM
#error
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE==TOKEN_SYSTEM
__(EOF, eof, "end of file",)
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE==TOKEN_LITERAL
__(COMMENT, comment, "comment", 'C')
__(IDENT, ident, "ident", 'i')
__(NUMBER, int, "number", 'n')
__(FLOAT, float, "floting point number", 'f')
__(CHAR, char, "character", 'c')
__(STR, str, "str", 's')
__(OPER, oper, "op", 'o')
__(NEWLINE, newline, "newline", '\n')
__(SEMICOLON,, "semi", ';')
__(COMMA,, ",", ',')
__(HASH,, "#", '#')
__(META,, "meta", '`')
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE==TOKEN_BRACKETS
__(LPAREN,, "(", '(')
__(LQBRACK,, "[", '[')
__(LCBRACK,, "{", '{')
__(RPAREN,, ")", ')')
__(RQBRACK,, "]", ']')
__(RCBRACK,, "}", '}')
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE==TOKEN_OPERATORS     
__(PLUS,, "+", '+')
__(MINUS,, "-", '-')
__(MUL,, "*", '*')
__(DIV,, "/", '/')
__(MOD,, "%", '%')
__(XOR,, "^", '^')
__(AND,, "&", '&')
__(OR,, "|", '|')
__(LT,, "<", '<')
__(GT,, ">", '>')
__(EQ,, "eq", '=')
__(RDIV,, "\\", '\\')
__(TILDE,, "~", '~')
__(DOT,, ".", '.')
__(COLON,, ":", ':')
__(QUEST,, "?", '?')
__(EXCLAM,, "!", '!')
__(AT,, "@", '@')
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE==TOKEN_KEYWORDS
__(MODULE, module, "module",)
__(IMPORT, import, "import",)
__(FROM, from, "from",)
__(VAR, var, "var",)
__(TYPE, type, "type",)
__(ALIAS, alias, "alias",)
__(STRUCT, struct, "struct",)
__(FUNC, func, "func",)
__(INTERFACE, interface, "interface",)
__(CONST, const, "const",)
__(RETURN, return, "return",)
__(BREAK, break, "break",)
__(CASE, case, "case",)
__(CONTINUE, continue, "continue",)
__(DEFAULT, default, "default",)
__(IF, if, "if",)
    __(ELSE, else, "else",)
    __(FOR, for, "for",)
        __(AS, as, "as",)
#endif

#undef __
#undef __TOKEN_TYPE
