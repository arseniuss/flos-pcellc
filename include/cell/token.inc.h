/*
 * Copyright (C) 2023 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
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

#ifndef CELL_TOKEN_SYSTEM
    #error
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE == CELL_TOKEN_SYSTEM
__(EOF, eof, "end of file", )
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE == CELL_TOKEN_LITERAL
__(COMMENT, comment, "comment", 'C')
__(IDENT, ident, "ident", 'i')
__(NUMBER, int, "number", 'n')
__(FLOAT, float, "floting point number", 'f')
__(CHAR, char, "character", 'c')
__(STR, str, "str", 's')
__(OPER, oper, "op", 'o')
__(NEWLINE, newline, "newline", '\n')
__(COMMA, , ",", ',')
__(META, , "meta", '`')
__(DOT, , ".", '.')
__(INTERVAL, interval, "..", '.')
__(ELLIPSIS, elipsis, "...", '.')
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE == CELL_TOKEN_BRACKETS
__(LQBRACK, , "[", '[')
__(LCBRACK, , "{", '{')
__(RQBRACK, , "]", ']')
__(RCBRACK, , "}", '}')
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE == CELL_TOKEN_OPERATORS
__(INVALID_OPER, , "operator start", -1)
__(LPAREN, , "(", '(')
__(RPAREN, , ")", ')')
__(PLUS, , "+", '+')
__(MINUS, , "-", '-')
__(MUL, , "*", '*')
__(DIV, , "/", '/')
__(MOD, , "%", '%')
__(XOR, , "^", '^')
__(AND, , "&", '&')
__(OR, , "|", '|')
__(LT, , "<", '<')
__(GT, , ">", '>')
__(EQ, , "eq", '=')
__(RDIV, , "\\", '\\')
__(TILDE, , "~", '~')
__(COLON, , ":", ':')
__(QUEST, , "?", '?')
__(EXCLAM, , "!", '!')
__(AT, , "@", '@')
__(HASH, , "#", '#')
__(LAST_OPER, , "last operator", -1)
#endif

#if !defined(__TOKEN_TYPE) || __TOKEN_TYPE == CELL_TOKEN_KEYWORDS
__(MODULE, module, "module", )
__(IMPORT, import, "import", )
__(FROM, from, "from", )
__(VAR, var, "var", )
__(TYPE, type, "type", )
__(ALIAS, alias, "alias", )
__(STRUCT, struct, "struct", )
__(FUNC, func, "func", )
__(INTERFACE, interface, "interface", )
__(CONST, const, "const", )
__(RETURN, return, "return", )
__(BREAK, break, "break", )
__(CASE, case, "case", )
__(CONTINUE, continue, "continue", )
__(DEFAULT, default, "default", )
__(IF, if, "if", )
__(ELSE, else, "else", )
__(FOR, for, "for",)
__(AS, as, "as", )
#endif

#undef __
#undef __TOKEN_TYPE
