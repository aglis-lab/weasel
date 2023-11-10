# ROADMAP

## Migrate LLVM

- [x] Use LLVM-13
- [ ] use LLVM-15

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

- [ ] By default use mangling name

### Self executable

- [x] Using lld for linker
- [ ] Add command run
- [ ] Add command build
- [ ] Add command version
- [ ] Add Command emit llvm ir

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
