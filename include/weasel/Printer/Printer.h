#pragma once

#include <fmt/core.h>

#include <weasel/AST/AST.h>
#include <weasel/IR/Module.h>
#include <fstream>

#define DEFAULT_SHIFT 3

namespace weasel
{
    class Printer
    {
    public:
        Printer(std::FILE *out = stdout) : _out(out) {}
        Printer(std::string outputFile) : _out(fopen(outputFile.c_str(), "w")) {}

        ~Printer() {}

    public:
        void print(Module *module);
        void print(Function *expr);
        void print(DeclarationStatement *expr);
        void print(CompoundStatement *expr);
        void print(ConditionStatement *expr);
        void print(AssignmentExpression *expr);
        void print(CallExpression *expr);
        void print(ReturnExpression *expr);
        void print(VariableExpression *expr);
        void print(ComparisonExpression *expr);
        void print(UnaryExpression *expr);
        void print(BoolLiteralExpression *expr);
        void print(NumberLiteralExpression *expr);
        void print(StringLiteralExpression *expr);
        void print(FieldExpression *expr);
        void print(IndexExpression *expr);
        void print(LoopingStatement *expr);
        void print(ArithmeticExpression *expr);
        void print(ArrayExpression *expr);
        void print(StructExpression *expr);
        void print(MethodCallExpression *expr);
        void print(GlobalVariable *expr);
        void print(BreakExpression *expr);

        // Without Newline Operand
        void printAsOperand(Function *expr);
        void printAsOperand(NumberLiteralExpression *expr);
        void printAsOperand(DoubleLiteralExpression *expr);
        void printAsOperand(BoolLiteralExpression *expr);
        void printAsOperand(VariableExpression *expr);
        void printAsOperand(AssignmentExpression *expr);
        void printAsOperand(ComparisonExpression *expr);
        void printAsOperand(CallExpression *expr);
        void printAsOperand(ReturnExpression *expr);
        void printAsOperand(UnaryExpression *expr);
        void printAsOperand(FieldExpression *expr);
        void printAsOperand(IndexExpression *expr);
        void printAsOperand(DeclarationStatement *expr);
        void printAsOperand(ArithmeticExpression *expr);
        void printAsOperand(StringLiteralExpression *expr);
        void printAsOperand(CharLiteralExpression *expr);
        void printAsOperand(ArrayExpression *expr);
        void printAsOperand(StructExpression *expr);
        void printAsOperand(MethodCallExpression *expr);
        void printAsOperand(GlobalVariable *expr);
        void printAsOperand(BreakExpression *expr);
        void printAsOperand(FloatLiteralExpression *expr);
        void printAsOperand(TypeCastExpression *expr);
        void printAsOperand(NilLiteralExpression *expr);

        string printAsOperand(Qualifier val)
        {
            switch (val)
            {
            case Qualifier::QualConst:
                return "const";
            case Qualifier::QualRestrict:
                return "final";
            default:
                return "mut";
            }
        }

    private:
        std::FILE *_out;

    private:
        int getCurrentShift() { return _currentShift; }
        void setCurrentShift(int shift) { _currentShift = shift; }

        int _currentShift = 0;
    };
} // namespace weasel
