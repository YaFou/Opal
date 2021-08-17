# Tokens

## Primitive values
|Name|Value|Example|
|-|-|-|
|digit|from `0` to `9`|`4`, `2`|
|char|all characters|`a`, `*`|
|alpha|from `a` to `z` or from `A` to `Z` or `_`|`a`, `K`, `_`|

## Token type

### Symbols

|Name|Value|Stored as|
|-|-|-|
|PLUS|`+`|
|MINUS|`-`|
|DOUBLE_STAR|`**`|
|STAR|`*`|
|SLASH|`/`|
|LEFT_PAREN|`(`|
|RIGHT_PAREN|`)`|
|LEFT_BRACE|`{`|
|RIGHT_BRACE|`}`|
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
|AMPERSAND|`&`|
|DOUBLE_AMPERSAND|`&&`|
|DOUBLE_PIPE|`||`|
|QUESTION_MARK|`?`|
|PLUS_EQUAL|`+=`|
|DOUBLE_EQUAL|`++`|
|MINUS_EQUAL|`-=`|
|DOUBLE_MINUS|`--`|
|STAR_EQUAL|`*=`|
|SLASH_EQUAL|`/=`|
|MODULO|`%`|
|MODULO_EQUAL|`%=`|
|LEFT_BRACKET|`[`|
|RIGHT_BRACKET|`]`|
|HASHTAG|`#`|
|COLON|`:`|
|AT_SYMBOL|`@`|
|ARROW|`=>`|
|UNDERSCORE|`_`|
|EOF|`\0`||

### Literals

|Name|Value|Stored as|
|-|-|-|
|INTEGER|`<digit>+`|integer|
|STRING|`"<char>*"`|string|
|CHAR|`'<char>'`|char|
|FLOAT|`<digit>+.<digit>+`|float|
|IDENTIFIER|`<alpha>(<alpha>\|<digit>)*`|string|

### Keywords

|Name|Value|Stored as|
|-|-|-|
|\<uppercase keyword>|\<keyword>||

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
