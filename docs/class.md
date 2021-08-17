# Class

*Syntax*
> class: `abstract`? `class` IDENTIFIER extends? interfaces? classBody
>
> extends: `:` type
>
> interfaces: `(` type* `)`
>
> classBody: `{` classDeclaration* `}`

```
abstract AbstractWriter (WriterInterface)
{
    // ...
}

class Writer : AbstractWriter
{
    // ...
}
```

## Class declaration

*Syntax*
> classDeclaration: property | method | abstractMethod | constructor

### Property

*Syntax*
> property: `static`? type IDENTIFIER `;`

```
String name;
```

```
static String global;
```

### Methods

*Syntax*
> method: `static`? function
>
> abstractMethod: `abstract` `static`? functionHeader `;`

```
write(String string) &
{
    // ...
}
```

```
abstract static write(String string) &;
```

### Constructor

*Syntax*
> constructor: `constructor` parameters type blockStatement

```
constructor (String file)
{
    // ...
}
```
