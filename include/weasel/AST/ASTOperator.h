#pragma once

#include "weasel/AST/ASTBase.h"

// Expression Without Block PART
namespace weasel
{
    // Type Casting Operator Expression
    class TypeCastExpression : public Expression
    {
    private:
        Expression *_rhs;

    public:
        TypeCastExpression(Token op, Type *type, Expression *rhs) : Expression(op, type), _rhs(rhs) {}

        Expression *getExpression() const { return _rhs; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Binary Operator Expression
    class ArithmeticExpression : public Expression
    {
    private:
        Expression *_lhs;
        Expression *_rhs;

    public:
        ArithmeticExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, lhs->getType()), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Binary Operator Expression
    class LogicalExpression : public Expression
    {
    private:
        Expression *_lhs;
        Expression *_rhs;

    public:
        LogicalExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, lhs->getType()), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Assignment Operator Expression
    class AssignmentExpression : public Expression
    {
    private:
        Expression *_lhs;
        Expression *_rhs;

    public:
        AssignmentExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, lhs->getType()), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Comparison Operator Expression
    class ComparisonExpression : public Expression
    {
    private:
        Expression *_lhs;
        Expression *_rhs;

    public:
        ComparisonExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, Type::getBoolType()), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Unary Operator Expression
    class UnaryExpression : public Expression
    {
    public:
        enum Operator
        {
            Borrow,      // & Expression
            Dereference, // * Expression
            Negative,    // - Expression
            Positive,    // + Expression
            Not,         // ! Expression
            Negation,    // ~ Expression
        };

    private:
        Expression *_rhs;
        Operator _op;

    public:
        UnaryExpression(Token token, Operator op, Expression *rhs) : Expression(token), _rhs(rhs), _op(op)
        {
            auto type = rhs->getType();
            if (op == Operator::Borrow)
                type = Type::getPointerType(type);
            else if (op == Operator::Dereference)
                type = type->getContainedType();
            else if (op == Operator::Not)
                type = Type::getBoolType();

            setType(type);
        }

        Expression *getExpression() const { return _rhs; }
        Operator getOperator() const { return _op; }

        ~UnaryExpression()
        {
            delete _rhs;
        }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };
} // namespace weasel
