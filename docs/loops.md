# Loop expressions

*Syntax*
> loopExpression: infiniteLoop | whileLoop | doWhileLoop

## Infinite loop

*Syntax*
> infiniteLoop: `loop` blockStatement

### Examples

```
var i = 0;

loop {
    if i == 10 {
        break;
    }

    print(i++);
}
```

## While loops

*Syntax*
> whileLoop: `while` expression blockStatement
>
> doWhileLoop: `do` blockStatement `while` expression `;`

### Examples

```
var i = 1;

while i < 10 {
    print(i++);
}
```

```
var i = 10;

do {
    print(i++);
} while i < 10;
```
