# Tokens

## Primitive values
|Name|Value|Example|
|-|-|-|
|digit|from `0` to `9`|`4`, `2`|
|char|all characters|`a`, `*`|
|alpha|from `a` to `z` or from `A` to `Z`|`a`, `K`|

## Token type

### Symbols

|Name|Value|
|-|-|
|PLUS|`+`|
|PLUS_EQUAL|`+=`|
|DOUBLE_PLUS|`++`|
|MINUS|`-`|
|MINUS_EQUAL|`-=`|
|DOUBLE_MINUS|`--`|
|STAR|`*`|
|STAR_EQUAL|`*=`|
|DOUBLE_STAR|`**`|
|DOUBLE_STAR_EQUAL|`**=`|
|SLASH|`/`|
|SLASH_EQUAL|`/=`|
|MODULO|`%`|
|MODULO_EQUAL|`%=`|
|LEFT_PAREN|`(`|
|RIGHT_PAREN|`)`|
|LEFT_BRACE|`{`|
|RIGHT_BRACE|`}`|
|LEFT_BRACKET|`[`|
|RIGHT_BRACKET|`]`|
|SEMILICON|`;`|
|EQUAL|`=`|
|DOUBLE_EQUAL|`==`|
|BANG_EQUAL|`!=`|
|BANG|`!`|
|LESS|`<`|
|LESS_EQUAL|`<=`|
|GREATER|`>`|
|GREATER_EQUAL|`>=`|
|DOT|`.`|
|COMMA|`,`|
|DOUBLE_AMPERSAND|`&&`|
|DOUBLE_PIPE|`\|\|`|
|QUESTION_MARK|`?`|
|HASHTAG|`#`|
|COLON|`:`|
|AT_SYMBOL|`@`|
|UNDERSCORE|`_`|
|EOF|`\0`||

### Literals

|Name|Value|Stored as|
|-|-|-|
|INTEGER|`<digit>+`|integer|
|STRING|`"<char>*"`|string|
|CHAR|`'<char>'`|char|
|FLOAT|`(<digit>+)?.<digit>+`|float|
|IDENTIFIER|`<alpha>(<alpha>\|<digit>\|_)*`|string|

### Keywords

|Name|Value|
|-|-|
|\<uppercase keyword>|\<keyword>|

- `abstract`
- `break`
- `class`
- `const`
- `constructor`
- `continue`
- `do`
- `else`
- `enum`
- `false`
- `if`
- `interface`
- `loop`
- `match`
- `new`
- `null`
- `return`
- `static`
- `true`
- `var`
- `while`
