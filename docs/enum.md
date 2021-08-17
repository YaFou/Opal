# Enumeration

*Syntax*
> enum: `enum` IDENTIFIER enumBody
>
> enumBody: `{` (enumItem (`,` enumItem))? `}`
>
> enumItem: IDENTIFIER

## Examples

```
enum Role
{
    ADMINISTRATOR,
    MODERATOR,
    MEMBER
}
```
