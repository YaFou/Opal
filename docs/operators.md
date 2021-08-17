# Operators

*Syntax*
> operatorExpression: unaryOperation | doubleOperation | incrementalOperation
>
> unaryOperation: (`-` | `!`) expression
>
> doubleOperation: expression operator expression
>
> operator: binaryOperator \
> | comparisonOperator \
> | booleanOperator \
> | compoundAssignmentOperator \
> | `=`
>
> binaryOperator: `+` | `-` | `*` | `/` | `%` | `**`
>
> comparisonOperator: `==` | `!=` | `<` | `<=` | `>` | `>=`
>
> booleanOperator: `&&` | `||`
>
> compoundAssignmentOperator: `+=` | `-=` | `*=` | `/=` | `%=`
>
> incrementalOperation: (incrementalOperator expression) | (expression incrementalOperator)
>
> incrementalOperator: `++` | `--`

## Examples

### Unary operations

```
-12
```

```
!true  // false
!false // true
```

### Binary operations

```
12 + 34 // 46
12 - 34 // -22
12 * 34 // 408
34 / 12 // 2
34 % 12 // 10
```

### Comparison operations

```
12 == 34 // false
12 != 34 // true
12 < 34  // true
12 <= 12 // true
12 > 34  // false
12 >= 34 // false+
```

### Binary operations

```
false && false // false
false && true  // false
true && true   // true

false || false // false
false || true  // true
true || true   // true
```

### Compound assignment operations

```
variable += 10 // variable = variable + 10
variable -= 10 // variable = variable - 10
variable *= 10 // variable = variable * 10
variable /= 10 // variable = variable / 10
variable %= 10 // variable = variable % 10
```

### Assignment operation

```
variable = 10
```

### Incremental operations

```
variable++ // return "variable" and increment it
++variable // increment "variable" and return it
variable-- // return "variable" and decrement it
--variable // decrement "variable" and return it
```
