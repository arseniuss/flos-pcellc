# Type system

Version 1.0.0, last edited by Armands Arseniuss Skolmeisters

## Basic types

### Boolean

```
<boolean types> ::= 'int1' | 'bool'

<boolean values> ::= 'true' | 'false' | '1' | '0'
```

```
alias bool = int1
```

### Integers

```
<integer types> ::= r/[us]int(8|16|32|64|128)/

<integer literals> ::=  r/0b[01_]+/ |
                        r/0o[0-7_]+/ |
                        r/[0-9_]+/ |
                        r/0x[0-9a-fA-F_]+/
```

```
alias byte sint8 // or uint 8
alias short sint16
alias int sint32
alias long sint64
alias cent sint128
```

### Floating point

```
<floating point types> ::= float(16|32|64|128)
```

```
alias float float32
alias double double32
```

## Pointers

```
<pointer> ::= '*' <type>
```

Examples:

```
var ptr *int
type e struct { field int; }

var ptr *e = e.new

e.field
```

## Nullable

```
<nullable> ::= '?' <type>
```

Examples:

```
var nullableBool ?bool // by default null

var n1 ?bool = false
var n2 ?bool = null

func hasValue(b ?bool) bool {
    return b // null is false
}
```

## Immutable

```
<immutable> ::= '!' <type>
```

Examples:

```
var value !bool = false // unchangeable, equal to const value = false
// Note. Should show warning

var v2 !bool // once set cannot change

v2 = true
v2 = false // Runtime error

@safe
func (value !any) hasValue() bool {
     return value.fixed
}
```

## Arrays

```
<array type> ::= '[' <capacity> ']' <type>
```

## Slices

```
<slice type> ::= '[]' <type>
```

## Structures

```
<structure type> ::= 'struct' <structure attributes>? '{' <identifier>? <type> ( ';' <identifier>? <type> )+ '}'

<structure attributes> ::= <structure attribute> ( <structure attribute> )*

<structure attribute> ::= 'packed'
```

## Interfaces

```
<interface type> ::= 'interface' '{' ( <prototype> )* '}'
```

```
alias any interface {}
```

## Functions

```
<function prototype> ::= 'func' <method object>? <identifier> '(' <arguments>? ')' <return type>

<method prototype> ::= <identifier> '(' <arguments>? ')' <return type>

<method object> ::= '(' <identifier> <type> ')'

<arguments> ::= <arguments> ( ',' <argument> )*

<argument> ::= <identifier> (',' <identifier> )* <type>

<return type> ::= <identifier>? <type> | '(' <arguments> ')'
```

1. If method object part does not include optional identifier then object can be accessed using keyword `self`.

Examples:

```
func (object) (i int) int
```

# Changes

    21.06.2022. Initial version, 1.0.0, Armands Arseniuss Skolmeisters
