## Current Task

- [x] Return struct from function
- [x] Change struct field
- [x] Use Mangling name
- [x] Add Parameter Reference Type
- [x] Remove ErrorTable from any source code

  - [x] Check Nil Literal Expression
  - [x] Create Error List
  - [x] Find Best strategy for creating syntax error (not continuing the code or parser)

- [x] Migrate System Install into vcpkg
- [x] Add Token into Type (partially done)
- [ ] Optimize Type Codegen (Save data type codegen - single type for all related expressions)
- [x] Make Parser can't detect semantic error
- [x] LLVM 17 Pointer Type System
- [x] Change ArgumentType into Expression
- [ ] Create Semantic Analysis

  - [x] Check Variable Exist
  - [x] Check Type Valid
  - [x] Check Field Expression Type
  - [ ] Implementation Scope System
  - [ ] Improve Return Type System

- [ ] Remove File Buffer
- [x] Nested Struct Expression
- [x] Nested Field Expression
- [ ] Improve Environment Variable (More stylish setup for env variable)
- [ ] Circular Struct Depedencies
- [x] Add Error into Expression as AST Error or Semantic Error
- [ ] Add Error to Token as Token Error
- [ ] Add token into every Type
- [x] Integrate Error into the Parser
- [x] Create LSP Example
- [x] Create standalone Analysis System
- [x] Create better error
- [ ] Variable Reference
- [ ] Global Variable or Value
- [ ] Add Variable Reference Type (Less Used, ignore it for a moment)
- [ ] Create LSP for Weasel
- [ ] Print AST as a JSON for debuggind purpose (NOT VERY IMPORTANT)

## Flow for struct inside function

### Function Return Alias

- Return use function return alias when returning a struct

### Direct return struct

- Need to add struct metadata
- Return directly mean allocate the memory as return alias

### Initialize struct

- Initialize struct withouth struct inside
- Initialize struct with struct inside
- Initialize struct with struct pointer

### Struct pointer

### Struct < 124 bits size
