# Intermediate representation

## Operand types

|Name|Letter|Meaning|Format|Example|
|-|-|-|-|-|
|Integer|I|A literal integer|\<value>|`12`|
|Register|R|An allocated memory to perform simple operations|`%`\<value>|`%12`|
|Memory|M|An allocated memory|`$`\<value>|`$12`|
|Storage|R|A register or a memory (R or M)|||
|All|A|All possible types of operands (I or S)|||

## Instructions

|Full name|Short name|Operands|Meaning|Example|
|-|-|-|-|-|
|MOVE|`MOV`|A, S|Move the value of the first operand to the second operand (`%2 = %1`)|`MOV 10, %0`|
|ADD|`ADD`|A, A, S|Move the sum of the two first operands in the third operand (`%3 = %1 + %2`)|`ADD %0, 10, %1`|
|SUBSTRACT|`SUB`|A, A, S|Move the difference of the two first operands in the third operand (`%3 = %1 - %2`)|`SUB %0, 10, %1`|
|MULTIPLY|`MUL`|A, A, S|Move the product of the two first operands in the third operand (`%3 = %1 * %2`)|`MUL %0, 10, %1`|
|DIVIDE|`DIV`|A, A, S|Move the quotient of the two first operands in the third operand (`%3 = %1 / %2`)|`DIV %0, 10, %1`|
|MODULO|`MOD`|A, A, S|Move the remainder of the two first operands in the third operand (`%3 = %1 % %2`)|`MOD %0, 10, %1`|
|RETURN|`RET`|A|Return the value of the first operand|`RET %0`|
|NEGATE|`NEG`|A, S|Move the negated value of the first operand to the second operand (`%2 = -%1`)|`NEG %0, %1`|
