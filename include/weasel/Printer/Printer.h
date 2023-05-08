#pragma once

#include <fmt/core.h>

#include <weasel/AST/AST.h>
#include <weasel/IR/Module.h>

#define DEFAULT_SHIFT 3

namespace weasel
{
    class Printer
    {
    public:
        Printer(/* args */) {}
        ~Printer() {}

    public:
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

        void print(weasel::Module *module);

    private:
        int getCurrentShift() { return _currentShift; }
        void setCurrentShift(int shift) { _currentShift = shift; }

        int _currentShift = 0;
    };
} // namespace weasel
