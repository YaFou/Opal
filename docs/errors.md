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
