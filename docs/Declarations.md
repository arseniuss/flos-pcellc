# Declarations

Version 1.0.0, last edited by Armands Arserniuss Skolmeisters

# Description

```
<declarations> ::= <declaration> ( <declaration> )*

<declaration> ::= <function declaration> |
                  <import declaration> |
                  <type declaration> |
                  <variable declaration> |
```

## Alias declaration

```
<alias declaration> ::= 'alias' <identifier> ( <type> ) <end>
```

## Const declaration

```
<const declaration> ::= 'const' <identifier> <type>? '=' <constant expression>
```

## Function declaration

```
<function> ::= <prototype> <block>

<block> ::= '{' <statements>? '}'
```

## Import declaration

```
<import declaration> ::= 'import' r\<identifier>(\.<identifier>)*\
                         ( 'as' <identifier> )?
                         ( 'from' <string> )?
                         <end>
```

## Module declaration

```
<module declaration> ::= 'module' r\<identifier>(\.<identifier>)*\ <end>
```

## Prototype declaration

```
<prototype declaration> ::= <prototype> <end>
```

## Type declaration

```
<type declaration> ::= 'type' <identifier> <type> <end>
```

## Variable declaration

```
<variable statement> ::= 'var' <identifier> <type>? ( '=' <value> )? <end>
```

# Changes

    21.06.2022. Initial version, 1.0.0, Armands Arseniuss Skolmeisters
