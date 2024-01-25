# Lexical

Version 1.0.0, last edited by Armands Arseniuss Skolmeisters

## End of line

```
<end-of-line> ::= <newline> | <end-of-file>
```

## Comments

```
<comment> ::= '//' <characters>? <end-of-line>

<multiline-comment> ::= '/*' <characters>? '*/'
```

## Identifiers

```
<identifier> ::= <identifier-start> <identifier-char>*

<identifier-start> ::= 'a' .. 'z' | 'A' .. 'Z' | '_'

<identifier-char> ::= <identifier-start> | '0' .. '9'
```

## Character literals

```
<chacter-literal> ::= ''' <single-quoted-character> '''

<single-quoted-character> ::= <character> | <escape-sequence>
```

## String literals

```
<string-literal> ::= <double-quoted-string>

<double-quoted-string> ::= '"' <double-quoted-characters>* '"'

<double-quoted-characters> ::= <character> | <escape-sequence>

<escape-sequence> ::= '\'' | '\"' | '\?' | '\\' | '\0' | '\a' | '\b' | '\f' |
                      '\n' | '\r' | '\t' | '\v' |
                      '\x' <hex-digit> <hex-digit> |
                      '\' <octal-digit> |
                      '\' <octal-digit> <octal-digit>  |
                      '\' <octal-digit>  <octal-digit>  <octal-digit> |
                      '\u'  ( <hex-digit> ){1,6}
```

## Floating point literal

TODO

## Integer literals

```
<integer-literal> ::= r/0[bB][01_]+/ |
                      r/0[oO][0-7_]+/ |
                      r/[0-9_]+/ | ; warn if begins with 0
                      r/0[xX][0-9a-fA-F_]+/
```

# Changes

    27.06.2022. Initial version, 1.0.0, Armands Arseniuss Skolmeisters
