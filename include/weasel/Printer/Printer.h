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

        // Without Newline Operand
        void print(weasel::NumberLiteralExpression *expr);
        void print(weasel::StringLiteralExpression *expr);
        void print(weasel::VariableExpression *expr);
        void print(weasel::ComparisonExpression *expr);

        void print(weasel::Module *module);

    private:
        int getCurrentShift() { return _currentShift; }
        void setCurrentShift(int shift) { _currentShift = shift; }

        int _currentShift = 0;
    };
} // namespace weasel
