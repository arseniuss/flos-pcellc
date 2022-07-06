# Statements

Version 1.0.0, last edited by Armands Arseniuss Skolmeisters

```
<statements> ::=  <statement> ( <statement> )*

<statement> ::= <block statement> |
                <break statement> |
                <conitnue statement> |
                <declaration statement> |
                <do statement> |
                <expression statement> |
                <for statement> |
                <goto statement> |
                <if statement> |
                <label statement> |
                <return statement> |
                <while statement>
```

## Block statement

```
<block statement> ::= '{' <statements>? '}'
```

## Break statement

Definition:

```
<break statement> ::= 'break' <end>
```

## Continue statement

Definition:

```
<continue statement> ::= 'continue' <end>
```

## Declaration statement

```
<declaration statement> ::= <declaration>
```

## Do-while statement

```
<do statement> ::= 'do' <block statement>
                   'while' <boolean expression> <end>
```

1. A do-while statement implements simple loop;
2. First block statement is executed. Then if boolean expression evaluates to `true` block statement is executed again.
This continues til boolean expression evaluates to `false`.
3. A break statement inside block statement exits the loop.
4. A continue statement insode block statement will transfer control to boolean expression evaluation.

## Expression statement

```
<expression statement> ::= <expression> <end>
```

## For statement

```
<for statement> ::= 'for' ( <initialization>
                    (( <end> <condition> )?
                    <end> <increment>)  )? <block statement>

<initialization> ::= <expression>
<condition> ::= <boolean expression>
<increment> ::= <expression>
```

## Goto statement

```
<goto statement> ::= 'goto' ( <identifier> | 'default' | <expression> )? <end>
```

## If statement

```
<if statement> ::= 'if' <boolean expression> <block statement>
                 ( 'else' 'if' <boolean expression> <block statement> )*
                 ( 'else' <block statement> )?
```

## Label statement

```
<label statement> <identifier> ':' <statement>? <end>
```

## Return statement

```
<return statement> ::= 'return' <expression>? <end>
```

## While statement

Definition:

```
<while statement> ::= 'while' <boolean expression> <block statement>
```

1. The while statement implements simple loop.
2. First a boolean expression after the `while` keyword is evaluated. If the booelean expression is `true` then block
statement is executed. After execution the boolean statement is again evaluated - if `true` block statement is executed again. This goes until the boolean expression evaluates to `false`.
3. A break statement inside block statement will exit the loop.
4. A continue statement inside block statement will transfer control to evaluation of boolean expression.


# Changes

    21.06.2022. Initial version, 1.0.0, Armands Arserniuss Skolmeisters
