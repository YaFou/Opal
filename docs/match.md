# Match expression

*Syntax*
> matchExpression: `match` expression matchBody
>
> matchBody: `{` matchArm* `}`
>
> matchArm: expression `:` ((inlineExpression `,`) | blockExpression)

```
match variable {
    1: print("one"),
    2: print("two"),
    3: print("three"),
    _: {
        if variable > 10 {
            print("enormous");
        } else {
            print("a lot");
        }
    }
}
```
