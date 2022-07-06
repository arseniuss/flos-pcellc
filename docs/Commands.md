# Commands

Version 1.0.0, last edited by Armands Arseniuss Skolmeisters

## Description

WIP

## Command declaration

```
<command declaration> ::= 'cmd' <identifier> <command parameters>

<command parameters> ::= <variable name> <variable type> | <command keyword>

<command keyword> ::= '<' ( <separator> | <keyword> ) '>'

<separator> ::= r/[-@+]*/

<keyword> ::= <identifier>
```

Examples:

```

func install_id(id string) []string {
    /* queries possible variants */
}

function install_source(src string) []string {
    ...
}

cmd install name @autocomplete(install_id)
<from> source @autocomplete(install_source) {
    ...
}

> install git from https://github.com/arserniuss/git-port
```

# Changes

    21.06.2022. Initial version, 1.0.0, Armands Arseniuss Skolmeisters
