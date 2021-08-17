# Interface

*Syntax*
> interface: `interface` IDENTIFIER implements? interfaceBody
>
> interfaceBody: `{` interfaceMethod* `}`

```
interface Writer
{
    // ...
}
```

```
interface ConsoleWriter (Writer)
{
    // ...
}
```

## Interface method

*Syntax*
> interfaceMethod: `static`? functionHeader `;`

```
write(String string) &;
```

```
static write(String string) &;
```
