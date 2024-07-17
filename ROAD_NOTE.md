## Current Task

- [x] Return struct from function
- [x] Change struct field
- [x] Use Mangling name
- [x] Add Parameter Reference Type
- [x] Remove ErrorTable from any source code
  - [ ] Check Nil Literal Expression
  - [x] Create Error List
  - [x] Find Best strategy for creating syntax error (not continuing the code or parser)
- [ ] Migrate System Install into vcpkg
- [ ] Make Parser can't detect semantic error
- [ ] Create Semantic Analysis
  - [ ] Check Variable Exist
  - [ ] Check Type Valid
  - [ ] Check Field Expression Type
  - [ ] Implementation Scope System
- [ ] Remove File Buffer
- [ ] Add Error into Expression as AST Error or Semantic Error
- [ ] Add Error to Token as Token Error
- [ ] Integrate Error into the Parser
- [ ] Create LSP Example
- [ ] Create standalone Analysis System
- [ ] Create better error
- [ ] Variable Reference
- [ ] Global Variable or Value
- [ ] Add Variable Reference Type (Less Used, ignore it for a moment)
- [ ] Create LSP for Weasel

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
