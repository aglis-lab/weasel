#pragma once

namespace weasel
{
    class WeaselCodegen;
    class AnalysisSemantic;
    class Printer;
} // namespace weasel

namespace llvm
{
    class Value;
    class AllocaInst;
} // namespace llvm

#define BASE_CODEGEN_EXPRESSION(XX)                         \
public:                                                     \
    virtual llvm::Value *accept(WeaselCodegen *codegen) XX; \
    virtual void accept(AnalysisSemantic *semantic) XX;     \
    virtual void print(Printer *printer) XX;                \
    virtual void printAsOperand(Printer *printer) XX;

// virtual bool isError() XX;

#define CODEGEN_TYPE                                    \
public:                                                 \
    virtual llvm::Type *accept(WeaselCodegen *codegen); \
    virtual void accept(AnalysisSemantic *semantic);

#define VIRTUAL_CODEGEN_EXPRESSION BASE_CODEGEN_EXPRESSION(= 0)
#define OVERRIDE_CODEGEN_EXPRESSION BASE_CODEGEN_EXPRESSION(override)
#define EMPTY_CODEGEN_EXPRESSION BASE_CODEGEN_EXPRESSION(override{})
