# Expressions

*Syntax*
> expression: inlineExpression | blockExpression

## Inline expressions

*Syntax*
> inlineExpression: varDeclaration \
> | literal \
> | assignment \
> | operatorExpression \
> | groupedExpression \
> | arrayExpression \
> | indexExpression \
> | callExpression \
> | newExpression \
> | memberExpression \
> | CONTINUE \
> | BREAK \
> | returnExpression \
> | ternaryExpression
>
> groupedExpression: `(` expression `)`
>
> returnExpression: `return` expression
>
> ternaryExpression: expression `?` expression `:` expression

### Examples

```
12 * (34 + 56)
```

```
return false
```

```
variable > 10 ? "It's true!" : "It's false..."
```

## Block expressions

*Syntax*
> blockExpression: loopExpression \
> | ifExpression \
> | matchExpression

## Precedences

*Higher number is a higher priority.*
1. ternary (`.. ? .. : ..`)
1. assignment (`=`)
1. or (`||`)
1. and (`&&`)
1. comparison (`==`, `!=`, `<`, `<=`, `>`, `>=`)
1. term (`+` or `-`)
1. factor (`*`, `/` or `%`)
1. unary (`-` or `!`)
1. power (`**`)
1. grouped (`()`)
1. call (`.`, `[..]` or `(..)`)

```
12 + 34 * 56 / (78 - 9)
^^   ^^^^^^^   ^^^^^^^^
 |         |   |
 |         ^^^^^
 |         |
 ^^^^^^^^^^^
 |
 result
```
