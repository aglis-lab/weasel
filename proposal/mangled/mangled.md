### Type Mangling

- void -> v
- boolean -> b
- byte -> ub
- sbyte -> sb
- short -> s
- ushort -> us
- int -> i
- uint -> ui
- long -> l
- ulong -> ul
- float -> f
- double -> d
- Pointer -> T
- Address Of -> AD
- Array -> AR

### Function Mangling

```
fun hello(a int) bool
```

- \_WRb7hello3i

```
impl Point {
  fun saying(test int) {}
}
```

- \_WIPointR7saying3i\_

| Symbol | Description                       | Value |
| ------ | --------------------------------- | ----- |
| \_W    | Prefix for weasel function        |       |
| I      | Prefix for impl<br />             |       |
| Point  | Struct to implement<br />         |       |
| R      | Prefix for return<br />           |       |
| v      | void return type<br />            |       |
| 7      | Prefix before function<br />      |       |
| saying | function name<br />               |       |
| 3      | Prefix before argument<br />      |       |
| i      | mangle for type integer<br />     |       |
| \_     | End of function or arugment<br /> |       |
