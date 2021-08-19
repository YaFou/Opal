# Grammar

## Notation

|Notation|Explaination|Example(s)|
|-|-|-|
|ITEM|a token produced by the scanner|`;`|
|item|a node produced by the parser|statement|
|item?|`item` is facultative|`abstract`?|
|item*|zero or more of `item`|statement*|
|item+|one or more of `item`|statement+|
|\||item or another item|function \| class|
|()|Grouped items|(`,` statement)|

## Source code

*Syntax*
> module: rootDeclaration* EOF
> 
> rootDeclaration: directive | function | class | enum | interface
> 
> complexIdentifier: (IDENTIFIER | `@` | `/` | `.`)+
