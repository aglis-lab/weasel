# ROADMAP

## Migrate LLVM

- [x] Use LLVM-13
- [x] use LLVM-15
- [ ] Use std::string as possible then use char\*
- [ ] Use sanitize address
- [ ] Add LLVM Debugging
  - [ ] Location
    - [ ] Line
    - [ ] Column
    - [ ] Scope
    - [ ] inlineAt
  - [ ] Variable
    - [ ] Name
    - [ ] Argument
    - [ ] Scope
    - [ ] FIle
    - [ ] Line
    - [ ] Type
    - [ ] Flags

## Features

### Function

- [x] Function Declaration
- [x] Function Definition
- [x] Function Call
- [ ] Function Variable Number Arguments

### Data Type

- [x] integer
- [x] float
- [x] boolean
- [ ] string
- [x] [ ] char
- [ ] [ ]rune

### Variable Table Declaration

- [x] Variable Declaration
- [x] Variable Lookup
- [x] Function Lookup
- [x] Variable names and constants
- [x] Procedure and function names
- [x] Literal Constant
- [x] Strings Literal
- [x] Alloca at the beginning of function

### Error Table

- [x] Simple table

### Identifier

- [x] variable declaration and definition

### Binary Expression

- [x] Simple Binary Expression
- [x] Support variable re-assign

### Variable Type (signed and unsigned)

- [ ] Support variable signed and unsigned

### Operator

- [x] Support Multiply
- [x] Support Division
- [x] Support Addition
- [x] Support Substract
- [x] Support Modulo

### Unary Operator

- [ ] Support Unary Operator

### Array

- [x] Simple pointer type
- [x] Simple Address of
- [x] Support nil value
- [x] Support Array Data Sequence
- [ ] Support Vector Data Sequence

### Dynamic Allocation

- [ ] Support Malloc and Free

### IO console (input/output)

- [x] Support simple output through c library
- [x] Support simple input through c library

### Heterogeneous Computing

- [x] Implement Simple Internal Heterogeneous Computing (for prove of concept)

### If / Else Statement

- [x] Support if decision
- [x] Support else if decision
- [x] Support else decision

### For Statement

- [x] Support For-loop statement
- [x] Support For(while) statement

### Switch Statement

- [ ] Support For(switch) statement

### Struct

- [x] Support Struct
- [ ] Support Struct Impl methods

### Function Struct

- [ ] Return struct from function

* [ ] Support internal function inside struct

### Function Struct by reference

- [ ] Support Function by reference

### Name Mangling

- [x] By default use mangling name

### Self executable

- [x] Using lld for linker
- [ ] Add command run
- [ ] Add command build
- [ ] Add command version
- [ ] Add Command emit llvm ir

## Memory Management

Memory management solution

Special Symbol and keyword
Result -> !
Optional -> ?
raw pointer -> \*
arc -> ref
weak -> weak

### Unsafe Pointer Memory

```
// Using Arena for creating safe pointer
fun main() {
  let arena = memory.NewArena()
  let point = NewPoint(arena)

  // Do Something with point
}

// New Point
fun NewPoint(arena memory.Arena) Pointer<Point> {
  // unsafe pointer with safe heap manager
  let p = arena.New<Point>() {
    X: 10.2
    Y: 11
  }

  p.DoSomething()            // Call some method just for example
  return p                   // Pass unsafe pointer
}
```

### Strong or Shared Pointer Memory

```
// Strong or Shared Pointer
fun main() {
  let point = NewPoint()

  // Do Something with point
  p.DoSomething()
}

fun NewPointer() ref Point {
  let p = ref(Point{})   // shared pointer
  return p             // automaticly shared pointer and increase counting pointer
}
```

### Weak Pointer Memory

```
fun main() {
  // Strong or Shared Pointer
  let point = NewPoint()

  // Do Something with point
  Flaying(weak(point))
}

fun Flaying(point weak Point) {
  // actualPoint is a shared pointer
  // get actual ref and ok boolean
  if let actualPoint, ok = point.Ref(); ok {
    // Do something if ok
  }
}
```

### Optional Data

```
fun main() {
  let point = GetPoint().Ref()
  if let point, ok = point.Ref(); ok {
    // Do something with point
  }

  // Another convention
  let point, ok = GetPoint().Ref()
  if ok {
    // Do something with point
  }
}

fun GetPoint() ?Point {
  if let ok = CheckCurrentPoint(); ok {
    return Point{X: 1}
  }

  return nil
}
```

## Analysis

### Type Checking

- [ ] Declaration and value type

  - [ ] DeclarationStatement
  - [ ] VariableStatement

## Weasel Library

### IO Library

- [ ] Output Library
- [ ] Input Library

# BUGS

- [x] Weird Lexeme Location(row, col, position)
- [ ] Special character need handled
- [ ] I implemented binaryexpression for precedence associative right to left wrongly
- [x] Library inside lib folder instead of inside tools folder

# BUGS SOLVED

- weird lexeme Location solved by use filemapped source
