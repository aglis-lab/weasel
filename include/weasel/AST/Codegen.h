#pragma once

namespace weasel
{
    class Codegen;
    class AnalysisSemantic;
    class Printer;
    class AnalysisError;
    class AnalysisEvaluate;

    using EvaluationValue = optional<variant<bool, long, double, string>>;
} // namespace weasel

namespace llvm
{
    class Value;
    class AllocaInst;
} // namespace llvm

#define BASE_CODEGEN_EXPRESSION(XX)                        \
public:                                                    \
    virtual llvm::Value *accept(Codegen *codegen) XX;      \
    virtual void accept(AnalysisSemantic *accept) XX;      \
    virtual void accept(AnalysisError *) XX;               \
    virtual EvaluationValue accept(AnalysisEvaluate *) XX; \
    virtual void print(Printer *printer) XX;               \
    virtual void printAsOperand(Printer *printer) XX;

#define VIRTUAL_CODEGEN_VALUE                      \
    friend class weasel::Codegen;                  \
                                                   \
protected:                                         \
    llvm::Value *_codegen = nullptr;               \
                                                   \
    virtual void setCodegen(llvm::Value *val) = 0; \
    virtual llvm::Value *getCodegen() = 0;

#define OVERRIDE_CODEGEN_VALUE                                             \
    friend class weasel::Codegen;                                          \
                                                                           \
protected:                                                                 \
    virtual void setCodegen(llvm::Value *val) override { _codegen = val; } \
    virtual llvm::Value *getCodegen() override { return _codegen; }

#define VIRTUAL_CODEGEN_TYPE                      \
public:                                           \
    virtual llvm::Type *accept(Codegen *codegen); \
    virtual void accept(AnalysisSemantic *accept);

#define OVERRIDE_CODEGEN_TYPE                      \
public:                                            \
    llvm::Type *accept(Codegen *codegen) override; \
    void accept(AnalysisSemantic *accept) override;

#define DECLARATION_EXPRESSION                                                  \
    friend class weasel::AnalysisSemantic;                                      \
                                                                                \
private:                                                                        \
    Expression *_declValue = nullptr;                                           \
                                                                                \
    void setDeclarationValue(Expression *declValue) { _declValue = declValue; } \
    Expression *getDeclarationValue() { return _declValue; }

#define VIRTUAL_CODEGEN_EXPRESSION \
    BASE_CODEGEN_EXPRESSION(= 0)   \
    VIRTUAL_CODEGEN_VALUE

#define OVERRIDE_CODEGEN_EXPRESSION   \
    BASE_CODEGEN_EXPRESSION(override) \
    OVERRIDE_CODEGEN_VALUE

#define EMPTY_CODEGEN_EXPRESSION        \
    BASE_CODEGEN_EXPRESSION(override{}) \
    OVERRIDE_CODEGEN_VALUE
