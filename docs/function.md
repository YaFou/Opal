# Function

*Syntax*
> function: functionHeader blockStatement
>
> functionHeader: IDENTIFIER parameters type?
> 
> parameters: `(` (parameter (`,` parameter)*)? `)`
>
> parameter: type IDENTIFIER

## Examples

```
power(Integer base, Integer power) Integer
{
    // ...
}
```

```
panic()
{
    // ...
}
```
