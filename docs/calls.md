# Calls

*Syntax*
> callExpression: expression callParameters
>
> callParameters: `(` (expression (`,` expression)*)? `)`
>
> newExpression `new` type callParameters
>
> memberExpression: expression `.` IDENTIFIER

## Examples

```
getConsole()
```

```
write("Hello world"!)
```

```
new Console()
```

```
getConsole().print("Hello world!")
```
