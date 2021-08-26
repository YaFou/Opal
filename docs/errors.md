# Errors code

## Errors

|Code|Meaning|Scope|
|-|-|-|
|E001|Failed to open a file|Module|
|E002|Failed to read a file|Module|
|E003|Failed to close a file|Module|
|[E004](#e004)|Unexpected character|Scanner|
|[E005](#e005)|Expect `\|\|`|Scanner|
|[E006](#e006)|Expect a character|Scanner|
|[E007](#e007)|Expect `'`|Scanner|
|[E008](#e008)|Expect `"`|Scanner|
|[E009](#e009)|Expect `&&`|Scanner|
|E010|Expect `(` for parameters|Parser|
|E011|Expect function name|Parser|
|E012|Expect a closed symbol|Parser|
|E013|Expect a statement block to start a function body|Parser|
|E014|This operation supports a type but received another|Type checker|
|E015|Expect an expression|Parser|
|E016|Expect variable name|Parser|
|E017|Expect `=` to assign the variable|Parser|
|E018|Expect member name|Parser|
|E019|Expect `while` to set the loop condition|Parser|
|E020|Expect `;` after a do-while loop condition|Parser|
|E021|Expect `:` to start case block|Parser|
|E022|Expect `,` after an inline expression of a match arm|Parser|
|E023|Expect `:` to set the else expression|Parser|
|E024|Expect a type but received another one|Type checker|
|E025|Types are incompatible|Type checker|
|E026|Variable already exists|Type checker|
|E027|Variable doesn't exist|Type checker|
|E028|Condition must be type of `<boolean>`|Type checker|
|E029|Expect `else` block because of the return type of the `if` block|Type checker|
|E030|Return types of `if` and `else` blocks must be the same|Type checker|
|E031|A `match` value must have a type|Type checker|
|E032|All match arms must have the same type|Type checker|

### E004
*Example*
```
Unexpected character "~".
> 12~34
    ^
```

### E005
*Example*
```
Expect "||" but received "|~".
> false |~ true
         ^
```

### E006
A literal character value must have a character but it's the end of the file.

*Example*
```
Expect a character but it's the end of the file.
> '
   ^
```

### E007
*Example*
```
Expect "'" but received "~".
> 'a~
    ^
```

### E008
*Example*
```
Expect '"' but it's end of the file.
> "abc
      ^
```

### E009
*Example*
```
Expect "&&" but received "&~".
> false &~ true
         ^
```

## Fatal errors

|Code|Meaning|
|-|-|
|F001|Failed to allocate memory|

## Warnings

|Code|Meaning|Scope|
|-|-|-|
|W001|Unnecessary `;`|Parser|
|W002|Unnecessary unary `+`|Parser|
