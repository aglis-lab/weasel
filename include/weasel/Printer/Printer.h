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
        void print(weasel::Module *module);
        void print(weasel::Function *expr);
        void print(weasel::DeclarationStatement *expr);
        void print(weasel::CompoundStatement *expr);
        void print(weasel::ConditionStatement *expr);
        void print(weasel::AssignmentExpression *expr);
        void print(weasel::CallExpression *expr);
        void print(weasel::ReturnExpression *expr);
        void print(weasel::VariableExpression *expr);
        void print(weasel::ComparisonExpression *expr);
        void print(weasel::UnaryExpression *expr);
        void print(weasel::BoolLiteralExpression *expr);
        void print(weasel::NumberLiteralExpression *expr);
        void print(weasel::StringLiteralExpression *expr);
        void print(weasel::FieldExpression *expr);
        void print(weasel::ArrayExpression *expr);
        void print(weasel::LoopingStatement *expr);
        void print(weasel::ArithmeticExpression *expr);
        void print(weasel::ArrayLiteralExpression *expr);
        void print(weasel::StructExpression *expr);

        // Without Newline Operand
        void printAsOperand(weasel::NumberLiteralExpression *expr);
        void printAsOperand(weasel::DoubleLiteralExpression *expr);
        void printAsOperand(weasel::BoolLiteralExpression *expr);
        void printAsOperand(weasel::VariableExpression *expr);
        void printAsOperand(weasel::AssignmentExpression *expr);
        void printAsOperand(weasel::ComparisonExpression *expr);
        void printAsOperand(weasel::CallExpression *expr);
        void printAsOperand(weasel::ReturnExpression *expr);
        void printAsOperand(weasel::UnaryExpression *expr);
        void printAsOperand(weasel::FieldExpression *expr);
        void printAsOperand(weasel::ArrayExpression *expr);
        void printAsOperand(weasel::DeclarationStatement *expr);
        void printAsOperand(weasel::ArithmeticExpression *expr);
        void printAsOperand(weasel::StringLiteralExpression *expr);
        void printAsOperand(weasel::CharLiteralExpression *expr);
        void printAsOperand(weasel::ArrayLiteralExpression *expr);
        void printAsOperand(weasel::StructExpression *expr);

    private:
        std::FILE *_out;

    private:
        int getCurrentShift() { return _currentShift; }
        void setCurrentShift(int shift) { _currentShift = shift; }

        int _currentShift = 0;
    };
} // namespace weasel
