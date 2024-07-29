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

#define BASE_CODEGEN_EXPRESSION(XX)                          \
public:                                                      \
    virtual llvm::Value *codegen(WeaselCodegen *codegen) XX; \
    virtual void print(Printer *printer) XX;                 \
    virtual void printAsOperand(Printer *printer) XX;        \
    virtual void semantic(AnalysisSemantic *semantic) XX;

// virtual bool isError() XX;

#define CODEGEN_TYPE                                     \
public:                                                  \
    virtual llvm::Type *codegen(WeaselCodegen *codegen); \
    virtual void semantic(AnalysisSemantic *semantic);

#define VIRTUAL_CODEGEN_EXPRESSION BASE_CODEGEN_EXPRESSION(= 0)
#define OVERRIDE_CODEGEN_EXPRESSION BASE_CODEGEN_EXPRESSION(override)
#define EMPTY_CODEGEN_EXPRESSION BASE_CODEGEN_EXPRESSION(override{})
